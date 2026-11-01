//-------------------------------------------------------------
//! @file   FeverConfigComponent.hpp
//! @brief  FeverConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverConfigComponent
    //! @brief  フィーバーの調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct FeverConfigComponent {
        float       duration              = 45.0f;    //!< フィーバーの長さ（秒）
        float       endDelayAfterComplete = 3.0f;     //!< 星座が完成してから終わるまでの余韻（秒）
        std::string starPinPrefab;                    //!< 星釘のPrefab
        float       minStarToPocketGap = 3.0f;        //!< 星と役物の間に要る隙間（玉の直径）。足りないと警告を出す
    };
}    // namespace StardustFever::ECS
