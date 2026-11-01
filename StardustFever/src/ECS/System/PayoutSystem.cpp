//-------------------------------------------------------------
//! @file   PayoutSystem.cpp
//! @brief  PayoutSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/PayoutSystem.hpp>

#include <StardustFever/ECS/Component/PayoutConfigComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    PayoutSystem::PayoutSystem(Tsukino::ECS::EventBus& eventBus)
        : m_pinHits(eventBus)
        , m_pocketEntries(eventBus)
        , m_earnings(eventBus) {
    }

    //-------------------------------------------------------------
    // 溜まった獲得を財布へ入れる
    //-------------------------------------------------------------
    void PayoutSystem::Update(Tsukino::ECS::Registry& registry, float) {
        // 財布が無いときも溜めた分は捨てる（次のフレームへ持ち越すと二重に払う恐れがある）
        const auto pinHits       = m_pinHits.Drain();
        const auto pocketEntries = m_pocketEntries.Drain();
        const auto earnings      = m_earnings.Drain();

        WalletComponent*             wallet = FindSingleton<WalletComponent>(registry);
        const PayoutConfigComponent* config = FindSingleton<PayoutConfigComponent>(registry);
        if(!wallet || !config)
            return;

        Core::BigNumber baseTotal;
        baseTotal += Core::BigNumber(config->pinHitPayout * static_cast<double>(pinHits.size()));
        for(const BallEnteredPocketEvent& entry : pocketEntries) {
            if(const PocketComponent* pocket = registry.try_get<PocketComponent>(entry.pocket))
                baseTotal += Core::BigNumber(pocket->basePayout);
        }
        for(const StardustEarnedEvent& earned : earnings) {
            baseTotal += earned.baseAmount;
        }
        if(baseTotal.IsZero())
            return;

        const ScoreMultiplierComponent* multiplier  = FindSingleton<ScoreMultiplierComponent>(registry);
        wallet->stardust                           += multiplier ? baseTotal * multiplier->total : baseTotal;
    }

}    // namespace StardustFever::ECS
