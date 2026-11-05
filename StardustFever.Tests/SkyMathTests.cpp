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

SF_TEST(ProjectPutsLargerRightAscensionToTheLeft) {
    SkyMath::View view;
    view.raCenter             = 90.0;
    view.decCenter            = 0.0;
    view.pixelsPerDegree      = 2.0;
    const SkyMath::Point east = SkyMath::Project(view, 100.0, 10.0);
    SF_EXPECT_NEAR(east.x, -20.0, 1e-9);
    // 北（赤緯が大きい）は画面の上（y が負）
    SF_EXPECT_NEAR(east.y, -20.0, 1e-9);
}

SF_TEST(ProjectWrapsAcrossZeroHours) {
    SkyMath::View view;
    view.raCenter        = 2.0;
    view.pixelsPerDegree = 1.0;
    // 赤経 358 度は中央（2 度）の 4 度西（右）にある。356 度東（左）に飛ばない
    SF_EXPECT_NEAR(SkyMath::Project(view, 358.0, 0.0).x, 4.0, 1e-9);
}

SF_TEST(ProjectFromKeepsLinesShort) {
    SkyMath::View view;
    view.raCenter        = 180.0;
    view.pixelsPerDegree = 1.0;
    // 中央から 179 度離れた所で 0 時をまたぐ線は、画面を横切らず短いまま
    const SkyMath::Point from = SkyMath::Project(view, 359.0, 0.0);
    const SkyMath::Point to   = SkyMath::ProjectFrom(view, from, 359.0, 3.0, 0.0);
    SF_EXPECT_NEAR(to.x - from.x, -4.0, 1e-9);
}

SF_TEST(CircularMeanHandlesWrap) {
    // 0度と360度は同じ向きなので、回し込んでから比べる
    SF_EXPECT_NEAR(SkyMath::Wrap180(SkyMath::CircularMeanDegrees({350.0, 10.0})), 0.0, 1e-6);
    SF_EXPECT_NEAR(SkyMath::CircularMeanDegrees({80.0, 100.0}), 90.0, 1e-6);
    SF_EXPECT_NEAR(SkyMath::CircularMeanDegrees({}), 0.0, 1e-9);
}
