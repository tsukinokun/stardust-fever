//-------------------------------------------------------------
//! @file   HudPartComponent.hpp
//! @brief  HudPartComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudPartComponent
    //! @brief  パネルの部品の名前
    //! @note   雛形のグループから作ったパネルの中で、System が部品を探すための名前
    //-------------------------------------------------------------
    struct HudPartComponent {
        std::string name;    //!< 部品の名前（例: "Cost"）
    };
}    // namespace StardustFever::ECS
