//-------------------------------------------------------------
//! @file   BallTouchedStarEvent.hpp
//! @brief  BallTouchedStarEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Entity/Entity.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BallTouchedStarEvent
    //! @brief  玉が星釘に触れた
    //! @note   既につながっている星に触れたときも発行される
    //-------------------------------------------------------------
    struct BallTouchedStarEvent {
        Tsukino::ECS::Entity ball;       //!< 触れた玉
        Tsukino::ECS::Entity starPin;    //!< 触れた星釘
    };
}    // namespace StardustFever::ECS
