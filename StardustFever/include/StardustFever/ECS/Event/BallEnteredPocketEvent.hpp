//-------------------------------------------------------------
//! @file   BallEnteredPocketEvent.hpp
//! @brief  BallEnteredPocketEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Entity/Entity.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BallEnteredPocketEvent
    //! @brief  玉が入賞口に入った
    //! @note   入った玉はこの後すぐ破棄される
    //-------------------------------------------------------------
    struct BallEnteredPocketEvent {
        Tsukino::ECS::Entity ball;      //!< 入った玉
        Tsukino::ECS::Entity pocket;    //!< 入賞口
    };
}    // namespace StardustFever::ECS
