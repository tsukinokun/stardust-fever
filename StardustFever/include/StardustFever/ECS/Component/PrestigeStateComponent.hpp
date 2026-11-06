//-------------------------------------------------------------
//! @file   PrestigeStateComponent.hpp
//! @brief  PrestigeStateComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PrestigeStateComponent
    //! @brief  ビッグバン（転生）の進行
    //! @note   ゲーム状態エンティティに1つだけ付く。セーブされる
    //-------------------------------------------------------------
    struct PrestigeStateComponent {
        StardustFever::Core::BigNumber cosmicConstants;        //!< 持っている宇宙定数（全体倍率を永続で上げる）
        int                            bigBangCount = 0;       //!< ビッグバンの回数
        StardustFever::Core::BigNumber universeEarned;         //!< この宇宙で稼いだ星屑の累計（使っても減らない。ビッグバンで 0 に戻る）
        bool                           confirmOpen = false;    //!< 確認画面を開いているか
    };
}    // namespace StardustFever::ECS
