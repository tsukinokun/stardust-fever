//-------------------------------------------------------------
//! @file   BallHitPinEvent.hpp
//! @brief  BallHitPinEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Entity/Entity.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BallHitPinEvent
    //! @brief  玉が釘に当たった
    //! @note   BallEventSystem が衝突から振り分けて発行する
    //-------------------------------------------------------------
    struct BallHitPinEvent {
        Tsukino::ECS::Entity ball;    //!< 当たった玉
        Tsukino::ECS::Entity pin;     //!< 当たった釘
    };
}    // namespace StardustFever::ECS
