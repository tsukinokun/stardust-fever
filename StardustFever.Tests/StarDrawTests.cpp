//-------------------------------------------------------------
//! @file   StarDrawTests.cpp
//! @brief  星を引く抽選の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/StarDraw.hpp>

namespace StarDraw = StardustFever::Core::StarDraw;

SF_TEST(BrighterStarsAreRarer) {
    // シリウス（-1.46）はミンタカ（2.25）より、ミンタカは5等星より出にくい
    SF_EXPECT(StarDraw::Weight(-1.46, 2.0) < StarDraw::Weight(2.25, 2.0));
    SF_EXPECT(StarDraw::Weight(2.25, 2.0) < StarDraw::Weight(5.0, 2.0));
    SF_EXPECT_NEAR(StarDraw::Weight(3.0, 2.0) / StarDraw::Weight(2.0, 2.0), 2.0, 1e-9);
}

SF_TEST(RarityBaseBelowOneMeansEqualWeights) {
    SF_EXPECT_NEAR(StarDraw::Weight(-1.0, 0.5), StarDraw::Weight(5.0, 0.5), 1e-12);
}

SF_TEST(PickIndexFollowsWeights) {
    const std::vector<double> weights = {1.0, 0.0, 3.0};
    SF_EXPECT(StarDraw::PickIndex(weights, 0.0) == 0);
    SF_EXPECT(StarDraw::PickIndex(weights, 0.24) == 0);
    SF_EXPECT(StarDraw::PickIndex(weights, 0.26) == 2);
    SF_EXPECT(StarDraw::PickIndex(weights, 0.999999) == 2);
    // 1.0 ちょうどが来ても範囲の外へ出ない
    SF_EXPECT(StarDraw::PickIndex(weights, 1.0) == 2);
}

SF_TEST(PickIndexWithoutWeightsReturnsSize) {
    SF_EXPECT(StarDraw::PickIndex({}, 0.5) == 0);
    SF_EXPECT(StarDraw::PickIndex({0.0, -1.0}, 0.5) == 2);
}

SF_TEST(ApplyNewStarStartsAtOne) {
    const StarDraw::Outcome outcome = StarDraw::Apply(0, 5);
    SF_EXPECT(outcome.isNew);
    SF_EXPECT(!outcome.converted);
    SF_EXPECT(outcome.level == 1);
}

SF_TEST(ApplyDuplicateLevelsUp) {
    const StarDraw::Outcome outcome = StarDraw::Apply(3, 5);
    SF_EXPECT(!outcome.isNew);
    SF_EXPECT(!outcome.converted);
    SF_EXPECT(outcome.level == 4);
}

SF_TEST(ApplyAtMaxConvertsToStardust) {
    const StarDraw::Outcome outcome = StarDraw::Apply(5, 5);
    SF_EXPECT(outcome.converted);
    SF_EXPECT(outcome.level == 5);
}
