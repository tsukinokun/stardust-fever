//-------------------------------------------------------------
//! @file   BallDrainedEvent.hpp
//! @brief  BallDrainedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Entity/Entity.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BallDrainedEvent
    //! @brief  玉がアウト口に入った、または盤面の外へ落ちた
    //! @note   入った玉はこの後すぐ破棄される
    //-------------------------------------------------------------
    struct BallDrainedEvent {
        Tsukino::ECS::Entity ball;      //!< 消える玉
        bool                 isFoul;    //!< 発射レーンへ戻ったファウルか
    };
}    // namespace StardustFever::ECS
