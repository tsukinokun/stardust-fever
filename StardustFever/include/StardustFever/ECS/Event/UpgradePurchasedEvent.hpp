//-------------------------------------------------------------
//! @file   UpgradePurchasedEvent.hpp
//! @brief  UpgradePurchasedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct UpgradePurchasedEvent
    //! @brief  強化を買った
    //-------------------------------------------------------------
    struct UpgradePurchasedEvent {
        std::string upgradeId;           //!< 買った強化
        int         newLevel;            //!< 買った後のレベル
        bool        repeated = false;    //!< 長押しの2回目以降か
    };

    //-------------------------------------------------------------
    //! @struct UpgradeHoldEndedEvent
    //! @brief  強化の長押しが終わった（途中の購入はまとめてここで保存する）
    //-------------------------------------------------------------
    struct UpgradeHoldEndedEvent {};
}    // namespace StardustFever::ECS
