//-------------------------------------------------------------
//! @file   LinkLineComponent.hpp
//! @brief  LinkLineComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LinkLineComponent
    //! @brief  つながった星同士を結ぶ光の線
    //! @note   フィーバー終了時にまとめて片付けるためのタグ
    //-------------------------------------------------------------
    struct LinkLineComponent {
        bool tag = true;    //!< EnTT の空型の特別扱いを避けるための値（PrefabFactory が GetComponent するため）
    };
}    // namespace StardustFever::ECS
