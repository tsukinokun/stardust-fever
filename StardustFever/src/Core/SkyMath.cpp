//-------------------------------------------------------------
//! @file   SkyMath.cpp
//! @brief  SkyMath の実装
//-------------------------------------------------------------
#include <StardustFever/Core/SkyMath.hpp>

#include <cmath>

// 名前空間 : StardustFever::Core::SkyMath
namespace StardustFever::Core::SkyMath {

    namespace {
        constexpr double kPi = 3.14159265358979323846;
    }

    double Wrap180(double degrees) {
        double wrapped = std::fmod(degrees + 180.0, 360.0);
        if(wrapped < 0.0)
            wrapped += 360.0;
        return wrapped - 180.0;
    }

    Point Project(const View& view, double raDeg, double decDeg) {
        Point point;
        point.x = -Wrap180(raDeg - view.raCenter) * view.pixelsPerDegree;
        point.y = -(decDeg - view.decCenter) * view.pixelsPerDegree;
        return point;
    }

    Point ProjectFrom(const View& view, const Point& from, double raFrom, double raTo, double decTo) {
        Point point;
        point.x = from.x - Wrap180(raTo - raFrom) * view.pixelsPerDegree;
        point.y = -(decTo - view.decCenter) * view.pixelsPerDegree;
        return point;
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
