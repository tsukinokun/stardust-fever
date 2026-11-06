//-------------------------------------------------------------
//! @file   StarGateRollTests.cpp
//! @brief  星座ルーレットの進み方の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/StarGateRoll.hpp>

namespace Roll = StardustFever::Core::StarGateRoll;

namespace {
    const Roll::Timeline kTimeline{2.0f, 1.0f, 0.5f};
}

SF_TEST(OnlyComebackIsLonger) {
    SF_EXPECT_NEAR(Roll::TotalSeconds(kTimeline, Roll::Scenario::Win), 3.0, 1e-6);
    SF_EXPECT_NEAR(Roll::TotalSeconds(kTimeline, Roll::Scenario::NearMiss), 3.0, 1e-6);
    SF_EXPECT_NEAR(Roll::TotalSeconds(kTimeline, Roll::Scenario::Comeback), 3.5, 1e-6);
}

SF_TEST(PhasesFollowTheTimeline) {
    SF_EXPECT(Roll::PhaseAt(kTimeline, Roll::Scenario::Win, 1.9f) == Roll::Phase::Spin);
    SF_EXPECT(Roll::PhaseAt(kTimeline, Roll::Scenario::Win, 2.1f) == Roll::Phase::Hang);
    SF_EXPECT(Roll::PhaseAt(kTimeline, Roll::Scenario::Win, 3.1f) == Roll::Phase::Done);
    SF_EXPECT(Roll::PhaseAt(kTimeline, Roll::Scenario::Comeback, 3.1f) == Roll::Phase::Fake);
    SF_EXPECT(Roll::PhaseAt(kTimeline, Roll::Scenario::Comeback, 3.6f) == Roll::Phase::Done);
}

SF_TEST(ReelSlowsDown) {
    const int first = Roll::ReelStep(0.25f, 100);
    const int last  = Roll::ReelStep(1.0f, 100) - Roll::ReelStep(0.75f, 100);
    SF_EXPECT(first > last * 3);
    SF_EXPECT(Roll::ReelStep(0.0f, 100) == 0);
    SF_EXPECT(Roll::ReelStep(1.0f, 100) == 100);
}

SF_TEST(LitStarsStopByScenario) {
    SF_EXPECT(Roll::LitStars(1.0f, 10, Roll::Scenario::Win) == 10);
    SF_EXPECT(Roll::LitStars(1.0f, 10, Roll::Scenario::NearMiss) == 9);
    SF_EXPECT(Roll::LitStars(1.0f, 10, Roll::Scenario::Comeback) == 9);
    SF_EXPECT(Roll::LitStars(1.0f, 10, Roll::Scenario::Miss) == 5);
    SF_EXPECT(Roll::LitStars(0.0f, 10, Roll::Scenario::Win) == 0);
    // 星が2つの星座でも、外れで全部は灯らない
    SF_EXPECT(Roll::LitStars(1.0f, 2, Roll::Scenario::NearMiss) == 1);
    SF_EXPECT(Roll::LitStars(1.0f, 2, Roll::Scenario::Miss) == 1);
}
