//-------------------------------------------------------------
//! @file   CollectionModifiersComponent.hpp
//! @brief  CollectionModifiersComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct CollectionModifiersComponent
    //! @brief  集めた星と星座の効果をまとめたもの
    //! @note   ゲーム状態エンティティに1つだけ付く。CollectionEffectSystem が毎フレーム計算し直し、
    //!         効果を使う System はこれを読むだけにする（星や星座のデータを直接見ない）。
    //!         星1つごとの全体倍率は大きくなりすぎるので BigNumber のまま ScoreMultiplierSystem が計算する
    //-------------------------------------------------------------
    struct CollectionModifiersComponent {
        double globalMultiplier        = 1.0;      //!< 星座の効果と、88星座がそろったときの全体倍率
        double pinPayoutMultiplier     = 1.0;      //!< 釘で得る星屑に掛ける倍率
        double pocketPayoutMultiplier  = 1.0;      //!< 入賞口で得る星屑に掛ける倍率
        double lotteryChanceBonus      = 0.0;      //!< フィーバーの当選確率に足す量
        double starChanceBonus         = 0.0;      //!< 星ゲットチャンスの当選確率に足す量
        float  feverDurationBonus      = 0.0f;     //!< フィーバーの長さに足す秒数
        int    ownedStars              = 0;        //!< 持っている星の数
        int    totalStars              = 0;        //!< 星の総数
        int    collectedConstellations = 0;        //!< 星を全部そろえた星座の数
        int    totalConstellations     = 0;        //!< 星座の総数
        bool   allComplete             = false;    //!< 全ての星座をそろえたか
        int    collectedGroups         = 0;        //!< 星座を全部そろえたグループの数
        int    totalGroups             = 0;        //!< グループの総数
        int    ownedObjects            = 0;        //!< 持っている天体の数
        int    totalObjects            = 0;        //!< 天体の総数
    };
}    // namespace StardustFever::ECS
