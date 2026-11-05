//-------------------------------------------------------------
//! @file   ToastComponent.hpp
//! @brief  ToastComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct ToastComponent
    //! @brief  右下に積む通知1枚
    //! @note   ToastSystem が作り、積む位置へ寄せてから寿命で消す
    //-------------------------------------------------------------
    struct ToastComponent {
        float targetY  = 0.0f;     //!< 寄せる先の Y（UI の座標）
        float age      = 0.0f;     //!< 出てからの時間（秒）
        float lifetime = 3.0f;     //!< 消え始めるまでの時間（秒）
        bool  leaving  = false;    //!< 消えるトゥイーンを始めたか
        bool  headline = false;    //!< 画面中央の大見出しか（積まずに1枚だけ出す）
    };
}    // namespace StardustFever::ECS
