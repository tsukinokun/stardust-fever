//-------------------------------------------------------------
//! @file   StarChanceStartedEvent.hpp
//! @brief  StarChanceStartedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarChanceStartedEvent
    //! @brief  星ゲットチャンスが始まった
    //-------------------------------------------------------------
    struct StarChanceStartedEvent {
        std::string constellationId;    //!< 出した星座
    };
}    // namespace StardustFever::ECS
