//-------------------------------------------------------------
//! @file   BallEventSystem.cpp
//! @brief  BallEventSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BallEventSystem.hpp>

#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/DrainComponent.hpp>
#include <StardustFever/ECS/Component/LauncherConfigComponent.hpp>
#include <StardustFever/ECS/Component/PinComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Event/BallDrainedEvent.hpp>
#include <StardustFever/ECS/Event/BallEnteredPocketEvent.hpp>
#include <StardustFever/ECS/Event/BallHitPinEvent.hpp>
#include <StardustFever/ECS/Event/BallTouchedStarEvent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    BallEventSystem::BallEventSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_collisions(eventBus) {
    }

    //-------------------------------------------------------------
    // 溜まった衝突を振り分ける
    //-------------------------------------------------------------
    void BallEventSystem::Update(Tsukino::ECS::Registry& registry, float) {
        //-------------------------------------------------------------
        // 衝突は (A,B) と (B,A) の両方向で届く。self が玉の側だけを見れば1回ずつになる
        //-------------------------------------------------------------
        for(const Tsukino::BuiltIn::ECS::CollisionEnterEvent& collision : m_collisions.Drain()) {
            // 衝突してから今までの間に破棄されたエンティティもある
            if(!registry.IsValid(collision.self) || !registry.IsValid(collision.other))
                continue;

            BallComponent* ball = registry.try_get<BallComponent>(collision.self);
            // 入賞・アウト済みの玉は破棄待ちの間も物理の分割ステップで接触し続けるため無視する
            if(!ball || ball->consumed)
                continue;

            if(registry.HasComponent<PinComponent>(collision.other)) {
                m_eventBus->Publish(BallHitPinEvent{collision.self, collision.other});
            } else if(registry.HasComponent<PocketComponent>(collision.other)) {
                ball->consumed = true;
                m_eventBus->Publish(BallEnteredPocketEvent{collision.self, collision.other});
                registry.QueueDestroy(collision.self);
            } else if(const DrainComponent* drain = registry.try_get<DrainComponent>(collision.other)) {
                ball->consumed = true;
                m_eventBus->Publish(BallDrainedEvent{collision.self, drain->isFoul});
                registry.QueueDestroy(collision.self);
            } else if(registry.HasComponent<StarPinComponent>(collision.other)) {
                m_eventBus->Publish(BallTouchedStarEvent{collision.self, collision.other});
            }
        }

        //-------------------------------------------------------------
        // 壁の隙間から抜けた玉が永遠に落ち続けないよう、盤面の下で消す
        //-------------------------------------------------------------
        const LauncherConfigComponent* config = FindSingleton<LauncherConfigComponent>(registry);
        if(!config)
            return;

        using Tsukino::BuiltIn::ECS::TransformComponent;
        for(const auto entity : registry.View<BallComponent, TransformComponent>()) {
            BallComponent& ball = registry.GetComponent<BallComponent>(entity);
            if(ball.consumed)
                continue;
            const float y = registry.GetComponent<TransformComponent>(entity).position.y;
            if(y < config->killPlaneY) {
                ball.consumed = true;
                m_eventBus->Publish(BallDrainedEvent{entity, false});
                registry.QueueDestroy(entity);
            }
        }
    }

}    // namespace StardustFever::ECS
