//-------------------------------------------------------------
//! @file   PrestigeConfigComponent.hpp
//! @brief  PrestigeConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PrestigeConfigComponent
    //! @brief  ビッグバンの調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct PrestigeConfigComponent {
        double threshold        = 1000000.0;    //!< 宇宙定数1つに要る、この宇宙の累計
        double exponent         = 0.5;          //!< 累計に対する宇宙定数の伸び方（0.5 なら平方根）
        double bonusPerConstant = 0.1;          //!< 宇宙定数1つあたりに全体倍率へ足す量

        float          readyPulseScale  = 1.12f;                                           //!< できる間にボタンが弾む大きさ（倍率）
        float          readyPulsePeriod = 0.9f;                                            //!< 弾む周期（秒）
        hlslpp::float4 readyHintColor   = hlslpp::float4(1.0f, 0.8f, 0.3f, 1.0f);          //!< できる間の案内文の色
        hlslpp::float4 hintArea         = hlslpp::float4(39.0f, 184.0f, 352.0f, 26.0f);    //!< 案内文の押せる範囲（左, 上, 幅, 高さ）
    };
}    // namespace StardustFever::ECS
