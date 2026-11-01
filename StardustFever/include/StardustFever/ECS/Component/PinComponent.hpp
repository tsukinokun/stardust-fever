//-------------------------------------------------------------
//! @file   PinComponent.hpp
//! @brief  PinComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PinComponent
    //! @brief  釘
    //! @note   当たると星屑が少し出る。中身は無く、種類を見分けるためのタグ
    //-------------------------------------------------------------
    struct PinComponent {
        bool tag = true;    //!< EnTT の空型の特別扱いを避けるための値（PrefabFactory が GetComponent するため）
    };
}    // namespace StardustFever::ECS
