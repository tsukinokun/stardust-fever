//-------------------------------------------------------------
//! @file   SkyObjectDraw.hpp
//! @brief  ビッグバンで天体を引くときの、レア度の確率の計算
//! @note   エンジンに依存させず、単体テストできるようにしてある
//-------------------------------------------------------------
#pragma once
#include <vector>

// 名前空間 : StardustFever::Core::SkyObjectDraw
namespace StardustFever::Core::SkyObjectDraw {

    //-------------------------------------------------------------
    //! @brief  稼ぎの大きさを求める
    //! @param  log10Earned    [in] この宇宙の累計の常用対数
    //! @param  log10Threshold [in] ビッグバンできる累計の常用対数
    //! @return 累計がビッグバンできる量の何桁上か（下限 0）
    //-------------------------------------------------------------
    double EarnedScore(double log10Earned, double log10Threshold);

    //-------------------------------------------------------------
    //! @brief  レア度ごとの確率を求める
    //! @param  baseWeights [in] 稼ぎが 0 のときの重み
    //! @param  growths     [in] 稼ぎ1桁あたりに重みが増える桁数
    //! @param  remaining   [in] レア度ごとの未入手の数
    //! @param  score       [in] 稼ぎの大きさ（EarnedScore）
    //! @return 確率。未入手が無いレア度は 0。全部無ければ全て 0
    //-------------------------------------------------------------
    std::vector<double> TierRates(const std::vector<double>& baseWeights, const std::vector<double>& growths, const std::vector<int>& remaining, double score);

}    // namespace StardustFever::Core::SkyObjectDraw
