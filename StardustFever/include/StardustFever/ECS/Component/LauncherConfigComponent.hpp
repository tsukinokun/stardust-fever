//-------------------------------------------------------------
//! @file   LauncherConfigComponent.hpp
//! @brief  LauncherConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LauncherConfigComponent
    //! @brief  発射の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct LauncherConfigComponent {
        float       minSpeed           = 380.0f;    //!< ハンドル0のときの初速（ワールド単位/秒）
        float       maxSpeed           = 560.0f;    //!< ハンドル1のときの初速（ワールド単位/秒）
        float       fireInterval       = 0.6f;      //!< 最短の発射間隔（秒）。自動発射はこの間隔で打ち続ける
        float       chargeCycleSeconds = 1.2f;      //!< 溜めの強さが 0→1→0 と1往復する時間（秒）。短いほど狙うのが難しい
        float       autoFireJitter     = 0.08f;     //!< 自動発射の強さの揺らぎ幅（±）
        int         maxBallsOnBoard    = 30;        //!< 盤面に同時に出せる玉の数
        float       killPlaneY         = -20.0f;    //!< これより下へ落ちた玉は消す
        std::string ballPrefab;                     //!< 玉のPrefab
    };
}    // namespace StardustFever::ECS
