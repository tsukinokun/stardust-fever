//-------------------------------------------------------------
//! @file   RollingNumberSystem.cpp
//! @brief  RollingNumberSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/RollingNumberSystem.hpp>

#include <StardustFever/ECS/Component/RollingNumberComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        // 桁の差がこれより小さくなったら追いかけ終える（端数で永遠に動き続けないように）
        constexpr double kSnapLog10 = 0.0004;
    }    // namespace

    RollingNumberSystem::RollingNumberSystem()
        : m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 星屑の数字を転がるように増やす
    //-------------------------------------------------------------
    void RollingNumberSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const WalletComponent* wallet = FindSingleton<WalletComponent>(registry);
        if(!wallet)
            return;
        const Core::BigNumber& target = wallet->stardust;

        using Tsukino::BuiltIn::ECS::FontComponent;
        for(const auto entity : registry.View<RollingNumberComponent>()) {
            RollingNumberComponent& rolling = registry.GetComponent<RollingNumberComponent>(entity);
            FontComponent*          font    = registry.try_get<FontComponent>(entity);

            if(!rolling.initialized) {
                // 起動直後（セーブの読み込み直後）から転がり始めると不自然なので、最初はそのまま出す
                rolling.displayed   = target;
                rolling.baseColor   = font ? font->color : hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);
                rolling.initialized = true;
                continue;
            }

            rolling.cooldownTimer = std::max(0.0f, rolling.cooldownTimer - deltaTime);

            if(target < rolling.displayed) {
                //-------------------------------------------------------------
                // 減ったとき（購入・ビッグバン）は転がさずにすぐ合わせ、一瞬赤く沈める。
                // 払ったことが目で分かるように
                //-------------------------------------------------------------
                rolling.displayed  = target;
                rolling.flashTimer = rolling.spendFlashSeconds;
                HudUi::PlayTo(registry,
                              entity,
                              Tsukino::BuiltIn::ECS::UITweenProperty::Scale,
                              0.9f,
                              1.0f,
                              0.3f,
                              Tsukino::Core::Math::Easing::EaseType::OutBack,
                              HudUi::Tag::kAppearScale);
            } else if(rolling.displayed < target) {
                if(rolling.displayed.IsZero() || rolling.displayed < Core::BigNumber(1.0)) {
                    rolling.displayed = std::min(target, Core::BigNumber(1.0));
                } else {
                    // 桁の上で詰めるので、10 → 1万 も 1万 → 1億 も同じくらいの時間で追いつく
                    const double from     = rolling.displayed.Log10();
                    const double gap      = target.Log10() - from;
                    const double progress = 1.0 - std::exp(-static_cast<double>(rolling.catchUpRate) * deltaTime);
                    rolling.displayed     = gap < kSnapLog10 ? target : Core::BigNumber::Pow(10.0, from + gap * progress);

                    // まだ大きく離れている間は、間隔を空けて弾ませる（ずっと震え続けないように）
                    const double ratio = std::pow(10.0, gap) - 1.0;
                    if(ratio > rolling.punchRatio && rolling.cooldownTimer <= 0.0f) {
                        std::uniform_real_distribution<float> wobble(0.85f, 1.15f);
                        Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, entity, rolling.punchAmount * wobble(m_random), 0.32f);
                        rolling.cooldownTimer = rolling.punchCooldown;
                    }
                }
            }

            //-------------------------------------------------------------
            // 減ったときの色を、元の色へ戻していく
            //-------------------------------------------------------------
            if(font) {
                rolling.flashTimer = std::max(0.0f, rolling.flashTimer - deltaTime);
                const float t      = rolling.spendFlashSeconds > 0.0f ? rolling.flashTimer / rolling.spendFlashSeconds : 0.0f;
                font->color        = rolling.baseColor + (rolling.spendColor - rolling.baseColor) * t;
            }
        }
    }

}    // namespace StardustFever::ECS
