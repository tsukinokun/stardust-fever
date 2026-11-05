//-------------------------------------------------------------
//! @file   HudGaugeSystem.cpp
//! @brief  HudGaugeSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/HudGaugeSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/HudGaugeComponent.hpp>
#include <StardustFever/ECS/Component/HudGaugeMarkerComponent.hpp>
#include <StardustFever/ECS/Component/LauncherComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIRectComponent.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        enum class GaugeSource : int {
            LaunchPower      = 0,
            FeverTime        = 1,
            PrestigeProgress = 2,
        };

        //-------------------------------------------------------------
        //! @brief  ゲージが見せる値（0〜1）を取る
        //-------------------------------------------------------------
        float ReadSource(Tsukino::ECS::Registry& registry, GaugeSource source) {
            switch(source) {
            case GaugeSource::LaunchPower:
                {
                    const LauncherComponent* launcher = FindSingleton<LauncherComponent>(registry);
                    return launcher ? launcher->handlePower : 0.0f;
                }
            case GaugeSource::FeverTime:
                {
                    const FeverStateComponent* fever = FindSingleton<FeverStateComponent>(registry);
                    if(!fever || !fever->active || fever->timeTotal <= 0.0f)
                        return 0.0f;
                    return fever->timeRemaining / fever->timeTotal;
                }
            case GaugeSource::PrestigeProgress:
                {
                    const PrestigeStateComponent*  prestige = FindSingleton<PrestigeStateComponent>(registry);
                    const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
                    if(!prestige || !config)
                        return 0.0f;
                    //-------------------------------------------------------------
                    // 今の獲得量から次の1つまでの間で、どこまで来たか。
                    // 次の宇宙定数がもらえた瞬間に 0 へ戻り、また伸び始める
                    //-------------------------------------------------------------
                    namespace Prestige             = Core::PrestigeFormula;
                    const Core::BigNumber gain     = Prestige::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);
                    const Core::BigNumber previous = gain.IsZero()
                                                         ? Core::BigNumber(0.0)
                                                         : Prestige::EarnedForNextConstant(gain - Core::BigNumber(1.0), config->threshold, config->exponent);
                    const Core::BigNumber next     = Prestige::EarnedForNextConstant(gain, config->threshold, config->exponent);
                    const Core::BigNumber range    = next - previous;
                    if(range.IsZero() || range.IsNegative())
                        return 0.0f;
                    return static_cast<float>(((prestige->universeEarned - previous) / range).ToDouble());
                }
            }
            return 0.0f;
        }

        //-------------------------------------------------------------
        //! @brief  値に合わせて 低 → 中 → 高 の色を混ぜる
        //-------------------------------------------------------------
        hlslpp::float4 GaugeColor(const HudGaugeComponent& gauge, float t) {
            if(t < 0.5f)
                return gauge.colorLow + (gauge.colorMid - gauge.colorLow) * (t * 2.0f);
            return gauge.colorMid + (gauge.colorHigh - gauge.colorMid) * ((t - 0.5f) * 2.0f);
        }
    }    // namespace

    HudGaugeSystem::HudGaugeSystem()
        : m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // ゲージを値に合わせて伸び縮みさせる
    //-------------------------------------------------------------
    void HudGaugeSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;
        using Tsukino::BuiltIn::ECS::UIRectComponent;

        const LauncherComponent* launcher = FindSingleton<LauncherComponent>(registry);
        const bool               charging = launcher && launcher->charging;

        for(const auto entity : registry.View<HudGaugeComponent, SpriteComponent>()) {
            HudGaugeComponent& gauge  = registry.GetComponent<HudGaugeComponent>(entity);
            SpriteComponent&   sprite = registry.GetComponent<SpriteComponent>(entity);
            const GaugeSource  source = static_cast<GaugeSource>(gauge.source);

            const float target  = std::clamp(ReadSource(registry, source), 0.0f, 1.0f);
            gauge.shown        += (target - gauge.shown) * (1.0f - std::exp(-gauge.smoothing * deltaTime));
            sprite.fillAmount   = gauge.shown;
            sprite.tintColor    = GaugeColor(gauge, gauge.shown);

            // 溜めている間だけ脈打たせ、「いま狙っている」ことを体で感じさせる
            if(source == GaugeSource::LaunchPower) {
                const bool pulsing = Tsukino::BuiltIn::ECS::UITweenUtility::IsPlaying(registry, entity, HudUi::Tag::kPulse);
                if(charging && !pulsing) {
                    std::uniform_real_distribution<float> period(0.22f, 0.3f);
                    HudUi::PlayPingPong(registry,
                                        entity,
                                        Tsukino::BuiltIn::ECS::UITweenProperty::ScaleY,
                                        1.0f,
                                        1.0f + gauge.pulseAmount * 4.0f,
                                        period(m_random),
                                        0.0f,
                                        HudUi::Tag::kPulse);
                } else if(!charging && pulsing) {
                    Tsukino::BuiltIn::ECS::UITweenUtility::Stop(registry, entity, HudUi::Tag::kPulse);
                }
            }
        }

        //-------------------------------------------------------------
        // 印は親のゲージの幅の上で、最後に打った強さの位置へ寄せる
        //-------------------------------------------------------------
        if(!launcher)
            return;
        for(const auto entity : registry.View<HudGaugeMarkerComponent, TransformComponent>()) {
            const HudGaugeMarkerComponent& marker    = registry.GetComponent<HudGaugeMarkerComponent>(entity);
            TransformComponent&            transform = registry.GetComponent<TransformComponent>(entity);
            const Tsukino::ECS::Entity     parent    = transform.parent;
            const UIRectComponent*         rect      = registry.IsValid(parent) ? registry.try_get<UIRectComponent>(parent) : nullptr;
            if(!rect)
                continue;

            if(SpriteComponent* sprite = registry.try_get<SpriteComponent>(entity)) {
                hlslpp::float4 tint = sprite->tintColor;
                tint.w              = launcher->lastShotPower < 0.0f ? 0.0f : 1.0f;
                sprite->tintColor   = tint;
            }
            const float width   = rect->size.x;
            const float targetX = (std::max(launcher->lastShotPower, 0.0f) - static_cast<float>(rect->pivot.x)) * width;
            const float x       = transform.position.x;
            const float y       = transform.position.y;
            const float z       = transform.position.z;
            const float newX    = x + (targetX - x) * (1.0f - std::exp(-marker.followSpeed * deltaTime));
            transform.position  = hlslpp::float3(newX, y, z);
            transform.dirty     = true;
        }
    }

}    // namespace StardustFever::ECS
