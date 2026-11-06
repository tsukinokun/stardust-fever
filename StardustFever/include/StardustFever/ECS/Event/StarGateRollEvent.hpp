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
        Tick,     //!< 星座が入れ替わった・星が1つ灯った
        Hang,     //!< 星座が止まり、星が灯り始めた
        Fake,     //!< 外れたと見せ始めた
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
