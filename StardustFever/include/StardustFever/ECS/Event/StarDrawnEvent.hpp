//-------------------------------------------------------------
//! @file   StarDrawnEvent.hpp
//! @brief  StarDrawnEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarDrawnEvent
    //! @brief  星ゲットチャンスで星を引いた
    //-------------------------------------------------------------
    struct StarDrawnEvent {
        std::string starId;             //!< 引いた星
        int         level;              //!< 加えた後のレベル
        bool        isNew;              //!< 初めて手に入れたか
        bool        converted;          //!< 最大レベルだったので星屑に換えたか
        std::string followUp;           //!< 演出のあとに出す大見出し（星座がそろったときなど。無ければ空）
        std::string constellationId;    //!< つないだ星座
    };
}    // namespace StardustFever::ECS
