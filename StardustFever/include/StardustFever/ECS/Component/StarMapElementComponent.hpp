//-------------------------------------------------------------
//! @file   StarMapElementComponent.hpp
//! @brief  StarMapElementComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   StarMapElementKind
    //! @brief  星図の画面の部品の種類
    //-------------------------------------------------------------
    enum class StarMapElementKind : int {
        Root,           //!< 画面全体（閉じるときにこの下をまとめて消す）
        Veil,           //!< 開け閉めの暗転の幕
        StarLabel,      //!< 選んだ星座の星の名前（index は使い回す枠の番号）
        Tooltip,        //!< マウスを乗せた星の吹き出しの板
        TooltipText,    //!< 吹き出しの文字（index は行の番号）
        Star,           //!< 星の点（index は StarTable::stars の添字）
        Line,           //!< 星座の線（index は星座の添字、subIndex は線の添字）
        Label,          //!< 星座名（index は星座の添字）
        Title,          //!< 選んだ星座の名前
        Subtitle,       //!< ラテン語名とそろった具合
        Effect,         //!< 星座の効果
        Row,            //!< 星の一覧の1行（index は行の番号）
        Summary,        //!< 上のまとめ（そろった星座・集めた星）
    };

    //-------------------------------------------------------------
    //! @struct StarMapElementComponent
    //! @brief  星図の画面の部品
    //! @note   StarMapViewSystem が開いたときに作り、閉じたときに消す。実行時だけの値
    //-------------------------------------------------------------
    struct StarMapElementComponent {
        StarMapElementKind kind     = StarMapElementKind::Root;    //!< 部品の種類
        int                index    = 0;                           //!< 何番目か（種類ごとの意味は StarMapElementKind）
        int                subIndex = 0;                           //!< 線の添字
    };
}    // namespace StardustFever::ECS
