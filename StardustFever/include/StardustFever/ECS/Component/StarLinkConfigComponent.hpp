//-------------------------------------------------------------
//! @file   StarLinkConfigComponent.hpp
//! @brief  StarLinkConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarLinkConfigComponent
    //! @brief  星ゲットチャンスで星座をつなぐ線の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct StarLinkConfigComponent {
        std::string    linkLinePrefab;                                         //!< 星座の線のPrefab
        std::string    litStarModel;                                           //!< つながった星のモデル
        float          lineThickness  = 0.3f;                                  //!< 灯った線の太さの半分
        float          guideThickness = 0.12f;                                 //!< ガイドの線の太さの半分
        hlslpp::float3 litEmissive    = hlslpp::float3(3.0f, 2.4f, 1.1f);      //!< 灯った線の自己発光（1 を超えるとブルームでにじむ）
        hlslpp::float3 guideEmissive  = hlslpp::float3(0.15f, 0.2f, 0.45f);    //!< ガイドの線の自己発光
    };
}    // namespace StardustFever::ECS
