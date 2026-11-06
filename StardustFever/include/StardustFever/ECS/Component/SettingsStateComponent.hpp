//-------------------------------------------------------------
//! @file   SettingsStateComponent.hpp
//! @brief  SettingsStateComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SettingsStateComponent
    //! @brief  設定の画面の状態
    //! @note   ゲーム状態エンティティに1つだけ付く。開いている間は遊びを止める（GamePause.hpp）
    //-------------------------------------------------------------
    struct SettingsStateComponent {
        bool open         = false;    //!< 開いているか
        bool showKeyHints = true;     //!< キー操作の表示を出すか（セーブに残す）
        bool previewSound = false;    //!< 効果音の音量を決めたので、確かめの音を鳴らす（SfxSystem が消す）
    };
}    // namespace StardustFever::ECS
