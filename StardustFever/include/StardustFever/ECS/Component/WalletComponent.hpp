//-------------------------------------------------------------
//! @file   WalletComponent.hpp
//! @brief  WalletComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct WalletComponent
    //! @brief  所持している星屑
    //! @note   ゲーム状態エンティティに1つだけ付く
    //-------------------------------------------------------------
    struct WalletComponent {
        StardustFever::Core::BigNumber stardust;    //!< 所持している星屑
    };
}    // namespace StardustFever::ECS
