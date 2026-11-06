//-------------------------------------------------------------
//! @file   KeyHintComponent.hpp
//! @brief  KeyHintComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct KeyHintComponent
    //! @brief  キー操作の表示の印。設定で隠せる
    //-------------------------------------------------------------
    struct KeyHintComponent {
        bool mark = true;    //!< 空型にしないための値
    };
}    // namespace StardustFever::ECS
