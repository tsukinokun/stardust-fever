//-------------------------------------------------------------
//! @file   BigBangSystem.cpp
//! @brief  BigBangSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BigBangSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Event/BigBangEvent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>
#include <Tsukino/Core/Input/KeyCodes.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    BigBangSystem::BigBangSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus) {
    }

    //-------------------------------------------------------------
    // 入力を見てビッグバンを行う
    //-------------------------------------------------------------
    void BigBangSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        // 星図の画面を開いている間は止める
        if(IsPlayPaused(registry))
            return;
        PrestigeStateComponent*        prestige = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
        if(!prestige || !config)
            return;

        prestige->confirmTimer = std::max(0.0f, prestige->confirmTimer - deltaTime);

        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->inputSystem || !context->inputSystem->IsKeyPressed(Tsukino::Input::KeyCode::B))
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

        if(prestige->confirmTimer <= 0.0f) {
            prestige->confirmTimer = config->confirmWindow;
            m_eventBus->Publish(HudMessageEvent{"ビッグバン？  宇宙定数 +" + gain.ToJapaneseString() + "\n星屑と強化は消えます。もう一度 B で実行"});
            return;
        }

        //-------------------------------------------------------------
        // 実行。星屑・強化・この宇宙の累計・抽選の保留・盤面の玉を捨てる。
        // 星のレベルと星座の完成回数（星図）は図鑑として残す
        //-------------------------------------------------------------
        prestige->confirmTimer     = 0.0f;
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

}    // namespace StardustFever::ECS
