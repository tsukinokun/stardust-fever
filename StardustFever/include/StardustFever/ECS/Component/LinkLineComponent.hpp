//-------------------------------------------------------------
//! @file   LinkLineComponent.hpp
//! @brief  LinkLineComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LinkLineComponent
    //! @brief  星座の線。フィーバー開始時に薄いガイドとして引き、両端の星がつながったら灯す
    //! @note   値は実行時に StarLinkSystem が入れる（Prefab には書かない）。フィーバー終了時にまとめて片付ける
    //-------------------------------------------------------------
    struct LinkLineComponent {
        int  from = -1;       //!< 片方の端の星（ConstellationDef::stars の添字）
        int  to   = -1;       //!< もう片方の端の星
        bool lit  = false;    //!< 両端がつながって灯ったか
    };
}    // namespace StardustFever::ECS
