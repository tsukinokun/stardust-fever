//-------------------------------------------------------------
//! @file   BallComponent.hpp
//! @brief  BallComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BallComponent
    //! @brief  盤面を転がる玉
    //! @note   入賞・アウトしたかを持つ
    //-------------------------------------------------------------
    struct BallComponent {
        bool consumed = false;    //!< 入賞・アウト済み（破棄待ちの間に届いた接触を無視するため）
    };
}    // namespace StardustFever::ECS
