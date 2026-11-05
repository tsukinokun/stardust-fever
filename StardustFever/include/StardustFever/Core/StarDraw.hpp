//-------------------------------------------------------------
//! @file   StarDraw.hpp
//! @brief  星ゲットチャンスで星を引く抽選の計算
//! @note   エンジンに依存させず、単体テストできるようにしてある
//-------------------------------------------------------------
#pragma once
#include <cstddef>
#include <vector>

// 名前空間 : StardustFever::Core::StarDraw
namespace StardustFever::Core::StarDraw {

    //-------------------------------------------------------------
    //! @brief  星の出やすさ（重み）を求める
    //! @param  magnitude  [in] 見かけの等級（小さいほど明るい）
    //! @param  rarityBase [in] 1等級ごとに出やすさが何倍になるか（1 より大きい）
    //! @return rarityBase^magnitude。明るい星ほど小さく、出にくい
    //-------------------------------------------------------------
    double Weight(double magnitude, double rarityBase);

    //-------------------------------------------------------------
    //! @brief  重みに比例した確率で添字を1つ選ぶ
    //! @param  weights [in] 重み（0 以下は選ばれない）
    //! @param  unit    [in] 0 以上 1 未満の乱数
    //! @return 選ばれた添字。全部の重みが 0 以下なら weights.size()
    //-------------------------------------------------------------
    size_t PickIndex(const std::vector<double>& weights, double unit);

    //-------------------------------------------------------------
    //! @struct Outcome
    //! @brief  引いた星を持ち物へ加えた結果
    //-------------------------------------------------------------
    struct Outcome {
        int  level     = 0;        //!< 加えた後のレベル
        bool isNew     = false;    //!< 初めて手に入れたか
        bool converted = false;    //!< 最大レベルだったので星屑に換えたか
    };

    //-------------------------------------------------------------
    //! @brief  引いた星を持ち物へ加える
    //! @param  currentLevel [in] 今のレベル（持っていなければ 0）
    //! @param  maxLevel     [in] レベルの上限
    //! @return 持っていなければ ★1、持っていれば ★+1、上限なら星屑に換える
    //-------------------------------------------------------------
    Outcome Apply(int currentLevel, int maxLevel);

}    // namespace StardustFever::Core::StarDraw
