//-------------------------------------------------------------
//! @file   TulipSystem.cpp
//! @brief  TulipSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/TulipSystem.hpp>

#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/TulipWingComponent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    // 羽を動かす
    //-------------------------------------------------------------
    void TulipSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        // 星図の画面を開いている間は止める（物理も止まっているので、羽だけ動くと玉にめり込む）
        if(IsPlayPaused(registry))
            return;

        const FeverStateComponent* fever  = FindSingleton<FeverStateComponent>(registry);
        const float                target = fever && fever->active ? 1.0f : 0.0f;

        using Tsukino::BuiltIn::ECS::TransformComponent;
        for(const auto entity : registry.View<TulipWingComponent, TransformComponent>()) {
            TulipWingComponent& wing      = registry.GetComponent<TulipWingComponent>(entity);
            TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

            // 盤面に置かれた位置が蝶番。最初に一度だけ覚える
            if(!wing.initialized) {
                wing.hinge       = hlslpp::float2(transform.position.x, transform.position.y);
                wing.initialized = true;
            }

            const float step = wing.moveSeconds > 0.0f ? deltaTime / wing.moveSeconds : 1.0f;
            wing.openness    = target > wing.openness ? std::min(target, wing.openness + step) : std::max(target, wing.openness - step);

            //-------------------------------------------------------------
            // 角度を開き具合で混ぜ、羽の中心を蝶番から羽の向きへ半分の長さだけずらす
            //-------------------------------------------------------------
            const float eased   = wing.openness * wing.openness * (3.0f - 2.0f * wing.openness);
            const float degrees = wing.closedDeg + (wing.openDeg - wing.closedDeg) * eased;
            const float radians = degrees * 3.14159265f / 180.0f;
            const float centerX = static_cast<float>(wing.hinge.x) + std::cos(radians) * wing.halfLength;
            const float centerY = static_cast<float>(wing.hinge.y) + std::sin(radians) * wing.halfLength;
            transform.position  = hlslpp::float3(centerX, centerY, transform.position.z);
            transform.rotation  = hlslpp::quaternion::rotation_z(radians);
            transform.dirty     = true;
        }
    }

}    // namespace StardustFever::ECS
