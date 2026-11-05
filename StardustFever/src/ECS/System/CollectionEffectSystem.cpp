//-------------------------------------------------------------
//! @file   CollectionEffectSystem.cpp
//! @brief  CollectionEffectSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/CollectionEffectSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/Log.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    // 効果を計算し直す
    //-------------------------------------------------------------
    void CollectionEffectSystem::Update(Tsukino::ECS::Registry& registry, float) {
        CollectionModifiersComponent*    modifiers  = FindSingleton<CollectionModifiersComponent>(registry);
        const StarCollectionComponent*   collection = FindSingleton<StarCollectionComponent>(registry);
        const StarChanceConfigComponent* config     = FindSingleton<StarChanceConfigComponent>(registry);
        if(!modifiers || !collection)
            return;

        const Data::StarTable&          stars          = Data::GetStarTable();
        const Data::ConstellationTable& constellations = Data::GetConstellationTable();

        CollectionModifiersComponent result;
        result.totalStars          = static_cast<int>(stars.stars.size());
        result.totalConstellations = static_cast<int>(constellations.constellations.size());
        for(const auto& [starId, level] : collection->starLevels) {
            if(level > 0 && stars.Find(starId))
                ++result.ownedStars;
        }

        //-------------------------------------------------------------
        // 星座の星を全部持っていれば、固有の効果を足す。
        // 効果の量は ★1 のときの値で、星の平均レベルが1上がるごとに effectLevelBonus 割ずつ伸ばす
        //-------------------------------------------------------------
        const double levelBonus = config ? config->effectLevelBonus : 0.0;
        for(const Data::ConstellationDef& constellation : constellations.constellations) {
            if(constellation.stars.empty())
                continue;
            int  levelSum = 0;
            bool complete = true;
            for(const Data::ConstellationStarDef& member : constellation.stars) {
                const auto found = collection->starLevels.find(member.starId);
                if(found == collection->starLevels.end() || found->second <= 0) {
                    complete = false;
                    break;
                }
                levelSum += found->second;
            }
            if(!complete)
                continue;
            ++result.collectedConstellations;

            const double       averageLevel = static_cast<double>(levelSum) / static_cast<double>(constellation.stars.size());
            const double       value        = constellation.effect.value * (1.0 + levelBonus * std::max(0.0, averageLevel - 1.0));
            const std::string& type         = constellation.effect.type;
            if(type == "global") {
                result.globalMultiplier *= 1.0 + value;
            } else if(type == "pinPayout") {
                result.pinPayoutMultiplier *= 1.0 + value;
            } else if(type == "pocketPayout") {
                result.pocketPayoutMultiplier *= 1.0 + value;
            } else if(type == "lotteryChance") {
                result.lotteryChanceBonus += value;
            } else if(type == "starChance") {
                result.starChanceBonus += value;
            } else if(type == "feverDuration") {
                result.feverDurationBonus += static_cast<float>(value);
            } else if(m_warnedTypes.insert(type).second) {
                Tsukino::Core::Log::Warn("Unknown constellation effect type '" + type + "' on " + constellation.id + ". It is ignored.");
            }
        }

        result.allComplete = result.totalConstellations > 0 && result.collectedConstellations == result.totalConstellations;
        if(result.allComplete && config)
            result.globalMultiplier *= config->allCompleteMultiplier;

        *modifiers = result;
    }

}    // namespace StardustFever::ECS
