//-------------------------------------------------------------
//! @file   LotteryViewSystem.cpp
//! @brief  LotteryViewSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/LotteryViewSystem.hpp>

#include <StardustFever/ECS/Component/HudPendingDotComponent.hpp>
#include <StardustFever/ECS/Component/HudTextComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/LotteryReach.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::UITweenProperty;
        using Tsukino::Core::Math::Easing::EaseType;
        namespace Tween = Tsukino::BuiltIn::ECS::UITweenUtility;

        //-------------------------------------------------------------
        //! @brief  index 番目のリールの文字を探す
        //-------------------------------------------------------------
        Tsukino::ECS::Entity FindReel(Tsukino::ECS::Registry& registry, int index) {
            for(const auto entity : registry.View<HudTextComponent>()) {
                const HudTextComponent& text = registry.GetComponent<HudTextComponent>(entity);
                if(text.kind == HudTextKind::Reel && text.index == index)
                    return entity;
            }
            return entt::null;
        }

        //-------------------------------------------------------------
        //! @brief  透明から一瞬だけ光らせて、また透明へ戻す
        //! @note   Punch の緩急は終わりで始めの値に戻るので、0 → peak → 0 になる
        //-------------------------------------------------------------
        void FlashAlpha(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, float peak, float seconds) {
            HudUi::PlayTo(registry, entity, UITweenProperty::Alpha, 0.0f, peak, seconds, EaseType::Punch, HudUi::Tag::kFlash);
        }
    }    // namespace

    LotteryViewSystem::LotteryViewSystem(Tsukino::ECS::EventBus& eventBus)
        : m_results(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // リールと保留の見た目を更新する
    //-------------------------------------------------------------
    void LotteryViewSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const auto                   results = m_results.Drain();
        const LotteryStateComponent* state   = FindSingleton<LotteryStateComponent>(registry);
        if(!state)
            return;

        const Tsukino::ECS::Entity frame = HudUi::FindAnyPart(registry, "ReelFrame");
        const Tsukino::ECS::Entity glow  = HudUi::FindAnyPart(registry, "ReelGlow");
        if(!m_initialized && glow != entt::null) {
            // 枠の光は当たったときだけ見せる。最初は透明にしておく
            HudUi::PlayTo(registry, glow, UITweenProperty::Alpha, 0.0f, 0.0f, 0.01f, EaseType::Linear, HudUi::Tag::kFlash);
            m_initialized = true;
        }

        //-------------------------------------------------------------
        // 回っている間は震わせ、止まった瞬間に弾ませる
        //-------------------------------------------------------------
        for(int reel = 0; reel < 3; ++reel) {
            const Tsukino::ECS::Entity entity  = FindReel(registry, reel);
            const bool                 stopped = state->reelStopped[static_cast<size_t>(reel)];
            if(entity != entt::null) {
                if(!stopped && m_wasStopped[static_cast<size_t>(reel)]) {
                    // 震えの周期はリールごとに少し変え、3つが同じ動きにならないようにする
                    std::uniform_real_distribution<float> period(0.07f, 0.11f);
                    HudUi::PlayPingPong(registry, entity, UITweenProperty::OffsetY, -4.0f, 4.0f, period(m_random), 0.0f, HudUi::Tag::kReelJitter);
                } else if(stopped && !m_wasStopped[static_cast<size_t>(reel)]) {
                    Tween::Stop(registry, entity, HudUi::Tag::kReelJitter);
                    Tween::Punch(registry, entity, 0.28f, 0.32f);
                }
            }
            m_wasStopped[static_cast<size_t>(reel)] = stopped;
        }

        //-------------------------------------------------------------
        // リーチ: 左と中が止まって同じ絵柄、右だけが回っている
        //-------------------------------------------------------------
        const bool reach = IsReach(*state);
        if(frame != entt::null) {
            if(reach && !m_wasReach)
                HudUi::PlayPingPong(registry, frame, UITweenProperty::Scale, 1.0f, 1.045f, 0.36f, 0.0f, HudUi::Tag::kPulse);
            else if(!reach && m_wasReach)
                Tween::Stop(registry, frame, HudUi::Tag::kPulse);
        }
        m_wasReach = reach;

        for(const LotteryResultEvent& result : results) {
            if(!result.won)
                continue;
            if(frame != entt::null)
                Tween::Punch(registry, frame, 0.12f, 0.55f);
            if(glow != entt::null)
                FlashAlpha(registry, glow, 1.0f, 1.1f);
        }

        //-------------------------------------------------------------
        // 保留の玉。増えた玉だけ弾ませ、無い分は沈める
        //-------------------------------------------------------------
        for(const auto entity : registry.View<HudPendingDotComponent>()) {
            HudPendingDotComponent& dot = registry.GetComponent<HudPendingDotComponent>(entity);
            const bool              lit = dot.index < state->pending;
            if(dot.applied && lit == dot.lit)
                continue;
            const bool firstTime = !dot.applied;
            dot.applied          = true;
            dot.lit              = lit;
            HudUi::PlayTo(registry,
                          entity,
                          UITweenProperty::Alpha,
                          lit ? 0.25f : 1.0f,
                          lit ? 1.0f : 0.25f,
                          firstTime ? 0.01f : 0.2f,
                          EaseType::OutQuad,
                          HudUi::Tag::kDim);
            if(lit && !firstTime)
                Tween::Punch(registry, entity, 0.7f, 0.4f);
        }
    }

}    // namespace StardustFever::ECS
