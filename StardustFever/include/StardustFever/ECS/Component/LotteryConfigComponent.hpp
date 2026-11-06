//-------------------------------------------------------------
//! @file   LotteryConfigComponent.hpp
//! @brief  LotteryConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LotteryConfigComponent
    //! @brief  抽選の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct LotteryConfigComponent {
        double                   winProbability     = 0.08;     //!< 1回の変動で当たる確率
        int                      maxPending         = 4;        //!< 保留の上限
        float                    spinDuration       = 1.6f;     //!< リールが回る時間（秒）
        float                    symbolInterval     = 0.06f;    //!< 回転中に絵柄を送る間隔（秒）
        float                    reachExtraDuration = 3.0f;     //!< リーチになる変動で、右のリールが余分に回る時間（秒）
        std::vector<std::string> symbols;                       //!< リールの絵柄（UTF-8）
    };
}    // namespace StardustFever::ECS
