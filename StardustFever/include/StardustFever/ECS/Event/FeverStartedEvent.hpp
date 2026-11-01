//-------------------------------------------------------------
//! @file   FeverStartedEvent.hpp
//! @brief  FeverStartedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverStartedEvent
    //! @brief  フィーバーが始まった
    //-------------------------------------------------------------
    struct FeverStartedEvent {
        std::string constellationId;    //!< 出した星座
    };
}    // namespace StardustFever::ECS
