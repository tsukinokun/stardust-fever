//-------------------------------------------------------------
//! @file   TulipWingComponent.hpp
//! @brief  TulipWingComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct TulipWingComponent
    //! @brief  チューリップ（横の入賞口）の羽1枚
    //! @note   盤面に置かれた位置を蝶番にして、TulipSystem が蝶番を中心に回す。
    //!         フィーバー中は開いて V の字になり、玉を受け止めやすくなる。
    //!         角度は +X から反時計回り（度）。左の羽は 180 - 右の羽の角度にする
    //-------------------------------------------------------------
    struct TulipWingComponent {
        float halfLength  = 1.5f;     //!< 羽の長さの半分（Prefab の scale.x と揃える）
        float closedDeg   = 90.0f;    //!< 閉じたときの角度
        float openDeg     = 40.0f;    //!< 開いたときの角度
        float moveSeconds = 0.35f;    //!< 開け閉めにかかる時間（秒）

        // 実行時の値（JSON には書かない）
        bool           initialized = false;                         //!< 蝶番の位置を決めたか
        hlslpp::float2 hinge       = hlslpp::float2(0.0f, 0.0f);    //!< 蝶番の位置（盤面に置かれた位置）
        float          openness    = 0.0f;                          //!< 開き具合（0 = 閉じた、1 = 開いた）
    };
}    // namespace StardustFever::ECS
