//-------------------------------------------------------------
//! @file   LotterySystem.hpp
//! @brief  黄道十二宮リールの抽選を行う System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BallEnteredPocketEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  LotterySystem
    //! @brief  スタート入賞で保留を貯め、保留を1つずつ消化してリールを回す
    //! @note   当否はリールが回り始めた時点で決め、止まったら LotteryResultEvent を出す。
    //!         フィーバー中は保留を貯めるだけで回さない
    //-------------------------------------------------------------
    class LotterySystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 入賞の購読元であり、抽選結果の発行先
        //-------------------------------------------------------------
        explicit LotterySystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  保留を貯め、リールを進める
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus*            m_eventBus = nullptr;
        EventQueue<BallEnteredPocketEvent> m_pocketEntries;
        std::mt19937                       m_random;
    };
}    // namespace StardustFever::ECS
