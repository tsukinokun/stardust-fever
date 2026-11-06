//-------------------------------------------------------------
//! @file   SettingsElementComponent.hpp
//! @brief  SettingsElementComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   SettingsElementKind
    //! @brief  設定の画面の部品の種類
    //-------------------------------------------------------------
    enum class SettingsElementKind : int {
        Decoration,     //!< 動かない部品
        OpenButton,     //!< 遊んでいる画面の「設定」のボタン
        CloseButton,    //!< 閉じるボタン
        Slider,         //!< スライダーの当たり（index は SettingsItem）
        Fill,           //!< スライダーの塗り
        Knob,           //!< スライダーのつまみ
        Percent,        //!< 音量の文字
        Toggle,         //!< 切り替えのボタン（index は SettingsItem）
        ToggleKnob,     //!< 切り替えの星
        ToggleText,     //!< 切り替えの文字
    };

    //-------------------------------------------------------------
    //! @enum   SettingsItem
    //! @brief  設定の項目
    //-------------------------------------------------------------
    enum SettingsItem : int {
        kSettingsBgm,
        kSettingsSfx,
        kSettingsMute,
        kSettingsKeyHints,
    };

    //-------------------------------------------------------------
    //! @struct SettingsElementComponent
    //! @brief  設定の画面の部品
    //! @note   SettingsViewSystem が作る。実行時だけの値
    //-------------------------------------------------------------
    struct SettingsElementComponent {
        SettingsElementKind kind  = SettingsElementKind::Decoration;    //!< 部品の種類
        int                 index = 0;                                  //!< 項目（SettingsItem）
    };
}    // namespace StardustFever::ECS
