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
    //! @note   1球で何個の星をつないだかを持ち、一筆書きの倍率に使う
    //-------------------------------------------------------------
    struct BallComponent {
        int  linkedStarCount = 0;        //!< このフィーバー中にこの玉がつないだ星の数
        bool consumed        = false;    //!< 入賞・アウト済み（破棄待ちの間に届いた接触を無視するため）
    };
}    // namespace StardustFever::ECS
