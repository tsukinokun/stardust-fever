//-------------------------------------------------------------
//! @file   BoardBuildRequestComponent.hpp
//! @brief  BoardBuildRequestComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct BoardBuildRequestComponent
    //! @brief  盤面の組み立て要求
    //! @note   BoardBuildSystem が配置データからエンティティを作り、このエンティティを消す
    //-------------------------------------------------------------
    struct BoardBuildRequestComponent {
        std::string layoutPath;    //!< BoardLayout.json のパス
    };
}    // namespace StardustFever::ECS
