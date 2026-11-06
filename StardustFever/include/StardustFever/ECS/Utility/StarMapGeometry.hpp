//-------------------------------------------------------------
//! @file   StarMapGeometry.hpp
//! @brief  星図の画面で使う、星座の中心などの計算
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/SkyMath.hpp>
#include <StardustFever/Data/ConstellationData.hpp>
#include <StardustFever/Data/StarData.hpp>

#include <algorithm>
#include <vector>

// 名前空間 : StardustFever::ECS::StarMapGeometry
namespace StardustFever::ECS::StarMapGeometry {

    //-------------------------------------------------------------
    //! @brief  星座の中心（星の向きの平均）を求める
    //! @param  constellation [in] 星座
    //! @param  stars         [in] 星のデータ
    //! @param  raDeg         [out] 中心の赤経（度）
    //! @param  decDeg        [out] 中心の赤緯（度）
    //! @return 星が1つも見つからなければ false
    //! @note   向きのまま平均するので、天の極を囲む星座でも中心が正しく出る
    //-------------------------------------------------------------
    inline bool Centroid(const Data::ConstellationDef& constellation, const Data::StarTable& stars, double& raDeg, double& decDeg) {
        Core::SkyMath::Direction sum{0.0, 0.0, 0.0};
        bool                     found = false;
        for(const Data::ConstellationStarDef& member : constellation.stars) {
            if(const Data::StarDef* star = stars.Find(member.starId)) {
                const Core::SkyMath::Direction direction  = Core::SkyMath::ToDirection(star->ra * 15.0, star->dec);
                sum.x                                    += direction.x;
                sum.y                                    += direction.y;
                sum.z                                    += direction.z;
                found                                     = true;
            }
        }
        if(found)
            Core::SkyMath::ToRaDec(sum, raDeg, decDeg);
        return found;
    }

    //-------------------------------------------------------------
    //! @struct Figure
    //! @brief  枠に収めた星座の形。添字は ConstellationDef::stars と同じ
    //-------------------------------------------------------------
    struct Figure {
        std::vector<Core::SkyMath::Point> points;    //!< 枠の中心から見た星の位置
        std::vector<const Data::StarDef*> defs;      //!< 星（見つからない星は nullptr）
    };

    //-------------------------------------------------------------
    //! @brief  星座の中心から見た向きで星を写し、枠に収める
    //! @param  constellation [in] 星座
    //! @param  stars         [in] 星のデータ
    //! @param  width         [in] 枠の幅
    //! @param  height        [in] 枠の高さ
    //! @param  figure        [out] 星座の形
    //! @return 星が1つも見つからなければ false
    //-------------------------------------------------------------
    inline bool FitFigure(const Data::ConstellationDef& constellation, const Data::StarTable& stars, double width, double height, Figure& figure) {
        double ra  = 0.0;
        double dec = 0.0;
        if(!Centroid(constellation, stars, ra, dec))
            return false;

        figure.points.assign(constellation.stars.size(), Core::SkyMath::Point{});
        figure.defs.assign(constellation.stars.size(), nullptr);
        double minX = 1e9;
        double maxX = -1e9;
        double minY = 1e9;
        double maxY = -1e9;
        for(size_t i = 0; i < constellation.stars.size(); ++i) {
            const Data::StarDef* member = stars.Find(constellation.stars[i].starId);
            if(!member || !Core::SkyMath::ProjectToScreen(ra, dec, 90.0, 1.0, member->ra * 15.0, member->dec, figure.points[i]))
                continue;
            figure.defs[i] = member;
            minX           = std::min(minX, figure.points[i].x);
            maxX           = std::max(maxX, figure.points[i].x);
            minY           = std::min(minY, figure.points[i].y);
            maxY           = std::max(maxY, figure.points[i].y);
        }
        const double scale   = std::min({width / std::max(maxX - minX, 1e-3), height / std::max(maxY - minY, 1e-3), 900.0});
        const double middleX = (minX + maxX) * 0.5;
        const double middleY = (minY + maxY) * 0.5;
        for(Core::SkyMath::Point& point : figure.points) {
            point.x = (point.x - middleX) * scale;
            point.y = (point.y - middleY) * scale;
        }
        return true;
    }

}    // namespace StardustFever::ECS::StarMapGeometry
