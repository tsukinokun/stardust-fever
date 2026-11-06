//-------------------------------------------------------------
//! @file   UpgradeShopConfigComponent.hpp
//! @brief  UpgradeShopConfigComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct UpgradeShopConfigComponent
    //! @brief  強化の長押しの調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct UpgradeShopConfigComponent {
        float repeatDelay        = 0.35f;    //!< 押してから連続で買い始めるまでの秒数
        float repeatInterval     = 0.16f;    //!< 連続で買う最初の間隔（秒）
        float repeatMinInterval  = 0.04f;    //!< 連続で買う一番短い間隔（秒）
        float repeatAcceleration = 0.9f;     //!< 1回買うごとに間隔へ掛ける値
    };
}    // namespace StardustFever::ECS
