//-------------------------------------------------------------
//! @file   StarPinComponent.hpp
//! @brief  StarPinComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarPinComponent
    //! @brief  フィーバー中に盤面へ出る星釘
    //! @note   玉が触れると星座の一部としてつながる
    //-------------------------------------------------------------
    struct StarPinComponent {
        std::string starId;               //!< StarDef::id
        int         starIndex = -1;       //!< ConstellationDef::stars の添字
        bool        linked    = false;    //!< つながったか
    };
}    // namespace StardustFever::ECS
