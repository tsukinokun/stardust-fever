//-------------------------------------------------------------
//! @file   StardustEarnedEvent.hpp
//! @brief  StardustEarnedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StardustEarnedEvent
    //! @brief  星屑を得た
    //! @note   倍率を掛ける前の量。PayoutSystem が全体倍率を掛けて財布へ入れる
    //-------------------------------------------------------------
    struct StardustEarnedEvent {
        StardustFever::Core::BigNumber baseAmount;                                        //!< 倍率を掛ける前の量
        hlslpp::float3                 position    = hlslpp::float3(0.0f, 0.0f, 0.0f);    //!< 得た場所（盤面のワールド座標）
        bool                           hasPosition = false;                               //!< position が意味を持つか（無ければ数字を浮かべない）
    };
}    // namespace StardustFever::ECS
