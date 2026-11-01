//-------------------------------------------------------------
//! @file   StarProgressSystem.cpp
//! @brief  StarProgressSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarProgressSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/StarLinkConfigComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Event/StardustEarnedEvent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/ECS/Event/EventBus.hpp>

#include <algorithm>
#include <cstdio>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        //-------------------------------------------------------------
        //! @brief  倍率を「×2.5」のような短い文字列にする
        //! @param  value [in] 倍率
        //! @return 表示用の文字列（UTF-8）
        //-------------------------------------------------------------
        std::string FormatMultiplier(double value) {
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "×%g", value);
            return buffer;
        }

        //-------------------------------------------------------------
        //! @brief  集めた星と星座から全体倍率を計算する
        //! @param  collection [in] 集めた星と星座
        //! @return 全体倍率
        //! @note   星は (1 + levelBonus × レベル) を、星座は completionMultiplier^完成回数 を掛け合わせる。
        //!         どちらも掛け算で積み上がるので、集めるほど指数的に増える（インフレの源）
        //-------------------------------------------------------------
        Core::BigNumber ComputeMultiplier(const StarCollectionComponent& collection) {
            const Data::StarTable&          stars          = Data::GetStarTable();
            const Data::ConstellationTable& constellations = Data::GetConstellationTable();

            Core::BigNumber total(1.0);
            for(const auto& [starId, level] : collection.starLevels) {
                if(const Data::StarDef* star = stars.Find(starId))
                    total *= Core::BigNumber(1.0 + star->levelBonus * static_cast<double>(level));
            }
            for(const auto& [constellationId, count] : collection.constellationCompletions) {
                if(const Data::ConstellationDef* constellation = constellations.Find(constellationId))
                    total *= Core::BigNumber::Pow(constellation->completionMultiplier, static_cast<double>(count));
            }
            return total;
        }
    }    // namespace

    StarProgressSystem::StarProgressSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_linkedStars(eventBus)
        , m_completions(eventBus) {
    }

    //-------------------------------------------------------------
    // 記録を更新し、倍率を計算し直す
    //-------------------------------------------------------------
    void StarProgressSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const auto                     linkedStars = m_linkedStars.Drain();
        const auto                     completions = m_completions.Drain();
        StarCollectionComponent*       collection  = FindSingleton<StarCollectionComponent>(registry);
        const StarLinkConfigComponent* config      = FindSingleton<StarLinkConfigComponent>(registry);
        if(!collection || !config)
            return;

        for(const StarLinkedEvent& linked : linkedStars) {
            int& level = collection->starLevels[linked.starId];
            level      = std::min(level + 1, std::max(1, config->maxStarLevel));

            const Data::StarDef* star    = Data::GetStarTable().Find(linked.starId);
            std::string          message = (star ? star->name : linked.starId) + " ★" + std::to_string(level);
            if(linked.chain >= 2)
                message += "  一筆書き " + std::to_string(linked.chain) + "連 " + FormatMultiplier(linked.strokeMultiplier);
            m_eventBus->Publish(HudMessageEvent{message});
        }

        for(const ConstellationCompletedEvent& completed : completions) {
            ++collection->constellationCompletions[completed.constellationId];

            const Data::ConstellationDef* constellation = Data::GetConstellationTable().Find(completed.constellationId);
            if(!constellation)
                continue;

            const double bonusRate = completed.perfect ? config->perfectMultiplier : 1.0;
            m_eventBus->Publish(StardustEarnedEvent{Core::BigNumber(constellation->completionPayout * bonusRate)});

            std::string message = constellation->name + " 完成！  全体倍率 " + FormatMultiplier(constellation->completionMultiplier);
            if(completed.perfect)
                message += "\n一筆書きパーフェクト！ 完成ボーナス " + FormatMultiplier(config->perfectMultiplier);
            m_eventBus->Publish(HudMessageEvent{message});
        }

        if(ScoreMultiplierComponent* multiplier = FindSingleton<ScoreMultiplierComponent>(registry))
            multiplier->total = ComputeMultiplier(*collection);
    }

}    // namespace StardustFever::ECS
