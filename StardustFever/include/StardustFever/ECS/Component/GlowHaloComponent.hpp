//-------------------------------------------------------------
//! @file   GlowHaloComponent.hpp
//! @brief  GlowHaloComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <entt/entt.hpp>

#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct GlowHaloComponent
    //! @brief  持ち主の位置に、光芒のビルボードと弱い点光源を重ねる（星釘）
    //! @note   GlowHaloSystem が光輪の実体を作って持ち主に付いていかせ、大きさを呼吸させ、持ち主が消えたら消す。
    //!         持ち主が StarPinComponent を持っていて linked なら、linkedColor / linkedLightIntensity に強める
    //-------------------------------------------------------------
    struct GlowHaloComponent {
        std::string    texture;                                                           //!< 光芒の画像
        float          texturePixels        = 256.0f;                                     //!< 画像の一辺のピクセル数
        float          size                 = 9.0f;                                       //!< 光輪の大きさ（ワールド単位）
        hlslpp::float4 color                = hlslpp::float4(0.6f, 0.75f, 1.6f, 0.8f);    //!< 光輪の色（HDR）
        hlslpp::float4 linkedColor          = hlslpp::float4(2.6f, 2.2f, 1.2f, 1.0f);     //!< つないだ後の光輪の色
        hlslpp::float3 offset               = hlslpp::float3(0.0f, 0.0f, -0.6f);          //!< 持ち主からのずれ（手前へ出して埋もれないようにする）
        float          pulseAmplitude       = 0.12f;                                      //!< 大きさの呼吸の幅（割合）
        float          periodMin            = 1.6f;                                       //!< 呼吸の周期の最小（秒）
        float          periodMax            = 2.8f;                                       //!< 呼吸の周期の最大（秒）
        hlslpp::float3 lightColor           = hlslpp::float3(0.55f, 0.7f, 1.0f);          //!< 点光源の色
        float          lightIntensity       = 0.0f;                                       //!< 点光源の強さ（0 なら点光源を付けない）
        float          linkedLightIntensity = 0.0f;                                       //!< つないだ後の点光源の強さ
        float          lightRange           = 18.0f;                                      //!< 点光源の届く距離

        // 実行時の値（JSON には書かない）
        entt::entity halo   = entt::null;    //!< 作った光輪
        float        phase  = 0.0f;          //!< 呼吸の位相（ラジアン）
        float        period = 0.0f;          //!< 呼吸の周期（0 ならまだ決めていない）
    };
}    // namespace StardustFever::ECS
