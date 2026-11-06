//-------------------------------------------------------------
//! @file   StarChanceStateComponent.hpp
//! @brief  StarChanceStateComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarChanceStateComponent
    //! @brief  星ゲットチャンス（星座をつなぐミニゲーム）の状態
    //! @note   ゲーム状態エンティティに1つだけ付く
    //-------------------------------------------------------------
    struct StarChanceStateComponent {
        bool        active = false;           //!< チャンス中か
        std::string constellationId;          //!< 出している星座
        int         starsLinked   = 0;        //!< つないだ星の数
        int         starsTotal    = 0;        //!< 盤面に出した星の数
        float       timeRemaining = 0.0f;     //!< 残り時間（秒）
        float       timeTotal     = 0.0f;     //!< 始まったときの長さ（秒）
        bool        completed     = false;    //!< 全部つないだか
        float       endDelay      = 0.0f;     //!< 全部つないでから終わるまでの残り時間（秒）
        bool        rolling       = false;    //!< 星の門の抽選の演出中か
        float       rollTimer     = 0.0f;     //!< 抽選の残り時間（秒）
        float       rollTotal     = 0.0f;     //!< 抽選の長さ（秒）
        bool        rollWin       = false;    //!< 抽選の結果（始めに決める）
        bool        rollGold      = false;    //!< 輪を金色にするか（当たりやすさの予告）
    };
}    // namespace StardustFever::ECS
