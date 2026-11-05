//-------------------------------------------------------------
//! @file   StarLinkedEvent.hpp
//! @brief  StarLinkedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarLinkedEvent
    //! @brief  星ゲットチャンスで星がつながった
    //-------------------------------------------------------------
    struct StarLinkedEvent {
        std::string    starId;      //!< つながった星
        hlslpp::float3 position;    //!< つながった星の位置（盤面のワールド座標。粒子を出す場所）
    };
}    // namespace StardustFever::ECS
