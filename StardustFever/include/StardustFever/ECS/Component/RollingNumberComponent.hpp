//-------------------------------------------------------------
//! @file   RollingNumberComponent.hpp
//! @brief  RollingNumberComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct RollingNumberComponent
    //! @brief  転がるように増える数字
    //! @note   表示中の値を本当の値へ追いかけさせる。大きく増えると弾み、減ると一瞬赤く沈む
    //-------------------------------------------------------------
    struct RollingNumberComponent {
        StardustFever::Core::BigNumber displayed;                                                       //!< 表示中の値（RollingNumberSystem が書く）
        float                          catchUpRate       = 6.0f;                                        //!< 追いかける速さ（1秒あたり、残りの桁差を詰める割合）
        float                          punchRatio        = 0.25f;                                       //!< 表示がこの割合以上ふえたら弾ませる
        float                          punchAmount       = 0.12f;                                       //!< 弾むときに拡大率へ足す量
        float                          punchCooldown     = 0.18f;                                       //!< 弾ませる最短の間隔（秒）
        hlslpp::float4                 spendColor        = hlslpp::float4(1.0f, 0.45f, 0.45f, 1.0f);    //!< 減ったときに一瞬なる色
        float                          spendFlashSeconds = 0.35f;                                       //!< 減ったときの色が戻るまでの時間（秒）
        float                          cooldownTimer     = 0.0f;                                        //!< 次に弾ませられるまでの残り時間（秒）
        float                          flashTimer        = 0.0f;                                        //!< 減ったときの色の残り時間（秒）
        hlslpp::float4                 baseColor         = hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);    //!< 元の文字の色（初めて見たときに FontComponent から取る）
        bool                           initialized       = false;                               //!< 表示中の値と元の色を取ったか
    };
}    // namespace StardustFever::ECS
