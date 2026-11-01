//-------------------------------------------------------------
//! @file   StardustEarnedEvent.hpp
//! @brief  StardustEarnedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StardustEarnedEvent
    //! @brief  星屑を得た
    //! @note   倍率を掛ける前の量。PayoutSystem が全体倍率を掛けて財布へ入れる
    //-------------------------------------------------------------
    struct StardustEarnedEvent {
        StardustFever::Core::BigNumber baseAmount;    //!< 倍率を掛ける前の量
    };
}    // namespace StardustFever::ECS
