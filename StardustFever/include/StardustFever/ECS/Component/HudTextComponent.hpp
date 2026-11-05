//-------------------------------------------------------------
//! @file   HudTextComponent.hpp
//! @brief  HudTextComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   HudTextKind
    //! @brief  HUDの文字が何を表示するか（JSON には整数で書く）
    //! @note   1つの文字は1つの値だけを出す。ラベルと数字を別の文字にして、大きさと色に差を付けるため
    //-------------------------------------------------------------
    enum class HudTextKind : int {
        Static         = 0,     //!< text をそのまま出す（ラベルやキーの名前）
        Stardust       = 1,     //!< 所持している星屑（RollingNumberComponent が付いていれば転がる値）
        Multiplier     = 2,     //!< 全体倍率（"×4.50"）
        Constants      = 3,     //!< 宇宙定数（"Λ2"）
        ConstantsBonus = 4,     //!< 宇宙定数の倍率（"×1.20"）
        PrestigeHint   = 5,     //!< ビッグバンで得られる量、または次までの残り
        Reel           = 6,     //!< index 番目のリールの絵柄
        LaunchState    = 7,     //!< 発射の状態（"溜め中" / "強さ"）
        LaunchPercent  = 8,     //!< 発射の強さ（"63%"）
        AutoFire       = 9,     //!< 自動発射の状態
        FeverTitle     = 10,    //!< 帯の中身（チャンス中は星座名、フィーバー中は倍率）
        FeverProgress  = 11,    //!< チャンス中につないだ星の数（"3 / 7"）
        FeverTime      = 12,    //!< チャンスかフィーバーの残り秒数
        FeverChain     = 13,    //!< 帯の補足（チャンス中は目的、フィーバー中は効果）
        BannerHeading  = 14,    //!< 帯の見出し（"★ 星ゲットチャンス ★" / "★ 星詠みタイム ★"）
    };

    //-------------------------------------------------------------
    //! @struct HudTextComponent
    //! @brief  HUDの文字
    //! @note   kind ごとに HudSystem が FontComponent::text を書き換える
    //-------------------------------------------------------------
    struct HudTextComponent {
        HudTextKind kind  = HudTextKind::Static;    //!< 表示する内容
        int         index = 0;                      //!< 何番目か（リールなど、同じ種類が複数あるもの）
        std::string text;                           //!< Static のときに出す文字（UTF-8）
    };
}    // namespace StardustFever::ECS
