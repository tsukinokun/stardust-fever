//-------------------------------------------------------------
//! @file   LauncherInputSystem.cpp
//! @brief  LauncherInputSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/LauncherInputSystem.hpp>

#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/LauncherComponent.hpp>
#include <StardustFever/ECS/Component/LauncherConfigComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>
#include <Tsukino/Core/Input/KeyCodes.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    LauncherInputSystem::LauncherInputSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus) {
    }

    //-------------------------------------------------------------
    // 入力を読んで発射台を更新する
    //-------------------------------------------------------------
    void LauncherInputSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        const LauncherConfigComponent*             config  = FindSingleton<LauncherConfigComponent>(registry);
        if(!context || !context->inputSystem || !config)
            return;

        using Tsukino::Input::KeyCode;
        const Tsukino::Input::InputSystem& input = *context->inputSystem;

        const bool  spaceDown     = input.IsKeyDown(KeyCode::Space);
        const bool  togglePressed = input.IsKeyPressed(KeyCode::A);
        const float cycle         = std::max(config->chargeCycleSeconds, 0.01f);

        for(const auto entity : registry.View<LauncherComponent>()) {
            LauncherComponent& launcher = registry.GetComponent<LauncherComponent>(entity);

            //-------------------------------------------------------------
            // 押した瞬間・離した瞬間は「押されているか」の変化で見る。
            // IsKeyPressed/IsKeyReleased の1フレームだけの合図だと、押したままウィンドウの
            // フォーカスが外れたときに離した合図を取りこぼし、溜めたまま戻らなくなるため
            //-------------------------------------------------------------
            if(spaceDown && !launcher.charging) {
                launcher.charging   = true;
                launcher.chargeTime = 0.0f;
            }
            if(launcher.charging) {
                if(spaceDown) {
                    // 強さは三角波で 0→1→0 を往復する。離すタイミングを狙うのが腕前になる
                    launcher.chargeTime  += deltaTime;
                    const float phase     = std::fmod(launcher.chargeTime / cycle, 1.0f);
                    launcher.handlePower  = 1.0f - std::fabs(phase * 2.0f - 1.0f);
                } else {
                    launcher.charging      = false;
                    launcher.fireRequested = true;
                }
            }

            if(AutoFireComponent* autoFire = registry.try_get<AutoFireComponent>(entity)) {
                if(togglePressed) {
                    if(autoFire->unlocked) {
                        autoFire->enabled = !autoFire->enabled;
                        m_eventBus->Publish(HudMessageEvent{autoFire->enabled ? "自動発射 ON" : "自動発射 OFF"});
                    } else {
                        m_eventBus->Publish(HudMessageEvent{"自動発射はまだ解禁されていません（U で解禁）"});
                    }
                }
            }
        }
    }

}    // namespace StardustFever::ECS
