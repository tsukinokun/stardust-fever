//-------------------------------------------------------------
//! @file   PrestigeFormulaTests.cpp
//! @brief  宇宙定数の計算の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/PrestigeFormula.hpp>

using StardustFever::Core::BigNumber;
namespace Prestige = StardustFever::Core::PrestigeFormula;

SF_TEST(NoGainBelowThreshold) {
    SF_EXPECT(Prestige::GainOnBigBang(BigNumber(0.0), 1.0e6, 0.5).IsZero());
    SF_EXPECT(Prestige::GainOnBigBang(BigNumber(999999.0), 1.0e6, 0.5).IsZero());
}

SF_TEST(GainIsFlooredRoot) {
    SF_EXPECT_NEAR(Prestige::GainOnBigBang(BigNumber(1.0e6), 1.0e6, 0.5).ToDouble(), 1.0, 1e-9);
    SF_EXPECT_NEAR(Prestige::GainOnBigBang(BigNumber(4.0e6), 1.0e6, 0.5).ToDouble(), 2.0, 1e-9);
    // √8.9 = 2.98… は 2 に切り捨てる
    SF_EXPECT_NEAR(Prestige::GainOnBigBang(BigNumber(8.9e6), 1.0e6, 0.5).ToDouble(), 2.0, 1e-9);
}

SF_TEST(GainHandlesHugeEarnings) {
    const BigNumber gain = Prestige::GainOnBigBang(BigNumber::FromParts(1.0, 406), 1.0e6, 0.5);
    SF_EXPECT(gain.Exponent() == 200);
}

SF_TEST(NextConstantRequirementIsInverseOfGain) {
    SF_EXPECT_NEAR(Prestige::EarnedForNextConstant(BigNumber(0.0), 1.0e6, 0.5).ToDouble(), 1.0e6, 1e-3);
    SF_EXPECT_NEAR(Prestige::EarnedForNextConstant(BigNumber(2.0), 1.0e6, 0.5).ToDouble(), 9.0e6, 1e-2);
}

SF_TEST(MultiplierIsLinearInConstants) {
    SF_EXPECT_NEAR(Prestige::Multiplier(BigNumber(0.0), 0.1).ToDouble(), 1.0, 1e-12);
    SF_EXPECT_NEAR(Prestige::Multiplier(BigNumber(3.0), 0.1).ToDouble(), 1.3, 1e-12);
}

SF_TEST(InvalidParametersGiveNoGain) {
    SF_EXPECT(Prestige::GainOnBigBang(BigNumber(1.0e9), 0.0, 0.5).IsZero());
    SF_EXPECT(Prestige::GainOnBigBang(BigNumber(1.0e9), 1.0e6, 0.0).IsZero());
}
