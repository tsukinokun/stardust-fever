//-------------------------------------------------------------
//! @file   ShootingStarSystem.cpp
//! @brief  ShootingStarSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/ShootingStarSystem.hpp>

#include <StardustFever/ECS/Component/CosmicFxConfigComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/BurstParticleComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    ShootingStarSystem::ShootingStarSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 間隔が来たら流れ星を流す
    //-------------------------------------------------------------
    void ShootingStarSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const CosmicFxConfigComponent*             config  = FindSingleton<CosmicFxConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager || config->streakTexture.empty())
            return;

        m_timer -= deltaTime;
        if(m_timer > 0.0f)
            return;

        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        auto                                  range = [&](float a, float b) { return a + (b - a) * unit(m_random); };

        const FeverStateComponent* fever = FindSingleton<FeverStateComponent>(registry);
        const float                scale = fever && fever->active ? config->feverIntervalScale : 1.0f;
        m_timer                          = range(config->shootingIntervalMin, config->shootingIntervalMax) * scale;

        //-------------------------------------------------------------
        // 流れ星を1つ作る。重力も減衰も無しで真っ直ぐ進め、寿命の間に透明へ消していく
        //-------------------------------------------------------------
        Tsukino::ECS::Entity entity    = m_scene->CreateEntity();
        auto&                transform = registry.AddComponent<Tsukino::BuiltIn::ECS::TransformComponent>(entity);
        transform.position             = hlslpp::float3(range(config->shootingSpawnMin.x, config->shootingSpawnMax.x),
                                            range(config->shootingSpawnMin.y, config->shootingSpawnMax.y),
                                            range(config->shootingSpawnMin.z, config->shootingSpawnMax.z));
        transform.scale                = hlslpp::float3(0.0f, 0.0f, 1.0f);

        auto& sprite         = registry.AddComponent<Tsukino::BuiltIn::ECS::SpriteComponent>(entity);
        sprite.textureHandle = context->assetManager->Load(Tsukino::Core::Path(config->streakTexture));
        sprite.space         = Tsukino::BuiltIn::ECS::SpriteSpace::World;
        sprite.blendMode     = Tsukino::BuiltIn::ECS::SpriteBlendMode::Additive;
        sprite.raycastTarget = false;
        sprite.tintColor     = config->shootingColor;

        const hlslpp::float3 direction = hlslpp::normalize(config->shootingDirection + hlslpp::float3(0.0f, 0.0f, 1e-4f));
        const float          size      = range(config->shootingSizeMin, config->shootingSizeMax);

        auto& particle         = registry.AddComponent<Tsukino::BuiltIn::ECS::BurstParticleComponent>(entity);
        particle.velocity      = direction * range(config->shootingSpeedMin, config->shootingSpeedMax);
        particle.gravity       = 0.0f;
        particle.drag          = 0.0f;
        particle.age           = 0.0f;
        particle.life          = std::max(config->shootingLife, 0.05f);
        particle.startSize     = size;
        particle.endSize       = size * 0.8f;
        particle.startColor    = config->shootingColor;
        particle.endColor      = hlslpp::float4(config->shootingColor.xyz, 0.0f);
        particle.texturePixels = config->streakPixels;
    }

}    // namespace StardustFever::ECS
