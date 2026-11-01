//-------------------------------------------------------------
//! @file   StarLinkConfigComponent.hpp
//! @brief  StarLinkConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarLinkConfigComponent
    //! @brief  星つなぎの調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct StarLinkConfigComponent {
        double      strokeBase        = 2.0;    //!< 一筆書きの倍率の底。1球で n 個目なら strokeBase^(n-1)
        double      perfectMultiplier = 5.0;    //!< 1球で星座を完成させたときの完成ボーナスの倍率
        int         maxStarLevel      = 5;      //!< 星のレベルの上限
        std::string linkLinePrefab;             //!< 星座の線のPrefab
        std::string litStarModel;               //!< つながった星のモデル
        float       lineThickness = 0.3f;       //!< 星座の線の太さの半分
    };
}    // namespace StardustFever::ECS
