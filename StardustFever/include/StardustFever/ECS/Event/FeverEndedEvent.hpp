//-------------------------------------------------------------
//! @file   FeverEndedEvent.hpp
//! @brief  FeverEndedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverEndedEvent
    //! @brief  フィーバーが終わった
    //-------------------------------------------------------------
    struct FeverEndedEvent {
        std::string constellationId;    //!< 出していた星座
        bool        completed;          //!< 星座を完成させたか
    };
}    // namespace StardustFever::ECS
