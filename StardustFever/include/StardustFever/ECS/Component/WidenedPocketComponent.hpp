//-------------------------------------------------------------
//! @file   WidenedPocketComponent.hpp
//! @brief  WidenedPocketComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct WidenedPocketComponent
    //! @brief  フィーバーの間だけ広げている入賞口
    //! @note   広げる前の大きさを取っておき、フィーバーが終わったら戻す。実行時だけの値
    //-------------------------------------------------------------
    struct WidenedPocketComponent {
        hlslpp::float3 extent = hlslpp::float3(0.0f, 0.0f, 0.0f);    //!< 広げる前の当たり判定の大きさ
        hlslpp::float3 scale  = hlslpp::float3(1.0f, 1.0f, 1.0f);    //!< 広げる前の見た目の大きさ
    };
}    // namespace StardustFever::ECS
