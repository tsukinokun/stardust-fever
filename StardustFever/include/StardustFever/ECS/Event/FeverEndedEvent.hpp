//-------------------------------------------------------------
//! @file   FeverEndedEvent.hpp
//! @brief  FeverEndedEvent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverEndedEvent
    //! @brief  フィーバーが終わった
    //-------------------------------------------------------------
    struct FeverEndedEvent {
        bool ended = true;    //!< 空型にしないための値
    };
}    // namespace StardustFever::ECS
