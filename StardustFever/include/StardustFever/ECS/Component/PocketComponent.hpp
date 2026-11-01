//-------------------------------------------------------------
//! @file   PocketComponent.hpp
//! @brief  PocketComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PocketComponent
    //! @brief  入賞口
    //! @note   玉が入ると星屑が出て、玉は消える
    //-------------------------------------------------------------
    struct PocketComponent {
        double basePayout    = 10.0;     //!< 入賞したときの基本の星屑
        bool   isStartPocket = false;    //!< スタート入賞口（ヘソ）なら抽選の保留が1つ増える
    };
}    // namespace StardustFever::ECS
