//-------------------------------------------------------------
//! @file   HudMessageEvent.hpp
//! @brief  HudMessageEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudMessageEvent
    //! @brief  画面中央にメッセージを出す
    //-------------------------------------------------------------
    struct HudMessageEvent {
        std::string text;    //!< 表示する文字（UTF-8）
    };
}    // namespace StardustFever::ECS
