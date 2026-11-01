//-------------------------------------------------------------
//! @file   DrainComponent.hpp
//! @brief  DrainComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct DrainComponent
    //! @brief  アウト口
    //! @note   玉が入ると何も出ずに消える
    //-------------------------------------------------------------
    struct DrainComponent {
        bool isFoul = false;    //!< 発射レーンへ戻ってきた玉（ファウル）用か
    };
}    // namespace StardustFever::ECS
