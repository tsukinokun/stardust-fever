//-------------------------------------------------------------
//! @file   HudIdleMotionSystem.hpp
//! @brief  待機中の HUD をゆらがせる System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  HudIdleMotionSystem
    //! @brief  待機中の HUD をゆらがせる System
    //! @note   HudIdleMotionComponent を初めて見たときに、周期と位相を乱数で決めた往復のトゥイーンを付ける。
    //!         パネルごとに揺れがずれるので、画面全体が揃って動く機械的な見た目にならない
    //-------------------------------------------------------------
    class HudIdleMotionSystem final : public Tsukino::ECS::ISystem {
    public:
        HudIdleMotionSystem();

        //-------------------------------------------------------------
        //! @brief  更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        std::mt19937 m_random;    //!< 見た目のばらつき用
    };
}    // namespace StardustFever::ECS
