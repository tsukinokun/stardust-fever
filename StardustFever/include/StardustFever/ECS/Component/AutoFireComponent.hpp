//-------------------------------------------------------------
//! @file   AutoFireComponent.hpp
//! @brief  AutoFireComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct AutoFireComponent
    //! @brief  自動発射
    //! @note   強化で解禁する。打ち続けてくれるが強さがランダムに揺らぐ
    //-------------------------------------------------------------
    struct AutoFireComponent {
        bool unlocked = false;    //!< 解禁済みか
        bool enabled  = false;    //!< 自動発射中か
    };
}    // namespace StardustFever::ECS
