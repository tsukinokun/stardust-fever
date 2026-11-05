//-------------------------------------------------------------
//! @file   HudMessageEvent.hpp
//! @brief  HudMessageEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   HudMessageImportance
    //! @brief  メッセージの出し方
    //-------------------------------------------------------------
    enum class HudMessageImportance {
        Toast,       //!< 右下に小さく積む（星の獲得・購入・足りないなど、よくある出来事）
        Headline,    //!< 画面中央に大きく出す（フィーバー開始・星座完成・ビッグバン）
    };

    //-------------------------------------------------------------
    //! @enum   HudMessageTone
    //! @brief  メッセージの色合い（トーストの縁の色を変え、全部同じ見た目にならないようにする）
    //-------------------------------------------------------------
    enum class HudMessageTone {
        Info,        //!< ふつうの知らせ
        Positive,    //!< 良い出来事（購入・星の獲得）
        Warning,     //!< できなかった（足りない・まだできない）
    };

    //-------------------------------------------------------------
    //! @struct HudMessageEvent
    //! @brief  プレイヤーへの通知を出す
    //! @note   text の1行目が見出し、2行目以降が補足になる
    //-------------------------------------------------------------
    struct HudMessageEvent {
        std::string          text;                                        //!< 表示する文字（UTF-8）
        HudMessageImportance importance = HudMessageImportance::Toast;    //!< 出し方
        HudMessageTone       tone       = HudMessageTone::Info;           //!< 色合い
    };
}    // namespace StardustFever::ECS
