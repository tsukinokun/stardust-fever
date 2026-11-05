//-------------------------------------------------------------
//! @file   GlowHaloSystem.cpp
//! @brief  GlowHaloSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/GlowHaloSystem.hpp>

#include <StardustFever/ECS/Component/GlowHaloComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>

#include <Tsukino/BuiltIn/ECS/Component/PointLightComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        constexpr float kTwoPi = 6.2831853f;
    }

    GlowHaloSystem::GlowHaloSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 光輪を作り、動かし、片付ける
    //-------------------------------------------------------------
    void GlowHaloSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        using Tsukino::BuiltIn::ECS::PointLightComponent;
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        m_time += deltaTime;

        // 持ち主が消えた光輪を片付ける
        std::erase_if(m_halos, [&](const std::pair<entt::entity, entt::entity>& pair) {
            if(registry.IsValid(pair.first) && registry.HasComponent<GlowHaloComponent>(pair.first))
                return false;
            if(registry.IsValid(pair.second))
                registry.QueueDestroy(pair.second);
            return true;
        });

        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->assetManager)
            return;
        std::uniform_real_distribution<float> unit(0.0f, 1.0f);

        for(const auto owner : registry.View<GlowHaloComponent, TransformComponent>()) {
            GlowHaloComponent&        glow      = registry.GetComponent<GlowHaloComponent>(owner);
            const TransformComponent& transform = registry.GetComponent<TransformComponent>(owner);

            //-------------------------------------------------------------
            // 初めて見た持ち主には光輪を作る。呼吸の周期と位相はここで乱数で決める
            //-------------------------------------------------------------
            if(glow.halo == entt::null || !registry.IsValid(glow.halo)) {
                if(glow.texture.empty())
                    continue;
                glow.halo   = m_scene->CreateEntity();
                glow.period = glow.periodMin + (glow.periodMax - glow.periodMin) * unit(m_random);
                glow.phase  = unit(m_random) * kTwoPi;

                registry.AddComponent<TransformComponent>(glow.halo);
                SpriteComponent& sprite = registry.AddComponent<SpriteComponent>(glow.halo);
                sprite.textureHandle    = context->assetManager->Load(Tsukino::Core::Path(glow.texture));
                sprite.space            = Tsukino::BuiltIn::ECS::SpriteSpace::World;
                sprite.blendMode        = Tsukino::BuiltIn::ECS::SpriteBlendMode::Additive;
                sprite.raycastTarget    = false;
                if(glow.lightIntensity > 0.0f || glow.linkedLightIntensity > 0.0f)
                    registry.AddComponent<PointLightComponent>(glow.halo);
                m_halos.emplace_back(owner, glow.halo);
            }

            const StarPinComponent* pin    = registry.try_get<StarPinComponent>(owner);
            const bool              linked = pin && pin->linked;

            const float breath = 1.0f + glow.pulseAmplitude * std::sin(m_time * kTwoPi / std::max(glow.period, 0.01f) + glow.phase);
            const float scale  = glow.size * (linked ? glow.linkedSizeScale : 1.0f) * breath / std::max(glow.texturePixels, 1.0f);

            TransformComponent& haloTransform = registry.GetComponent<TransformComponent>(glow.halo);
            haloTransform.position            = transform.position + glow.offset;
            haloTransform.scale               = hlslpp::float3(scale, scale, 1.0f);
            haloTransform.dirty               = true;

            registry.GetComponent<SpriteComponent>(glow.halo).tintColor = linked ? glow.linkedColor : glow.color;

            if(PointLightComponent* light = registry.try_get<PointLightComponent>(glow.halo)) {
                light->color     = glow.lightColor;
                light->intensity = (linked ? glow.linkedLightIntensity : glow.lightIntensity) * breath;
                light->range     = glow.lightRange;
                light->enabled   = light->intensity > 0.0f;
            }
        }
    }

}    // namespace StardustFever::ECS
