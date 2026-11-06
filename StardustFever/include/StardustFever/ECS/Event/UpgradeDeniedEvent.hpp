//-------------------------------------------------------------
//! @file   UpgradeDeniedEvent.hpp
//! @brief  UpgradeDeniedEvent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct UpgradeDeniedEvent
    //! @brief  強化を買えなかった（星屑が足りない、または最大レベル）
    //-------------------------------------------------------------
    struct UpgradeDeniedEvent {
        bool denied = true;    //!< 空型にしないための値
    };
}    // namespace StardustFever::ECS
