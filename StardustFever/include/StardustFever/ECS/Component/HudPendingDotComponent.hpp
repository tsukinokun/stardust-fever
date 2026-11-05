//-------------------------------------------------------------
//! @file   HudPendingDotComponent.hpp
//! @brief  HudPendingDotComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudPendingDotComponent
    //! @brief  抽選の保留を表す光の玉
    //! @note   index 番目の保留があれば明るく光る
    //-------------------------------------------------------------
    struct HudPendingDotComponent {
        int  index   = 0;        //!< 何番目の保留か
        bool lit     = false;    //!< 光っているか（光った瞬間に弾ませるため）
        bool applied = false;    //!< 見た目を1度でも当てたか（最初のフレームに、保留の無い玉を沈めるため）
    };
}    // namespace StardustFever::ECS
