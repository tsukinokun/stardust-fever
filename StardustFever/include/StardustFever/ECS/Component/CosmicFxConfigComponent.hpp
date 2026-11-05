//-------------------------------------------------------------
//! @file   CosmicFxConfigComponent.hpp
//! @brief  CosmicFxConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <Tsukino/BuiltIn/ECS/Particle/BurstEmitter.hpp>

#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct CosmicFxConfigComponent
    //! @brief  盤面の宇宙の演出（釘の火花・流れ星）の調整値
    //! @note   設定のエンティティに1つだけ付ける。ホットリロードで読み直す。
    //!         流れ星の画像は向きを描き込んである（左下が頭）ので、shootingDirection はその向きに合わせる
    //-------------------------------------------------------------
    struct CosmicFxConfigComponent {
        // 釘の火花（TwinkleSystem）
        std::string                        sparkTexture;             //!< 火花の画像
        float                              pinSparkChance = 0.5f;    //!< 当たったときに火花を出す割合（全部出すと散らかるため）
        Tsukino::BuiltIn::ECS::BurstParams pinSpark;                 //!< 火花の撒き方

        // 流れ星（ShootingStarSystem）
        std::string    streakTexture;                                                   //!< 流れ星の画像
        float          streakPixels        = 256.0f;                                    //!< 画像の一辺のピクセル数
        float          shootingIntervalMin = 2.5f;                                      //!< 次の流れ星までの最短（秒）
        float          shootingIntervalMax = 7.0f;                                      //!< 次の流れ星までの最長（秒）
        float          feverIntervalScale  = 0.35f;                                     //!< フィーバー中の間隔の倍率（流星群にする）
        hlslpp::float3 shootingSpawnMin    = hlslpp::float3(-120.0f, 60.0f, 100.0f);    //!< 出る範囲の最小
        hlslpp::float3 shootingSpawnMax    = hlslpp::float3(160.0f, 140.0f, 110.0f);    //!< 出る範囲の最大
        hlslpp::float3 shootingDirection   = hlslpp::float3(-1.0f, -1.0f, 0.0f);        //!< 進む向き（画像の向きに合わせる）
        float          shootingSpeedMin    = 140.0f;                                    //!< 速さの最小
        float          shootingSpeedMax    = 240.0f;                                    //!< 速さの最大
        float          shootingSizeMin     = 30.0f;                                     //!< 大きさの最小（ワールド単位）
        float          shootingSizeMax     = 60.0f;                                     //!< 大きさの最大
        float          shootingLife        = 0.9f;                                      //!< 見えている長さ（秒）
        hlslpp::float4 shootingColor       = hlslpp::float4(1.6f, 1.7f, 2.2f, 1.0f);    //!< 色（HDR）
    };
}    // namespace StardustFever::ECS
