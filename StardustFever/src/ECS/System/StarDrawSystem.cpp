//-------------------------------------------------------------
//! @file   StarDrawSystem.cpp
//! @brief  StarDrawSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarDrawSystem.hpp>

#include <StardustFever/Core/StarDraw.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Event/ConstellationCollectedEvent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Event/StarDrawnEvent.hpp>
#include <StardustFever/ECS/Event/StardustEarnedEvent.hpp>
#include <StardustFever/ECS/Utility/CollectionText.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/ECS/Event/EventBus.hpp>

#include <algorithm>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        //-------------------------------------------------------------
        //! @brief  星座の星を全部持っているか
        //! @param  constellation [in] 星座
        //! @param  collection    [in] 持ち物
        //! @return 線の星を全部持っていれば true
        //-------------------------------------------------------------
        bool HasAllStars(const Data::ConstellationDef& constellation, const StarCollectionComponent& collection) {
            if(constellation.stars.empty())
                return false;
            return std::all_of(constellation.stars.begin(), constellation.stars.end(), [&](const Data::ConstellationStarDef& star) {
                return collection.starLevels.contains(star.starId);
            });
        }
    }    // namespace

    StarDrawSystem::StarDrawSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_ends(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 終わったチャンスの結果を処理する
    //-------------------------------------------------------------
    void StarDrawSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const auto                       ends       = m_ends.Drain();
        StarCollectionComponent*         collection = FindSingleton<StarCollectionComponent>(registry);
        const StarChanceConfigComponent* config     = FindSingleton<StarChanceConfigComponent>(registry);
        if(!collection || !config)
            return;

        const Data::StarTable&          stars          = Data::GetStarTable();
        const Data::ConstellationTable& constellations = Data::GetConstellationTable();

        for(const StarChanceEndedEvent& end : ends) {
            if(!end.success) {
                m_eventBus->Publish(HudMessageEvent{"星ゲットならず  " + std::to_string(end.starsLinked) + " / " + std::to_string(end.starsTotal),
                                                    HudMessageImportance::Toast,
                                                    HudMessageTone::Warning});
                continue;
            }

            //-------------------------------------------------------------
            // 明るい星ほど出にくい重みで1つ引く
            //-------------------------------------------------------------
            std::vector<double> weights;
            weights.reserve(stars.stars.size());
            for(const Data::StarDef& star : stars.stars) {
                weights.push_back(Core::StarDraw::Weight(star.magnitude, config->rarityBase));
            }
            const size_t index = Core::StarDraw::PickIndex(weights, std::uniform_real_distribution<double>(0.0, 1.0)(m_random));
            if(index >= stars.stars.size())
                continue;
            const Data::StarDef& star = stars.stars[index];

            const auto                    owned   = collection->starLevels.find(star.id);
            const int                     current = owned == collection->starLevels.end() ? 0 : owned->second;
            const Core::StarDraw::Outcome outcome = Core::StarDraw::Apply(current, config->maxStarLevel);
            if(!outcome.converted)
                collection->starLevels[star.id] = outcome.level;
            if(outcome.converted)
                m_eventBus->Publish(StardustEarnedEvent{Core::BigNumber(star.basePayout * config->duplicatePayoutScale)});

            // 最近手に入れた星（新しい順、重なりなし）
            std::erase(collection->recentStars, star.id);
            collection->recentStars.insert(collection->recentStars.begin(), star.id);
            if(static_cast<int>(collection->recentStars.size()) > std::max(1, config->recentCount))
                collection->recentStars.resize(static_cast<size_t>(std::max(1, config->recentCount)));

            //-------------------------------------------------------------
            // 初めての星なら、それで星座がそろったかを確かめる
            //-------------------------------------------------------------
            std::vector<const Data::ConstellationDef*> collected;
            if(outcome.isNew) {
                for(const Data::ConstellationDef& constellation : constellations.constellations) {
                    const bool contains = std::any_of(constellation.stars.begin(), constellation.stars.end(), [&](const Data::ConstellationStarDef& member) {
                        return member.starId == star.id;
                    });
                    if(contains && HasAllStars(constellation, *collection))
                        collected.push_back(&constellation);
                }
            }
            const bool allComplete = !collected.empty()
                                     && std::all_of(constellations.constellations.begin(),
                                                    constellations.constellations.end(),
                                                    [&](const Data::ConstellationDef& c) { return HasAllStars(c, *collection); });

            //-------------------------------------------------------------
            // 星座がそろったなら、それでグループ（季節の星座など）もそろったかを確かめる
            //-------------------------------------------------------------
            std::vector<const Data::ConstellationGroupDef*> completedGroups;
            for(const Data::ConstellationGroupDef& group : Data::GetConstellationGroupTable().groups) {
                const bool touched  = std::any_of(collected.begin(), collected.end(), [&](const Data::ConstellationDef* c) {
                    return std::find(group.constellationIds.begin(), group.constellationIds.end(), c->id) != group.constellationIds.end();
                });
                const bool complete = touched && std::all_of(group.constellationIds.begin(), group.constellationIds.end(), [&](const std::string& id) {
                                          const Data::ConstellationDef* member = constellations.Find(id);
                                          return member && HasAllStars(*member, *collection);
                                      });
                if(complete)
                    completedGroups.push_back(&group);
            }

            //-------------------------------------------------------------
            // 知らせる。星そのものは StarGetFxSystem が見せるので、ここでは星座がそろった大見出しを
            // 演出のあとへ回す。大見出しは1枚だけなので、いちばん大きな出来事を選ぶ
            //-------------------------------------------------------------
            std::string followUp;
            if(allComplete) {
                followUp = "★ 88星座 すべてそろった ★\n全ての効果が大きく強まる";
            } else if(!completedGroups.empty()) {
                const Data::ConstellationGroupDef& group = *completedGroups.front();
                followUp                                 = "★ " + group.name + " がそろった ★\n" + CollectionText::Effect(group.effect, group.effect.value);
                for(const Data::ConstellationDef* constellation : collected) {
                    m_eventBus->Publish(
                        HudMessageEvent{constellation->name + "がそろった  " + CollectionText::Effect(constellation->effect, constellation->effect.value),
                                        HudMessageImportance::Toast,
                                        HudMessageTone::Positive});
                }
            } else if(!collected.empty()) {
                const Data::ConstellationDef& first = *collected.front();
                followUp                            = "★ " + first.name + "がそろった ★\n" + CollectionText::Effect(first.effect, first.effect.value);
            }

            m_eventBus->Publish(StarDrawnEvent{star.id, outcome.level, outcome.isNew, outcome.converted, followUp});
            for(const Data::ConstellationDef* constellation : collected) {
                m_eventBus->Publish(ConstellationCollectedEvent{constellation->id});
            }
        }
    }

}    // namespace StardustFever::ECS
