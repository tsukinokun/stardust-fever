//-------------------------------------------------------------
//! @file   BallStuckSystem.cpp
//! @brief  BallStuckSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BallStuckSystem.hpp>

#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/LauncherConfigComponent.hpp>
#include <StardustFever/ECS/Event/BallDrainedEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/ImpulseRequestComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/RigidbodyComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Core/Log.hpp>

#include <cmath>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    BallStuckSystem::BallStuckSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 止まった玉を探して動かす
    //-------------------------------------------------------------
    void BallStuckSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        // 星図の画面を開いている間は物理も止まるので、止まったと数えない
        if(IsPlayPaused(registry))
            return;
        const LauncherConfigComponent* config = FindSingleton<LauncherConfigComponent>(registry);
        if(!config)
            return;

        using Tsukino::BuiltIn::ECS::TransformComponent;
        for(const auto entity : registry.View<BallComponent, TransformComponent>()) {
            BallComponent& ball = registry.GetComponent<BallComponent>(entity);
            if(ball.consumed)
                continue;

            //-------------------------------------------------------------
            // 基準の位置から離れていれば、動いているので数え直す
            //-------------------------------------------------------------
            const hlslpp::float3& position = registry.GetComponent<TransformComponent>(entity).position;
            const float           x        = static_cast<float>(position.x);
            const float           y        = static_cast<float>(position.y);
            if(std::hypot(x - ball.restX, y - ball.restY) > config->stuckDistance) {
                ball.restX     = x;
                ball.restY     = y;
                ball.restTimer = 0.0f;
                ball.nudges    = 0;
                continue;
            }

            ball.restTimer += deltaTime;
            if(ball.restTimer < config->stuckSeconds)
                continue;
            ball.restTimer = 0.0f;

            if(ball.nudges >= config->stuckMaxNudges) {
                Tsukino::Core::Log::Info("Ball stayed stuck, removing it at " + std::to_string(x) + ", " + std::to_string(y));
                ball.consumed = true;
                m_eventBus->Publish(BallDrainedEvent{entity, false});
                registry.QueueDestroy(entity);
                continue;
            }

            //-------------------------------------------------------------
            // 釣り合いを崩すよう、少し上向きに左右どちらかへ押す
            //-------------------------------------------------------------
            ++ball.nudges;
            Tsukino::Core::Log::Info("Ball stuck, nudging it at " + std::to_string(x) + ", " + std::to_string(y));
            float mass = 1.0f;
            if(const auto* rigidbody = registry.try_get<Tsukino::BuiltIn::ECS::RigidbodyComponent>(entity))
                mass = rigidbody->mass;
            const float side = std::bernoulli_distribution(0.5)(m_random) ? 1.0f : -1.0f;
            registry.AddComponent<Tsukino::BuiltIn::ECS::ImpulseRequestComponent>(entity).impulse =
                hlslpp::float3(side * mass * config->stuckNudge, mass * config->stuckNudge * 0.5f, 0.0f);
        }
    }

}    // namespace StardustFever::ECS
