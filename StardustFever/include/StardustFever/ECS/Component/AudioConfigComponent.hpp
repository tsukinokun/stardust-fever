//-------------------------------------------------------------
//! @file   AudioConfigComponent.hpp
//! @brief  AudioConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SfxDef
    //! @brief  効果音の1つ
    //! @note   同じ id を並べると高さ違いの組になる（エンジンがピッチを変えられないため）
    //-------------------------------------------------------------
    struct SfxDef {
        std::string id;               //!< 鳴らすときの名前
        std::string path;             //!< 音のファイル
        float       volume = 1.0f;    //!< 音量（0〜1）
    };

    //-------------------------------------------------------------
    //! @struct AudioConfigComponent
    //! @brief  BGM と効果音の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct AudioConfigComponent {
        std::string         bgmNormal;                  //!< 通常プレイの曲
        std::string         bgmReach;                   //!< リーチの曲
        std::string         bgmFever;                   //!< 星詠みタイムの曲
        std::string         bgmChance;                  //!< 星ゲットチャンスの曲
        std::string         bgmMap;                     //!< 星図の曲
        float               fadeSeconds     = 1.2f;     //!< 曲の切り替えにかける時間（秒）
        float               fastFadeSeconds = 0.3f;     //!< リーチの出入りの切り替えにかける時間（秒）
        int                 bgmBus          = 1;        //!< BGM のバスの番号
        int                 sfxBus          = 2;        //!< 効果音のバスの番号
        float               volumeStep      = 0.1f;     //!< キーを1回押したときの音量の変化
        float               pinMinInterval  = 0.05f;    //!< 釘の音の最短の間隔（秒）。玉が多いと連発するので間引く
        std::vector<SfxDef> sounds;                     //!< 効果音の一覧
    };
}    // namespace StardustFever::ECS
