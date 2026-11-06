//-------------------------------------------------------------
//! @file   ShopViewSystem.cpp
//! @brief  ShopViewSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/ShopViewSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/HudLayoutConfigComponent.hpp>
#include <StardustFever/ECS/Component/LauncherConfigComponent.hpp>
#include <StardustFever/ECS/Component/ShopCardComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>
#include <StardustFever/ECS/Utility/UpgradeText.hpp>

#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

#include <cstdio>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::UITweenProperty;
        using Tsukino::Core::Math::Easing::EaseType;
        namespace Tween = Tsukino::BuiltIn::ECS::UITweenUtility;

        enum class CardState : int {
            Unaffordable = 0,
            Affordable   = 1,
            Maxed        = 2,
        };
    }    // namespace

    ShopViewSystem::ShopViewSystem(Tsukino::ECS::EventBus& eventBus)
        : m_purchases(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // カードを全て作り直す
    //-------------------------------------------------------------
    void ShopViewSystem::Rebuild(Tsukino::ECS::Registry& registry) {
        const HudLayoutConfigComponent*            layout  = FindSingleton<HudLayoutConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!layout || !context || !context->prefabFactory)
            return;

        std::vector<Tsukino::ECS::Entity> old;
        for(const auto entity : registry.View<ShopCardComponent>()) {
            old.push_back(entity);
        }
        for(const Tsukino::ECS::Entity entity : old) {
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, entity);
        }

        const auto&                           upgrades = Data::GetUpgradeTable().upgrades;
        std::uniform_real_distribution<float> tilt(-0.008f, 0.008f);
        for(size_t i = 0; i < upgrades.size(); ++i) {
            const Tsukino::ECS::Entity card = HudUi::InstantiateTemplate(*context->prefabFactory, registry, layout->shopCardGroup);
            if(card == entt::null)
                return;

            HudUi::SetPosition(registry, card, layout->shopX, layout->shopY + layout->shopSpacing * static_cast<float>(i));
            ShopCardComponent& component = registry.AddComponent<ShopCardComponent>(card);
            component.index              = static_cast<int>(i);
            component.upgradeId          = upgrades[i].id;

            HudUi::SetText(registry, HudUi::FindPart(registry, card, "Key"), std::to_string(i + 1));
            HudUi::SetText(registry, HudUi::FindPart(registry, card, "Name"), upgrades[i].name);

            // カードは押して買える。乗ると少し大きくなる
            UiParts::MakeClickable(registry, card, UiAction::BuyUpgrade, static_cast<int>(i));
            Tsukino::BuiltIn::ECS::UIButtonFeedbackComponent& feedback = registry.AddComponent<Tsukino::BuiltIn::ECS::UIButtonFeedbackComponent>(card);
            feedback.hoverScale                                        = 1.025f;
            feedback.pressScale                                        = 0.97f;
            feedback.clickPunch                                        = 0.05f;

            //-------------------------------------------------------------
            // 上から順に少しずつ遅れて滑り込ませる。傾きもカードごとにわずかに変え、
            // 定規で引いたように揃いすぎて見えないようにする
            //-------------------------------------------------------------
            Tween::SlideIn(registry, card, hlslpp::float2(-70.0f, 0.0f), layout->shopStagger * static_cast<float>(i), 0.5f);
            HudUi::PlayTo(registry, card, UITweenProperty::Rotation, 0.0f, tilt(m_random), 0.6f, EaseType::OutCubic, HudUi::Tag::kTilt);
        }
    }

    //-------------------------------------------------------------
    // カードを作り、見た目を更新する
    //-------------------------------------------------------------
    void ShopViewSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const auto                      purchases = m_purchases.Drain();
        const HudLayoutConfigComponent* layout    = FindSingleton<HudLayoutConfigComponent>(registry);
        const WalletComponent*          wallet    = FindSingleton<WalletComponent>(registry);
        const UpgradeLevelsComponent*   levels    = FindSingleton<UpgradeLevelsComponent>(registry);
        if(!layout || !wallet || !levels)
            return;

        const LauncherConfigComponent* launcher  = FindSingleton<LauncherConfigComponent>(registry);
        const auto&                    upgrades  = Data::GetUpgradeTable().upgrades;
        size_t                         cardCount = 0;
        for([[maybe_unused]] const auto entity : registry.View<ShopCardComponent>()) {
            ++cardCount;
        }
        if(cardCount != upgrades.size())
            Rebuild(registry);

        for(const auto card : registry.View<ShopCardComponent>()) {
            ShopCardComponent& component = registry.GetComponent<ShopCardComponent>(card);
            if(component.index < 0 || component.index >= static_cast<int>(upgrades.size()))
                continue;

            const Data::UpgradeDef& upgrade = upgrades[static_cast<size_t>(component.index)];
            const auto              found   = levels->levels.find(upgrade.id);
            const int               level   = found == levels->levels.end() ? 0 : found->second;
            const bool              maxed   = Core::UpgradeFormula::IsMaxed(level, upgrade.maxLevel);
            const Core::BigNumber   cost    = upgrade.CostAt(level);
            const CardState         state   = maxed ? CardState::Maxed : (wallet->stardust >= cost ? CardState::Affordable : CardState::Unaffordable);

            const Tsukino::ECS::Entity costText = HudUi::FindPart(registry, card, "Cost");
            HudUi::SetText(registry, HudUi::FindPart(registry, card, "Level"), "Lv" + std::to_string(level));
            HudUi::SetText(registry, costText, maxed ? "MAX" : cost.ToJapaneseString());
            HudUi::SetText(
                registry,
                HudUi::FindPart(registry, card, "Effect"),
                upgrade.effect == "autoFire"
                    ? (level > 0 ? "発射間隔 " + UpgradeText::Interval(upgrade, level, launcher ? launcher->autoFireInterval : 0.0f) : upgrade.description)
                    : (level > 0 ? upgrade.description + " " + UpgradeText::Effect(upgrade, level) : upgrade.description));

            if(static_cast<int>(state) == component.state)
                continue;

            //-------------------------------------------------------------
            // 状態が変わったときだけ見た目を切り替える（毎フレームトゥイーンを掛け直さない）
            //-------------------------------------------------------------
            const bool                 firstTime = component.state < 0;
            const Tsukino::ECS::Entity frame     = HudUi::FindPart(registry, card, "Frame");
            const float                fromAlpha = component.state == static_cast<int>(CardState::Unaffordable) ? layout->unaffordableAlpha : 1.0f;
            component.state                      = static_cast<int>(state);

            switch(state) {
            case CardState::Affordable:
                {
                    HudUi::SetTextColor(registry, costText, layout->affordableColor);
                    HudUi::PlayTo(registry, card, UITweenProperty::Alpha, fromAlpha, 1.0f, 0.25f, EaseType::OutQuad, HudUi::Tag::kDim);
                    // 縁がゆっくり呼吸するように明滅する。周期はカードごとに変える
                    std::uniform_real_distribution<float> period(1.5f, 2.3f);
                    HudUi::PlayPingPong(registry, frame, UITweenProperty::Alpha, 1.0f, 0.35f, period(m_random), 0.0f, HudUi::Tag::kBreath);
                    if(!firstTime)
                        Tween::Punch(registry, HudUi::FindPart(registry, card, "KeyBadge"), 0.35f, 0.4f);
                    break;
                }
            case CardState::Unaffordable:
                HudUi::SetTextColor(registry, costText, layout->unaffordableColor);
                HudUi::PlayTo(registry, card, UITweenProperty::Alpha, fromAlpha, layout->unaffordableAlpha, 0.25f, EaseType::OutQuad, HudUi::Tag::kDim);
                Tween::Stop(registry, frame, HudUi::Tag::kBreath);
                HudUi::PlayTo(registry, frame, UITweenProperty::Alpha, 1.0f, 0.25f, 0.2f, EaseType::OutQuad, HudUi::Tag::kBreath);
                break;
            case CardState::Maxed:
                HudUi::SetTextColor(registry, costText, layout->maxedColor);
                HudUi::PlayTo(registry, card, UITweenProperty::Alpha, fromAlpha, 0.92f, 0.25f, EaseType::OutQuad, HudUi::Tag::kDim);
                Tween::Stop(registry, frame, HudUi::Tag::kBreath);
                HudUi::SetTint(registry, frame, layout->maxedColor);
                break;
            }
        }

        //-------------------------------------------------------------
        // 買ったカードを弾ませる
        //-------------------------------------------------------------
        for(const UpgradePurchasedEvent& purchase : purchases) {
            for(const auto card : registry.View<ShopCardComponent>()) {
                if(registry.GetComponent<ShopCardComponent>(card).upgradeId != purchase.upgradeId)
                    continue;
                Tween::Punch(registry, card, 0.07f, 0.4f);
                Tween::Punch(registry, HudUi::FindPart(registry, card, "Level"), 0.5f, 0.45f);
            }
        }
    }

}    // namespace StardustFever::ECS
