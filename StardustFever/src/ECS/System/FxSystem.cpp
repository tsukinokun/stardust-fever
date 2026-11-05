//-------------------------------------------------------------
//! @file   FxSystem.cpp
//! @brief  FxSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/FxSystem.hpp>

#include <StardustFever/ECS/Component/HudFxConfigComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/LifetimeComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/WorldAnchorComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Particle/BurstEmitter.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::UITweenProperty;
        using Tsukino::Core::Math::Easing::EaseType;

        hlslpp::float4 SourceColor(const HudFxConfigComponent& fx, StardustSource source) {
            switch(source) {
            case StardustSource::Pin:
                return fx.pinColor;
            case StardustSource::Pocket:
                return fx.pocketColor;
            case StardustSource::Star:
                return fx.starColor;
            case StardustSource::Bonus:
            default:
                return fx.bonusColor;
            }
        }

        //-------------------------------------------------------------
        //! @brief  粒子の撒き方（盤面は cm 単位なので、エンジンの既定値より小さくする）
        //-------------------------------------------------------------
        Tsukino::BuiltIn::ECS::BurstParams MakeBurst(int count, float speed, float size, const hlslpp::float4& colorA, const hlslpp::float4& colorB) {
            Tsukino::BuiltIn::ECS::BurstParams params;
            params.count        = count;
            params.speedMin     = speed * 0.35f;
            params.speedMax     = speed;
            params.upwardBias   = 0.9f;
            params.spread       = 1.0f;
            params.lifeMin      = 0.45f;
            params.lifeMax      = 1.1f;
            params.sizeMin      = size * 0.4f;
            params.sizeMax      = size;
            params.endSizeScale = 0.1f;
            params.gravity      = 35.0f;
            params.drag         = 2.2f;
            params.radius       = size * 0.6f;
            params.delayMax     = 0.08f;
            params.colorA       = colorA;
            params.colorB       = colorB;
            params.endAlpha     = 0.0f;
            params.additive     = true;
            return params;
        }
    }    // namespace

    FxSystem::FxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus)
        : m_scene(&scene)
        , m_credits(eventBus)
        , m_linkedStars(eventBus)
        , m_completions(eventBus)
        , m_pocketEntries(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 浮かぶ数字を1つ出す
    //-------------------------------------------------------------
    void FxSystem::SpawnPopup(Tsukino::ECS::Registry& registry, const StardustCreditedEvent& credited) {
        const HudFxConfigComponent*                fx      = FindSingleton<HudFxConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!fx || !context || !context->prefabFactory)
            return;

        const Tsukino::ECS::Entity popup = HudUi::InstantiateTemplate(*context->prefabFactory, registry, fx->popupGroup);
        if(popup == entt::null)
            return;
        m_popups.push_back(popup);

        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        std::uniform_real_distribution<float> spread(-1.0f, 1.0f);

        //-------------------------------------------------------------
        // 盤面の点へ貼り付ける。出る位置は少しずらし、同じ釘から続けて出ても重ならないようにする
        //-------------------------------------------------------------
        auto& anchor                 = registry.AddComponent<Tsukino::BuiltIn::ECS::WorldAnchorComponent>(popup);
        anchor.useFixedWorldPosition = true;
        anchor.fixedWorldPosition    = credited.position + hlslpp::float3(spread(m_random) * 1.2f, 1.8f, 0.0f);
        anchor.screenOffset          = hlslpp::float2(spread(m_random) * 10.0f, -6.0f);

        //-------------------------------------------------------------
        // 金額の桁で大きさを、出どころで色を変える
        //-------------------------------------------------------------
        if(auto* font = registry.try_get<Tsukino::BuiltIn::ECS::FontComponent>(popup)) {
            const double digits  = std::max(0.0, credited.amount.Log10());
            font->scale         *= std::clamp(0.72f + 0.1f * static_cast<float>(digits), 0.7f, 1.75f);
            font->color          = SourceColor(*fx, credited.source);
        }
        HudUi::SetText(registry, popup, "+" + credited.amount.ToJapaneseString());

        const float seconds = fx->popupSeconds * (0.85f + 0.3f * unit(m_random));
        const float rise    = fx->popupRise * (0.75f + 0.5f * unit(m_random));
        HudUi::PlayTo(registry, popup, UITweenProperty::OffsetY, 0.0f, -rise, seconds, EaseType::OutCubic, HudUi::Tag::kRise);
        HudUi::PlayTo(registry, popup, UITweenProperty::OffsetX, 0.0f, spread(m_random) * 14.0f, seconds, EaseType::OutQuad, HudUi::Tag::kTilt);
        HudUi::PlayTo(registry, popup, UITweenProperty::Scale, 0.4f, 1.0f, 0.24f, EaseType::OutBack, HudUi::Tag::kAppearScale);
        HudUi::PlayTo(registry, popup, UITweenProperty::Alpha, 1.0f, 0.0f, seconds * 0.45f, EaseType::InQuad, HudUi::Tag::kFlash, seconds * 0.55f);
        registry.AddComponent<Tsukino::BuiltIn::ECS::LifetimeComponent>(popup).remaining = seconds + 0.05f;
    }

    //-------------------------------------------------------------
    // 演出を出す
    //-------------------------------------------------------------
    void FxSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const auto                                 credits       = m_credits.Drain();
        const auto                                 linkedStars   = m_linkedStars.Drain();
        const auto                                 completions   = m_completions.Drain();
        const auto                                 pocketEntries = m_pocketEntries.Drain();
        const HudFxConfigComponent*                fx            = FindSingleton<HudFxConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context       = GetEngineContext(registry);
        if(!fx || !context || !context->assetManager)
            return;

        // 消えた数字を数えから外す
        while(!m_popups.empty() && !registry.IsValid(m_popups.front())) {
            m_popups.pop_front();
        }

        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        for(const StardustCreditedEvent& credited : credits) {
            if(!credited.hasPosition || static_cast<int>(m_popups.size()) >= fx->maxPopups)
                continue;
            if(credited.source == StardustSource::Pin && unit(m_random) > fx->pinPopupChance)
                continue;
            SpawnPopup(registry, credited);
        }

        if(fx->particleTexture.empty())
            return;
        const Tsukino::Asset::AssetHandle texture = context->assetManager->Load(Tsukino::Core::Path(fx->particleTexture));

        for(const StarLinkedEvent& linked : linkedStars) {
            Tsukino::BuiltIn::ECS::BurstEmitter::Emit(*m_scene,
                                                      texture,
                                                      MakeBurst(fx->starBurstCount, 55.0f, 1.6f, fx->starColor, hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f)),
                                                      linked.position,
                                                      m_random);
        }
        // 星ゲットチャンスで全部つないだら、盤面いっぱいに散らす
        for(const StarChanceEndedEvent& completed : completions) {
            if(!completed.success)
                continue;
            Tsukino::BuiltIn::ECS::BurstParams params = MakeBurst(fx->completeBurstCount, 150.0f, 2.4f, fx->starColor, fx->bonusColor);
            params.radius                             = 14.0f;
            params.lifeMax                            = 1.8f;
            Tsukino::BuiltIn::ECS::BurstEmitter::Emit(*m_scene, texture, params, hlslpp::float3(fx->completeBurstX, fx->completeBurstY, 0.0f), m_random);
        }
        for(const BallEnteredPocketEvent& entry : pocketEntries) {
            const auto* transform = registry.IsValid(entry.pocket) ? registry.try_get<Tsukino::BuiltIn::ECS::TransformComponent>(entry.pocket) : nullptr;
            if(!transform)
                continue;
            Tsukino::BuiltIn::ECS::BurstEmitter::Emit(
                *m_scene, texture, MakeBurst(10, 40.0f, 1.0f, fx->pocketColor, fx->pocketColor), transform->position, m_random);
        }
    }

}    // namespace StardustFever::ECS
