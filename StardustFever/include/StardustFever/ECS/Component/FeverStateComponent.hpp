//-------------------------------------------------------------
//! @file   FeverStateComponent.hpp
//! @brief  FeverStateComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <entt/entt.hpp>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct FeverStateComponent
    //! @brief  フィーバー「星詠みタイム」の状態
    //! @note   ゲーム状態エンティティに1つだけ付く
    //-------------------------------------------------------------
    struct FeverStateComponent {
        bool         active        = false;         //!< フィーバー中か
        float        timeRemaining = 0.0f;          //!< 残り時間（秒）
        float        timeTotal     = 0.0f;          //!< 始まったときの長さ（秒。残り時間のゲージに使う）
        std::string  constellationId;               //!< 出している星座
        int          starsLinked   = 0;             //!< つないだ星の数
        int          starsTotal    = 0;             //!< 星座の星の数
        entt::entity firstLinkBall = entt::null;    //!< 最初に星をつないだ玉（一筆書き判定用）
        bool         perfect       = true;          //!< ここまで全て同じ玉でつないでいるか
        bool         completed     = false;         //!< 星座が完成したか
        float        endDelay      = 0.0f;          //!< 完成してから終わるまでの残り時間（秒）
        int          bestChain     = 0;             //!< このフィーバーで1球がつないだ最大数
    };
}    // namespace StardustFever::ECS
