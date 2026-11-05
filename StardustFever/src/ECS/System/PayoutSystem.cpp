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
#include <StardustFever/ECS/Event/StardustCreditedEvent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Core/ECS/Event/EventBus.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        //-------------------------------------------------------------
        //! @brief  エンティティの位置を取る
        //! @param  registry [in]  レジストリ
        //! @param  entity   [in]  エンティティ
        //! @param  position [out] 位置
        //! @return 取れたか（破棄済み・Transform 無しなら false）
        //-------------------------------------------------------------
        bool TryGetPosition(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, hlslpp::float3& position) {
            if(!registry.IsValid(entity))
                return false;
            const auto* transform = registry.try_get<Tsukino::BuiltIn::ECS::TransformComponent>(entity);
            if(!transform)
                return false;
            position = transform->position;
            return true;
        }
    }    // namespace

    PayoutSystem::PayoutSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_pinHits(eventBus)
        , m_pocketEntries(eventBus)
        , m_earnings(eventBus) {
    }

    //-------------------------------------------------------------
    // 溜まった獲得を財布へ入れる
    //
    // 1件ずつ倍率を掛けて入れ、入れた量と場所を StardustCreditedEvent で知らせる。
    // まとめて足すと、浮かぶ数字が実際に入った額とずれるため
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

        const ScoreMultiplierComponent*  score            = FindSingleton<ScoreMultiplierComponent>(registry);
        const Core::BigNumber            total            = score ? score->total : Core::BigNumber(1.0);
        const UpgradeModifiersComponent* upgrades         = FindSingleton<UpgradeModifiersComponent>(registry);
        PrestigeStateComponent*          prestige         = FindSingleton<PrestigeStateComponent>(registry);
        const double                     pinMultiplier    = upgrades ? upgrades->pinPayoutMultiplier : 1.0;
        const double                     pocketMultiplier = upgrades ? upgrades->pocketPayoutMultiplier : 1.0;

        auto credit = [&](const Core::BigNumber& base, StardustSource source, bool hasPosition, const hlslpp::float3& position) {
            if(base.IsZero())
                return;
            const Core::BigNumber amount  = base * total;
            wallet->stardust             += amount;
            // ビッグバンの獲得量は「この宇宙で稼いだ累計」で決まる。使っても減らないよう財布とは別に数える
            if(prestige)
                prestige->universeEarned += amount;
            m_eventBus->Publish(StardustCreditedEvent{amount, source, position, hasPosition});
        };

        // 強化の倍率は釘・入賞口それぞれの払い出しにだけ掛ける（星つなぎの獲得は対象外）
        for(const BallHitPinEvent& hit : pinHits) {
            hlslpp::float3 position(0.0f, 0.0f, 0.0f);
            const bool     hasPosition = TryGetPosition(registry, hit.ball, position);
            credit(Core::BigNumber(config->pinHitPayout * pinMultiplier), StardustSource::Pin, hasPosition, position);
        }
        for(const BallEnteredPocketEvent& entry : pocketEntries) {
            const PocketComponent* pocket = registry.try_get<PocketComponent>(entry.pocket);
            if(!pocket)
                continue;
            hlslpp::float3 position(0.0f, 0.0f, 0.0f);
            const bool     hasPosition = TryGetPosition(registry, entry.pocket, position);
            credit(Core::BigNumber(pocket->basePayout * pocketMultiplier), StardustSource::Pocket, hasPosition, position);
        }
        for(const StardustEarnedEvent& earned : earnings) {
            credit(earned.baseAmount, earned.hasPosition ? StardustSource::Star : StardustSource::Bonus, earned.hasPosition, earned.position);
        }
    }

}    // namespace StardustFever::ECS
