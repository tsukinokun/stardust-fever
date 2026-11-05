//-------------------------------------------------------------
//! @file   StarMapConfigComponent.hpp
//! @brief  StarMapConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarMapConfigComponent
    //! @brief  88星座を眺める星図の画面の調整値
    //! @note   設定エンティティに付く。ホットリロード対象。位置と大きさは UI の座標（1600×1000）
    //-------------------------------------------------------------
    struct StarMapConfigComponent {
        std::string    backgroundTexture;                                                       //!< 背景・線の板に使う白い画像
        float          backgroundPixels = 8.0f;                                                 //!< 背景の画像の一辺のピクセル数
        std::string    starTexture;                                                             //!< 星の点の画像
        float          starPixels             = 64.0f;                                          //!< 星の画像の一辺のピクセル数
        hlslpp::float4 backgroundColor        = hlslpp::float4(0.01f, 0.012f, 0.04f, 0.96f);    //!< 画面全体を覆う背景の色
        hlslpp::float4 mapColor               = hlslpp::float4(0.02f, 0.03f, 0.09f, 1.0f);      //!< 星図の枠の中の色
        hlslpp::float4 panelColor             = hlslpp::float4(0.05f, 0.06f, 0.14f, 0.95f);     //!< 右の詳しいパネルの色
        float          mapX                   = 20.0f;                                          //!< 星図の枠の左端（UI の座標）
        float          mapY                   = 70.0f;                                          //!< 星図の枠の上端
        float          mapWidth               = 1130.0f;                                        //!< 星図の枠の幅
        float          mapHeight              = 880.0f;                                         //!< 星図の枠の高さ
        float          pixelsPerDegree        = 3.0f;                                           //!< 拡大率 1 のとき、1度が何ピクセルか（360度でおよそ枠の幅）
        float          minZoom                = 1.0f;                                           //!< 拡大率の最小
        float          maxZoom                = 7.0f;                                           //!< 拡大率の最大
        float          zoomStep               = 1.2f;                                           //!< ホイール1目盛りで拡大率に掛ける量
        float          keyZoomSpeed           = 2.0f;                                           //!< Q / E を押している間、1秒で拡大率に掛ける量
        float          keyPanSpeed            = 90.0f;    //!< 矢印キーで1秒に動かす角度（拡大率 1 のとき。拡大するほど遅くする）
        float          maxDec                 = 80.0f;    //!< 画面の中央に来る赤緯の上限（下限はこの負の値）
        float          brightStarSize         = 15.0f;    //!< 0等級の星の点の大きさ（ピクセル）
        float          faintStarSize          = 4.0f;     //!< 5等級の星の点の大きさ
        float          starZoomGrowth         = 0.18f;    //!< 拡大率が 1 上がるごとに星の点を何割大きくするか
        hlslpp::float4 missingStarColor       = hlslpp::float4(0.32f, 0.36f, 0.5f, 0.55f);    //!< 持っていない星の色
        hlslpp::float4 lineColor              = hlslpp::float4(0.35f, 0.45f, 0.8f, 0.28f);    //!< そろっていない星座の線の色
        hlslpp::float4 selectedLineColor      = hlslpp::float4(0.6f, 0.75f, 1.0f, 0.8f);      //!< 選んだ星座の線の色
        hlslpp::float4 collectedLineColor     = hlslpp::float4(1.0f, 0.82f, 0.42f, 0.9f);     //!< そろった星座の線の色
        float          lineThickness          = 1.2f;                                         //!< 線の太さ（ピクセル）
        float          collectedLineThickness = 2.2f;                                         //!< そろった星座と選んだ星座の線の太さ
        hlslpp::float4 labelColor             = hlslpp::float4(0.55f, 0.6f, 0.78f, 0.7f);     //!< 星座名の色
        hlslpp::float4 collectedLabelColor    = hlslpp::float4(1.0f, 0.86f, 0.5f, 1.0f);      //!< そろった星座の名前の色
        hlslpp::float4 selectedLabelColor     = hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);       //!< 選んだ星座の名前の色
        float          labelScale             = 0.4f;                                         //!< 星座名の文字の大きさ
        float          labelMinZoom           = 1.6f;       //!< この拡大率より小さいときは、そろった星座と選んだ星座の名前だけ出す（重なって読めないため）
        float          panelX                 = 1170.0f;    //!< 右の詳しいパネルの左端
        float          panelWidth             = 410.0f;     //!< 右の詳しいパネルの幅
        float          titleScale             = 0.78f;      //!< 選んだ星座の名前の文字の大きさ
        float          textScale              = 0.42f;      //!< パネルの文字の大きさ
        float          rowScale               = 0.36f;      //!< 星の一覧の文字の大きさ
        float          rowSpacing             = 21.0f;      //!< 星の一覧の行の間隔
        int            maxRows                = 32;         //!< 星の一覧に出す行の数の上限
        float          clickRadius            = 36.0f;      //!< クリックで星座を選べる距離（ピクセル）
        float          dragThreshold          = 6.0f;       //!< これより動かしたらクリックではなくドラッグとみなす（ピクセル）
        int            sortOrder              = 40;         //!< 描画順の基準（HUD とトーストより上に出す）
    };
}    // namespace StardustFever::ECS
