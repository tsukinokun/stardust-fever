//-------------------------------------------------------------
//! @file   SkyObjectConfigComponent.hpp
//! @brief  SkyObjectConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SkyObjectConfigComponent
    //! @brief  ビッグバンで手に入る天体の調整値
    //! @note   設定エンティティに付く。ホットリロード対象。配列はレア度の順
    //-------------------------------------------------------------
    struct SkyObjectConfigComponent {
        std::vector<std::string>    rarityNames;                //!< レア度の名前
        std::vector<double>         baseWeights;                //!< 稼ぎが 0 のときの重み
        std::vector<double>         growths;                    //!< 稼ぎ1桁あたりに重みが増える桁数
        std::vector<hlslpp::float4> rarityColors;               //!< レア度の色
        float                       revealDelay    = 1.3f;      //!< ビッグバンの光の後、カードを出すまでの秒数
        float                       cardWidth      = 660.0f;    //!< カードの幅
        float                       cardHeight     = 620.0f;    //!< カードの高さ
        int                         captionColumns = 72;        //!< カードの文章の1行の幅（全角を2と数える）
        int                         flashRarity    = 2;         //!< このレア度から、カードと一緒に画面を光らせる
    };
}    // namespace StardustFever::ECS
