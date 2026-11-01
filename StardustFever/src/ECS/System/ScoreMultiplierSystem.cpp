//-------------------------------------------------------------
//! @file   ScoreMultiplierSystem.cpp
//! @brief  ScoreMultiplierSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/ScoreMultiplierSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    // 全体倍率を計算し直す
    //
    // 星は (1 + levelBonus × レベル)、星座は completionMultiplier^完成回数、
    // 宇宙定数は (1 + bonusPerConstant × Λ) を掛け合わせる。
    // どれも掛け算で積み上がるので、集めるほど指数的に増える（インフレの源）
    //-------------------------------------------------------------
    void ScoreMultiplierSystem::Update(Tsukino::ECS::Registry& registry, float) {
        ScoreMultiplierComponent* multiplier = FindSingleton<ScoreMultiplierComponent>(registry);
        if(!multiplier)
            return;

        Core::BigNumber total(1.0);

        if(const StarCollectionComponent* collection = FindSingleton<StarCollectionComponent>(registry)) {
            const Data::StarTable&          stars          = Data::GetStarTable();
            const Data::ConstellationTable& constellations = Data::GetConstellationTable();
            for(const auto& [starId, level] : collection->starLevels) {
                if(const Data::StarDef* star = stars.Find(starId))
                    total *= Core::BigNumber(1.0 + star->levelBonus * static_cast<double>(level));
            }
            for(const auto& [constellationId, count] : collection->constellationCompletions) {
                if(const Data::ConstellationDef* constellation = constellations.Find(constellationId))
                    total *= Core::BigNumber::Pow(constellation->completionMultiplier, static_cast<double>(count));
            }
        }

        const PrestigeStateComponent*  prestige = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
        if(prestige && config)
            total *= Core::PrestigeFormula::Multiplier(prestige->cosmicConstants, config->bonusPerConstant);

        multiplier->total = total;
    }

}    // namespace StardustFever::ECS
