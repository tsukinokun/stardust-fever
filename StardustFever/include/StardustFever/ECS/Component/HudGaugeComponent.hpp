//-------------------------------------------------------------
//! @file   HudGaugeComponent.hpp
//! @brief  HudGaugeComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudGaugeComponent
    //! @brief  値を横の長さで見せるゲージ
    //! @note   SpriteComponent の fillAmount と色を HudGaugeSystem が書く
    //-------------------------------------------------------------
    struct HudGaugeComponent {
        int            source      = 0;    //!< 見せる値（0: 発射の強さ、1: フィーバーの残り時間、2: 次の宇宙定数までの進み）
        hlslpp::float4 colorLow    = hlslpp::float4(0.35f, 0.65f, 1.0f, 1.0f);    //!< 値が小さいときの色
        hlslpp::float4 colorMid    = hlslpp::float4(1.0f, 0.82f, 0.35f, 1.0f);    //!< 値が半分のときの色
        hlslpp::float4 colorHigh   = hlslpp::float4(1.0f, 0.38f, 0.32f, 1.0f);    //!< 値が大きいときの色
        float          smoothing   = 14.0f;                                       //!< 表示を値へ寄せる速さ（大きいほど素早い）
        float          pulseAmount = 0.06f;                                       //!< 溜めている間の脈打ちの大きさ（発射の強さのゲージだけ）
        float          shown       = 0.0f;                                        //!< 表示中の長さ（0〜1）
    };
}    // namespace StardustFever::ECS
