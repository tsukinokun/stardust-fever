//-------------------------------------------------------------
//! @file   StarMapConfigComponent.hpp
//! @brief  StarMapConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarMapConfigComponent
    //! @brief  88星座を眺める星図の画面の調整値
    //! @note   設定エンティティに付く。ホットリロード対象。位置と大きさは UI の座標（1600×1000）
    //-------------------------------------------------------------
    struct StarMapConfigComponent {
        std::string    backgroundTexture;                                                      //!< 背景・線の板に使う白い画像
        float          backgroundPixels = 8.0f;                                                //!< 背景の画像の一辺のピクセル数
        std::string    starTexture;                                                            //!< 星の点の画像
        float          starPixels             = 64.0f;                                         //!< 星の画像の一辺のピクセル数
        hlslpp::float4 panelColor             = hlslpp::float4(0.05f, 0.06f, 0.14f, 0.95f);    //!< 右の詳しいパネルの色
        float          mapX                   = 20.0f;                                         //!< 星図の枠の左端（UI の座標）
        float          mapY                   = 70.0f;                                         //!< 星図の枠の上端
        float          mapWidth               = 1130.0f;                                       //!< 星図の枠の幅
        float          mapHeight              = 880.0f;                                        //!< 星図の枠の高さ
        float          minZoom                = 1.0f;                                          //!< 拡大率の最小
        float          maxZoom                = 7.0f;                                          //!< 拡大率の最大
        float          zoomStep               = 1.2f;                                          //!< ホイール1目盛りで拡大率に掛ける量
        float          keyZoomSpeed           = 2.0f;                                          //!< Q / E を押している間、1秒で拡大率に掛ける量
        float          keyPanSpeed            = 90.0f;    //!< 矢印キーで1秒に動かす角度（拡大率 1 のとき。拡大するほど遅くする）
        float          maxDec                 = 80.0f;    //!< 画面の中央に来る赤緯の上限（下限はこの負の値）
        float          brightStarSize         = 15.0f;    //!< 0等級の星の点の大きさ（ピクセル）
        float          faintStarSize          = 4.0f;     //!< 5等級の星の点の大きさ
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
        float          reticleRadius          = 170.0f;     //!< 中央の照準が星座を拾う距離（UI のピクセル）
        float          clickRadius            = 36.0f;      //!< クリックで星座を選べる距離（ピクセル）
        float          dragThreshold          = 6.0f;       //!< これより動かしたらクリックではなくドラッグとみなす（ピクセル）
        int            sortOrder              = 40;         //!< 描画順の基準（HUD とトーストより上に出す）
        // 天球
        float                    skyOffsetZ         = -20000.0f;                                 //!< 天球の中心の Z。盤面がカメラの遠い面より外へ出る距離にする
        float                    skyRadius          = 1000.0f;                                   //!< 天球の半径
        float                    baseFov            = 70.0f;                                     //!< 拡大率 1 の縦の視野（度）
        float                    worldUnitsPerPixel = 1.2f;                                      //!< 星の大きさ（ピクセル）を天球の上の大きさへ直す倍率
        float                    ownedStarGlow      = 3.0f;                                      //!< 持っている星の明るさの倍率（1を超えた分がブルームで光る）
        float                    lineGlow           = 2.0f;                                      //!< 線の明るさの倍率
        float                    lineOpacity        = 0.9f;                                      //!< 線の不透明度（1未満にすると影の描画から外れる）
        std::string              lineModel;                                                      //!< 線のモデル
        std::vector<std::string> nebulaTextures;                                                 //!< 空に散らす星雲の画像
        int                      nebulaCount   = 10;                                             //!< 星雲の数
        float                    nebulaScale   = 2.0f;                                           //!< 星雲の大きさ
        hlslpp::float4           nebulaColor   = hlslpp::float4(0.2f, 0.2f, 0.3f, 1.0f);         //!< 星雲の色
        hlslpp::float4           skyClearColor = hlslpp::float4(0.002f, 0.003f, 0.01f, 1.0f);    //!< 空の地の色
        // 動き
        float fadeOutSeconds    = 0.3f;     //!< 暗転にかける時間（秒）
        float revealSeconds     = 1.1f;     //!< 星が浮かび上がる時間（秒）
        float revealLookDegrees = 25.0f;    //!< 開いたとき、この角度だけ下から見上げていく（度）
        float inertiaFriction   = 3.5f;     //!< 手を離した後の回転の止まりやすさ（1秒あたり）
        float focusSpeed        = 4.0f;     //!< 選んだ星座へ回る速さ（1秒あたり）
        // 名前と文章
        std::string    reticleTexture;                                                     //!< 照準の絵
        float          reticleSize         = 56.0f;                                        //!< 照準の大きさ（UI のピクセル）
        float          objectReticleRadius = 60.0f;                                        //!< 中央の照準が天体を拾う距離（UI のピクセル）
        float          objectPixels        = 256.0f;                                       //!< 天体の絵の幅
        std::string    objectUnknownTexture;                                               //!< まだ手に入れていない天体の絵
        float          objectUnknownSize  = 40.0f;                                         //!< まだ手に入れていない天体の大きさ（UI のピクセル）
        float          objectMissingAlpha = 0.4f;                                          //!< 持っていない天体の濃さ
        float          hoverRadius        = 14.0f;                                         //!< マウスがこの距離まで近づくと星の吹き出しを出す（UI のピクセル）
        float          tooltipWidth       = 330.0f;                                        //!< 吹き出しの幅
        float          tooltipHeight      = 84.0f;                                         //!< 吹き出しの高さ
        float          tooltipScale       = 0.42f;                                         //!< 吹き出しの文字の大きさ
        hlslpp::float4 tooltipColor       = hlslpp::float4(0.01f, 0.012f, 0.04f, 0.9f);    //!< 吹き出しの板の色
        int            maxStarLabels      = 14;                                            //!< 選んだ星座に添える星の名前の数
        float          starLabelScale     = 0.34f;                                         //!< 星の名前の大きさ
        float          starLabelOffset    = 12.0f;                                         //!< 星の名前を星から右へずらす量（ピクセル）
        hlslpp::float4 starLabelColor     = hlslpp::float4(0.85f, 0.9f, 1.0f, 0.95f);      //!< 星の名前の色
        int            captionWidth       = 54;                                            //!< 図鑑の文章の1行の幅（全角1文字を2と数える）
    };
}    // namespace StardustFever::ECS
