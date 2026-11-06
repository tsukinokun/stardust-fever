//-------------------------------------------------------------
//! @file   StarMapStateComponent.hpp
//! @brief  StarMapStateComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarMapStateComponent
    //! @brief  88星座を眺める星図の画面の状態
    //! @note   ゲーム状態エンティティに1つだけ付く。開いている間は遊びを止める（GamePause.hpp）
    //-------------------------------------------------------------
    struct StarMapStateComponent {
        bool        open        = false;       //!< 開いているか
        double      raCenter    = 84.0;        //!< 見ている向きの赤経（度。初めはオリオン座のあたり）
        double      decCenter   = 0.0;         //!< 見ている向きの赤緯（度）
        double      raVelocity  = 0.0;         //!< 手を離した後に回り続ける速さ（度/秒）
        double      decVelocity = 0.0;         //!< 同上
        bool        focusing    = false;       //!< 選んだ星座へ向かって回っているか
        double      focusRa     = 0.0;         //!< 向かう先の赤経（度）
        double      focusDec    = 0.0;         //!< 向かう先の赤緯（度）
        double      zoom        = 1.0;         //!< 拡大率
        std::string selectedId;                //!< 選んでいる星座（空なら選んでいない）
        bool        groupMode      = false;    //!< グループの一覧を出しているか（G で切り替える）
        int         selectedGroup  = 0;        //!< グループの一覧で選んでいるグループの添字
        bool        dragging       = false;    //!< 星図をつかんでいるか
        float       dragMoved      = 0.0f;     //!< つかんでから動かした量（ピクセル。少なければクリック）
        float       lastX          = 0.0f;     //!< 前のフレームのマウスの位置（UI の座標）
        float       lastY          = 0.0f;
        int         rowScroll      = 0;       //!< 右のパネルの先頭から何行送ったか
        int         rowScrollMax   = 0;       //!< 送れる行数（StarMapViewSystem が書く）
        int         hoverStar      = -1;      //!< マウスが乗っている星（StarTable::stars の添字。無ければ -1）
        float       hoverX         = 0.0f;    //!< マウスの位置（UI の座標）
        float       hoverY         = 0.0f;
        int         focusStar      = -1;       //!< キーで選んでいる星（StarTable::stars の添字。無ければ -1）
        bool        objectMode     = false;    //!< 天体の図鑑を出しているか（T で切り替える）
        int         selectedObject = 0;        //!< 図鑑で選んでいる天体（SkyObjectTable::objects の添字）
        int         hoverObject    = -1;       //!< マウスが乗っている天体（同上。無ければ -1）
        bool        objectFollow   = false;    //!< 図鑑の一覧を選んだ行へ合わせ直す
        int         reticleObject  = -1;       //!< 画面の中央に一番近い天体（無ければ -1）
        std::string reticleId;                 //!< 画面の中央に一番近い星座（無ければ空）
        int         version = 0;               //!< 見え方が変わるたびに増える（StarMapViewSystem が並べ直す合図）
    };
}    // namespace StardustFever::ECS
