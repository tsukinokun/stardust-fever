//-------------------------------------------------------------
//! @file   StarDraw.cpp
//! @brief  StarDraw の実装
//-------------------------------------------------------------
#include <StardustFever/Core/StarDraw.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::Core::StarDraw
namespace StardustFever::Core::StarDraw {

    double Weight(double magnitude, double rarityBase) {
        return std::pow(std::max(rarityBase, 1.0), magnitude);
    }

    size_t PickIndex(const std::vector<double>& weights, double unit) {
        double total = 0.0;
        for(const double weight : weights) {
            total += std::max(weight, 0.0);
        }
        if(total <= 0.0)
            return weights.size();

        //-------------------------------------------------------------
        // 重みを順に積んで、乱数が入った区間を選ぶ。
        // 端数の誤差で最後まで届かなかったときは、重みのある最後の添字にする
        //-------------------------------------------------------------
        const double target = std::clamp(unit, 0.0, 1.0) * total;
        double       sum    = 0.0;
        size_t       last   = weights.size();
        for(size_t i = 0; i < weights.size(); ++i) {
            if(weights[i] <= 0.0)
                continue;
            sum  += weights[i];
            last  = i;
            if(target < sum)
                return i;
        }
        return last;
    }

    Outcome Apply(int currentLevel, int maxLevel) {
        const int limit = std::max(1, maxLevel);
        Outcome   outcome;
        if(currentLevel <= 0) {
            outcome.level = 1;
            outcome.isNew = true;
        } else if(currentLevel >= limit) {
            outcome.level     = limit;
            outcome.converted = true;
        } else {
            outcome.level = currentLevel + 1;
        }
        return outcome;
    }

}    // namespace StardustFever::Core::StarDraw
