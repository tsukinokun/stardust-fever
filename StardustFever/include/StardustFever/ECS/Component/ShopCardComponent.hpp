//-------------------------------------------------------------
//! @file   ShopCardComponent.hpp
//! @brief  ShopCardComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct ShopCardComponent
    //! @brief  強化ショップのカード1枚
    //! @note   ShopViewSystem が作り、表す強化の並び順を持つ
    //-------------------------------------------------------------
    struct ShopCardComponent {
        int         index = -1;    //!< Upgrades.json の何番目の強化か
        std::string upgradeId;     //!< 表す強化のID（購入イベントと突き合わせる）
        int         state = -1;    //!< 前のフレームの状態（0: 買えない、1: 買える、2: MAX）。変わった時だけ見た目を切り替える
    };
}    // namespace StardustFever::ECS
