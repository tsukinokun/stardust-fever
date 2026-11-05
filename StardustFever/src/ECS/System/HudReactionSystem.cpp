//-------------------------------------------------------------
//! @file   HudReactionSystem.cpp
//! @brief  HudReactionSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/HudReactionSystem.hpp>

#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/HudFxConfigComponent.hpp>
#include <StardustFever/ECS/Component/HudTextComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/UIVisibilityComponent.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::UITweenProperty;
        using Tsukino::Core::Math::Easing::EaseType;
        namespace Tween = Tsukino::BuiltIn::ECS::UITweenUtility;

        void SetVisible(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, bool visible) {
            if(entity == entt::null)
                return;
            auto& visibility   = registry.HasComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(entity)
                                     ? registry.GetComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(entity)
                                     : registry.AddComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(entity);
            visibility.visible = visible;
        }

        Tsukino::ECS::Entity FindText(Tsukino::ECS::Registry& registry, HudTextKind kind) {
            for(const auto entity : registry.View<HudTextComponent>()) {
                if(registry.GetComponent<HudTextComponent>(entity).kind == kind)
                    return entity;
            }
            return entt::null;
        }
    }    // namespace

    HudReactionSystem::HudReactionSystem(Tsukino::ECS::EventBus& eventBus)
        : m_feverStarts(eventBus)
        , m_feverEnds(eventBus)
        , m_bigBangs(eventBus) {
    }

    //-------------------------------------------------------------
    // 出来事に反応する
    //-------------------------------------------------------------
    void HudReactionSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto                  feverStarts = m_feverStarts.Drain();
        const auto                  feverEnds   = m_feverEnds.Drain();
        const auto                  bigBangs    = m_bigBangs.Drain();
        const Tsukino::ECS::Entity  banner      = HudUi::FindAnyPart(registry, "FeverBanner");
        const Tsukino::ECS::Entity  flash       = HudUi::FindAnyPart(registry, "Flash");
        const HudFxConfigComponent* fx          = FindSingleton<HudFxConfigComponent>(registry);
        if(!fx)
            return;

        if(!m_initialized && banner != entt::null) {
            // 帯はフィーバー中だけ出す。閃光は普段は透明
            SetVisible(registry, banner, false);
            HudUi::PlayTo(registry, flash, UITweenProperty::Alpha, 0.0f, 0.0f, 0.01f, EaseType::Linear, HudUi::Tag::kFlash);
            m_initialized = true;
        }

        if(!feverStarts.empty() && banner != entt::null) {
            m_hideTimer = -1.0f;
            SetVisible(registry, banner, true);
            Tween::SlideIn(registry, banner, hlslpp::float2(0.0f, -fx->bannerTravel), 0.15f, 0.6f);
        }
        if(!feverEnds.empty() && banner != entt::null) {
            Tween::SlideOut(registry, banner, hlslpp::float2(0.0f, -fx->bannerTravel), 0.0f, 0.35f);
            m_hideTimer = 0.4f;
        }
        if(m_hideTimer >= 0.0f) {
            m_hideTimer -= deltaTime;
            if(m_hideTimer < 0.0f)
                SetVisible(registry, banner, false);
        }

        //-------------------------------------------------------------
        // 残り時間が少ないときは秒数を脈打たせ、急かす
        //-------------------------------------------------------------
        const FeverStateComponent* fever       = FindSingleton<FeverStateComponent>(registry);
        const bool                 timeWarning = fever && fever->active && !fever->completed && fever->timeRemaining < fx->timeWarningSeconds;
        if(timeWarning != m_timeWarning) {
            const Tsukino::ECS::Entity timeText = FindText(registry, HudTextKind::FeverTime);
            if(timeWarning)
                HudUi::PlayPingPong(registry, timeText, UITweenProperty::Scale, 1.0f, 1.3f, 0.5f, 0.0f, HudUi::Tag::kPulse);
            else
                Tween::Stop(registry, timeText, HudUi::Tag::kPulse);
            m_timeWarning = timeWarning;
        }

        //-------------------------------------------------------------
        // ビッグバン: 画面全体が白く光ってから、ゆっくり晴れる
        //-------------------------------------------------------------
        if(!bigBangs.empty()) {
            HudUi::PlayTo(registry, flash, UITweenProperty::Alpha, 0.95f, 0.0f, 1.4f, EaseType::OutQuad, HudUi::Tag::kFlash);
            Tween::Punch(registry, HudUi::FindAnyPart(registry, "WalletCard"), 0.1f, 0.6f);
        }
    }

}    // namespace StardustFever::ECS
