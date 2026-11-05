//-------------------------------------------------------------
//! @file   FeverStartedEvent.hpp
//! @brief  FeverStartedEvent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverStartedEvent
    //! @brief  フィーバーが始まった
    //-------------------------------------------------------------
    struct FeverStartedEvent {
        bool started = true;    //!< 空型にしないための値
    };
}    // namespace StardustFever::ECS
