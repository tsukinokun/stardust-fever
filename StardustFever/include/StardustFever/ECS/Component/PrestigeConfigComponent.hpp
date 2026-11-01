//-------------------------------------------------------------
//! @file   PrestigeConfigComponent.hpp
//! @brief  PrestigeConfigComponent の宣言
//-------------------------------------------------------------
#pragma once

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
        float  confirmWindow    = 3.0f;         //!< 1回目の B から2回目を受け付ける時間（秒）
    };
}    // namespace StardustFever::ECS
