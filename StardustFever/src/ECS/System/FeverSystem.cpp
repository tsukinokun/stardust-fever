//-------------------------------------------------------------
//! @file   FeverSystem.cpp
//! @brief  FeverSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/FeverSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/FeverConfigComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LinkLineComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Event/FeverEndedEvent.hpp>
#include <StardustFever/ECS/Event/FeverStartedEvent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <StardustFever/ECS/Component/DrainComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>

#include <Tsukino/BuiltIn/ECS/Component/CollisionComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/Log.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::CollisionComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        //-------------------------------------------------------------
        //! @brief  星釘が、玉を消してしまう役物（入賞口・アウト口）に近すぎないか確かめる
        //! @param  registry   [in] レジストリ
        //! @param  starEntity [in] 置いたばかりの星釘
        //! @param  starId     [in] 星のID（ログ用）
        //! @param  minGap     [in] 星の縁と役物の間に要る隙間
        //! @note   近すぎると、星へ向かう玉が先に役物へ入って消え、その星はつなげなくなる。
        //!         配置は Tools/GenerateConstellations.py が確かめているが、手で書き換えたときの保険として警告を出す。
        //!         役物は回転させていない前提で、当たり判定の箱を軸に揃った箱として測る
        //-------------------------------------------------------------
        void WarnIfStarBlocked(Tsukino::ECS::Registry& registry, entt::entity starEntity, const std::string& starId, float minGap) {
            const TransformComponent* starTransform = registry.try_get<TransformComponent>(starEntity);
            const CollisionComponent* starCollision = registry.try_get<CollisionComponent>(starEntity);
            if(!starTransform || !starCollision)
                return;

            const float starX      = starTransform->position.x;
            const float starY      = starTransform->position.y;
            const float starRadius = starCollision->extent.x;

            auto check = [&](entt::entity blocker) {
                const TransformComponent* transform = registry.try_get<TransformComponent>(blocker);
                const CollisionComponent* collision = registry.try_get<CollisionComponent>(blocker);
                if(!transform || !collision)
                    return;

                const float centerX = transform->position.x;
                const float centerY = transform->position.y;
                const float halfX   = collision->extent.x;
                const float halfY   = collision->extent.y;
                const float dx      = std::max({centerX - halfX - starX, 0.0f, starX - (centerX + halfX)});
                const float dy      = std::max({centerY - halfY - starY, 0.0f, starY - (centerY + halfY)});
                const float gap     = std::sqrt(dx * dx + dy * dy) - starRadius;
                if(gap < minGap) {
                    char buffer[256];
                    std::snprintf(
                        buffer,
                        sizeof(buffer),
                        "Star '%s' at (%.1f, %.1f) is only %.2f from a pocket/drain at (%.1f, %.1f); balls may be swallowed before linking it. Run Tools/GenerateConstellations.py.",
                        starId.c_str(),
                        starX,
                        starY,
                        gap,
                        centerX,
                        centerY);
                    Tsukino::Core::Log::Warn(buffer);
                }
            };

            for(const auto entity : registry.View<PocketComponent>()) {
                check(entity);
            }
            for(const auto entity : registry.View<DrainComponent>()) {
                check(entity);
            }
        }
    }    // namespace

    FeverSystem::FeverSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_results(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // フィーバーを始める・進める・終える
    //-------------------------------------------------------------
    void FeverSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto                  results = m_results.Drain();
        FeverStateComponent*        state   = FindSingleton<FeverStateComponent>(registry);
        const FeverConfigComponent* config  = FindSingleton<FeverConfigComponent>(registry);
        if(!state || !config)
            return;

        for(const LotteryResultEvent& result : results) {
            if(result.won && !state->active)
                StartFever(registry);
        }

        if(!state->active)
            return;

        state->timeRemaining -= deltaTime;
        if(state->completed)
            state->endDelay -= deltaTime;

        if(state->timeRemaining <= 0.0f || (state->completed && state->endDelay <= 0.0f))
            EndFever(registry);
    }

    //-------------------------------------------------------------
    // 星座を選んで星釘を並べる
    //-------------------------------------------------------------
    void FeverSystem::StartFever(Tsukino::ECS::Registry& registry) {
        FeverStateComponent*                       state   = FindSingleton<FeverStateComponent>(registry);
        const FeverConfigComponent*                config  = FindSingleton<FeverConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!state || !config || !context || !context->prefabFactory)
            return;

        const Data::ConstellationTable& table = Data::GetConstellationTable();
        if(table.constellations.empty()) {
            Tsukino::Core::Log::Warn("Fever won, but no constellation data is loaded. Skipping the fever.");
            return;
        }

        std::uniform_int_distribution<size_t> pick(0, table.constellations.size() - 1);
        const Data::ConstellationDef&         constellation = table.constellations[pick(m_random)];

        for(size_t i = 0; i < constellation.stars.size(); ++i) {
            const Data::ConstellationStarDef& star   = constellation.stars[i];
            const entt::entity                entity = context->prefabFactory->Instantiate(config->starPinPrefab, registry);
            if(entity == entt::null) {
                Tsukino::Core::Log::Error("Failed to instantiate star pin prefab: " + config->starPinPrefab);
                continue;
            }

            if(TransformComponent* transform = registry.try_get<TransformComponent>(entity)) {
                const float z       = transform->position.z;
                transform->position = hlslpp::float3(star.x, star.y, z);
                transform->dirty    = true;
            }

            StarPinComponent& pin = registry.HasComponent<StarPinComponent>(entity) ? registry.GetComponent<StarPinComponent>(entity)
                                                                                    : registry.AddComponent<StarPinComponent>(entity);
            pin.starId            = star.starId;
            pin.starIndex         = static_cast<int>(i);
            pin.linked            = false;

            WarnIfStarBlocked(registry, entity, star.starId, config->minStarToPocketGap);
        }

        // 一筆書きは「このフィーバーの中で1球が何個つないだか」なので、前のフィーバーの分を消す
        for(const auto ball : registry.View<BallComponent>()) {
            registry.GetComponent<BallComponent>(ball).linkedStarCount = 0;
        }

        state->active                             = true;
        const UpgradeModifiersComponent* upgrades = FindSingleton<UpgradeModifiersComponent>(registry);
        state->timeRemaining                      = config->duration + (upgrades ? upgrades->feverDurationBonus : 0.0f);
        state->timeTotal                          = state->timeRemaining;
        state->constellationId                    = constellation.id;
        state->starsLinked                        = 0;
        state->starsTotal                         = static_cast<int>(constellation.stars.size());
        state->firstLinkBall                      = entt::null;
        state->perfect                            = true;
        state->completed                          = false;
        state->endDelay                           = config->endDelayAfterComplete;
        state->bestChain                          = 0;

        m_eventBus->Publish(FeverStartedEvent{constellation.id});
        m_eventBus->Publish(HudMessageEvent{"★ 星詠みタイム ★\n" + constellation.name + "を描け！", HudMessageImportance::Headline});
    }

    //-------------------------------------------------------------
    // 星釘と線を片付ける
    //-------------------------------------------------------------
    void FeverSystem::EndFever(Tsukino::ECS::Registry& registry) {
            FeverStateComponent* state = FindSingleton<FeverStateComponent>(registry);
            if(!state)
                return;

            for(const auto entity : registry.View<StarPinComponent>()) {
                registry.QueueDestroy(entity);
            }
            for(const auto entity : registry.View<LinkLineComponent>()) {
                registry.QueueDestroy(entity);
            }

            state->active = false;
            m_eventBus->Publish(FeverEndedEvent{state->constellationId, state->completed});
            if(!state->completed)
                m_eventBus->Publish(HudMessageEvent{"星詠みタイム終了  " + std::to_string(state->starsLinked) + " / " + std::to_string(state->starsTotal)});
    }

    }    // namespace StardustFever::ECS
