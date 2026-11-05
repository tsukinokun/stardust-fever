//-------------------------------------------------------------
//! @file   HiddenPinComponent.hpp
//! @brief  HiddenPinComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <Tsukino/BuiltIn/ECS/Component/CollisionComponent.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HiddenPinComponent
    //! @brief  星ゲットチャンスの間だけ隠している普通の釘
    //! @note   星釘と重なる釘は、見えなくして当たり判定を外す（CollisionComponent を外すと物理の Body も消える）。
    //!         外した当たり判定をここへ取っておき、チャンスが終わったら付け直す。実行時だけの値
    //-------------------------------------------------------------
    struct HiddenPinComponent {
        Tsukino::BuiltIn::ECS::CollisionComponent collision;    //!< 外した当たり判定
    };
}    // namespace StardustFever::ECS
