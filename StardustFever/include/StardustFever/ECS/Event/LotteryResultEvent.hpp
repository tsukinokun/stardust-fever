//-------------------------------------------------------------
//! @file   LotteryResultEvent.hpp
//! @brief  LotteryResultEvent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LotteryResultEvent
    //! @brief  抽選リールが止まった
    //-------------------------------------------------------------
    struct LotteryResultEvent {
        bool won;    //!< 当たりか
    };
}    // namespace StardustFever::ECS
