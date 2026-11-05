//-------------------------------------------------------------
//! @file   HudIdleMotionSystem.cpp
//! @brief  HudIdleMotionSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/HudIdleMotionSystem.hpp>

#include <StardustFever/ECS/Component/HudIdleMotionComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    HudIdleMotionSystem::HudIdleMotionSystem()
        : m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 待機中の HUD をゆらがせる
    //-------------------------------------------------------------
    void HudIdleMotionSystem::Update(Tsukino::ECS::Registry& registry, float) {
        using Tsukino::BuiltIn::ECS::UITweenProperty;

        for(const auto entity : registry.View<HudIdleMotionComponent>()) {
            HudIdleMotionComponent& idle = registry.GetComponent<HudIdleMotionComponent>(entity);
            if(idle.started)
                continue;
            idle.started = true;

            //-------------------------------------------------------------
            // 漂い・呼吸・明滅は、それぞれ別の周期と位相にする。
            // 同じ周期だと動きが重なって「1つの機械が動いている」ように見えるため
            //-------------------------------------------------------------
            const float                           low  = std::min(idle.periodMin, idle.periodMax);
            const float                           high = std::max(idle.periodMin, idle.periodMax);
            std::uniform_real_distribution<float> period(low, std::max(high, low + 0.01f));
            std::uniform_real_distribution<float> unit(0.0f, 1.0f);

            if(idle.floatAmplitude > 0.0f) {
                const float p = period(m_random);
                HudUi::PlayPingPong(
                    registry, entity, UITweenProperty::OffsetY, -idle.floatAmplitude, idle.floatAmplitude, p, unit(m_random) * p, HudUi::Tag::kIdleFloat);
            }
            if(idle.scaleAmplitude > 0.0f) {
                const float p = period(m_random) * 1.37f;
                HudUi::PlayPingPong(registry,
                                    entity,
                                    UITweenProperty::Scale,
                                    1.0f - idle.scaleAmplitude,
                                    1.0f + idle.scaleAmplitude,
                                    p,
                                    unit(m_random) * p,
                                    HudUi::Tag::kIdleScale);
            }
            if(idle.alphaAmplitude > 0.0f) {
                const float p = period(m_random) * 0.71f;
                HudUi::PlayPingPong(registry, entity, UITweenProperty::Alpha, 1.0f, 1.0f - idle.alphaAmplitude, p, unit(m_random) * p, HudUi::Tag::kIdleAlpha);
            }
        }
    }

}    // namespace StardustFever::ECS
