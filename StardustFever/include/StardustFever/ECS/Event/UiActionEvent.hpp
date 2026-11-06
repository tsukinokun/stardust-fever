//-------------------------------------------------------------
//! @file   UiActionEvent.hpp
//! @brief  UiActionEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/UiActionComponent.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct UiActionEvent
    //! @brief  押せる部品が押された
    //-------------------------------------------------------------
    struct UiActionEvent {
        UiAction action = UiAction::None;    //!< 意味
        int      index  = 0;                 //!< 意味ごとの番号
    };
}    // namespace StardustFever::ECS
