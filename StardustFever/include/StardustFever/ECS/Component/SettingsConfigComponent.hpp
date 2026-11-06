//-------------------------------------------------------------
//! @file   SettingsConfigComponent.hpp
//! @brief  SettingsConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SettingsConfigComponent
    //! @brief  設定の画面の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct SettingsConfigComponent {
        std::string    whiteTexture;                                                         //!< 幕と線に使う白い画像
        std::string    panelTexture;                                                         //!< カードの面の画像
        std::string    frameTexture;                                                         //!< カードの縁の画像
        std::string    pillTexture;                                                          //!< カプセルの画像
        std::string    barTexture;                                                           //!< スライダーの塗りの画像
        std::string    starTexture;                                                          //!< 星の画像
        std::string    starOutlineTexture;                                                   //!< 星の輪郭の画像
        std::string    glowTexture;                                                          //!< 光の画像
        int            sortOrder          = 70;                                              //!< 星図の幕より手前に置く
        float          panelWidth         = 620.0f;                                          //!< カードの幅
        float          panelHeight        = 470.0f;                                          //!< カードの高さ
        float          rowStartY          = 130.0f;                                          //!< 最初の項目のカードの上からの位置
        float          rowSpacing         = 62.0f;                                           //!< 項目の間隔
        float          labelX             = 44.0f;                                           //!< 項目の名前のカードの左からの位置
        float          controlX           = 250.0f;                                          //!< スライダーと切り替えのカードの左からの位置
        float          trackWidth         = 240.0f;                                          //!< スライダーの長さ
        float          titleScale         = 0.8f;                                            //!< 題の文字の大きさ
        float          textScale          = 0.5f;                                            //!< 項目の文字の大きさ
        float          switchWidth        = 84.0f;                                           //!< 切り替えの幅
        float          switchHeight       = 34.0f;                                           //!< 切り替えの高さ
        float          closeButtonWidth   = 170.0f;                                          //!< 閉じるボタンの幅
        float          closeButtonHeight  = 42.0f;                                           //!< 閉じるボタンの高さ
        float          openButtonX        = 1535.0f;                                         //!< 「設定」のボタンの中心の X
        float          openButtonY        = 966.0f;                                          //!< 「設定」のボタンの中心の Y
        float          openButtonWidth    = 96.0f;                                           //!< 「設定」のボタンの幅
        float          openButtonHeight   = 30.0f;                                           //!< 「設定」のボタンの高さ
        float          actionBarX         = 87.0f;                                           //!< 左下の操作ボタンの最初の中心の X
        float          actionBarY         = 936.0f;                                          //!< 左下の操作ボタンの中心の Y
        float          actionButtonWidth  = 124.0f;                                          //!< 左下の操作ボタンの幅
        float          actionButtonHeight = 40.0f;                                           //!< 左下の操作ボタンの高さ
        float          actionButtonGap    = 10.0f;                                           //!< 左下の操作ボタンの間隔
        float          tooltipX           = 590.0f;                                          //!< 強化の吹き出しの中心の X
        float          tooltipWidth       = 340.0f;                                          //!< 強化の吹き出しの幅
        float          tooltipHeight      = 108.0f;                                          //!< 強化の吹き出しの高さ
        hlslpp::float4 dimColor           = hlslpp::float4(0.006f, 0.008f, 0.03f, 0.72f);    //!< 後ろの幕の色
        hlslpp::float4 panelColor         = hlslpp::float4(0.008f, 0.01f, 0.035f, 1.0f);     //!< カードの面の色
        hlslpp::float4 frameColor         = hlslpp::float4(1.0f, 0.8f, 0.45f, 0.6f);         //!< カードの縁と題の線の色
        hlslpp::float4 glowColor          = hlslpp::float4(0.16f, 0.12f, 0.32f, 0.6f);       //!< カードの後ろの光の色
        hlslpp::float4 accentColor        = hlslpp::float4(1.0f, 0.82f, 0.42f, 1.0f);        //!< 星の色
        hlslpp::float4 trackColor         = hlslpp::float4(0.035f, 0.045f, 0.12f, 1.0f);     //!< スライダーの線と切のスイッチの色
        hlslpp::float4 fillColor          = hlslpp::float4(1.0f, 0.85f, 0.45f, 1.0f);        //!< スライダーの塗りの色
        hlslpp::float4 onColor            = hlslpp::float4(0.85f, 0.6f, 0.18f, 1.0f);        //!< 入のスイッチの色
        hlslpp::float4 buttonColor        = hlslpp::float4(0.05f, 0.065f, 0.17f, 1.0f);      //!< ボタンの色
        hlslpp::float4 hoverColor         = hlslpp::float4(0.12f, 0.15f, 0.36f, 1.0f);       //!< マウスが乗ったボタンの色
        hlslpp::float4 textColor          = hlslpp::float4(0.86f, 0.9f, 1.0f, 1.0f);         //!< 文字の色
        hlslpp::float4 titleColor         = hlslpp::float4(1.0f, 0.92f, 0.68f, 1.0f);        //!< 題と値の文字の色
    };
}    // namespace StardustFever::ECS
