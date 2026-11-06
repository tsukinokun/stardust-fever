//-------------------------------------------------------------
//! @file   SkyMathTests.cpp
//! @brief  星図の座標の計算の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/SkyMath.hpp>

namespace SkyMath = StardustFever::Core::SkyMath;

SF_TEST(Wrap180KeepsAnglesInRange) {
    SF_EXPECT_NEAR(SkyMath::Wrap180(0.0), 0.0, 1e-9);
    SF_EXPECT_NEAR(SkyMath::Wrap180(190.0), -170.0, 1e-9);
    SF_EXPECT_NEAR(SkyMath::Wrap180(-190.0), 170.0, 1e-9);
    SF_EXPECT_NEAR(SkyMath::Wrap180(725.0), 5.0, 1e-9);
}

SF_TEST(DirectionRoundTrips) {
    double ra  = 0.0;
    double dec = 0.0;
    SkyMath::ToRaDec(SkyMath::ToDirection(123.0, -45.0), ra, dec);
    SF_EXPECT_NEAR(ra, 123.0, 1e-9);
    SF_EXPECT_NEAR(dec, -45.0, 1e-9);
}

SF_TEST(NorthPoleIsUp) {
    const SkyMath::Direction pole = SkyMath::ToDirection(10.0, 90.0);
    SF_EXPECT_NEAR(pole.y, 1.0, 1e-9);
}

SF_TEST(LargerRightAscensionIsToTheLeftSeenFromInside) {
    SkyMath::Point point;
    SF_EXPECT(SkyMath::ProjectToScreen(100.0, 0.0, 60.0, 500.0, 110.0, 0.0, point));
    SF_EXPECT(point.x < 0.0);
    SF_EXPECT_NEAR(point.y, 0.0, 1e-9);
}

SF_TEST(HigherDeclinationIsUpOnScreen) {
    SkyMath::Point point;
    SF_EXPECT(SkyMath::ProjectToScreen(100.0, 20.0, 60.0, 500.0, 100.0, 30.0, point));
    SF_EXPECT(point.y < 0.0);
    SF_EXPECT_NEAR(point.x, 0.0, 1e-9);
}

SF_TEST(ViewCentreProjectsToTheMiddle) {
    SkyMath::Point point;
    SF_EXPECT(SkyMath::ProjectToScreen(250.0, -30.0, 60.0, 500.0, 250.0, -30.0, point));
    SF_EXPECT_NEAR(point.x, 0.0, 1e-9);
    SF_EXPECT_NEAR(point.y, 0.0, 1e-9);
}

SF_TEST(StarsBehindTheViewAreNotProjected) {
    SkyMath::Point point;
    SF_EXPECT(!SkyMath::ProjectToScreen(0.0, 0.0, 60.0, 500.0, 180.0, 0.0, point));
}

SF_TEST(ArcTurnsXAxisOntoTheTarget) {
    const SkyMath::Direction to = SkyMath::ToDirection(70.0, 25.0);
    const SkyMath::Rotation  q  = SkyMath::ArcFromXAxis(to);
    // 四元数で (1,0,0) を回した結果の式
    const double x = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    const double y = 2.0 * (q.x * q.y + q.w * q.z);
    const double z = 2.0 * (q.x * q.z - q.w * q.y);
    SF_EXPECT_NEAR(x, to.x, 1e-9);
    SF_EXPECT_NEAR(y, to.y, 1e-9);
    SF_EXPECT_NEAR(z, to.z, 1e-9);
}

SF_TEST(CircularMeanHandlesWrap) {
    // 0度と360度は同じ向きなので、回し込んでから比べる
    SF_EXPECT_NEAR(SkyMath::Wrap180(SkyMath::CircularMeanDegrees({350.0, 10.0})), 0.0, 1e-6);
    SF_EXPECT_NEAR(SkyMath::CircularMeanDegrees({80.0, 100.0}), 90.0, 1e-6);
    SF_EXPECT_NEAR(SkyMath::CircularMeanDegrees({}), 0.0, 1e-9);
}
