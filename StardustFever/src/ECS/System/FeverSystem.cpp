//-------------------------------------------------------------
//! @file   FeverSystem.cpp
//! @brief  FeverSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/FeverSystem.hpp>

#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/FeverConfigComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Component/WidenedPocketComponent.hpp>
#include <StardustFever/ECS/Event/FeverEndedEvent.hpp>
#include <StardustFever/ECS/Event/FeverStartedEvent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/CollisionComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>

#include <cstdio>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::CollisionComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        //-------------------------------------------------------------
        //! @brief  入賞口の当たり判定を、新しい大きさで付け直す
        //! @param  registry [in] レジストリ
        //! @param  entity   [in] 入賞口
        //! @param  extent   [in] 新しい当たり判定の大きさ
        //! @note   物理の Body の形は作ったときに決まるので、CollisionComponent を外して付け直し、作り直してもらう
        //-------------------------------------------------------------
        void ReplaceCollisionExtent(Tsukino::ECS::Registry& registry, entt::entity entity, const hlslpp::float3& extent) {
            CollisionComponent collision = registry.GetComponent<CollisionComponent>(entity);
            collision.extent             = extent;
            collision.isInitialized      = false;
            collision.bodyID             = {};
            registry.RemoveComponent<CollisionComponent>(entity);
            registry.AddComponent<CollisionComponent>(entity) = collision;
        }

        //-------------------------------------------------------------
        //! @brief  横の入賞口（role "normal"）を横へ広げる
        //! @param  registry [in] レジストリ
        //! @param  scale    [in] 横へ広げる倍率
        //-------------------------------------------------------------
        void WidenSidePockets(Tsukino::ECS::Registry& registry, float scale) {
            std::vector<entt::entity> targets;
            for(const auto entity : registry.View<PocketComponent, CollisionComponent, TransformComponent>()) {
                if(registry.GetComponent<PocketComponent>(entity).role == "normal" && !registry.HasComponent<WidenedPocketComponent>(entity))
                    targets.push_back(entity);
            }
            for(const entt::entity entity : targets) {
                TransformComponent&     transform = registry.GetComponent<TransformComponent>(entity);
                WidenedPocketComponent& widened   = registry.AddComponent<WidenedPocketComponent>(entity);
                widened.extent                    = registry.GetComponent<CollisionComponent>(entity).extent;
                widened.scale                     = transform.scale;

                ReplaceCollisionExtent(registry, entity, hlslpp::float3(widened.extent.x * scale, widened.extent.y, widened.extent.z));
                transform.scale = hlslpp::float3(widened.scale.x * scale, widened.scale.y, widened.scale.z);
                transform.dirty = true;
            }
        }

        //-------------------------------------------------------------
        //! @brief  広げた入賞口を元の大きさへ戻す
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void RestoreSidePockets(Tsukino::ECS::Registry& registry) {
            std::vector<entt::entity> targets;
            for(const auto entity : registry.View<WidenedPocketComponent>()) {
                targets.push_back(entity);
            }
            for(const entt::entity entity : targets) {
                const WidenedPocketComponent widened = registry.GetComponent<WidenedPocketComponent>(entity);
                registry.RemoveComponent<WidenedPocketComponent>(entity);
                if(registry.HasComponent<CollisionComponent>(entity))
                    ReplaceCollisionExtent(registry, entity, widened.extent);
                if(TransformComponent* transform = registry.try_get<TransformComponent>(entity)) {
                    transform->scale = widened.scale;
                    transform->dirty = true;
                }
            }
        }
    }    // namespace

    FeverSystem::FeverSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_results(eventBus) {
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
        if(state->timeRemaining <= 0.0f)
            EndFever(registry);
    }

    //-------------------------------------------------------------
    // フィーバーを始め、横の入賞口を広げる
    //-------------------------------------------------------------
    void FeverSystem::StartFever(Tsukino::ECS::Registry& registry) {
        FeverStateComponent*        state  = FindSingleton<FeverStateComponent>(registry);
        const FeverConfigComponent* config = FindSingleton<FeverConfigComponent>(registry);
        if(!state || !config)
            return;

        const UpgradeModifiersComponent*    upgrades   = FindSingleton<UpgradeModifiersComponent>(registry);
        const CollectionModifiersComponent* collection = FindSingleton<CollectionModifiersComponent>(registry);
        state->active                                  = true;
        state->timeRemaining = config->duration + (upgrades ? upgrades->feverDurationBonus : 0.0f) + (collection ? collection->feverDurationBonus : 0.0f);
        state->timeTotal     = state->timeRemaining;

        if(config->sidePocketScale > 0.0f && config->sidePocketScale != 1.0f)
            WidenSidePockets(registry, config->sidePocketScale);

        char sub[96];
        std::snprintf(sub, sizeof(sub), "釘 ×%g  入賞 ×%g  横の入賞口が広がる", config->pinPayoutMultiplier, config->pocketPayoutMultiplier);
        m_eventBus->Publish(FeverStartedEvent{});
        m_eventBus->Publish(HudMessageEvent{std::string("★ 星詠みタイム ★\n") + sub, HudMessageImportance::Headline});
    }

    //-------------------------------------------------------------
    // フィーバーを終え、横の入賞口を戻す
    //-------------------------------------------------------------
    void FeverSystem::EndFever(Tsukino::ECS::Registry& registry) {
        FeverStateComponent* state = FindSingleton<FeverStateComponent>(registry);
        if(!state)
            return;

        RestoreSidePockets(registry);
        state->active = false;
        m_eventBus->Publish(FeverEndedEvent{});
        m_eventBus->Publish(HudMessageEvent{"星詠みタイム終了"});
    }

}    // namespace StardustFever::ECS
