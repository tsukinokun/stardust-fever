//-------------------------------------------------------------
//! @file   PocketComponent.hpp
//! @brief  PocketComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PocketComponent
    //! @brief  入賞口。玉が入ると星屑が出て、玉は消える
    //! @note   role で入ったときの働きを分ける
    //!         - "start"    : フィーバーの抽選を1つ保留する（スタート入賞口）
    //!         - "starGate" : 星ゲットチャンスの抽選をする（星の門）
    //!         - "normal"   : 星屑だけ（横の入賞口。フィーバー中は広がる）
    //-------------------------------------------------------------
    struct PocketComponent {
        double      basePayout = 10.0;        //!< 入ったときの基本の星屑
        std::string role       = "normal";    //!< 入賞口の働き
    };
}    // namespace StardustFever::ECS
