//-------------------------------------------------------------
//! @file   HudFxConfigComponent.hpp
//! @brief  HudFxConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudFxConfigComponent
    //! @brief  HUD の演出の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct HudFxConfigComponent {
        std::string    toastGroup;                                                       //!< トーストの雛形（グループ JSON）
        std::string    headlineGroup;                                                    //!< 大見出しの雛形（グループ JSON）
        std::string    popupGroup;                                                       //!< 浮かぶ数字の雛形（グループ JSON）
        float          toastRight      = 1572.0f;                                        //!< トーストの右端の X
        float          toastBottom     = 890.0f;                                         //!< いちばん下のトーストの Y
        float          toastSpacing    = 58.0f;                                          //!< トーストを積む間隔
        float          toastSeconds    = 3.2f;                                           //!< トーストが残る時間（秒）
        int            maxToasts       = 5;                                              //!< 同時に出すトーストの上限（超えたら古いものから消す）
        float          headlineSeconds = 2.4f;                                           //!< 大見出しが残る時間（秒）
        float          popupRise       = 70.0f;                                          //!< 浮かぶ数字が昇る高さ（UI の座標）
        float          popupSeconds    = 0.95f;                                          //!< 浮かぶ数字が消えるまでの時間（秒）
        float          pinPopupChance  = 0.3f;                                           //!< 釘に当たったときに数字を出す割合（毎回出すと盤面が埋まる）
        std::string    particleTexture;                                                  //!< 粒子の画像
        int            starBurstCount     = 26;                                          //!< 星をつないだときの粒子の数
        int            completeBurstCount = 90;                                          //!< 星座が完成したときの粒子の数
        hlslpp::float4 pinColor           = hlslpp::float4(0.85f, 0.9f, 1.0f, 0.9f);     //!< 釘の数字の色
        hlslpp::float4 pocketColor        = hlslpp::float4(0.45f, 0.95f, 1.0f, 1.0f);    //!< 入賞口の数字の色
        hlslpp::float4 starColor          = hlslpp::float4(1.0f, 0.86f, 0.4f, 1.0f);     //!< 星の数字の色
        hlslpp::float4 bonusColor         = hlslpp::float4(0.85f, 0.55f, 1.0f, 1.0f);    //!< 完成ボーナスなどの数字の色
        hlslpp::float4 toneInfoColor      = hlslpp::float4(0.55f, 0.75f, 1.0f, 1.0f);    //!< ふつうの知らせのトーストの縁の色
        hlslpp::float4 tonePositiveColor  = hlslpp::float4(1.0f, 0.84f, 0.38f, 1.0f);    //!< 良い出来事のトーストの縁の色
        hlslpp::float4 toneWarningColor   = hlslpp::float4(1.0f, 0.42f, 0.42f, 1.0f);    //!< できなかったときのトーストの縁の色
        float          headlineX          = 810.0f;                                      //!< 大見出しの中心の X
        float          headlineY          = 420.0f;                                      //!< 大見出しの中心の Y
        int            maxPopups          = 40;                                          //!< 同時に浮かべる数字の上限
        float          bannerTravel       = 150.0f;                                      //!< フィーバーの帯が上へ引っ込む距離（UI の座標）
        float          timeWarningSeconds = 6.0f;                                        //!< 残り時間がこれを切ったら秒数を脈打たせる（秒）
        float          completeBurstX     = 0.0f;                                        //!< 星座が完成したときに粒子を弾けさせる場所の X（ワールド座標）
        float          completeBurstY     = 38.0f;                                       //!< 星座が完成したときに粒子を弾けさせる場所の Y（ワールド座標）
    };
}    // namespace StardustFever::ECS
