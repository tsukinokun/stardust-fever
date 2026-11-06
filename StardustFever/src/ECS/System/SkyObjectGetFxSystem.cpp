//-------------------------------------------------------------
//! @file   SkyObjectGetFxSystem.cpp
//! @brief  SkyObjectGetFxSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/SkyObjectGetFxSystem.hpp>

#include <StardustFever/Core/TextUtility.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Component/SkyObjectConfigComponent.hpp>
#include <StardustFever/ECS/Utility/CollectionText.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>
#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    SkyObjectGetFxSystem::SkyObjectGetFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus)
        : m_scene(&scene)
        , m_eventBus(&eventBus)
        , m_draws(eventBus)
        , m_actions(eventBus) {
    }

    //-------------------------------------------------------------
    // カードを出し・片付ける
    //-------------------------------------------------------------
    void SkyObjectGetFxSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        PrestigeStateComponent*         prestige = FindSingleton<PrestigeStateComponent>(registry);
        const SkyObjectConfigComponent* config   = FindSingleton<SkyObjectConfigComponent>(registry);
        const auto                      actions  = m_actions.Drain();
        if(!prestige || !config)
            return;

        for(const SkyObjectDrawnEvent& drawn : m_draws.Drain()) {
            m_objectId           = drawn.objectId;
            m_delay              = config->revealDelay;
            prestige->rewardOpen = true;    // 光の間も遊びを止める
        }
        if(m_objectId.empty())
            return;

        if(m_root == entt::null) {
            m_delay -= deltaTime;
            if(m_delay <= 0.0f)
                Build(registry);
            return;
        }

        //-------------------------------------------------------------
        // 閉じる
        //-------------------------------------------------------------
        using Tsukino::Input::KeyCode;
        bool close = false;
        for(const UiActionEvent& action : actions) {
            close = close || action.action == UiAction::SkyObjectClose;
        }
        if(Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry); context && context->inputSystem)
            close = close || context->inputSystem->IsKeyPressed(KeyCode::Space) || context->inputSystem->IsKeyPressed(KeyCode::Enter);
        if(!close)
            return;

        if(registry.IsValid(m_root))
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
        m_root = entt::null;
        m_objectId.clear();
        prestige->rewardOpen = false;
    }

    //-------------------------------------------------------------
    // カードを作る
    //-------------------------------------------------------------
    void SkyObjectGetFxSystem::Build(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             style   = FindSingleton<SettingsConfigComponent>(registry);
        const SkyObjectConfigComponent*            config  = FindSingleton<SkyObjectConfigComponent>(registry);
        const Data::SkyObjectDef*                  object  = Data::GetSkyObjectTable().Find(m_objectId);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!style || !config || !object || !context || !context->assetManager)
            return;
        Tsukino::Asset::AssetManager& assets = *context->assetManager;
        const int                     order  = style->sortOrder;
        const size_t                  rarity = static_cast<size_t>(object->rarity);
        const hlslpp::float4          color  = rarity < config->rarityColors.size() ? config->rarityColors[rarity] : style->titleColor;

        m_root = UiParts::MakeNode(*m_scene, registry, entt::null, 0.0f, 0.0f);

        // 後ろの幕。後ろの部品を押せないよう、当たりも受ける
        UiParts::AddImage(registry,
                          UiParts::MakeNode(*m_scene, registry, m_root, 800.0f, 500.0f),
                          assets.Load(Tsukino::Core::Path(style->whiteTexture)),
                          1600.0f,
                          1000.0f,
                          order,
                          style->dimColor)
            .raycastTarget = true;

        const float        width  = config->cardWidth;
        const float        height = config->cardHeight;
        const entt::entity card   = UiParts::MakeNode(*m_scene, registry, m_root, 800.0f, 500.0f);
        UiParts::AddCard(*m_scene, registry, assets, *style, card, width, height, order + 2);

        const float top  = -height * 0.5f;
        const auto  text = [&](const std::string& value, float y, float scale, const hlslpp::float4& tint) {
            const entt::entity line = UiParts::MakeNode(*m_scene, registry, card, 0.0f, top + y);
            UiParts::AddLabel(registry, line, value, scale, order + 6, tint, true);
            registry.GetComponent<Tsukino::BuiltIn::ECS::FontComponent>(line).maxWidth = width - 80.0f;
        };

        // レア度の色の光と星
        hlslpp::float4 glowColor = color;
        glowColor.w              = 0.55f;
        const entt::entity glow  = UiParts::MakeNode(*m_scene, registry, card, 0.0f, top + 92.0f);
        UiParts::AddImage(registry, glow, assets.Load(Tsukino::Core::Path(style->glowTexture)), 300.0f, 300.0f, order + 3, glowColor);
        const entt::entity star = UiParts::MakeNode(*m_scene, registry, card, 0.0f, top + 92.0f);
        UiParts::AddImage(registry, star, assets.Load(Tsukino::Core::Path(style->starTexture)), 96.0f, 96.0f, order + 4, color);
        HudUi::PlayPingPong(registry, star, Tsukino::BuiltIn::ECS::UITweenProperty::Scale, 1.0f, 1.12f, 1.4f, 0.0f, HudUi::Tag::kPulse);

        text(rarity < config->rarityNames.size() ? config->rarityNames[rarity] : std::string(), 168.0f, style->textScale * 1.1f, color);
        text(object->name, 214.0f, style->titleScale, style->titleColor);
        text(object->nameEn + "  " + CollectionText::SkyObjectKind(object->kind), 258.0f, style->textScale * 0.8f, style->textColor);
        text(CollectionText::Effect(object->effect, object->effect.value), 294.0f, style->textScale, color);

        float y = 338.0f;
        for(const std::string& line : Core::WrapText(object->caption, config->captionColumns)) {
            text(line, y, style->textScale * 0.8f, style->textColor);
            y += 26.0f;
        }

        UiParts::MakeButton(*m_scene,
                            registry,
                            assets,
                            *style,
                            card,
                            0.0f,
                            height * 0.5f - 44.0f,
                            style->closeButtonWidth,
                            style->closeButtonHeight,
                            order + 5,
                            "閉じる",
                            UiAction::SkyObjectClose);

        Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, card, 0.08f, 0.4f);
        if(object->rarity >= config->flashRarity)
            HudUi::PlayTo(registry,
                          HudUi::FindAnyPart(registry, "Flash"),
                          Tsukino::BuiltIn::ECS::UITweenProperty::Alpha,
                          0.6f,
                          0.0f,
                          0.9f,
                          Tsukino::Core::Math::Easing::EaseType::OutQuad,
                          HudUi::Tag::kFlash);
        m_eventBus->Publish(SkyObjectShownEvent{object->rarity});
    }

}    // namespace StardustFever::ECS
