//-------------------------------------------------------------
//! @file   StarCollectionComponent.hpp
//! @brief  StarCollectionComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>
#include <unordered_map>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarCollectionComponent
    //! @brief  集めた星と完成させた星座
    //! @note   ゲーム状態エンティティに1つだけ付く
    //-------------------------------------------------------------
    struct StarCollectionComponent {
        std::unordered_map<std::string, int> starLevels;                  //!< 星ID → レベル（未獲得は載らない）
        std::unordered_map<std::string, int> constellationCompletions;    //!< 星座ID → 完成回数
    };
}    // namespace StardustFever::ECS
