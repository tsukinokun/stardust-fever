//-------------------------------------------------------------
//! @file   HudConfigComponent.hpp
//! @brief  HudConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct HudConfigComponent
    //! @brief  HUDの調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct HudConfigComponent {
        float       messageDuration     = 2.5f;    //!< メッセージを出しておく時間（秒）
        int         handleGaugeSegments = 10;      //!< ハンドルのゲージの目盛りの数
        std::string helpText;                      //!< 操作説明（UTF-8）
    };
}    // namespace StardustFever::ECS
