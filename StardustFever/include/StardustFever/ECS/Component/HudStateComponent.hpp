//-------------------------------------------------------------
//! @file   HudStateComponent.hpp
//! @brief  HudStateComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudStateComponent
    //! @brief  HUDの一時的な表示
    //! @note   ゲーム状態エンティティに1つだけ付く
    //-------------------------------------------------------------
    struct HudStateComponent {
        std::string message;                //!< 画面中央に出すメッセージ（UTF-8）
        float       messageTimer = 0.0f;    //!< メッセージを消すまでの残り時間（秒）
    };
}    // namespace StardustFever::ECS
