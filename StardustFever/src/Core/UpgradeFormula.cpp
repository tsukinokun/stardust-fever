//-------------------------------------------------------------
//! @file   UpgradeFormula.cpp
//! @brief  強化の値段と効果量の計算の実装
//-------------------------------------------------------------
#include <StardustFever/Core/UpgradeFormula.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::Core::UpgradeFormula
namespace StardustFever::Core::UpgradeFormula {

    //-------------------------------------------------------------
    // 次のレベルへ上げる値段
    //-------------------------------------------------------------
    BigNumber Cost(double baseCost, double costGrowth, int level) {
        const int safeLevel = std::max(level, 0);
        return BigNumber(baseCost) * BigNumber::Pow(costGrowth, static_cast<double>(safeLevel));
    }

    //-------------------------------------------------------------
    // 今のレベルの効果量
    //-------------------------------------------------------------
    double EffectValue(Stacking stacking, double valuePerLevel, int level) {
        const int safeLevel = std::max(level, 0);
        if(stacking == Stacking::Add)
            return valuePerLevel * static_cast<double>(safeLevel);
        // 1+v が 0 以下だと符号が反転したり 0 で止まったりするので、ほぼ 0 で下げ止める
        return std::pow(std::max(1.0 + valuePerLevel, 1e-6), static_cast<double>(safeLevel));
    }

    //-------------------------------------------------------------
    // 上限に達しているか
    //-------------------------------------------------------------
    bool IsMaxed(int level, int maxLevel) {
        return maxLevel > 0 && level >= maxLevel;
    }

}    // namespace StardustFever::Core::UpgradeFormula
