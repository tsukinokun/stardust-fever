//-------------------------------------------------------------
//! @file   StarLinkedEvent.hpp
//! @brief  StarLinkedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarLinkedEvent
    //! @brief  星がつながった
    //-------------------------------------------------------------
    struct StarLinkedEvent {
        std::string starId;              //!< つながった星
        int         chain;               //!< この玉がつないだ何個目か
        double      strokeMultiplier;    //!< 一筆書きの倍率
    };
}    // namespace StardustFever::ECS
