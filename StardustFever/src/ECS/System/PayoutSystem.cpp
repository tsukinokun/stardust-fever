//-------------------------------------------------------------
//! @file   PayoutSystem.cpp
//! @brief  PayoutSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/PayoutSystem.hpp>

#include <StardustFever/ECS/Component/PayoutConfigComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
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

        // 強化の倍率は釘・入賞口それぞれの払い出しにだけ掛ける（星つなぎの獲得は対象外）
        const UpgradeModifiersComponent* upgrades         = FindSingleton<UpgradeModifiersComponent>(registry);
        const double                     pinMultiplier    = upgrades ? upgrades->pinPayoutMultiplier : 1.0;
        const double                     pocketMultiplier = upgrades ? upgrades->pocketPayoutMultiplier : 1.0;

        Core::BigNumber baseTotal;
        baseTotal += Core::BigNumber(config->pinHitPayout * static_cast<double>(pinHits.size())) * Core::BigNumber(pinMultiplier);
        for(const BallEnteredPocketEvent& entry : pocketEntries) {
            if(const PocketComponent* pocket = registry.try_get<PocketComponent>(entry.pocket))
                baseTotal += Core::BigNumber(pocket->basePayout) * Core::BigNumber(pocketMultiplier);
        }
        for(const StardustEarnedEvent& earned : earnings) {
            baseTotal += earned.baseAmount;
        }
        if(baseTotal.IsZero())
            return;

        const ScoreMultiplierComponent* multiplier  = FindSingleton<ScoreMultiplierComponent>(registry);
        const Core::BigNumber           credited    = multiplier ? baseTotal * multiplier->total : baseTotal;
        wallet->stardust                           += credited;

        // ビッグバンの獲得量は「この宇宙で稼いだ累計」で決まる。使っても減らないよう財布とは別に数える
        if(PrestigeStateComponent* prestige = FindSingleton<PrestigeStateComponent>(registry))
            prestige->universeEarned += credited;
    }

}    // namespace StardustFever::ECS
