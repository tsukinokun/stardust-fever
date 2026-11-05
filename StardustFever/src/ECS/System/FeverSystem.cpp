//-------------------------------------------------------------
//! @file   FeverSystem.cpp
//! @brief  FeverSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/FeverSystem.hpp>

#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/FeverConfigComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Event/FeverEndedEvent.hpp>
#include <StardustFever/ECS/Event/FeverStartedEvent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <cstdio>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    FeverSystem::FeverSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_results(eventBus) {
    }

    //-------------------------------------------------------------
    // フィーバーを始める・進める・終える
    //-------------------------------------------------------------
    void FeverSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto results = m_results.Drain();
        // 星図の画面を開いている間は止める
        if(IsPlayPaused(registry))
            return;
        FeverStateComponent*        state  = FindSingleton<FeverStateComponent>(registry);
        const FeverConfigComponent* config = FindSingleton<FeverConfigComponent>(registry);
        if(!state || !config)
            return;

        for(const LotteryResultEvent& result : results) {
            if(result.won && !state->active)
                StartFever(registry);
        }

        if(!state->active)
            return;

        state->timeRemaining -= deltaTime;
        if(state->timeRemaining <= 0.0f)
            EndFever(registry);
    }

    //-------------------------------------------------------------
    // フィーバーを始める
    //-------------------------------------------------------------
    void FeverSystem::StartFever(Tsukino::ECS::Registry& registry) {
        FeverStateComponent*        state  = FindSingleton<FeverStateComponent>(registry);
        const FeverConfigComponent* config = FindSingleton<FeverConfigComponent>(registry);
        if(!state || !config)
            return;

        const UpgradeModifiersComponent*    upgrades   = FindSingleton<UpgradeModifiersComponent>(registry);
        const CollectionModifiersComponent* collection = FindSingleton<CollectionModifiersComponent>(registry);
        state->active                                  = true;
        state->timeRemaining = config->duration + (upgrades ? upgrades->feverDurationBonus : 0.0f) + (collection ? collection->feverDurationBonus : 0.0f);
        state->timeTotal     = state->timeRemaining;

        char sub[96];
        std::snprintf(sub, sizeof(sub), "釘 ×%g  入賞 ×%g  チューリップが開く", config->pinPayoutMultiplier, config->pocketPayoutMultiplier);
        m_eventBus->Publish(FeverStartedEvent{});
        m_eventBus->Publish(HudMessageEvent{std::string("★ 星詠みタイム ★\n") + sub, HudMessageImportance::Headline});
    }

    //-------------------------------------------------------------
    // フィーバーを終える
    //-------------------------------------------------------------
    void FeverSystem::EndFever(Tsukino::ECS::Registry& registry) {
        FeverStateComponent* state = FindSingleton<FeverStateComponent>(registry);
        if(!state)
            return;

        state->active = false;
        m_eventBus->Publish(FeverEndedEvent{});
        m_eventBus->Publish(HudMessageEvent{"星詠みタイム終了"});
    }

}    // namespace StardustFever::ECS
