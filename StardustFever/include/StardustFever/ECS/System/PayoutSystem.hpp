//-------------------------------------------------------------
//! @file   PayoutSystem.hpp
//! @brief  星屑を財布へ入れる System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BallEnteredPocketEvent.hpp>
#include <StardustFever/ECS/Event/BallHitPinEvent.hpp>
#include <StardustFever/ECS/Event/StardustEarnedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  PayoutSystem
    //! @brief  釘・入賞口・星つなぎで得た星屑に全体倍率を掛け、WalletComponent へ足す
    //! @note   財布へ足すのはこの System だけ。倍率の掛け忘れや二重掛けを防ぐため、
    //!         他の System は StardustEarnedEvent に「倍率を掛ける前の量」を載せて出す
    //-------------------------------------------------------------
    class PayoutSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 獲得イベントの購読元
        //-------------------------------------------------------------
        explicit PayoutSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  溜まった獲得を財布へ入れる
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus*            m_eventBus = nullptr;    //!< 入った量（StardustCreditedEvent）の発行先
        EventQueue<BallHitPinEvent>        m_pinHits;
        EventQueue<BallEnteredPocketEvent> m_pocketEntries;
        EventQueue<StardustEarnedEvent>    m_earnings;
    };
}    // namespace StardustFever::ECS
