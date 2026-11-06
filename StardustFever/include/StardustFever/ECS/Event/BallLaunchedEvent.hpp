//-------------------------------------------------------------
//! @file   BallLaunchedEvent.hpp
//! @brief  BallLaunchedEvent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BallLaunchedEvent
    //! @brief  玉を打ち出した
    //-------------------------------------------------------------
    struct BallLaunchedEvent {
        bool launched = true;    //!< 空型にしないための値
    };
}    // namespace StardustFever::ECS
