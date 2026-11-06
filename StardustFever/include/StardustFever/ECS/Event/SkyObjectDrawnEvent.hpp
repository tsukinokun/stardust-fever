//-------------------------------------------------------------
//! @file   SkyObjectDrawnEvent.hpp
//! @brief  SkyObjectDrawnEvent・SkyObjectShownEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SkyObjectDrawnEvent
    //! @brief  ビッグバンで天体を手に入れた
    //-------------------------------------------------------------
    struct SkyObjectDrawnEvent {
        std::string objectId;    //!< 手に入れた天体
    };

    //-------------------------------------------------------------
    //! @struct SkyObjectShownEvent
    //! @brief  手に入れた天体のカードが現れた（効果音のきっかけ）
    //-------------------------------------------------------------
    struct SkyObjectShownEvent {
        int rarity = 0;    //!< レア度
    };
}    // namespace StardustFever::ECS
