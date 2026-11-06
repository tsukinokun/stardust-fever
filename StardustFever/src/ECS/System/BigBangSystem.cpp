//-------------------------------------------------------------
//! @file   BigBangSystem.cpp
//! @brief  BigBangSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BigBangSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/Core/StarDraw.hpp>
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Event/BigBangEvent.hpp>
#include <StardustFever/ECS/Event/SkyObjectDrawnEvent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/SkyObjectOdds.hpp>

#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>
#include <Tsukino/Core/Input/KeyCodes.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    BigBangSystem::BigBangSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_actions(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 入力を見てビッグバンを行う
    //-------------------------------------------------------------
    void BigBangSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto                                 actions  = m_actions.Drain();
        PrestigeStateComponent*                    prestige = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent*             config   = FindSingleton<PrestigeConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context  = GetEngineContext(registry);
        if(!prestige || !config || !context || !context->inputSystem)
            return;

        using Tsukino::Input::KeyCode;
        const bool keyPressed = context->inputSystem->IsKeyPressed(KeyCode::B);
        bool       open       = keyPressed;
        bool       confirm    = keyPressed;
        bool       cancel     = context->inputSystem->IsKeyPressed(KeyCode::Escape);
        for(const UiActionEvent& action : actions) {
            open    = open || action.action == UiAction::BigBang;
            confirm = confirm || action.action == UiAction::BigBangConfirm;
            cancel  = cancel || action.action == UiAction::BigBangCancel;
        }

        //-------------------------------------------------------------
        // 確認画面を開いている間は、実行かやめるかだけを受ける
        //-------------------------------------------------------------
        if(prestige->confirmOpen) {
            if(confirm)
                Execute(registry);
            else if(cancel)
                prestige->confirmOpen = false;
            return;
        }

        // 星図や設定の画面を開いている間は止める
        if(IsPlayPaused(registry) || !open)
            return;

        const Core::BigNumber gain = Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);
        if(gain.IsZero()) {
            const Core::BigNumber needed = Core::PrestigeFormula::EarnedForNextConstant(gain, config->threshold, config->exponent);
            m_eventBus->Publish(HudMessageEvent{"まだビッグバンできません  あと " + (needed - prestige->universeEarned).ToJapaneseString() + " 稼ぐ",
                                                HudMessageImportance::Toast,
                                                HudMessageTone::Warning});
            return;
        }

        if(const FeverStateComponent* fever = FindSingleton<FeverStateComponent>(registry); fever && fever->active) {
            m_eventBus->Publish(HudMessageEvent{"星詠みタイム中はビッグバンできません", HudMessageImportance::Toast, HudMessageTone::Warning});
            return;
        }
        if(const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry); chance && chance->active) {
            m_eventBus->Publish(HudMessageEvent{"星ゲットチャンス中はビッグバンできません", HudMessageImportance::Toast, HudMessageTone::Warning});
            return;
        }

        prestige->confirmOpen = true;
    }

    //-------------------------------------------------------------
    // ビッグバンを行う
    //-------------------------------------------------------------
    void BigBangSystem::Execute(Tsukino::ECS::Registry& registry) {
        PrestigeStateComponent*        prestige = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
        if(!prestige || !config)
            return;
        const Core::BigNumber gain = Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);

        DrawSkyObject(registry);

        //-------------------------------------------------------------
        // 実行。星屑・強化・この宇宙の累計・抽選の保留・盤面の玉を捨てる。
        // 星のレベルと星座の完成回数（星図）は図鑑として残す
        //-------------------------------------------------------------
        prestige->confirmOpen      = false;
        prestige->cosmicConstants += gain;
        prestige->universeEarned   = Core::BigNumber(0.0);
        ++prestige->bigBangCount;

        if(WalletComponent* wallet = FindSingleton<WalletComponent>(registry))
            wallet->stardust = Core::BigNumber(0.0);
        if(UpgradeLevelsComponent* upgrades = FindSingleton<UpgradeLevelsComponent>(registry))
            upgrades->levels.clear();
        if(LotteryStateComponent* lottery = FindSingleton<LotteryStateComponent>(registry)) {
            lottery->pending  = 0;
            lottery->spinning = false;
        }
        for(const auto ball : registry.View<BallComponent>()) {
            registry.GetComponent<BallComponent>(ball).consumed = true;
            registry.QueueDestroy(ball);
        }

        m_eventBus->Publish(BigBangEvent{gain, prestige->cosmicConstants});
        m_eventBus->Publish(
            HudMessageEvent{"★ ビッグバン ★\n宇宙定数 +" + gain.ToJapaneseString() + "（合計 " + prestige->cosmicConstants.ToJapaneseString() + "）",
                            HudMessageImportance::Headline});
    }

    //-------------------------------------------------------------
    // 天体を1つ引いて持ち物へ加える
    //-------------------------------------------------------------
    void BigBangSystem::DrawSkyObject(Tsukino::ECS::Registry& registry) {
        const PrestigeStateComponent*   prestige   = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent*  config     = FindSingleton<PrestigeConfigComponent>(registry);
        const SkyObjectConfigComponent* objects    = FindSingleton<SkyObjectConfigComponent>(registry);
        StarCollectionComponent*        collection = FindSingleton<StarCollectionComponent>(registry);
        if(!prestige || !config || !objects || !collection)
            return;

        // レア度を引いてから、その中の未入手を等確率で選ぶ。全部持っていれば何も出ない
        std::uniform_real_distribution<double> unit(0.0, 1.0);
        const SkyObjectOdds                    odds = ComputeSkyObjectOdds(*objects, *collection, prestige->universeEarned, config->threshold);
        const size_t                           tier = Core::StarDraw::PickIndex(odds.rates, unit(m_random));
        if(tier >= odds.rates.size())
            return;

        std::vector<const Data::SkyObjectDef*> candidates;
        for(const Data::SkyObjectDef& object : Data::GetSkyObjectTable().objects) {
            if(object.rarity == static_cast<int>(tier) && !OwnsSkyObject(*collection, object.id))
                candidates.push_back(&object);
        }
        if(candidates.empty())
            return;

        const Data::SkyObjectDef& drawn = *candidates[std::uniform_int_distribution<size_t>(0, candidates.size() - 1)(m_random)];
        collection->skyObjects.push_back(drawn.id);
        collection->unseenObjects.push_back(drawn.id);
        m_eventBus->Publish(SkyObjectDrawnEvent{drawn.id});
    }

}    // namespace StardustFever::ECS
