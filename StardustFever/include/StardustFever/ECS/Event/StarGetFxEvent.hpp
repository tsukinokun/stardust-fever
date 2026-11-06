//-------------------------------------------------------------
//! @file   StarGetFxEvent.hpp
//! @brief  StarGetFxEvent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarGetFxEvent
    //! @brief  星の演出の節目（効果音のきっかけ）
    //-------------------------------------------------------------
    struct StarGetFxEvent {
        bool arrived = false;    //!< false: 飛び始めた、true: ボタンへ届いた
    };
}    // namespace StardustFever::ECS
