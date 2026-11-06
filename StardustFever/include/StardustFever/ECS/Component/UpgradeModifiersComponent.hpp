//-------------------------------------------------------------
//! @file   UpgradeModifiersComponent.hpp
//! @brief  UpgradeModifiersComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct UpgradeModifiersComponent
    //! @brief  強化の効果をまとめたもの
    //! @note   ゲーム状態エンティティに1つだけ付く。UpgradeEffectSystem が毎フレーム計算し直し、
    //!         効果を使う System はこれを読むだけにする（強化のデータやレベルを直接見ない）
    //-------------------------------------------------------------
    struct UpgradeModifiersComponent {
        double pinPayoutMultiplier    = 1.0;      //!< 釘で得る星屑に掛ける倍率
        double pocketPayoutMultiplier = 1.0;      //!< 入賞口で得る星屑に掛ける倍率
        double lotteryChanceBonus     = 0.0;      //!< 抽選の当選確率に足す量
        float  feverDurationBonus     = 0.0f;     //!< フィーバーの長さに足す秒数
        double starChanceBonus        = 0.0;      //!< 星ゲットチャンスの当選確率に足す量
        float  autoFireJitterScale    = 1.0f;     //!< 自動発射の揺らぎ幅に掛ける倍率
        float  autoFireIntervalScale  = 1.0f;     //!< 自動発射の間隔に掛ける倍率
        bool   autoFireUnlocked       = false;    //!< 自動発射を解禁したか
    };
}    // namespace StardustFever::ECS
