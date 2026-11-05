//-------------------------------------------------------------
//! @file   LotteryViewSystem.hpp
//! @brief  抽選リールと保留の見た目を動かす System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/LotteryResultEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <array>
#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  LotteryViewSystem
    //! @brief  回っているリールを震わせ、止まった瞬間に弾ませる。
    //!         2つ揃って最後の1つを待つ「リーチ」では枠を脈打たせ、当たれば枠の光を弾けさせる。
    //!         保留の玉は、保留があるものだけ明るくし、増えた瞬間に弾ませる
    //! @note   絵柄の文字そのものは HudSystem が書く。ここは動きだけ
    //-------------------------------------------------------------
    class LotteryViewSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 抽選結果の購読元
        //-------------------------------------------------------------
        explicit LotteryViewSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  リールと保留の見た目を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        EventQueue<LotteryResultEvent> m_results;
        std::array<bool, 3>            m_wasStopped  = {true, true, true};    //!< 前のフレームに止まっていたか（止まった瞬間を拾う）
        bool                           m_wasReach    = false;                 //!< 前のフレームにリーチだったか
        bool                           m_initialized = false;                 //!< 枠の光を消しておいたか
        std::mt19937                   m_random;
    };
}    // namespace StardustFever::ECS
