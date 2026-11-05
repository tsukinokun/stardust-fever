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
        double      chanceProbability     = 0.15;     //!< 星の門に入ったときにチャンスが始まる確率
        float       duration              = 30.0f;    //!< チャンスの長さ（秒）
        float       endDelayAfterComplete = 2.5f;     //!< 全部つないでから終わるまでの余韻（秒）
        std::string starPinPrefab;                    //!< 星釘のPrefab
        float       pinHideRadius         = 2.6f;     //!< 星釘からこの距離の中の普通の釘は、チャンスの間だけ隠して当たり判定を外す
        double      rarityBase            = 1.8;      //!< 1等級暗くなるごとに出やすさが何倍になるか
        int         maxStarLevel          = 5;        //!< 星のレベルの上限
        double      duplicatePayoutScale  = 20.0;     //!< 最大レベルの星を引いたとき、星の値打ちに掛けて星屑に換える
        double      effectLevelBonus      = 0.1;      //!< 星座の効果を、星の平均レベルが1上がるごとに何割伸ばすか
        double      allCompleteMultiplier = 10.0;     //!< 88星座を全部そろえたときの全体倍率
        int         recentCount           = 5;        //!< 右の星図に出す、最近手に入れた星の数
        float       missToastInterval     = 8.0f;     //!< 星の門で外れたと知らせる間隔の最小（秒）。門にはよく入るので毎回は出さない
    };
}    // namespace StardustFever::ECS
