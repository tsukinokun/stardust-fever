//-------------------------------------------------------------
//! @file   SkyObjectOdds.hpp
//! @brief  今のビッグバンで天体が出る確率。確認画面と実際の抽選で同じものを使う
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>
#include <StardustFever/Core/SkyObjectDraw.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/SkyObjectConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct SkyObjectOdds
    //! @brief  レア度ごとの確率と未入手の数
    //-------------------------------------------------------------
    struct SkyObjectOdds {
        std::vector<double> rates;        //!< 確率
        std::vector<int>    remaining;    //!< 未入手の数
    };

    //-------------------------------------------------------------
    //! @brief  天体を持っているか
    //! @param  collection [in] 持ち物
    //! @param  id         [in] 天体のID
    //! @return 持っていれば true
    //-------------------------------------------------------------
    inline bool OwnsSkyObject(const StarCollectionComponent& collection, const std::string& id) {
        return std::find(collection.skyObjects.begin(), collection.skyObjects.end(), id) != collection.skyObjects.end();
    }

    //-------------------------------------------------------------
    //! @brief  今の確率を求める
    //! @param  config     [in] 天体の設定
    //! @param  collection [in] 持ち物
    //! @param  earned     [in] この宇宙の累計
    //! @param  threshold  [in] ビッグバンできる累計
    //! @return レア度ごとの確率と未入手の数
    //-------------------------------------------------------------
    inline SkyObjectOdds ComputeSkyObjectOdds(const SkyObjectConfigComponent& config,
                                              const StarCollectionComponent&  collection,
                                              const Core::BigNumber&          earned,
                                              double                          threshold) {
        SkyObjectOdds odds;
        odds.remaining.assign(config.baseWeights.size(), 0);
        for(const Data::SkyObjectDef& object : Data::GetSkyObjectTable().objects) {
            if(object.rarity >= 0 && object.rarity < static_cast<int>(odds.remaining.size()) && !OwnsSkyObject(collection, object.id))
                ++odds.remaining[static_cast<size_t>(object.rarity)];
        }
        const double score = earned.IsZero() ? 0.0 : Core::SkyObjectDraw::EarnedScore(earned.Log10(), std::log10(threshold));
        odds.rates         = Core::SkyObjectDraw::TierRates(config.baseWeights, config.growths, odds.remaining, score);
        return odds;
    }
}    // namespace StardustFever::ECS
