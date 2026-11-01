//-------------------------------------------------------------
//! @file   SaveConfigComponent.hpp
//! @brief  SaveConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SaveConfigComponent
    //! @brief  セーブの調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct SaveConfigComponent {
        std::string savePath         = "Saves/save.json";    //!< セーブファイルのパス（作業ディレクトリからの相対）
        float       autoSaveInterval = 30.0f;                //!< 自動保存の間隔（秒）
    };
}    // namespace StardustFever::ECS
