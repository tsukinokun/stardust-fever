//-------------------------------------------------------------
//! @file   LauncherComponent.hpp
//! @brief  LauncherComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LauncherComponent
    //! @brief  発射台
    //! @note   Space を押している間は強さが 0→1→0 と往復し、離した瞬間の強さで1球打つ。
    //!         発射位置は TransformComponent
    //-------------------------------------------------------------
    struct LauncherComponent {
        float handlePower   = 0.5f;     //!< 強さ（0〜1）。溜めている間は往復し、離した後は最後の強さを残す（自動発射の基準にもなる）
        float cooldown      = 0.0f;     //!< 次に打てるまでの残り時間（秒）
        bool  charging      = false;    //!< Space を押して溜めているか（LauncherInputSystem が書く）
        float chargeTime    = 0.0f;     //!< 溜め始めてからの時間（秒）
        bool  fireRequested = false;    //!< 離して打つ要求。打てるようになるまで残し、打ったら LaunchSystem が下ろす
    };
}    // namespace StardustFever::ECS
