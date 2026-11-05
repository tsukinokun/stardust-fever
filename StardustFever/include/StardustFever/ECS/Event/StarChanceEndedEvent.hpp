//-------------------------------------------------------------
//! @file   StarChanceEndedEvent.hpp
//! @brief  StarChanceEndedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarChanceEndedEvent
    //! @brief  星ゲットチャンスが終わった
    //-------------------------------------------------------------
    struct StarChanceEndedEvent {
        std::string constellationId;    //!< 出していた星座
        bool        success;            //!< 全部つないだか
        int         starsLinked;        //!< つないだ星の数
        int         starsTotal;         //!< 盤面に出した星の数
    };
}    // namespace StardustFever::ECS
