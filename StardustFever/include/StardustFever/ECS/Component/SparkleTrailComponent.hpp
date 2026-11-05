//-------------------------------------------------------------
//! @file   SparkleTrailComponent.hpp
//! @brief  SparkleTrailComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <Tsukino/BuiltIn/ECS/Particle/BurstEmitter.hpp>

#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SparkleTrailComponent
    //! @brief  動いている間、後ろに星屑の粒子を撒く（玉の尾）
    //! @note   SparkleTrailSystem が interval ごとに burst を1回撒く。止まりかけの玉は撒かない。
    //!         フィーバー中は粒子の色を feverColorA / feverColorB に替える
    //-------------------------------------------------------------
    struct SparkleTrailComponent {
        std::string                        texture;                                                 //!< 粒子の画像
        float                              interval = 0.04f;                                        //!< 撒く間隔（秒）
        float                              minSpeed = 8.0f;                                         //!< これより遅いと撒かない（1秒あたりの距離）
        Tsukino::BuiltIn::ECS::BurstParams burst;                                                   //!< 1回に撒く粒子
        hlslpp::float4                     feverColorA = hlslpp::float4(2.4f, 1.9f, 0.8f, 1.0f);    //!< フィーバー中の色 A
        hlslpp::float4                     feverColorB = hlslpp::float4(2.0f, 1.2f, 2.4f, 1.0f);    //!< フィーバー中の色 B

        // 実行時の値（JSON には書かない）
        float          timer        = 0.0f;                                //!< 次に撒くまでの残り
        hlslpp::float3 lastPosition = hlslpp::float3(0.0f, 0.0f, 0.0f);    //!< 前のフレームの位置（速さを測る）
        bool           hasLast      = false;                               //!< lastPosition が入っているか
    };
}    // namespace StardustFever::ECS
