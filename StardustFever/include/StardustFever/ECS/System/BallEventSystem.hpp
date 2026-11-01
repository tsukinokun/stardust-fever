//-------------------------------------------------------------
//! @file   BallEventSystem.hpp
//! @brief  物理の衝突をゲームの出来事へ振り分ける System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/BuiltIn/ECS/Event/CollisionEnterEvent.hpp>
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  BallEventSystem
    //! @brief  玉と何かの CollisionEnterEvent を、釘・入賞口・アウト口・星釘ごとのイベントにする
    //! @note   入賞口とアウト口に入った玉、盤面の下へ落ちた玉はここで破棄する。
    //!         他の System は物理の衝突を直接見ず、ここが出すイベントだけを見る
    //-------------------------------------------------------------
    class BallEventSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 衝突の購読元であり、ゲームのイベントの発行先
        //-------------------------------------------------------------
        explicit BallEventSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  溜まった衝突を振り分ける
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus*                                m_eventBus = nullptr;
        EventQueue<Tsukino::BuiltIn::ECS::CollisionEnterEvent> m_collisions;
    };
}    // namespace StardustFever::ECS
