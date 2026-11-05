//-------------------------------------------------------------
//! @file   StarMapStateComponent.hpp
//! @brief  StarMapStateComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarMapStateComponent
    //! @brief  88星座を眺める星図の画面の状態
    //! @note   ゲーム状態エンティティに1つだけ付く。開いている間は遊びを止める（GamePause.hpp）
    //-------------------------------------------------------------
    struct StarMapStateComponent {
        bool        open      = false;        //!< 開いているか
        double      raCenter  = 84.0;         //!< 画面の中央の赤経（度。初めはオリオン座のあたり）
        double      decCenter = 0.0;          //!< 画面の中央の赤緯（度）
        double      zoom      = 1.0;          //!< 拡大率
        std::string selectedId;               //!< 選んでいる星座（空なら選んでいない）
        bool        groupMode     = false;    //!< グループの一覧を出しているか（G で切り替える）
        int         selectedGroup = 0;        //!< グループの一覧で選んでいるグループの添字
        bool        dragging      = false;    //!< 星図をつかんでいるか
        float       dragMoved     = 0.0f;     //!< つかんでから動かした量（ピクセル。少なければクリック）
        float       lastX         = 0.0f;     //!< 前のフレームのマウスの位置（UI の座標）
        float       lastY         = 0.0f;
        int         version       = 0;    //!< 見え方が変わるたびに増える（StarMapViewSystem が並べ直す合図）
    };
}    // namespace StardustFever::ECS
