//-------------------------------------------------------------
//! @file   StarChanceConfigComponent.hpp
//! @brief  StarChanceConfigComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct StarChanceConfigComponent
    //! @brief  星ゲットチャンスと、星を引く抽選と、集めた星の効果の調整値
    //! @note   設定エンティティに付く。ホットリロード対象
    //-------------------------------------------------------------
    struct StarChanceConfigComponent {
        double      chanceProbability     = 0.15;      //!< 星の門に入ったときにチャンスが始まる確率
        float       duration              = 30.0f;     //!< チャンスの長さ（秒）
        float       endDelayAfterComplete = 2.5f;      //!< 全部つないでから終わるまでの余韻（秒）
        std::string starPinPrefab;                     //!< 星釘のPrefab
        float       pinHideRadius         = 2.6f;      //!< 星釘からこの距離の中の普通の釘は、チャンスの間だけ隠して当たり判定を外す
        double      rarityBase            = 1.8;       //!< 1等級暗くなるごとに出やすさが何倍になるか
        int         maxStarLevel          = 5;         //!< 星のレベルの上限
        double      duplicatePayoutScale  = 20.0;      //!< 最大レベルの星を引いたとき、星の値打ちに掛けて星屑に換える
        double      effectLevelBonus      = 0.1;       //!< 星座の効果を、星の平均レベルが1上がるごとに何割伸ばすか
        double      allCompleteMultiplier = 10.0;      //!< 88星座を全部そろえたときの全体倍率
        int         recentCount           = 5;         //!< 右の星図に出す、最近手に入れた星の数
        float       rollSpinSeconds       = 2.2f;      //!< 星座が入れ替わる長さ（秒）
        float       rollHangSeconds       = 1.4f;      //!< 星が1つずつ灯る長さ（秒）
        float       rollFakeSeconds       = 0.8f;      //!< 復活の前に外れたと見せる長さ（秒）
        int         rollSteps             = 26;        //!< 星座が入れ替わる回数
        float       rollComebackRate      = 0.3f;      //!< 当たりのうち、外れたと見せてから復活する割合
        float       rollNearMissRate      = 0.45f;     //!< 外れのうち、あと1つまで灯る割合
        float       goldOnWin             = 0.6f;      //!< 当たりのとき輪が金色になる確率
        float       goldOnMiss            = 0.1f;      //!< 外れのとき輪が金色になる確率
        float       rollFigureX           = 800.0f;    //!< ルーレットの星座の中心（UI の座標）
        float       rollFigureY           = 420.0f;    //!< ルーレットの星座の中心（UI の座標）
        float       rollFigureWidth       = 480.0f;    //!< 星座を収める枠の幅
        float       rollFigureHeight      = 340.0f;    //!< 星座を収める枠の高さ
        float       rollVeilAlpha         = 0.75f;     //!< 画面を暗くする濃さ
        std::string rollStarTexture;                   //!< 星座の星の絵
        std::string rollNebulaTexture;                 //!< 星座の後ろの雲の絵
        std::string rollDustTexture;                   //!< 金色の予告で舞う星屑の絵
    };
}    // namespace StardustFever::ECS
