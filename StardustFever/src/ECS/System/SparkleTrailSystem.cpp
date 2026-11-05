//-------------------------------------------------------------
//! @file   SparkleTrailSystem.cpp
//! @brief  SparkleTrailSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/SparkleTrailSystem.hpp>

#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/SparkleTrailComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Particle/BurstEmitter.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>
#include <Tsukino/Core/Path.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    SparkleTrailSystem::SparkleTrailSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 動いている実体の後ろに粒子を撒く
    //-------------------------------------------------------------
    void SparkleTrailSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->assetManager || deltaTime <= 0.0f)
            return;

        const FeverStateComponent* fever       = FindSingleton<FeverStateComponent>(registry);
        const bool                 feverActive = fever && fever->active;

        registry.View<SparkleTrailComponent, Tsukino::BuiltIn::ECS::TransformComponent>().each(
            [&](entt::entity entity, SparkleTrailComponent& trail, const Tsukino::BuiltIn::ECS::TransformComponent& transform) {
                const hlslpp::float3 position = transform.position;
                const float          speed    = trail.hasLast ? static_cast<float>(hlslpp::length(position - trail.lastPosition)) / deltaTime : 0.0f;
                trail.lastPosition            = position;
                trail.hasLast                 = true;

                // 入賞口やアウト口に入って消える途中の玉は撒かない
                if(const BallComponent* ball = registry.try_get<BallComponent>(entity); ball && ball->consumed)
                    return;

                trail.timer -= deltaTime;
                if(trail.timer > 0.0f || speed < trail.minSpeed || trail.texture.empty())
                    return;
                trail.timer = trail.interval;

                Tsukino::BuiltIn::ECS::BurstParams params = trail.burst;
                if(feverActive) {
                    params.colorA = trail.feverColorA;
                    params.colorB = trail.feverColorB;
                }
                const Tsukino::Asset::AssetHandle texture = context->assetManager->Load(Tsukino::Core::Path(trail.texture));
                Tsukino::BuiltIn::ECS::BurstEmitter::Emit(*m_scene, texture, params, position, m_random);
            });
    }

}    // namespace StardustFever::ECS
