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
        std::string upgradeId;    //!< 買った強化
        int         newLevel;     //!< 買った後のレベル
    };
}    // namespace StardustFever::ECS
