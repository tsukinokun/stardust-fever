//-------------------------------------------------------------
//! @file   HudIdleMotionComponent.hpp
//! @brief  HudIdleMotionComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudIdleMotionComponent
    //! @brief  待機中のゆらぎ
    //! @note   HudIdleMotionSystem が周期と位相を乱数で決めてトゥイーンを付ける。パネルごとに揺れがずれ、揃って動かない
    //-------------------------------------------------------------
    struct HudIdleMotionComponent {
        float floatAmplitude = 3.0f;      //!< 上下に漂う幅（UI の座標）
        float scaleAmplitude = 0.012f;    //!< 呼吸するような拡大の幅
        float alphaAmplitude = 0.0f;      //!< 明滅の幅（0 なら明滅しない）
        float periodMin      = 2.6f;      //!< 周期の最小（秒）
        float periodMax      = 4.4f;      //!< 周期の最大（秒）
        bool  started        = false;     //!< トゥイーンを付けたか
    };
}    // namespace StardustFever::ECS
