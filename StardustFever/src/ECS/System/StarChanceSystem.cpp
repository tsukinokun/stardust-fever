//-------------------------------------------------------------
//! @file   StarChanceSystem.cpp
//! @brief  StarChanceSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarChanceSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/HiddenPinComponent.hpp>
#include <StardustFever/ECS/Component/LinkLineComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PinComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Event/StarChanceEndedEvent.hpp>
#include <StardustFever/ECS/Event/StarChanceStartedEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/CollisionComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/ModelComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/Log.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::CollisionComponent;
        using Tsukino::BuiltIn::ECS::ModelComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        //-------------------------------------------------------------
        //! @brief  星釘と重なる普通の釘を隠し、当たり判定を外す
        //! @param  registry [in] レジストリ
        //! @param  stars    [in] 星釘を置く位置
        //! @param  radius   [in] この距離の中の釘を隠す
        //! @note   CollisionComponent を外すと物理の Body も消える。外したものは HiddenPinComponent に取っておく
        //-------------------------------------------------------------
        void HideOverlappingPins(Tsukino::ECS::Registry& registry, const std::vector<hlslpp::float2>& stars, float radius) {
            std::vector<entt::entity> targets;
            for(const auto entity : registry.View<PinComponent, TransformComponent, CollisionComponent>()) {
                const TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
                const float               x         = transform.position.x;
                const float               y         = transform.position.y;
                for(const hlslpp::float2& star : stars) {
                    const float dx = x - static_cast<float>(star.x);
                    const float dy = y - static_cast<float>(star.y);
                    if(dx * dx + dy * dy < radius * radius) {
                        targets.push_back(entity);
                        break;
                    }
                }
            }

            for(const entt::entity entity : targets) {
                HiddenPinComponent& hidden = registry.AddComponent<HiddenPinComponent>(entity);
                hidden.collision           = registry.GetComponent<CollisionComponent>(entity);
                registry.RemoveComponent<CollisionComponent>(entity);
                if(ModelComponent* model = registry.try_get<ModelComponent>(entity))
                    model->visible = false;
            }
        }

        //-------------------------------------------------------------
        //! @brief  隠した釘を戻す
        //! @param  registry [in] レジストリ
        //! @note   取っておいた当たり判定は作り直してもらうため、物理の Body を持っていない状態に戻してから付ける
        //-------------------------------------------------------------
        void RestoreHiddenPins(Tsukino::ECS::Registry& registry) {
            std::vector<entt::entity> targets;
            for(const auto entity : registry.View<HiddenPinComponent>()) {
                targets.push_back(entity);
            }
            for(const entt::entity entity : targets) {
                CollisionComponent collision = registry.GetComponent<HiddenPinComponent>(entity).collision;
                collision.isInitialized      = false;
                collision.bodyID             = {};
                registry.RemoveComponent<HiddenPinComponent>(entity);
                if(!registry.HasComponent<CollisionComponent>(entity))
                    registry.AddComponent<CollisionComponent>(entity) = collision;
                if(ModelComponent* model = registry.try_get<ModelComponent>(entity))
                    model->visible = true;
            }
        }
    }    // namespace

    StarChanceSystem::StarChanceSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_pocketEntries(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 抽選し、残り時間を進め、終わったら片付ける
    //-------------------------------------------------------------
    void StarChanceSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto entries = m_pocketEntries.Drain();
        // 星図の画面を開いている間は止める
        if(IsPlayPaused(registry))
            return;
        StarChanceStateComponent*        state  = FindSingleton<StarChanceStateComponent>(registry);
        const StarChanceConfigComponent* config = FindSingleton<StarChanceConfigComponent>(registry);
        if(!state || !config)
            return;

        state->missToastTimer = std::max(0.0f, state->missToastTimer - deltaTime);

        //-------------------------------------------------------------
        // 星の門に入ったら抽選する。フィーバー中や、フィーバーの抽選のリールが回っている間は黙って見送る
        // （チャンスの途中でフィーバーが当たると、星釘とフィーバーが重なってしまうため）
        //-------------------------------------------------------------
        for(const BallEnteredPocketEvent& entry : entries) {
            const PocketComponent* pocket = registry.IsValid(entry.pocket) ? registry.try_get<PocketComponent>(entry.pocket) : nullptr;
            if(!pocket || pocket->role != "starGate" || state->active)
                continue;

            const FeverStateComponent*   fever   = FindSingleton<FeverStateComponent>(registry);
            const LotteryStateComponent* lottery = FindSingleton<LotteryStateComponent>(registry);
            if((fever && fever->active) || (lottery && lottery->spinning))
                continue;

            const UpgradeModifiersComponent*    upgrades   = FindSingleton<UpgradeModifiersComponent>(registry);
            const CollectionModifiersComponent* collection = FindSingleton<CollectionModifiersComponent>(registry);
            const double                        probability =
                config->chanceProbability + (upgrades ? upgrades->starChanceBonus : 0.0) + (collection ? collection->starChanceBonus : 0.0);
            if(std::bernoulli_distribution(std::clamp(probability, 0.0, 1.0))(m_random)) {
                StartChance(registry);
            } else if(state->missToastTimer <= 0.0f) {
                m_eventBus->Publish(HudMessageEvent{"星の門  星は応えなかった", HudMessageImportance::Toast, HudMessageTone::Info});
                state->missToastTimer = config->missToastInterval;
            }
        }

        if(!state->active)
            return;

        state->timeRemaining -= deltaTime;
        if(state->completed)
            state->endDelay -= deltaTime;
        if(state->timeRemaining <= 0.0f || (state->completed && state->endDelay <= 0.0f))
            EndChance(registry);
    }

    //-------------------------------------------------------------
    // 星座を選んで星釘を並べる
    //-------------------------------------------------------------
    void StarChanceSystem::StartChance(Tsukino::ECS::Registry& registry) {
        StarChanceStateComponent*                  state   = FindSingleton<StarChanceStateComponent>(registry);
        const StarChanceConfigComponent*           config  = FindSingleton<StarChanceConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!state || !config || !context || !context->prefabFactory)
            return;

        // 盤面に出せる星が2つ以上ある星座から選ぶ
        const Data::ConstellationTable&            table = Data::GetConstellationTable();
        std::vector<const Data::ConstellationDef*> candidates;
        for(const Data::ConstellationDef& constellation : table.constellations) {
            if(constellation.board.stars.size() >= 2)
                candidates.push_back(&constellation);
        }
        if(candidates.empty()) {
            Tsukino::Core::Log::Warn("Star chance won, but no constellation has board stars. Run Tools/BuildSkyCatalog.py.");
            return;
        }
        const Data::ConstellationDef& constellation = *candidates[std::uniform_int_distribution<size_t>(0, candidates.size() - 1)(m_random)];

        //-------------------------------------------------------------
        // 先に重なる釘を隠してから星釘を置く（星釘はセンサーなので、釘が残ると玉が星へ届かない）
        //-------------------------------------------------------------
        std::vector<hlslpp::float2> positions;
        for(const Data::ConstellationBoardStarDef& star : constellation.board.stars) {
            positions.emplace_back(star.x, star.y);
        }
        HideOverlappingPins(registry, positions, config->pinHideRadius);

        int placed = 0;
        for(size_t i = 0; i < constellation.board.stars.size(); ++i) {
            const Data::ConstellationBoardStarDef& star   = constellation.board.stars[i];
            const entt::entity                     entity = context->prefabFactory->Instantiate(config->starPinPrefab, registry);
            if(entity == entt::null) {
                Tsukino::Core::Log::Error("Failed to instantiate star pin prefab: " + config->starPinPrefab);
                continue;
            }
            if(TransformComponent* transform = registry.try_get<TransformComponent>(entity)) {
                transform->position = hlslpp::float3(star.x, star.y, transform->position.z);
                transform->dirty    = true;
            }
            StarPinComponent& pin = registry.HasComponent<StarPinComponent>(entity) ? registry.GetComponent<StarPinComponent>(entity)
                                                                                    : registry.AddComponent<StarPinComponent>(entity);
            pin.starId            = star.starId;
            pin.starIndex         = static_cast<int>(i);
            pin.linked            = false;
            ++placed;
        }

        state->active          = true;
        state->constellationId = constellation.id;
        state->starsLinked     = 0;
        state->starsTotal      = placed;
        state->timeRemaining   = config->duration;
        state->timeTotal       = config->duration;
        state->completed       = false;
        state->endDelay        = config->endDelayAfterComplete;

        m_eventBus->Publish(StarChanceStartedEvent{constellation.id});
        m_eventBus->Publish(HudMessageEvent{"★ 星ゲットチャンス ★\n" + constellation.name + "を全部つなげ！", HudMessageImportance::Headline});
    }

    //-------------------------------------------------------------
    // 星釘と線を片付け、隠した釘を戻して、結果を知らせる
    //-------------------------------------------------------------
    void StarChanceSystem::EndChance(Tsukino::ECS::Registry& registry) {
        StarChanceStateComponent* state = FindSingleton<StarChanceStateComponent>(registry);
        if(!state)
            return;

        for(const auto entity : registry.View<StarPinComponent>()) {
            registry.QueueDestroy(entity);
        }
        for(const auto entity : registry.View<LinkLineComponent>()) {
            registry.QueueDestroy(entity);
        }
        RestoreHiddenPins(registry);

        state->active = false;
        m_eventBus->Publish(StarChanceEndedEvent{state->constellationId, state->completed, state->starsLinked, state->starsTotal});
    }

}    // namespace StardustFever::ECS
