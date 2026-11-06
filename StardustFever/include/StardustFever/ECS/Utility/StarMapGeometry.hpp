//-------------------------------------------------------------
//! @file   StarMapGeometry.hpp
//! @brief  星図の画面で使う、星座の中心などの計算
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/SkyMath.hpp>
#include <StardustFever/Data/ConstellationData.hpp>
#include <StardustFever/Data/StarData.hpp>

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

}    // namespace StardustFever::ECS::StarMapGeometry
