//-------------------------------------------------------------
//! @file   SkyObjectDraw.cpp
//! @brief  SkyObjectDraw の実装
//-------------------------------------------------------------
#include <StardustFever/Core/SkyObjectDraw.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::Core::SkyObjectDraw
namespace StardustFever::Core::SkyObjectDraw {

    //-------------------------------------------------------------
    // 稼ぎの大きさを求める
    //-------------------------------------------------------------
    double EarnedScore(double log10Earned, double log10Threshold) {
        return std::max(0.0, log10Earned - log10Threshold);
    }

    //-------------------------------------------------------------
    // レア度ごとの確率を求める
    //-------------------------------------------------------------
    std::vector<double> TierRates(const std::vector<double>& baseWeights, const std::vector<double>& growths, const std::vector<int>& remaining, double score) {
        std::vector<double> rates(remaining.size(), 0.0);
        double              total = 0.0;
        for(size_t i = 0; i < remaining.size(); ++i) {
            if(remaining[i] <= 0 || i >= baseWeights.size())
                continue;
            const double growth  = i < growths.size() ? growths[i] : 0.0;
            rates[i]             = baseWeights[i] * std::pow(10.0, growth * score);
            total               += rates[i];
        }
        if(total <= 0.0)
            return std::vector<double>(remaining.size(), 0.0);
        for(double& rate : rates) {
            rate /= total;
        }
        return rates;
    }

}    // namespace StardustFever::Core::SkyObjectDraw
