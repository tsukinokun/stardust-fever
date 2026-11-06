//-------------------------------------------------------------
//! @file   StarGetFxConfigComponent.hpp
//! @brief  StarGetFxConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarGetFxConfigComponent
    //! @brief  星を手に入れた演出の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct StarGetFxConfigComponent {
        std::string    whiteTexture;                                               //!< 閃光の画像
        std::string    starTexture;                                                //!< 星の画像
        std::string    glowTexture;                                                //!< 光の画像
        std::string    flareTexture;                                               //!< 光の筋の画像
        int            sortOrder     = 62;                                         //!< HUD より手前、設定の画面より奥
        float          appearSeconds = 0.3f;                                       //!< 現れるまでの時間（秒）
        float          holdSeconds   = 1.1f;                                       //!< 見せる時間（秒）
        float          flySeconds    = 0.6f;                                       //!< 飛ぶ時間（秒）
        float          centerX       = 800.0f;                                     //!< 星の位置
        float          centerY       = 455.0f;                                     //!< 星の位置
        float          starSize      = 200.0f;                                     //!< 星の大きさ
        float          flareSize     = 720.0f;                                     //!< 光の筋の大きさ（最も明るい星のとき）
        float          flashAlpha    = 0.55f;                                      //!< 閃光の濃さ
        int            sparkMin      = 12;                                         //!< 星屑の数（暗い星）
        int            sparkMax      = 30;                                         //!< 星屑の数（明るい星）
        float          sparkDistance = 420.0f;                                     //!< 星屑が飛ぶ距離
        int            trailCount    = 12;                                         //!< 飛ぶ星の尾の粒の数
        float          textY         = 150.0f;                                     //!< 名前の星からのずれ
        float          nameScale     = 1.15f;                                      //!< 名前の文字の大きさ
        float          tagScale      = 0.66f;                                      //!< 上の見出しの文字の大きさ
        float          factsScale    = 0.5f;                                       //!< 呼び名の文字の大きさ
        float          walletX       = 72.0f;                                      //!< 星屑に換わった星が飛ぶ先
        float          walletY       = 104.0f;                                     //!< 星屑に換わった星が飛ぶ先
        hlslpp::float4 tagColor      = hlslpp::float4(1.0f, 0.86f, 0.5f, 1.0f);    //!< 上の見出しの色
        hlslpp::float4 textColor     = hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);     //!< 名前の色
    };
}    // namespace StardustFever::ECS
