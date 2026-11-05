//-------------------------------------------------------------
//! @file   StarProgressSystem.cpp
//! @brief  StarProgressSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarProgressSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
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
    }    // namespace

    StarProgressSystem::StarProgressSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_linkedStars(eventBus)
        , m_completions(eventBus) {
    }

    //-------------------------------------------------------------
    // 星のレベルと星座の完成回数を記録する
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
            m_eventBus->Publish(HudMessageEvent{message, HudMessageImportance::Toast, HudMessageTone::Positive});
        }

        for(const ConstellationCompletedEvent& completed : completions) {
            ++collection->constellationCompletions[completed.constellationId];

            const Data::ConstellationDef* constellation = Data::GetConstellationTable().Find(completed.constellationId);
            if(!constellation)
                continue;

            const double bonusRate = completed.perfect ? config->perfectMultiplier : 1.0;
            m_eventBus->Publish(StardustEarnedEvent{Core::BigNumber(constellation->completionPayout * bonusRate)});

            // 1行目が大見出し、2行目が補足になる
            std::string message = "★ " + constellation->name + " 完成 ★\n全体倍率 " + FormatMultiplier(constellation->completionMultiplier);
            if(completed.perfect)
                message += "  一筆書きパーフェクト！ 完成ボーナス " + FormatMultiplier(config->perfectMultiplier);
            m_eventBus->Publish(HudMessageEvent{message, HudMessageImportance::Headline});
        }
    }

}    // namespace StardustFever::ECS
