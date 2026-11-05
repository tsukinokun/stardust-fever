//-------------------------------------------------------------
//! @file   ScoreMultiplierComponent.hpp
//! @brief  ScoreMultiplierComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct ScoreMultiplierComponent
    //! @brief  星屑の全体倍率
    //! @note   星のレベル・集めた星座の効果・宇宙定数から ScoreMultiplierSystem が計算し直す
    //-------------------------------------------------------------
    struct ScoreMultiplierComponent {
        StardustFever::Core::BigNumber total = 1.0;    //!< 全ての獲得に掛ける倍率
    };
}    // namespace StardustFever::ECS
