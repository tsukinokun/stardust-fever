//-------------------------------------------------------------
//! @file   TwinkleSystem.cpp
//! @brief  TwinkleSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/TwinkleSystem.hpp>

#include <StardustFever/ECS/Component/CosmicFxConfigComponent.hpp>
#include <StardustFever/ECS/Component/TwinkleComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/MaterialPropertyBlockComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Particle/BurstEmitter.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    TwinkleSystem::TwinkleSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus)
        : m_scene(&scene)
        , m_hits(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 瞬きと閃きを自己発光に反映する
    //-------------------------------------------------------------
    void TwinkleSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        using Tsukino::BuiltIn::ECS::MaterialPropertyBlockComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        const auto                                 hits    = m_hits.Drain();
        const CosmicFxConfigComponent*             config  = FindSingleton<CosmicFxConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        std::uniform_real_distribution<float>      unit(0.0f, 1.0f);

        //-------------------------------------------------------------
        // 当たった釘を閃かせ、ときどき火花を出す
        //-------------------------------------------------------------
        for(const BallHitPinEvent& hit : hits) {
            if(!registry.IsValid(hit.pin))
                continue;
            if(TwinkleComponent* twinkle = registry.try_get<TwinkleComponent>(hit.pin))
                twinkle->flash = 1.0f;

            const TransformComponent* transform = registry.try_get<TransformComponent>(hit.pin);
            if(!config || !context || !context->assetManager || !transform || config->sparkTexture.empty())
                continue;
            if(unit(m_random) > config->pinSparkChance)
                continue;
            const Tsukino::Asset::AssetHandle texture = context->assetManager->Load(Tsukino::Core::Path(config->sparkTexture));
            Tsukino::BuiltIn::ECS::BurstEmitter::Emit(*m_scene, texture, config->pinSpark, transform->position, m_random);
        }

        //-------------------------------------------------------------
        // 自己発光 = ふだん → 瞬き（山なり）→ 閃き（すぐ強く、ゆっくり消える）の大きい方
        //-------------------------------------------------------------
        for(const auto entity : registry.View<TwinkleComponent>()) {
            TwinkleComponent& twinkle = registry.GetComponent<TwinkleComponent>(entity);

            if(twinkle.timer < 0.0f)
                twinkle.timer = twinkle.intervalMin + (twinkle.intervalMax - twinkle.intervalMin) * unit(m_random);

            twinkle.timer -= deltaTime;
            if(twinkle.timer <= 0.0f) {
                twinkle.twinkleTime = twinkle.twinkleSeconds;
                twinkle.timer       = twinkle.intervalMin + (twinkle.intervalMax - twinkle.intervalMin) * unit(m_random);
            }
            twinkle.twinkleTime = std::max(0.0f, twinkle.twinkleTime - deltaTime);
            twinkle.flash       = std::max(0.0f, twinkle.flash - twinkle.flash * twinkle.flashDecay * deltaTime - 0.02f * deltaTime);

            // 瞬きは 0 → 1 → 0 の山。sin の半周で、急に点いて消えるように見えないようにする
            const float    progress = twinkle.twinkleSeconds > 0.0f ? twinkle.twinkleTime / twinkle.twinkleSeconds : 0.0f;
            const float    peak     = std::sin(progress * 3.14159265f);
            hlslpp::float3 emissive = hlslpp::lerp(twinkle.baseEmissive, twinkle.peakEmissive, hlslpp::float3(peak));
            emissive                = hlslpp::lerp(emissive, twinkle.flashEmissive, hlslpp::float3(twinkle.flash));

            MaterialPropertyBlockComponent& block = registry.HasComponent<MaterialPropertyBlockComponent>(entity)
                                                        ? registry.GetComponent<MaterialPropertyBlockComponent>(entity)
                                                        : registry.AddComponent<MaterialPropertyBlockComponent>(entity);
            block.emissive                        = emissive;
        }
    }

}    // namespace StardustFever::ECS
