//-------------------------------------------------------------
//! @file   AudioSettingsComponent.hpp
//! @brief  AudioSettingsComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct AudioSettingsComponent
    //! @brief  プレイヤーが決めた音量
    //! @note   ゲーム状態エンティティに1つだけ付く。セーブに残す
    //-------------------------------------------------------------
    struct AudioSettingsComponent {
        float bgmVolume = 0.8f;     //!< BGM の音量（0〜1）
        float sfxVolume = 0.6f;     //!< 効果音の音量（0〜1）
        bool  muted     = false;    //!< 消音しているか
    };
}    // namespace StardustFever::ECS
