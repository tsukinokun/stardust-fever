//-------------------------------------------------------------
//! @file   LotterySystem.cpp
//! @brief  LotterySystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/LotterySystem.hpp>

#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LotteryConfigComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Event/LotteryResultEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        //-------------------------------------------------------------
        // 左と中のリールが止まる時刻（回転時間に対する割合）。右は変動の終わりに止まる。
        // 左から順に止めることで、最後の1つを待つ「リーチ」の間が生まれる
        //-------------------------------------------------------------
        constexpr float kReelStopRatios[2] = {0.45f, 0.7f};
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
        const auto entries = m_pocketEntries.Drain();
        // 星図の画面を開いている間は止める
        if(IsPlayPaused(registry))
            return;
        LotteryStateComponent*        state  = FindSingleton<LotteryStateComponent>(registry);
        const LotteryConfigComponent* config = FindSingleton<LotteryConfigComponent>(registry);
        if(!state || !config)
            return;

        for(const BallEnteredPocketEvent& entry : entries) {
            const PocketComponent* pocket = registry.try_get<PocketComponent>(entry.pocket);
            if(pocket && pocket->role == "start")
                state->pending = std::min(state->pending + 1, config->maxPending);
        }

        const int symbolCount = static_cast<int>(config->symbols.size());
        if(symbolCount == 0)
            return;
        std::uniform_int_distribution<int> pickSymbol(0, symbolCount - 1);

        const FeverStateComponent* fever       = FindSingleton<FeverStateComponent>(registry);
        const bool                 feverActive = fever && fever->active;
        // 星ゲットチャンスの間もリールは待つ（同時に始まると、どちらの演出も見えにくくなる）
        const StarChanceStateComponent* chance       = FindSingleton<StarChanceStateComponent>(registry);
        const bool                      chanceActive = chance && chance->active;

        //-------------------------------------------------------------
        // 回り始め。当否と止まる絵柄をここで決めておく
        //-------------------------------------------------------------
        if(!state->spinning && state->pending > 0 && !feverActive && !chanceActive) {
            --state->pending;
            state->spinning                                = true;
            state->symbolTimer                             = 0.0f;
            const UpgradeModifiersComponent*    upgrades   = FindSingleton<UpgradeModifiersComponent>(registry);
            const CollectionModifiersComponent* collection = FindSingleton<CollectionModifiersComponent>(registry);
            const double                        winProbability =
                config->winProbability + (upgrades ? upgrades->lotteryChanceBonus : 0.0) + (collection ? collection->lotteryChanceBonus : 0.0);
            state->willWin = std::bernoulli_distribution(std::clamp(winProbability, 0.0, 1.0))(m_random);

            if(state->willWin) {
                const int symbol    = pickSymbol(m_random);
                state->finalSymbols = {symbol, symbol, symbol};
            } else {
                state->finalSymbols = {pickSymbol(m_random), pickSymbol(m_random), pickSymbol(m_random)};
                // 外れなのに3つ揃ってしまったら、右のリールを1つずらす（絵柄が1種類なら揃うしかない）
                if(symbolCount > 1 && state->finalSymbols[0] == state->finalSymbols[1] && state->finalSymbols[1] == state->finalSymbols[2])
                    state->finalSymbols[2] = (state->finalSymbols[2] + 1) % symbolCount;
            }

            // 左と中がそろうならリーチ。右のリールを長く回して、BGM と演出の間を作る
            const bool reach = state->finalSymbols[0] == state->finalSymbols[1];
            state->spinTotal = config->spinDuration + (reach ? config->reachExtraDuration : 0.0f);
            state->spinTimer = state->spinTotal;
        }

        if(!state->spinning)
            return;

        state->spinTimer    -= deltaTime;
        state->symbolTimer  -= deltaTime;
        const float elapsed  = state->spinTotal - state->spinTimer;
        const bool  advance  = state->symbolTimer <= 0.0f;
        if(advance)
            state->symbolTimer = config->symbolInterval;

        for(int reel = 0; reel < 3; ++reel) {
            const float stopAt = reel < 2 ? kReelStopRatios[reel] * config->spinDuration : state->spinTotal;
            if(elapsed >= stopAt) {
                state->symbols[reel]     = state->finalSymbols[reel];
                state->reelStopped[reel] = true;
            } else {
                state->reelStopped[reel] = false;
                if(advance)
                    state->symbols[reel] = (state->symbols[reel] + 1) % symbolCount;
            }
        }

        if(state->spinTimer <= 0.0f) {
            state->spinning    = false;
            state->symbols     = state->finalSymbols;
            state->reelStopped = {true, true, true};
            m_eventBus->Publish(LotteryResultEvent{state->willWin});
        }
    }

}    // namespace StardustFever::ECS
