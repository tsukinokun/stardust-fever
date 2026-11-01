//-------------------------------------------------------------
//! @file   UpgradeFormulaTests.cpp
//! @brief  強化の値段と効果量の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/UpgradeFormula.hpp>

namespace Formula = StardustFever::Core::UpgradeFormula;

SF_TEST(CostAtLevelZeroIsBaseCost) {
    SF_EXPECT_NEAR(Formula::Cost(50.0, 1.35, 0).ToDouble(), 50.0, 1e-9);
}

SF_TEST(CostGrowsGeometrically) {
    SF_EXPECT_NEAR(Formula::Cost(100.0, 2.0, 3).ToDouble(), 800.0, 1e-6);
    SF_EXPECT_NEAR(Formula::Cost(50.0, 1.35, 2).ToDouble(), 50.0 * 1.35 * 1.35, 1e-6);
}

SF_TEST(CostBeyondDoubleStaysFinite) {
    const StardustFever::Core::BigNumber cost = Formula::Cost(1000.0, 10.0, 400);
    SF_EXPECT(cost.Exponent() == 403);
    SF_EXPECT_NEAR(cost.Mantissa(), 1.0, 1e-6);
}

SF_TEST(CostTreatsNegativeLevelAsZero) {
    SF_EXPECT_NEAR(Formula::Cost(50.0, 1.35, -3).ToDouble(), 50.0, 1e-9);
}

SF_TEST(AddEffectIsLinear) {
    SF_EXPECT_NEAR(Formula::EffectValue(Formula::Stacking::Add, 0.01, 0), 0.0, 1e-12);
    SF_EXPECT_NEAR(Formula::EffectValue(Formula::Stacking::Add, 5.0, 3), 15.0, 1e-12);
}

SF_TEST(MultiplyEffectCompounds) {
    SF_EXPECT_NEAR(Formula::EffectValue(Formula::Stacking::Multiply, 0.25, 0), 1.0, 1e-12);
    SF_EXPECT_NEAR(Formula::EffectValue(Formula::Stacking::Multiply, 0.25, 3), 1.953125, 1e-12);
    // 負の値は小さくしていく（揺らぎの軽減など）
    SF_EXPECT_NEAR(Formula::EffectValue(Formula::Stacking::Multiply, -0.2, 2), 0.64, 1e-12);
}

SF_TEST(MultiplyEffectNeverFlipsSign) {
    SF_EXPECT(Formula::EffectValue(Formula::Stacking::Multiply, -1.5, 3) > 0.0);
}

SF_TEST(MaxLevelZeroMeansUnlimited) {
    SF_EXPECT(!Formula::IsMaxed(1000, 0));
    SF_EXPECT(!Formula::IsMaxed(4, 5));
    SF_EXPECT(Formula::IsMaxed(5, 5));
    SF_EXPECT(Formula::IsMaxed(1, 1));
}
