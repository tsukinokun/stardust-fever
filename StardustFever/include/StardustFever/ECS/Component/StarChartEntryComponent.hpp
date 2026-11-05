//-------------------------------------------------------------
//! @file   StarChartEntryComponent.hpp
//! @brief  StarChartEntryComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarChartEntryComponent
    //! @brief  星図の星1つ分の行
    //! @note   StarChartViewSystem が作る
    //-------------------------------------------------------------
    struct StarChartEntryComponent {
        std::string starId;             //!< 表す星
        int         level = -1;         //!< 前のフレームのレベル（上がった時だけ弾ませる）
        std::string constellationId;    //!< 見出しの行なら表す星座（星の行なら空）
    };
}    // namespace StardustFever::ECS
