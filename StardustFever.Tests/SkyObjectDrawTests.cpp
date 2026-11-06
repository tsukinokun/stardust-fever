//-------------------------------------------------------------
//! @file   SkyObjectDrawTests.cpp
//! @brief  天体のレア度の確率の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/SkyObjectDraw.hpp>

#include <numeric>

namespace SkyObjectDraw = StardustFever::Core::SkyObjectDraw;

namespace {
    const std::vector<double> kBase   = {80.0, 17.0, 3.0, 0.2};
    const std::vector<double> kGrowth = {0.0, 0.12, 0.22, 0.35};
}

SF_TEST(TierRatesSumToOne) {
    const auto rates = SkyObjectDraw::TierRates(kBase, kGrowth, {18, 13, 8, 3}, 2.0);
    SF_EXPECT_NEAR(std::accumulate(rates.begin(), rates.end(), 0.0), 1.0, 1e-9);
}

SF_TEST(EarningMoreRaisesHighRarity) {
    const auto low  = SkyObjectDraw::TierRates(kBase, kGrowth, {18, 13, 8, 3}, 0.0);
    const auto high = SkyObjectDraw::TierRates(kBase, kGrowth, {18, 13, 8, 3}, 6.0);
    SF_EXPECT(high[3] > low[3]);
    SF_EXPECT(high[2] > low[2]);
    SF_EXPECT(high[0] < low[0]);
}

SF_TEST(FinishedTierIsNeverDrawn) {
    const auto rates = SkyObjectDraw::TierRates(kBase, kGrowth, {0, 13, 0, 3}, 1.0);
    SF_EXPECT_NEAR(rates[0], 0.0, 1e-12);
    SF_EXPECT_NEAR(rates[2], 0.0, 1e-12);
    SF_EXPECT_NEAR(rates[1] + rates[3], 1.0, 1e-9);
}

SF_TEST(NothingLeftGivesNoRates) {
    const auto rates = SkyObjectDraw::TierRates(kBase, kGrowth, {0, 0, 0, 0}, 3.0);
    SF_EXPECT_NEAR(std::accumulate(rates.begin(), rates.end(), 0.0), 0.0, 1e-12);
}

SF_TEST(EarnedScoreStopsAtZero) {
    SF_EXPECT_NEAR(SkyObjectDraw::EarnedScore(5.0, 6.0), 0.0, 1e-12);
    SF_EXPECT_NEAR(SkyObjectDraw::EarnedScore(8.0, 6.0), 2.0, 1e-12);
}
