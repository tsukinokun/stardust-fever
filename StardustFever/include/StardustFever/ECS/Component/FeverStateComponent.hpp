//-------------------------------------------------------------
//! @file   FeverStateComponent.hpp
//! @brief  FeverStateComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverStateComponent
    //! @brief  フィーバー「星詠みタイム」の状態
    //! @note   ゲーム状態エンティティに1つだけ付く
    //-------------------------------------------------------------
    struct FeverStateComponent {
        bool  active        = false;    //!< フィーバー中か
        float timeRemaining = 0.0f;     //!< 残り時間（秒）
        float timeTotal     = 0.0f;     //!< 始まったときの長さ（秒。残り時間のゲージに使う）
    };
}    // namespace StardustFever::ECS
