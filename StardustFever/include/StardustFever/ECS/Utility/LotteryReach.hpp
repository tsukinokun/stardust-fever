//-------------------------------------------------------------
//! @file   LotteryReach.hpp
//! @brief  リーチかどうかの判定
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    //! @brief  リーチかどうか
    //! @param  state [in] 抽選の状態
    //! @return 左と中が止まって同じ絵柄で、右だけが回っていれば true
    //-------------------------------------------------------------
    inline bool IsReach(const LotteryStateComponent& state) {
        return state.spinning && state.reelStopped[0] && state.reelStopped[1] && !state.reelStopped[2] && state.symbols[0] == state.symbols[1];
    }

}    // namespace StardustFever::ECS
