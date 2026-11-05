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
    //! @brief  星座の中心（星の位置の平均）を求める
    //! @param  constellation [in] 星座
    //! @param  stars         [in] 星のデータ
    //! @param  raDeg         [out] 中心の赤経（度）
    //! @param  decDeg        [out] 中心の赤緯（度）
    //! @return 星が1つも見つからなければ false
    //-------------------------------------------------------------
    inline bool Centroid(const Data::ConstellationDef& constellation, const Data::StarTable& stars, double& raDeg, double& decDeg) {
        std::vector<double> ras;
        double              decSum = 0.0;
        for(const Data::ConstellationStarDef& member : constellation.stars) {
            if(const Data::StarDef* star = stars.Find(member.starId)) {
                ras.push_back(star->ra * 15.0);
                decSum += star->dec;
            }
        }
        if(ras.empty())
            return false;
        raDeg  = Core::SkyMath::CircularMeanDegrees(ras);
        decDeg = decSum / static_cast<double>(ras.size());
        return true;
    }

    //-------------------------------------------------------------
    //! @brief  星図の見え方を作る
    //! @param  raCenter        [in] 中央の赤経（度）
    //! @param  decCenter       [in] 中央の赤緯（度）
    //! @param  pixelsPerDegree [in] 拡大率 1 のときの1度のピクセル数
    //! @param  zoom            [in] 拡大率
    //! @return 見え方
    //-------------------------------------------------------------
    inline Core::SkyMath::View MakeView(double raCenter, double decCenter, double pixelsPerDegree, double zoom) {
        Core::SkyMath::View view;
        view.raCenter        = raCenter;
        view.decCenter       = decCenter;
        view.pixelsPerDegree = pixelsPerDegree * zoom;
        return view;
    }

}    // namespace StardustFever::ECS::StarMapGeometry
