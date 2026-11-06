//-------------------------------------------------------------
//! @file   SettingsViewSystem.cpp
//! @brief  SettingsViewSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/SettingsViewSystem.hpp>

#include <StardustFever/ECS/Component/AudioSettingsComponent.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Component/SettingsElementComponent.hpp>
#include <StardustFever/ECS/Component/SettingsStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/PointerTargetComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIRectComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UISliderComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIToggleComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>
#include <Tsukino/Core/Path.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>

#include <algorithm>
#include <cmath>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::FontComponent;
        using Tsukino::BuiltIn::ECS::PointerTargetComponent;
        using Tsukino::BuiltIn::ECS::SpriteBlendMode;
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;
        using Tsukino::BuiltIn::ECS::UIRectComponent;

        using UiParts::AddImage;
        using UiParts::AddLabel;
        using UiParts::kCardBorder;
        using UiParts::kPillBorder;
        using UiParts::SetSlice;

        //-------------------------------------------------------------
        //! @brief  部品のエンティティを作る（位置は親からのずれ）
        //-------------------------------------------------------------
        entt::entity MakeElement(Tsukino::ECS::Scene&    scene,
                                 Tsukino::ECS::Registry& registry,
                                 entt::entity            parent,
                                 float                   x,
                                 float                   y,
                                 SettingsElementKind     kind  = SettingsElementKind::Decoration,
                                 int                     index = 0) {
            const entt::entity  entity    = scene.CreateEntity();
            TransformComponent& transform = registry.AddComponent<TransformComponent>(entity);
            if(parent != entt::null)
                transform.parent = parent;
            transform.position                = hlslpp::float3(x, y, 0.0f);
            transform.dirty                   = true;
            SettingsElementComponent& element = registry.AddComponent<SettingsElementComponent>(entity);
            element.kind                      = kind;
            element.index                     = index;
            return entity;
        }
    }    // namespace

    SettingsViewSystem::SettingsViewSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // 画面を作り・片付け、値に合わせて動かす
    //-------------------------------------------------------------
    void SettingsViewSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const SettingsStateComponent* state = FindSingleton<SettingsStateComponent>(registry);
        if(!state)
            return;

        if(m_openButton == entt::null)
            BuildOpenButton(registry);

        if(state->open && m_root == entt::null)
            Build(registry);
        if(!state->open && m_root != entt::null) {
            if(registry.IsValid(m_root))
                Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
            m_root = entt::null;
        }
        Sync(registry, deltaTime);
    }

    //-------------------------------------------------------------
    // 「設定」のボタンを作る
    //-------------------------------------------------------------
    void SettingsViewSystem::BuildOpenButton(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             config  = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager)
            return;
        Tsukino::Asset::AssetManager& assets = *context->assetManager;

        // 遊んでいる画面の部品なので、設定の画面より奥に置く
        const int order       = config->sortOrder - 40;
        m_openButton          = MakeElement(*m_scene, registry, entt::null, config->openButtonX, config->openButtonY, SettingsElementKind::OpenButton);
        SpriteComponent& pill = AddImage(registry,
                                         m_openButton,
                                         assets.Load(Tsukino::Core::Path(config->pillTexture)),
                                         config->openButtonWidth,
                                         config->openButtonHeight,
                                         order,
                                         config->buttonColor);
        SetSlice(pill, kPillBorder, 0.8f);
        pill.raycastTarget = true;
        registry.AddComponent<PointerTargetComponent>(m_openButton);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIButtonFeedbackComponent>(m_openButton);

        const float        half = config->openButtonWidth * 0.5f;
        const entt::entity star = MakeElement(*m_scene, registry, m_openButton, -half + 18.0f, 0.0f);
        AddImage(registry, star, assets.Load(Tsukino::Core::Path(config->starTexture)), 15.0f, 15.0f, order + 1, config->accentColor);
        AddLabel(registry, MakeElement(*m_scene, registry, m_openButton, 10.0f, 0.0f), "設定", config->textScale * 0.8f, order + 1, config->textColor, true);
    }

    //-------------------------------------------------------------
    // 設定の画面を作る
    //-------------------------------------------------------------
    void SettingsViewSystem::Build(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             config  = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager)
            return;
        Tsukino::Asset::AssetManager&     assets = *context->assetManager;
        const Tsukino::Asset::AssetHandle white  = assets.Load(Tsukino::Core::Path(config->whiteTexture));
        const Tsukino::Asset::AssetHandle pill   = assets.Load(Tsukino::Core::Path(config->pillTexture));
        const Tsukino::Asset::AssetHandle bar    = assets.Load(Tsukino::Core::Path(config->barTexture));
        const Tsukino::Asset::AssetHandle star   = assets.Load(Tsukino::Core::Path(config->starTexture));
        const Tsukino::Asset::AssetHandle glow   = assets.Load(Tsukino::Core::Path(config->glowTexture));
        const int                         order  = config->sortOrder;

        m_root = MakeElement(*m_scene, registry, entt::null, 0.0f, 0.0f);

        // 後ろを暗くする幕。後ろの部品を押せないよう、当たりも受ける
        AddImage(registry, MakeElement(*m_scene, registry, m_root, 800.0f, 500.0f), white, 1600.0f, 1000.0f, order, config->dimColor).raycastTarget = true;

        //-------------------------------------------------------------
        // カード: 後ろの光、濃紺の面、金の縁。部品はみなカードの子にして、一緒に弾ませる
        //-------------------------------------------------------------
        const entt::entity card  = MakeElement(*m_scene, registry, m_root, 800.0f, 500.0f);
        const float        cardW = config->panelWidth;
        const float        cardH = config->panelHeight;
        AddImage(registry, MakeElement(*m_scene, registry, card, 0.0f, 0.0f), glow, cardW * 1.6f, cardH * 1.7f, order + 1, config->glowColor).blendMode =
            SpriteBlendMode::Additive;
        SpriteComponent& fill = AddImage(registry, card, assets.Load(Tsukino::Core::Path(config->panelTexture)), cardW, cardH, order + 2, config->panelColor);
        SetSlice(fill, kCardBorder);
        fill.raycastTarget    = true;
        SpriteComponent& edge = AddImage(registry,
                                         MakeElement(*m_scene, registry, card, 0.0f, 0.0f),
                                         assets.Load(Tsukino::Core::Path(config->frameTexture)),
                                         cardW,
                                         cardH,
                                         order + 3,
                                         config->frameColor);
        SetSlice(edge, kCardBorder);

        const float left = -cardW * 0.5f;
        const float top  = -cardH * 0.5f;

        //-------------------------------------------------------------
        // 題: 金の星と文字、下に細い金の線
        //-------------------------------------------------------------
        const float titleY = top + 52.0f;
        AddImage(registry,
                 MakeElement(*m_scene, registry, card, left + config->labelX + 14.0f, titleY),
                 glow,
                 70.0f,
                 70.0f,
                 order + 4,
                 config->accentColor * hlslpp::float4(1.0f, 1.0f, 1.0f, 0.5f))
            .blendMode = SpriteBlendMode::Additive;
        AddImage(registry, MakeElement(*m_scene, registry, card, left + config->labelX + 14.0f, titleY), star, 32.0f, 32.0f, order + 5, config->accentColor);
        AddLabel(registry,
                 MakeElement(*m_scene, registry, card, left + config->labelX + 42.0f, titleY),
                 "設定",
                 config->titleScale,
                 order + 6,
                 config->titleColor,
                 false);
        AddImage(
            registry, MakeElement(*m_scene, registry, card, 0.0f, titleY + 34.0f), white, cardW - config->labelX * 2.0f, 1.5f, order + 4, config->frameColor);

        //-------------------------------------------------------------
        // スライダー: カプセルの線に金の塗り、つまみは光る星。線の始点を原点にして値を測る
        //-------------------------------------------------------------
        const auto slider = [&](int item, const char* name, float y) {
            AddLabel(registry, MakeElement(*m_scene, registry, card, left + config->labelX, y), name, config->textScale, order + 6, config->textColor, false);

            const float        width  = config->trackWidth;
            const entt::entity origin = MakeElement(*m_scene, registry, card, left + config->controlX, y);
            SpriteComponent&   track =
                AddImage(registry, MakeElement(*m_scene, registry, origin, width * 0.5f, 0.0f), pill, width + 12.0f, 14.0f, order + 4, config->trackColor);
            SetSlice(track, kPillBorder, 0.4f);

            SpriteComponent& bright = AddImage(registry,
                                               MakeElement(*m_scene, registry, origin, width * 0.5f, 0.0f, SettingsElementKind::Fill, item),
                                               bar,
                                               width,
                                               7.0f,
                                               order + 5,
                                               config->fillColor);
            bright.fillMode         = Tsukino::BuiltIn::ECS::SpriteFillMode::Horizontal;

            const entt::entity knob = MakeElement(*m_scene, registry, origin, 0.0f, 0.0f, SettingsElementKind::Knob, item);
            AddImage(registry,
                     MakeElement(*m_scene, registry, knob, 0.0f, 0.0f),
                     glow,
                     58.0f,
                     58.0f,
                     order + 6,
                     config->accentColor * hlslpp::float4(1.0f, 1.0f, 1.0f, 0.55f))
                .blendMode = SpriteBlendMode::Additive;
            AddImage(registry, knob, star, 30.0f, 30.0f, order + 7, config->accentColor);

            // 当たりは線より太い透明な四角
            const entt::entity hit = MakeElement(*m_scene, registry, origin, width * 0.5f, 0.0f, SettingsElementKind::Slider, item);
            AddImage(registry, hit, white, width + 32.0f, 44.0f, order + 8, hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f)).raycastTarget = true;
            registry.AddComponent<PointerTargetComponent>(hit);
            Tsukino::BuiltIn::ECS::UISliderComponent& component = registry.AddComponent<Tsukino::BuiltIn::ECS::UISliderComponent>(hit);
            component.track                                     = origin;
            component.trackLength                               = width;

            AddLabel(registry,
                     MakeElement(*m_scene, registry, card, left + config->controlX + width + 26.0f, y, SettingsElementKind::Percent, item),
                     "",
                     config->textScale,
                     order + 6,
                     config->titleColor,
                     false);
        };

        //-------------------------------------------------------------
        // 切り替え: カプセルのスイッチ。星が左右へ動き、入なら金色に光る
        //-------------------------------------------------------------
        const auto toggle = [&](int item, const char* name, float y) {
            AddLabel(registry, MakeElement(*m_scene, registry, card, left + config->labelX, y), name, config->textScale, order + 6, config->textColor, false);

            const float        x      = left + config->controlX + config->switchWidth * 0.5f;
            const entt::entity button = MakeElement(*m_scene, registry, card, x, y, SettingsElementKind::Toggle, item);
            SpriteComponent&   body   = AddImage(registry, button, pill, config->switchWidth, config->switchHeight, order + 4, config->trackColor);
            SetSlice(body, kPillBorder);
            body.raycastTarget = true;
            registry.AddComponent<PointerTargetComponent>(button);
            registry.AddComponent<Tsukino::BuiltIn::ECS::UIToggleComponent>(button);

            const entt::entity knob = MakeElement(*m_scene, registry, button, 0.0f, 0.0f, SettingsElementKind::ToggleKnob, item);
            AddImage(registry, knob, star, config->switchHeight - 8.0f, config->switchHeight - 8.0f, order + 6, config->accentColor);

            AddLabel(registry,
                     MakeElement(*m_scene, registry, card, x + config->switchWidth * 0.5f + 16.0f, y, SettingsElementKind::ToggleText, item),
                     "",
                     config->textScale,
                     order + 6,
                     config->textColor,
                     false);
        };

        const float y0 = top + config->rowStartY;
        slider(kSettingsBgm, "BGM", y0);
        slider(kSettingsSfx, "効果音", y0 + config->rowSpacing);
        toggle(kSettingsMute, "消音", y0 + config->rowSpacing * 2.0f);
        toggle(kSettingsKeyHints, "キー操作の表示", y0 + config->rowSpacing * 3.0f);

        //-------------------------------------------------------------
        // 閉じるボタン
        //-------------------------------------------------------------
        const float        closeY = cardH * 0.5f - 46.0f;
        const entt::entity close  = MakeElement(*m_scene, registry, card, 0.0f, closeY, SettingsElementKind::CloseButton);
        SpriteComponent&   button = AddImage(registry, close, pill, config->closeButtonWidth, config->closeButtonHeight, order + 4, config->buttonColor);
        SetSlice(button, kPillBorder);
        button.raycastTarget = true;
        registry.AddComponent<PointerTargetComponent>(close);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIButtonFeedbackComponent>(close);
        AddLabel(registry, MakeElement(*m_scene, registry, close, 0.0f, 0.0f), "閉じる", config->textScale, order + 6, config->textColor, true);

        // 開いたときに小さく弾ませる
        Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, card, 0.06f, 0.35f);
    }

    //-------------------------------------------------------------
    // 部品を今の値に合わせる
    //-------------------------------------------------------------
    void SettingsViewSystem::Sync(Tsukino::ECS::Registry& registry, float deltaTime) {
        const SettingsStateComponent*              state   = FindSingleton<SettingsStateComponent>(registry);
        const AudioSettingsComponent*              audio   = FindSingleton<AudioSettingsComponent>(registry);
        const SettingsConfigComponent*             config  = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!state || !audio || !config || !context || !context->assetManager)
            return;

        const Tsukino::Asset::AssetHandle star    = context->assetManager->Load(Tsukino::Core::Path(config->starTexture));
        const Tsukino::Asset::AssetHandle outline = context->assetManager->Load(Tsukino::Core::Path(config->starOutlineTexture));

        const auto  volume = [&](int item) { return item == kSettingsBgm ? audio->bgmVolume : audio->sfxVolume; };
        const auto  isOn   = [&](int item) { return item == kSettingsMute ? audio->muted : state->showKeyHints; };
        const float blend  = 1.0f - std::exp(-14.0f * deltaTime);    // スイッチの星が滑る速さ

        // スライダーをつかんでいる項目を先に調べる（つまみを大きくする）
        bool dragging[2] = {false, false};

        for(const auto entity : registry.View<SettingsElementComponent, Tsukino::BuiltIn::ECS::UISliderComponent>()) {
            const int item = registry.GetComponent<SettingsElementComponent>(entity).index;
            auto&     knob = registry.GetComponent<Tsukino::BuiltIn::ECS::UISliderComponent>(entity);
            if(!knob.dragging)
                knob.value = volume(item);
            if(item == kSettingsBgm || item == kSettingsSfx)
                dragging[item] = knob.dragging;
        }

        for(const auto entity : registry.View<SettingsElementComponent>()) {
            const SettingsElementComponent& element = registry.GetComponent<SettingsElementComponent>(entity);
            const PointerTargetComponent*   pointer = registry.try_get<PointerTargetComponent>(entity);
            const bool                      hovered = pointer && pointer->hovered;
            switch(element.kind) {
            case SettingsElementKind::Fill:
                registry.GetComponent<SpriteComponent>(entity).fillAmount = volume(element.index);
                break;
            case SettingsElementKind::Knob:
                {
                    const float         size      = dragging[element.index] ? 1.3f : 1.0f;
                    TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
                    transform.position            = hlslpp::float3(config->trackWidth * volume(element.index), 0.0f, 0.0f);
                    transform.scale               = hlslpp::float3(size, size, 1.0f);
                    transform.dirty               = true;
                    break;
                }
            case SettingsElementKind::Percent:
                HudUi::SetText(registry, entity, std::to_string(static_cast<int>(std::round(volume(element.index) * 100.0f))) + "%");
                break;
            case SettingsElementKind::Toggle:
                {
                    const bool on                                                                = isOn(element.index);
                    registry.GetComponent<Tsukino::BuiltIn::ECS::UIToggleComponent>(entity).isOn = on;
                    registry.GetComponent<SpriteComponent>(entity).tintColor = on ? config->onColor : (hovered ? config->hoverColor : config->trackColor);
                    break;
                }
            case SettingsElementKind::ToggleKnob:
                {
                    // 入は右で光る星、切は左で星の輪郭
                    const bool          on        = isOn(element.index);
                    const float         reach     = (config->switchWidth - config->switchHeight) * 0.5f;
                    const float         target    = on ? reach : -reach;
                    TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
                    const float         x         = static_cast<float>(transform.position.x);
                    // 開いた直後は滑らせず、今の位置に置く
                    transform.position      = hlslpp::float3(x == 0.0f ? target : x + (target - x) * blend, 0.0f, 0.0f);
                    transform.dirty         = true;
                    SpriteComponent& sprite = registry.GetComponent<SpriteComponent>(entity);
                    sprite.textureHandle    = on ? star : outline;
                    sprite.tintColor        = on ? hlslpp::float4(1.0f, 0.97f, 0.8f, 1.0f) : config->textColor * hlslpp::float4(1.0f, 1.0f, 1.0f, 0.6f);
                    break;
                }
            case SettingsElementKind::ToggleText:
                HudUi::SetText(registry, entity, isOn(element.index) ? "入" : "切");
                HudUi::SetTextColor(registry, entity, isOn(element.index) ? config->titleColor : config->textColor * hlslpp::float4(1.0f, 1.0f, 1.0f, 0.6f));
                break;
            case SettingsElementKind::OpenButton:
            case SettingsElementKind::CloseButton:
                registry.GetComponent<SpriteComponent>(entity).tintColor = hovered ? config->hoverColor : config->buttonColor;
                break;
            default:
                break;
            }
        }
    }

}    // namespace StardustFever::ECS
