//-------------------------------------------------------------
//! @file   LotteryStateComponent.hpp
//! @brief  LotteryStateComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <array>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LotteryStateComponent
    //! @brief  黄道十二宮リールの抽選状態
    //! @note   ゲーム状態エンティティに1つだけ付く
    //-------------------------------------------------------------
    struct LotteryStateComponent {
        int                pending      = 0;            //!< 保留の数
        bool               spinning     = false;        //!< リールが回っているか
        float              spinTimer    = 0.0f;         //!< 止まるまでの残り時間（秒）
        float              symbolTimer  = 0.0f;         //!< 次に絵柄を送るまでの時間（秒）
        bool               willWin      = false;        //!< 今回の変動が当たりか（回り始めに決める）
        std::array<int, 3> symbols      = {0, 1, 2};    //!< 表示中の絵柄（LotteryConfigComponent::symbols の添字）
        std::array<int, 3> finalSymbols = {0, 1, 2};    //!< 止まったときの絵柄（回り始めに決める）
    };
}    // namespace StardustFever::ECS
