//-------------------------------------------------------------
//! @file   HudTextComponent.hpp
//! @brief  HudTextComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   HudTextKind
    //! @brief  HUDの文字が何を表示するか（JSON には整数で書く）
    //-------------------------------------------------------------
    enum class HudTextKind : int {
        Stardust   = 0,     //!< 所持している星屑
        Multiplier = 1,     //!< 全体倍率
        Handle     = 2,     //!< ハンドルの強さ
        AutoFire   = 3,     //!< 自動発射の状態
        Lottery    = 4,     //!< 抽選リールと保留
        Fever      = 5,     //!< フィーバーの状態
        Message    = 6,     //!< 一時的なメッセージ
        Help       = 7,     //!< 操作説明
        Collection = 8,     //!< 集めた星
        Upgrades   = 9,     //!< 強化の一覧と値段
        Prestige   = 10,    //!< 宇宙定数とビッグバンの獲得量
    };

    //-------------------------------------------------------------
    //! @struct HudTextComponent
    //! @brief  HUDの文字
    //! @note   kind ごとに HudSystem が FontComponent::text を書き換える
    //-------------------------------------------------------------
    struct HudTextComponent {
        HudTextKind kind = HudTextKind::Stardust;    //!< 表示する内容
    };
}    // namespace StardustFever::ECS
