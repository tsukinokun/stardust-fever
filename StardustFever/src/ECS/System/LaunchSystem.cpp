//-------------------------------------------------------------
//! @file   LaunchSystem.cpp
//! @brief  LaunchSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/LaunchSystem.hpp>

#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/LauncherComponent.hpp>
#include <StardustFever/ECS/Component/LauncherConfigComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Event/BallLaunchedEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/ImpulseRequestComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/RigidbodyComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/Log.hpp>

#include <algorithm>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    LaunchSystem::LaunchSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 玉を打ち出す
    //-------------------------------------------------------------
    void LaunchSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        // 星図の画面を開いている間は止める
        if(IsPlayPaused(registry))
            return;
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        const LauncherConfigComponent*             config  = FindSingleton<LauncherConfigComponent>(registry);
        if(!context || !context->prefabFactory || !config)
            return;

        int ballsOnBoard = 0;
        for([[maybe_unused]] const auto entity : registry.View<BallComponent>()) {
            ++ballsOnBoard;
        }

        //-------------------------------------------------------------
        // 玉の生成は TransformComponent を追加するので、それを含む View を回しながら
        // 行うと EnTT のイテレータが壊れる。先に発射台を集めておく
        //-------------------------------------------------------------
        using Tsukino::BuiltIn::ECS::TransformComponent;
        std::vector<entt::entity> launchers;
        for(const auto launcherEntity : registry.View<LauncherComponent, TransformComponent>()) {
            launchers.push_back(launcherEntity);
        }

        for(const entt::entity launcherEntity : launchers) {
            LauncherComponent& launcher = registry.GetComponent<LauncherComponent>(launcherEntity);
            launcher.cooldown           = std::max(0.0f, launcher.cooldown - deltaTime);
            launcher.autoCooldown       = std::max(0.0f, launcher.autoCooldown - deltaTime);
            if(ballsOnBoard >= config->maxBallsOnBoard)
                continue;

            //-------------------------------------------------------------
            // 手打ち（離した要求）は狙った強さそのまま。
            // 自動発射は溜めていない間だけ、最後に打った強さを基準に揺らいで打ち続ける（自動発射の代償）。
            // 溜めている最中に自動で打つと、狙っている最中の強さが勝手に使われてしまうため止める
            //-------------------------------------------------------------
            const AutoFireComponent* autoFire = registry.try_get<AutoFireComponent>(launcherEntity);
            // 自動発射の待ちは手打ちと別に数える。長い間隔の間も手打ちは止めない
            const bool autoFiring = autoFire && autoFire->unlocked && autoFire->enabled && !launcher.charging && launcher.autoCooldown <= 0.0f;
            const bool handShot   = launcher.fireRequested && launcher.cooldown <= 0.0f;
            if(!handShot && !autoFiring)
                continue;

            float                            power    = launcher.handlePower;
            const UpgradeModifiersComponent* upgrades = FindSingleton<UpgradeModifiersComponent>(registry);
            if(handShot) {
                launcher.fireRequested = false;
                launcher.cooldown      = config->fireInterval;
            } else {
                launcher.autoCooldown                       = config->autoFireInterval * (upgrades ? upgrades->autoFireIntervalScale : 1.0f);
                const float                           range = config->autoFireJitter * (upgrades ? upgrades->autoFireJitterScale : 1.0f);
                std::uniform_real_distribution<float> jitter(-range, range);
                power += jitter(m_random);
            }
            power                  = std::clamp(power, 0.0f, 1.0f);
            launcher.lastShotPower = power;
            const float speed      = config->minSpeed + (config->maxSpeed - config->minSpeed) * power;

            const entt::entity ball = context->prefabFactory->Instantiate(config->ballPrefab, registry);
            if(ball == entt::null) {
                Tsukino::Core::Log::Error("Failed to launch: ball prefab could not be instantiated: " + config->ballPrefab);
                continue;
            }

            const hlslpp::float3 launchPosition = registry.GetComponent<TransformComponent>(launcherEntity).position;
            if(TransformComponent* ballTransform = registry.try_get<TransformComponent>(ball)) {
                ballTransform->position = launchPosition;
                ballTransform->dirty    = true;
            }

            //-------------------------------------------------------------
            // 初速は力積で与える。RigidbodyComponent::linearVelocity はボディ生成時に
            // 物理へ渡されないため。力積はボディが作られたのと同じ物理の更新で適用される
            //-------------------------------------------------------------
            float mass = 1.0f;
            if(const auto* rigidbody = registry.try_get<Tsukino::BuiltIn::ECS::RigidbodyComponent>(ball))
                mass = rigidbody->mass;
            Tsukino::BuiltIn::ECS::ImpulseRequestComponent& impulse = registry.AddComponent<Tsukino::BuiltIn::ECS::ImpulseRequestComponent>(ball);
            impulse.impulse                                         = hlslpp::float3(0.0f, mass * speed, 0.0f);

            ++ballsOnBoard;
            m_eventBus->Publish(BallLaunchedEvent{});
        }
    }

}    // namespace StardustFever::ECS
