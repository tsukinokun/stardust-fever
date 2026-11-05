//-------------------------------------------------------------
//! @file   ToastSystem.cpp
//! @brief  ToastSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/ToastSystem.hpp>

#include <StardustFever/ECS/Component/HudFxConfigComponent.hpp>
#include <StardustFever/ECS/Component/ToastComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/LifetimeComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::TransformComponent;
        using Tsukino::BuiltIn::ECS::UITweenProperty;
        using Tsukino::Core::Math::Easing::EaseType;
        namespace Tween = Tsukino::BuiltIn::ECS::UITweenUtility;

        // 消えるトゥイーンが終わってから破棄するまでの時間（秒）
        constexpr float kLeaveSeconds = 0.4f;

        //-------------------------------------------------------------
        //! @brief  1行目と残りに分ける
        //-------------------------------------------------------------
        std::pair<std::string, std::string> SplitFirstLine(const std::string& text) {
            const size_t newline = text.find('\n');
            if(newline == std::string::npos)
                return {text, std::string()};
            return {text.substr(0, newline), text.substr(newline + 1)};
        }

        hlslpp::float4 ToneColor(const HudFxConfigComponent& fx, HudMessageTone tone) {
            switch(tone) {
            case HudMessageTone::Positive:
                return fx.tonePositiveColor;
            case HudMessageTone::Warning:
                return fx.toneWarningColor;
            case HudMessageTone::Info:
            default:
                return fx.toneInfoColor;
            }
        }
    }    // namespace

    ToastSystem::ToastSystem(Tsukino::ECS::EventBus& eventBus)
        : m_messages(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 通知を消し始める
    //-------------------------------------------------------------
    void ToastSystem::Leave(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity) {
        ToastComponent& toast = registry.GetComponent<ToastComponent>(entity);
        if(toast.leaving)
            return;
        toast.leaving = true;

        if(toast.headline) {
            // 大見出しは少し膨らみながら消える
            Tween::FadeOut(registry, entity, 0.0f, kLeaveSeconds * 0.9f);
            HudUi::PlayTo(registry, entity, UITweenProperty::Scale, 1.0f, 1.12f, kLeaveSeconds, EaseType::OutQuad, HudUi::Tag::kAppearScale);
        } else {
            Tween::SlideOut(registry, entity, hlslpp::float2(80.0f, 0.0f), 0.0f, kLeaveSeconds * 0.85f);
        }
        registry.AddComponent<Tsukino::BuiltIn::ECS::LifetimeComponent>(entity).remaining = kLeaveSeconds;
    }

    //-------------------------------------------------------------
    // 通知を出し、動かし、消す
    //-------------------------------------------------------------
    void ToastSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto                                 messages = m_messages.Drain();
        const HudFxConfigComponent*                fx       = FindSingleton<HudFxConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context  = GetEngineContext(registry);
        if(!fx || !context || !context->prefabFactory)
            return;

        std::uniform_real_distribution<float> tilt(-0.012f, 0.012f);
        for(const HudMessageEvent& message : messages) {
            const auto [first, rest] = SplitFirstLine(message.text);

            if(message.importance == HudMessageImportance::Headline) {
                // 大見出しは1枚だけ。前のものはすぐ消し始める
                std::vector<Tsukino::ECS::Entity> previous;
                for(const auto entity : registry.View<ToastComponent>()) {
                    if(registry.GetComponent<ToastComponent>(entity).headline)
                        previous.push_back(entity);
                }
                for(const Tsukino::ECS::Entity entity : previous) {
                    Leave(registry, entity);
                }

                const Tsukino::ECS::Entity headline = HudUi::InstantiateTemplate(*context->prefabFactory, registry, fx->headlineGroup);
                if(headline == entt::null)
                    continue;
                HudUi::SetPosition(registry, headline, fx->headlineX, fx->headlineY);
                HudUi::SetText(registry, HudUi::FindPart(registry, headline, "Title"), first);
                HudUi::SetText(registry, HudUi::FindPart(registry, headline, "Sub"), rest);
                ToastComponent& toast = registry.AddComponent<ToastComponent>(headline);
                toast.headline        = true;
                toast.lifetime        = fx->headlineSeconds;

                //-------------------------------------------------------------
                // 小さく現れて弾みながら大きくなる。傾きは毎回少し変える
                //-------------------------------------------------------------
                HudUi::PlayTo(registry, headline, UITweenProperty::Scale, 0.45f, 1.0f, 0.75f, EaseType::OutElastic, HudUi::Tag::kAppearScale);
                HudUi::PlayTo(
                    registry, headline, UITweenProperty::Rotation, tilt(m_random) * 4.0f, tilt(m_random) * 1.5f, 0.6f, EaseType::OutBack, HudUi::Tag::kTilt);
                Tween::FadeIn(registry, headline, 0.0f, 0.18f);
                if(const Tsukino::ECS::Entity glow = HudUi::FindPart(registry, headline, "Glow"); glow != entt::null)
                    HudUi::PlayPingPong(registry, glow, UITweenProperty::Scale, 0.92f, 1.08f, 0.9f, 0.0f, HudUi::Tag::kPulse);
                continue;
            }

            const Tsukino::ECS::Entity root = HudUi::InstantiateTemplate(*context->prefabFactory, registry, fx->toastGroup);
            if(root == entt::null)
                continue;
            // トーストは1行にまとめる（2行目があれば続けて出す）
            HudUi::SetText(registry, HudUi::FindPart(registry, root, "Text"), rest.empty() ? first : first + "  " + rest);
            HudUi::SetTint(registry, HudUi::FindPart(registry, root, "Accent"), ToneColor(*fx, message.tone));
            HudUi::SetPosition(registry, root, fx->toastRight, fx->toastBottom);
            ToastComponent& toast = registry.AddComponent<ToastComponent>(root);
            toast.targetY         = fx->toastBottom;
            toast.lifetime        = fx->toastSeconds;
            Tween::SlideIn(registry, root, hlslpp::float2(110.0f, 0.0f), 0.0f, 0.42f);
            HudUi::PlayTo(registry, root, UITweenProperty::Rotation, 0.0f, tilt(m_random), 0.4f, EaseType::OutCubic, HudUi::Tag::kTilt);
            if(message.tone == HudMessageTone::Warning)
                Tween::Punch(registry, root, 0.06f, 0.35f);
        }

        //-------------------------------------------------------------
        // 新しい順に並べ、下から積む。多すぎる分は古いものから消し始める
        //-------------------------------------------------------------
        std::vector<std::pair<float, Tsukino::ECS::Entity>> stack;
        std::vector<Tsukino::ECS::Entity>                   expired;
        for(const auto entity : registry.View<ToastComponent>()) {
            ToastComponent& toast  = registry.GetComponent<ToastComponent>(entity);
            toast.age             += deltaTime;
            if(!toast.leaving && toast.age >= toast.lifetime)
                expired.push_back(entity);
            if(!toast.headline && !toast.leaving)
                stack.emplace_back(toast.age, entity);
        }
        std::sort(stack.begin(), stack.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
        for(size_t rank = 0; rank < stack.size(); ++rank) {
            const Tsukino::ECS::Entity entity = stack[rank].second;
            if(static_cast<int>(rank) >= fx->maxToasts) {
                expired.push_back(entity);
                continue;
            }
            ToastComponent& toast = registry.GetComponent<ToastComponent>(entity);
            toast.targetY         = fx->toastBottom - fx->toastSpacing * static_cast<float>(rank);
            if(auto* transform = registry.try_get<TransformComponent>(entity)) {
                const float x = transform->position.x;
                const float y = transform->position.y;
                const float z = transform->position.z;
                // 一気に飛ばさず、押し上げられるように寄せる
                transform->position = hlslpp::float3(x, y + (toast.targetY - y) * (1.0f - std::exp(-12.0f * deltaTime)), z);
                transform->dirty    = true;
            }
        }
        for(const Tsukino::ECS::Entity entity : expired) {
            Leave(registry, entity);
        }
    }

}    // namespace StardustFever::ECS
