//-------------------------------------------------------------
//! @file   StarGateRollEvent.hpp
//! @brief  StarGateRollEvent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   StarGateRollStage
    //! @brief  星の門の抽選の節目
    //-------------------------------------------------------------
    enum class StarGateRollStage : int {
        Start,    //!< 抽選が始まった
        Tick,     //!< 光が次の星へ移った
        Win,      //!< 当たった
        Miss,     //!< 外れた
    };

    //-------------------------------------------------------------
    //! @struct StarGateRollEvent
    //! @brief  星の門の抽選が進んだ
    //-------------------------------------------------------------
    struct StarGateRollEvent {
        StarGateRollStage stage = StarGateRollStage::Start;    //!< 節目
    };
}    // namespace StardustFever::ECS
