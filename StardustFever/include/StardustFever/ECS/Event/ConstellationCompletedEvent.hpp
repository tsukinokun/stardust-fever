//-------------------------------------------------------------
//! @file   ConstellationCompletedEvent.hpp
//! @brief  ConstellationCompletedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct ConstellationCompletedEvent
    //! @brief  星座が完成した
    //-------------------------------------------------------------
    struct ConstellationCompletedEvent {
        std::string constellationId;    //!< 完成した星座
        bool        perfect;            //!< 1球で全てつないだか
    };
}    // namespace StardustFever::ECS
