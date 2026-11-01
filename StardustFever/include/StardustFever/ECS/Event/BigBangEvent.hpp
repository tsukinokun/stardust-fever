//-------------------------------------------------------------
//! @file   BigBangEvent.hpp
//! @brief  BigBangEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BigBangEvent
    //! @brief  ビッグバン（転生）が起きた
    //-------------------------------------------------------------
    struct BigBangEvent {
        StardustFever::Core::BigNumber gained;            //!< 今回得た宇宙定数
        StardustFever::Core::BigNumber totalConstants;    //!< 得た後の宇宙定数
    };
}    // namespace StardustFever::ECS
