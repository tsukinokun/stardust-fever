//-------------------------------------------------------------
//! @file   LauncherConfigComponent.hpp
//! @brief  LauncherConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct LauncherConfigComponent
    //! @brief  発射の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct LauncherConfigComponent {
        float          minSpeed           = 380.0f;    //!< ハンドル0のときの初速（ワールド単位/秒）
        float          maxSpeed           = 560.0f;    //!< ハンドル1のときの初速（ワールド単位/秒）
        float          fireInterval       = 0.6f;      //!< 最短の発射間隔（秒）。自動発射はこの間隔で打ち続ける
        float          autoFireInterval   = 2.4f;      //!< 自動発射の間隔の元（秒）。強化のレベルで短くなる
        float          chargeCycleSeconds = 1.2f;      //!< 溜めの強さが 0→1→0 と1往復する時間（秒）。短いほど狙うのが難しい
        float          autoFireJitter     = 0.08f;     //!< 自動発射の強さの揺らぎ幅（±）
        int            maxBallsOnBoard    = 30;        //!< 盤面に同時に出せる玉の数
        float          killPlaneY         = -20.0f;    //!< これより下へ落ちた玉は消す
        float          stuckSeconds       = 2.5f;      //!< この時間ほぼ動かない玉は止まったとみなす（秒）
        float          stuckDistance      = 0.3f;      //!< この距離より動かなければ止まっている
        float          stuckNudge         = 40.0f;     //!< 止まった玉を押す強さ（速さの変化）
        int            stuckMaxNudges     = 2;         //!< この回数押しても動かない玉はアウトにする
        hlslpp::float4 mouseChargeArea    = hlslpp::float4(440.0f, 60.0f, 720.0f, 880.0f);    //!< 長押しで溜められる盤面の範囲（左・上・幅・高さ。UI の座標）
        std::string    ballPrefab;                                                            //!< 玉のPrefab
    };
}    // namespace StardustFever::ECS
