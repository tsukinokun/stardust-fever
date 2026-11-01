//-------------------------------------------------------------
//! @file   UpgradeLevelsComponent.hpp
//! @brief  UpgradeLevelsComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>
#include <unordered_map>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct UpgradeLevelsComponent
    //! @brief  買った強化のレベル
    //! @note   ゲーム状態エンティティに1つだけ付く。効果の計算は UpgradeEffectSystem が行う
    //-------------------------------------------------------------
    struct UpgradeLevelsComponent {
        std::unordered_map<std::string, int> levels;    //!< 強化ID → レベル（未購入は載らない）
    };
}    // namespace StardustFever::ECS
