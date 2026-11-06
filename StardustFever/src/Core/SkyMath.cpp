//-------------------------------------------------------------
//! @file   SkyMath.cpp
//! @brief  SkyMath の実装
//-------------------------------------------------------------
#include <StardustFever/Core/SkyMath.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::Core::SkyMath
namespace StardustFever::Core::SkyMath {

    namespace {
        constexpr double kPi               = 3.14159265358979323846;
        constexpr double kDegreesToRadians = kPi / 180.0;
    }

    double Wrap180(double degrees) {
        double wrapped = std::fmod(degrees + 180.0, 360.0);
        if(wrapped < 0.0)
            wrapped += 360.0;
        return wrapped - 180.0;
    }

    Direction ToDirection(double raDeg, double decDeg) {
        const double ra  = raDeg * kDegreesToRadians;
        const double dec = decDeg * kDegreesToRadians;
        return Direction{-std::cos(dec) * std::sin(ra), std::sin(dec), std::cos(dec) * std::cos(ra)};
    }

    void ToRaDec(const Direction& direction, double& raDeg, double& decDeg) {
        const double length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        decDeg              = length > 0.0 ? std::asin(std::clamp(direction.y / length, -1.0, 1.0)) / kDegreesToRadians : 0.0;
        raDeg               = std::atan2(-direction.x, direction.z) / kDegreesToRadians;
        if(raDeg < 0.0)
            raDeg += 360.0;
    }

    Rotation ArcFromXAxis(const Direction& to) {
        const double length = std::sqrt(to.x * to.x + to.y * to.y + to.z * to.z);
        if(length <= 0.0)
            return Rotation{};
        const double x = to.x / length;
        const double y = to.y / length;
        const double z = to.z / length;
        // 真後ろは外積が0になるので、Y軸回りの半回転にする
        if(x < -0.999999)
            return Rotation{0.0, 1.0, 0.0, 0.0};
        // (1,0,0) と向きの外積が回転軸、1 + 内積が実部（半角の公式）
        const double w    = 1.0 + x;
        const double norm = std::sqrt(z * z + y * y + w * w);
        return Rotation{0.0, -z / norm, y / norm, w / norm};
    }

    bool ProjectToScreen(double raCenter, double decCenter, double fovDeg, double halfHeight, double raDeg, double decDeg, Point& point) {
        const Direction forward = ToDirection(raCenter, decCenter);
        const Direction target  = ToDirection(raDeg, decDeg);
        const double    depth   = forward.x * target.x + forward.y * target.y + forward.z * target.z;
        if(depth <= 0.0)
            return false;

        // カメラの右と上。上の向きは世界の +Y を基準にする（左手系なので右 = 上 × 前）
        const double rightLength = std::sqrt(forward.z * forward.z + forward.x * forward.x);
        if(rightLength <= 0.0)
            return false;
        const Direction right{forward.z / rightLength, 0.0, -forward.x / rightLength};
        const Direction up{forward.y * right.z - forward.z * right.y, forward.z * right.x - forward.x * right.z, forward.x * right.y - forward.y * right.x};

        const double scale = halfHeight / std::tan(fovDeg * 0.5 * kDegreesToRadians);
        point.x            = (target.x * right.x + target.y * right.y + target.z * right.z) / depth * scale;
        point.y            = -(target.x * up.x + target.y * up.y + target.z * up.z) / depth * scale;
        return true;
    }

    double CircularMeanDegrees(const std::vector<double>& degrees) {
        if(degrees.empty())
            return 0.0;
        double sumSin = 0.0;
        double sumCos = 0.0;
        for(const double degree : degrees) {
            sumSin += std::sin(degree * kPi / 180.0);
            sumCos += std::cos(degree * kPi / 180.0);
        }
        double mean = std::atan2(sumSin, sumCos) * 180.0 / kPi;
        // ほぼ 0 の負の値に 360 を足すと丸めで 360 ちょうどになるので、もう一度回し込む
        if(mean < 0.0)
            mean += 360.0;
        if(mean >= 360.0)
            mean -= 360.0;
        return mean;
    }

}    // namespace StardustFever::Core::SkyMath
