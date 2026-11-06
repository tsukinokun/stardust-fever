//-------------------------------------------------------------
//! @file   StarCollectionComponent.hpp
//! @brief  StarCollectionComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarCollectionComponent
    //! @brief  集めた星
    //! @note   ゲーム状態エンティティに1つだけ付く。星座がそろったかは、ここから毎回求める
    //-------------------------------------------------------------
    struct StarCollectionComponent {
        std::vector<std::string>             skyObjects;       //!< 手に入れた天体（手に入れた順）
        std::vector<std::string>             unseenObjects;    //!< 星図でまだ見ていない天体
        std::unordered_map<std::string, int> starLevels;       //!< 星ID → レベル（未獲得は載らない）
        std::vector<std::string>             unseenStars;      //!< 星図でまだ見ていない星（古い順。セーブに残す）
        std::vector<std::string>             recentStars;      //!< 最近手に入れた星（新しい順。実行時だけの値）
    };
}    // namespace StardustFever::ECS
