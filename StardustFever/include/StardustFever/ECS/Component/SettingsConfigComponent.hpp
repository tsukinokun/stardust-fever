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
        std::string    whiteTexture;                                                   //!< 板に使う白い画像
        float          whitePixels  = 8.0f;                                            //!< 白い画像の大きさ
        int            sortOrder    = 70;                                              //!< 星図の幕より手前に置く
        float          panelWidth   = 560.0f;                                          //!< パネルの幅
        float          panelHeight  = 430.0f;                                          //!< パネルの高さ
        float          rowStartY    = 110.0f;                                          //!< 最初の項目のパネルの上からの位置
        float          rowSpacing   = 62.0f;                                           //!< 項目の間隔
        float          labelX       = 40.0f;                                           //!< 項目の名前のパネルの左からの位置
        float          controlX     = 220.0f;                                          //!< スライダーと切り替えのパネルの左からの位置
        float          trackWidth   = 230.0f;                                          //!< スライダーの長さ
        float          titleScale   = 0.78f;                                           //!< 題の文字の大きさ
        float          textScale    = 0.5f;                                            //!< 項目の文字の大きさ
        float          buttonWidth  = 110.0f;                                          //!< ボタンの幅
        float          buttonHeight = 38.0f;                                           //!< ボタンの高さ
        float          openButtonX  = 1535.0f;                                         //!< 「設定」のボタンの中心の X
        float          openButtonY  = 966.0f;                                          //!< 「設定」のボタンの中心の Y
        hlslpp::float4 dimColor     = hlslpp::float4(0.0f, 0.0f, 0.0f, 0.6f);          //!< 後ろを暗くする色
        hlslpp::float4 panelColor   = hlslpp::float4(0.012f, 0.015f, 0.05f, 0.97f);    //!< パネルの色
        hlslpp::float4 trackColor   = hlslpp::float4(0.1f, 0.12f, 0.25f, 1.0f);        //!< スライダーの線の色
        hlslpp::float4 fillColor    = hlslpp::float4(1.0f, 0.75f, 0.3f, 1.0f);         //!< スライダーの塗りの色
        hlslpp::float4 onColor      = hlslpp::float4(0.7f, 0.5f, 0.12f, 1.0f);         //!< 入のボタンの色
        hlslpp::float4 offColor     = hlslpp::float4(0.06f, 0.07f, 0.16f, 1.0f);       //!< 切のボタンの色
        hlslpp::float4 hoverColor   = hlslpp::float4(0.14f, 0.16f, 0.34f, 1.0f);       //!< マウスが乗ったボタンの色
        hlslpp::float4 textColor    = hlslpp::float4(0.92f, 0.94f, 1.0f, 1.0f);        //!< 文字の色
    };
}    // namespace StardustFever::ECS
