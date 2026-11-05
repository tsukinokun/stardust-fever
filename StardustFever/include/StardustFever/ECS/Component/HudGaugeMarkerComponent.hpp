//-------------------------------------------------------------
//! @file   HudGaugeMarkerComponent.hpp
//! @brief  HudGaugeMarkerComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudGaugeMarkerComponent
    //! @brief  ゲージの上の印
    //! @note   親のゲージの幅に合わせて、最後に打った強さの位置へ動く
    //-------------------------------------------------------------
    struct HudGaugeMarkerComponent {
        float followSpeed = 18.0f;    //!< 印が新しい位置へ寄る速さ
    };
}    // namespace StardustFever::ECS
