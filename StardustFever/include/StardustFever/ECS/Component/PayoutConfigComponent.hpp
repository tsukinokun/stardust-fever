//-------------------------------------------------------------
//! @file   PayoutConfigComponent.hpp
//! @brief  PayoutConfigComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PayoutConfigComponent
    //! @brief  星屑の払い出しの調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct PayoutConfigComponent {
        double pinHitPayout = 1.0;    //!< 釘に1回当たったときの基本の星屑
    };
}    // namespace StardustFever::ECS
