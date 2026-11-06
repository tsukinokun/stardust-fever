//-------------------------------------------------------------
//! @file   HudLayoutConfigComponent.hpp
//! @brief  HudLayoutConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudLayoutConfigComponent
    //! @brief  HUD の並べ方の調整値
    //! @note   設定エンティティに付く。ホットリロード対象（並べ直しは強化・星座の数が変わったとき）
    //-------------------------------------------------------------
    struct HudLayoutConfigComponent {
        std::string    shopCardGroup;                                                      //!< 強化カードの雛形（グループ JSON）
        float          shopX       = 34.0f;                                                //!< 強化カードの左端の X
        float          shopY       = 322.0f;                                               //!< いちばん上の強化カードの Y
        float          shopSpacing = 56.0f;                                                //!< 強化カードの間隔
        float          shopStagger = 0.045f;                                               //!< カードが現れる時間差（秒）
        std::string    chartHeaderGroup;                                                   //!< 星図の星座見出しの雛形
        std::string    chartRowGroup;                                                      //!< 星図の星の行の雛形
        float          chartX             = 1196.0f;                                       //!< 星図の左端の X
        float          chartY             = 92.0f;                                         //!< 星図のいちばん上の Y
        float          chartRowSpacing    = 30.0f;                                         //!< 星の行の間隔
        float          chartHeaderSpacing = 40.0f;                                         //!< 星座見出しの高さ
        float          chartWidth         = 360.0f;                                        //!< 星図の行の幅（「そろったグループ」を押せる範囲）
        float          chartIndent        = 14.0f;                                         //!< 星の行の字下げ
        hlslpp::float4 colorO             = hlslpp::float4(0.6f, 0.72f, 1.0f, 1.0f);       //!< O 型の星の色
        hlslpp::float4 colorB             = hlslpp::float4(0.72f, 0.82f, 1.0f, 1.0f);      //!< B 型の星の色
        hlslpp::float4 colorA             = hlslpp::float4(0.95f, 0.97f, 1.0f, 1.0f);      //!< A 型の星の色
        hlslpp::float4 colorF             = hlslpp::float4(1.0f, 0.97f, 0.85f, 1.0f);      //!< F 型の星の色
        hlslpp::float4 colorG             = hlslpp::float4(1.0f, 0.9f, 0.6f, 1.0f);        //!< G 型の星の色
        hlslpp::float4 colorK             = hlslpp::float4(1.0f, 0.72f, 0.42f, 1.0f);      //!< K 型の星の色
        hlslpp::float4 colorM             = hlslpp::float4(1.0f, 0.5f, 0.38f, 1.0f);       //!< M 型の星の色
        hlslpp::float4 undiscoveredColor  = hlslpp::float4(0.45f, 0.48f, 0.62f, 0.55f);    //!< まだ見つけていない星の色
        hlslpp::float4 affordableColor    = hlslpp::float4(1.0f, 0.86f, 0.42f, 1.0f);      //!< 買える強化の値段の色
        hlslpp::float4 unaffordableColor  = hlslpp::float4(0.62f, 0.58f, 0.72f, 1.0f);     //!< 買えない強化の値段の色
        hlslpp::float4 maxedColor         = hlslpp::float4(1.0f, 0.7f, 0.3f, 1.0f);        //!< 最大レベルの強化の色
        float          unaffordableAlpha  = 0.55f;                                         //!< 買えない強化のカードの不透明度
        std::string    starIconTexture;                                                    //!< 見つけた星のアイコン
        std::string    starOutlineTexture;                                                 //!< まだ見つけていない星のアイコン
    };
}    // namespace StardustFever::ECS
