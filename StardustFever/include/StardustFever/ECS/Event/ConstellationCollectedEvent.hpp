//-------------------------------------------------------------
//! @file   ConstellationCollectedEvent.hpp
//! @brief  ConstellationCollectedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct ConstellationCollectedEvent
    //! @brief  星座の星が全部そろった（固有の効果が発動した）
    //-------------------------------------------------------------
    struct ConstellationCollectedEvent {
        std::string constellationId;    //!< そろった星座
    };
}    // namespace StardustFever::ECS
