//-------------------------------------------------------------
//! @file   TwinkleComponent.hpp
//! @brief  TwinkleComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct TwinkleComponent
    //! @brief  星のように時々ふっと明るくなり、玉が当たると閃く（釘）
    //! @note   TwinkleSystem がモデルの自己発光（MaterialPropertyBlockComponent.emissive）を書き換える。
    //!         瞬く間隔は釘ごとに乱数で決め、全部の釘が揃って光らないようにする
    //-------------------------------------------------------------
    struct TwinkleComponent {
        hlslpp::float3 baseEmissive   = hlslpp::float3(0.6f, 0.45f, 0.15f);    //!< ふだんの光
        hlslpp::float3 peakEmissive   = hlslpp::float3(2.6f, 2.1f, 1.0f);      //!< 瞬いたときの光
        hlslpp::float3 flashEmissive  = hlslpp::float3(5.0f, 4.4f, 2.6f);      //!< 玉が当たったときの光
        float          intervalMin    = 2.0f;                                  //!< 次に瞬くまでの最短（秒）
        float          intervalMax    = 9.0f;                                  //!< 次に瞬くまでの最長（秒）
        float          twinkleSeconds = 0.45f;                                 //!< 1回の瞬きの長さ（秒）
        float          flashDecay     = 5.0f;                                  //!< 閃きが消える速さ（1秒あたりの割合）

        // 実行時の値（JSON には書かない）
        float timer       = -1.0f;    //!< 次に瞬くまでの残り（負ならまだ決めていない）
        float twinkleTime = 0.0f;     //!< 瞬きの残り時間
        float flash       = 0.0f;     //!< 閃きの強さ（1 → 0）
    };
}    // namespace StardustFever::ECS
