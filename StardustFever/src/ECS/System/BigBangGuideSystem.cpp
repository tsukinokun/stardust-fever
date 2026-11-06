//-------------------------------------------------------------
//! @file   BigBangGuideSystem.cpp
//! @brief  BigBangGuideSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BigBangGuideSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/HudTextComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/UiActionComponent.hpp>
#include <StardustFever/ECS/Event/BigBangReadyEvent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>
#include <Tsukino/Core/ECS/Event/EventBus.hpp>

#include <array>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    BigBangGuideSystem::BigBangGuideSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus) {
    }

    //-------------------------------------------------------------
    // ボタンと案内文の見た目を切り替える
    //-------------------------------------------------------------
    void BigBangGuideSystem::SetActive(Tsukino::ECS::Registry& registry, bool active) {
        const PrestigeConfigComponent* config = FindSingleton<PrestigeConfigComponent>(registry);
        if(!config)
            return;

        // 弾ませるのは、文字の付いたボタンと、下の操作の案内
        entt::entity button = entt::null;
        for(const auto entity : registry.View<UiActionComponent>()) {
            const UiActionComponent& action = registry.GetComponent<UiActionComponent>(entity);
            if(action.action == UiAction::BigBang && action.label != entt::null)
                button = entity;
        }
        const std::array<entt::entity, 2> targets = {button, HudUi::FindAnyPart(registry, "Chip3")};
        for(const entt::entity target : targets) {
            if(target == entt::null)
                continue;
            if(active)
                HudUi::PlayPingPong(registry,
                                    target,
                                    Tsukino::BuiltIn::ECS::UITweenProperty::Scale,
                                    1.0f,
                                    config->readyPulseScale,
                                    config->readyPulsePeriod,
                                    0.0f,
                                    HudUi::Tag::kPulse);
            else
                Tsukino::BuiltIn::ECS::UITweenUtility::Stop(registry, target, HudUi::Tag::kPulse);
        }

        for(const auto entity : registry.View<HudTextComponent>()) {
            if(registry.GetComponent<HudTextComponent>(entity).kind != HudTextKind::PrestigeHint)
                continue;
            if(!m_hasColor) {
                m_hintColor = registry.GetComponent<Tsukino::BuiltIn::ECS::FontComponent>(entity).color;
                m_hasColor  = true;
            }
            HudUi::SetTextColor(registry, entity, active ? config->readyHintColor : m_hintColor);
        }
    }

    //-------------------------------------------------------------
    // できるかどうかを見て、案内を出す
    //-------------------------------------------------------------
    void BigBangGuideSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const PrestigeStateComponent*  prestige = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
        if(!prestige || !config)
            return;

        const Core::BigNumber           gain   = Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);
        const FeverStateComponent*      fever  = FindSingleton<FeverStateComponent>(registry);
        const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry);
        const bool                      ready  = !gain.IsZero();
        const bool                      active = ready && !(fever && fever->active) && !(chance && chance->active);

        if(active != m_active) {
            SetActive(registry, active);
            m_active = active;
        }

        // ビッグバンで獲得が0へ戻ると、次の宇宙でまた知らせる
        if(!ready)
            m_announced = false;
        if(active && !m_announced && !IsPlayPaused(registry)) {
            m_announced = true;
            m_eventBus->Publish(HudMessageEvent{"★ ビッグバンできます ★\n宇宙定数 +" + gain.ToJapaneseString() + "  左下のボタンか B キーで",
                                                HudMessageImportance::Headline,
                                                HudMessageTone::Positive});
            m_eventBus->Publish(BigBangReadyEvent{});
        }
    }

}    // namespace StardustFever::ECS
