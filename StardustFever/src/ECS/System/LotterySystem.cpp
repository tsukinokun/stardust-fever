//-------------------------------------------------------------
//! @file   LotterySystem.cpp
//! @brief  LotterySystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/LotterySystem.hpp>

#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LotteryConfigComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Event/LotteryResultEvent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        //-------------------------------------------------------------
        // 左・中・右のリールが止まる時刻（回転時間に対する割合）。
        // 左から順に止めることで、最後の1つを待つ「リーチ」の間が生まれる
        //-------------------------------------------------------------
        constexpr float kReelStopRatios[3] = {0.45f, 0.7f, 1.0f};
    }    // namespace

    LotterySystem::LotterySystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_pocketEntries(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 保留を貯め、リールを進める
    //-------------------------------------------------------------
    void LotterySystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto                    entries = m_pocketEntries.Drain();
        LotteryStateComponent*        state   = FindSingleton<LotteryStateComponent>(registry);
        const LotteryConfigComponent* config  = FindSingleton<LotteryConfigComponent>(registry);
        if(!state || !config)
            return;

        for(const BallEnteredPocketEvent& entry : entries) {
            const PocketComponent* pocket = registry.try_get<PocketComponent>(entry.pocket);
            if(pocket && pocket->isStartPocket)
                state->pending = std::min(state->pending + 1, config->maxPending);
        }

        const int symbolCount = static_cast<int>(config->symbols.size());
        if(symbolCount == 0)
            return;
        std::uniform_int_distribution<int> pickSymbol(0, symbolCount - 1);

        const FeverStateComponent* fever       = FindSingleton<FeverStateComponent>(registry);
        const bool                 feverActive = fever && fever->active;

        //-------------------------------------------------------------
        // 回り始め。当否と止まる絵柄をここで決めておく
        //-------------------------------------------------------------
        if(!state->spinning && state->pending > 0 && !feverActive) {
            --state->pending;
            state->spinning    = true;
            state->spinTimer   = config->spinDuration;
            state->symbolTimer = 0.0f;
            state->willWin     = std::bernoulli_distribution(std::clamp(config->winProbability, 0.0, 1.0))(m_random);

            if(state->willWin) {
                const int symbol    = pickSymbol(m_random);
                state->finalSymbols = {symbol, symbol, symbol};
            } else {
                state->finalSymbols = {pickSymbol(m_random), pickSymbol(m_random), pickSymbol(m_random)};
                // 外れなのに3つ揃ってしまったら、右のリールを1つずらす（絵柄が1種類なら揃うしかない）
                if(symbolCount > 1 && state->finalSymbols[0] == state->finalSymbols[1] && state->finalSymbols[1] == state->finalSymbols[2])
                    state->finalSymbols[2] = (state->finalSymbols[2] + 1) % symbolCount;
            }
        }

        if(!state->spinning)
            return;

        state->spinTimer         -= deltaTime;
        state->symbolTimer       -= deltaTime;
        const float elapsedRatio  = config->spinDuration > 0.0f ? 1.0f - state->spinTimer / config->spinDuration : 1.0f;
        const bool  advance       = state->symbolTimer <= 0.0f;
        if(advance)
            state->symbolTimer = config->symbolInterval;

        for(int reel = 0; reel < 3; ++reel) {
            if(elapsedRatio >= kReelStopRatios[reel]) {
                state->symbols[reel] = state->finalSymbols[reel];
            } else if(advance) {
                state->symbols[reel] = (state->symbols[reel] + 1) % symbolCount;
            }
        }

        if(state->spinTimer <= 0.0f) {
            state->spinning = false;
            state->symbols  = state->finalSymbols;
            m_eventBus->Publish(LotteryResultEvent{state->willWin});
        }
    }

}    // namespace StardustFever::ECS
