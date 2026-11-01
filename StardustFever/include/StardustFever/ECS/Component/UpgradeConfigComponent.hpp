//-------------------------------------------------------------
//! @file   UpgradeConfigComponent.hpp
//! @brief  UpgradeConfigComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct UpgradeConfigComponent
    //! @brief  強化の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct UpgradeConfigComponent {
        double autoFireUnlockCost = 2000.0;    //!< 自動発射の解禁に要る星屑
    };
}    // namespace StardustFever::ECS
