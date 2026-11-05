//-------------------------------------------------------------
//! @file   FeverConfigComponent.hpp
//! @brief  FeverConfigComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverConfigComponent
    //! @brief  フィーバー「星詠みタイム」（星屑の稼ぎ時）の調整値。チューリップの開け閉めは TulipWingComponent
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct FeverConfigComponent {
        float  duration               = 45.0f;    //!< フィーバーの長さ（秒）
        double pinPayoutMultiplier    = 3.0;      //!< フィーバー中に釘の星屑へ掛ける倍率
        double pocketPayoutMultiplier = 3.0;      //!< フィーバー中に入賞の星屑へ掛ける倍率
    };
}    // namespace StardustFever::ECS
