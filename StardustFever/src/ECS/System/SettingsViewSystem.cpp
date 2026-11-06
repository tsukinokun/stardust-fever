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

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/PointerTargetComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UISliderComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIToggleComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
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
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

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

        //-------------------------------------------------------------
        //! @brief  白い板を付ける（位置が板の中心になる）
        //-------------------------------------------------------------
        SpriteComponent& AddBoard(Tsukino::ECS::Registry&        registry,
                                  entt::entity                   entity,
                                  Tsukino::Asset::AssetHandle    texture,
                                  const SettingsConfigComponent& config,
                                  float                          width,
                                  float                          height,
                                  int                            order,
                                  const hlslpp::float4&          color) {
            SpriteComponent& sprite                                 = registry.AddComponent<SpriteComponent>(entity);
            sprite.textureHandle                                    = texture;
            sprite.space                                            = Tsukino::BuiltIn::ECS::SpriteSpace::Screen;
            sprite.sortOrder                                        = config.sortOrder + order;
            sprite.tintColor                                        = color;
            const float pixels                                      = std::max(config.whitePixels, 1.0f);
            registry.GetComponent<TransformComponent>(entity).scale = hlslpp::float3(width / pixels, height / pixels, 1.0f);
            return sprite;
        }

        //-------------------------------------------------------------
        //! @brief  文字を付ける
        //-------------------------------------------------------------
        void AddLabel(Tsukino::ECS::Registry&        registry,
                      entt::entity                   entity,
                      const SettingsConfigComponent& config,
                      const std::string&             text,
                      float                          scale,
                      int                            order,
                      bool                           centered) {
            FontComponent& font  = registry.AddComponent<FontComponent>(entity);
            font.scale           = scale;
            font.sortOrder       = config.sortOrder + order;
            font.color           = config.textColor;
            font.horizontalAlign = centered ? Tsukino::BuiltIn::ECS::HorizontalAlign::Center : Tsukino::BuiltIn::ECS::HorizontalAlign::Left;
            font.verticalAlign   = Tsukino::BuiltIn::ECS::VerticalAlign::Middle;
            HudUi::SetText(registry, entity, text);
        }
    }    // namespace

    SettingsViewSystem::SettingsViewSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // 画面を作り・片付け、値に合わせて動かす
    //-------------------------------------------------------------
    void SettingsViewSystem::Update(Tsukino::ECS::Registry& registry, float) {
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
        Sync(registry);
    }

    //-------------------------------------------------------------
    // 「設定」のボタンを作る
    //-------------------------------------------------------------
    void SettingsViewSystem::BuildOpenButton(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             config  = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager)
            return;
        const Tsukino::Asset::AssetHandle white = context->assetManager->Load(Tsukino::Core::Path(config->whiteTexture));
        m_openButton = MakeElement(*m_scene, registry, entt::null, config->openButtonX, config->openButtonY, SettingsElementKind::OpenButton);
        // 遊んでいる画面の HUD と同じ手前の高さに置き、設定の画面より奥にする
        AddBoard(registry, m_openButton, white, *config, config->buttonWidth, config->buttonHeight * 0.8f, -40, config->offColor);
        registry.AddComponent<PointerTargetComponent>(m_openButton);
        const entt::entity text = MakeElement(*m_scene, registry, entt::null, config->openButtonX, config->openButtonY);
        AddLabel(registry, text, *config, "設定", config->textScale * 0.85f, -39, true);
    }

    //-------------------------------------------------------------
    // 設定の画面を作る
    //-------------------------------------------------------------
    void SettingsViewSystem::Build(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             config  = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager)
            return;
        const Tsukino::Asset::AssetHandle white = context->assetManager->Load(Tsukino::Core::Path(config->whiteTexture));

        m_root = MakeElement(*m_scene, registry, entt::null, 0.0f, 0.0f);

        // 後ろを暗くする板。後ろの部品を押せないよう、当たりも受ける
        AddBoard(registry, MakeElement(*m_scene, registry, m_root, 800.0f, 500.0f), white, *config, 1600.0f, 1000.0f, 0, config->dimColor);

        const float left = 800.0f - config->panelWidth * 0.5f;
        const float top  = 500.0f - config->panelHeight * 0.5f;
        AddBoard(
            registry, MakeElement(*m_scene, registry, m_root, 800.0f, 500.0f), white, *config, config->panelWidth, config->panelHeight, 1, config->panelColor);
        AddLabel(registry, MakeElement(*m_scene, registry, m_root, left + config->labelX, top + 50.0f), *config, "設定", config->titleScale, 3, false);

        //-------------------------------------------------------------
        // スライダー。線の始点を原点にして、値を測る
        //-------------------------------------------------------------
        const auto slider = [&](int item, const char* name, float y) {
            AddLabel(registry, MakeElement(*m_scene, registry, m_root, left + config->labelX, y), *config, name, config->textScale, 3, false);

            const float        width  = config->trackWidth;
            const entt::entity origin = MakeElement(*m_scene, registry, m_root, left + config->controlX, y);
            AddBoard(registry, MakeElement(*m_scene, registry, origin, width * 0.5f, 0.0f), white, *config, width, 6.0f, 2, config->trackColor).raycastTarget =
                false;
            AddBoard(registry,
                     MakeElement(*m_scene, registry, origin, 0.0f, 0.0f, SettingsElementKind::Fill, item),
                     white,
                     *config,
                     width,
                     6.0f,
                     3,
                     config->fillColor)
                .raycastTarget = false;
            AddBoard(registry,
                     MakeElement(*m_scene, registry, origin, 0.0f, 0.0f, SettingsElementKind::Knob, item),
                     white,
                     *config,
                     14.0f,
                     26.0f,
                     4,
                     config->textColor)
                .raycastTarget = false;

            // 当たりは線より太い透明な板
            const entt::entity hit = MakeElement(*m_scene, registry, origin, width * 0.5f, 0.0f, SettingsElementKind::Slider, item);
            AddBoard(registry, hit, white, *config, width + 24.0f, 40.0f, 5, hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f));
            registry.AddComponent<PointerTargetComponent>(hit);
            Tsukino::BuiltIn::ECS::UISliderComponent& component = registry.AddComponent<Tsukino::BuiltIn::ECS::UISliderComponent>(hit);
            component.track                                     = origin;
            component.trackLength                               = width;

            AddLabel(registry,
                     MakeElement(*m_scene, registry, m_root, left + config->controlX + width + 22.0f, y, SettingsElementKind::Percent, item),
                     *config,
                     "",
                     config->textScale,
                     3,
                     false);
        };

        //-------------------------------------------------------------
        // 切り替えのボタン
        //-------------------------------------------------------------
        const auto toggle = [&](int item, const char* name, float y) {
            AddLabel(registry, MakeElement(*m_scene, registry, m_root, left + config->labelX, y), *config, name, config->textScale, 3, false);
            const float        x      = left + config->controlX + config->buttonWidth * 0.5f;
            const entt::entity button = MakeElement(*m_scene, registry, m_root, x, y, SettingsElementKind::Toggle, item);
            AddBoard(registry, button, white, *config, config->buttonWidth, config->buttonHeight, 2, config->offColor);
            registry.AddComponent<PointerTargetComponent>(button);
            registry.AddComponent<Tsukino::BuiltIn::ECS::UIToggleComponent>(button);
            AddLabel(registry, MakeElement(*m_scene, registry, m_root, x, y, SettingsElementKind::ToggleText, item), *config, "", config->textScale, 3, true);
        };

        const float y0 = top + config->rowStartY;
        slider(kSettingsBgm, "BGM", y0);
        slider(kSettingsSfx, "効果音", y0 + config->rowSpacing);
        toggle(kSettingsMute, "消音", y0 + config->rowSpacing * 2.0f);
        toggle(kSettingsKeyHints, "キー操作の表示", y0 + config->rowSpacing * 3.0f);

        const float        closeY = top + config->panelHeight - 44.0f;
        const entt::entity close  = MakeElement(*m_scene, registry, m_root, 800.0f, closeY, SettingsElementKind::CloseButton);
        AddBoard(registry, close, white, *config, config->buttonWidth * 1.3f, config->buttonHeight, 2, config->offColor);
        registry.AddComponent<PointerTargetComponent>(close);
        AddLabel(registry, MakeElement(*m_scene, registry, m_root, 800.0f, closeY), *config, "閉じる", config->textScale, 3, true);
    }

    //-------------------------------------------------------------
    // 部品を今の値に合わせる
    //-------------------------------------------------------------
    void SettingsViewSystem::Sync(Tsukino::ECS::Registry& registry) {
        const SettingsStateComponent*  state  = FindSingleton<SettingsStateComponent>(registry);
        const AudioSettingsComponent*  audio  = FindSingleton<AudioSettingsComponent>(registry);
        const SettingsConfigComponent* config = FindSingleton<SettingsConfigComponent>(registry);
        if(!state || !audio || !config)
            return;

        const auto  volume = [&](int item) { return item == kSettingsBgm ? audio->bgmVolume : audio->sfxVolume; };
        const auto  isOn   = [&](int item) { return item == kSettingsMute ? audio->muted : state->showKeyHints; };
        const float pixels = std::max(config->whitePixels, 1.0f);

        for(const auto entity : registry.View<SettingsElementComponent>()) {
            const SettingsElementComponent& element = registry.GetComponent<SettingsElementComponent>(entity);
            const PointerTargetComponent*   pointer = registry.try_get<PointerTargetComponent>(entity);
            const bool                      hovered = pointer && pointer->hovered;
            switch(element.kind) {
            case SettingsElementKind::Slider:
                {
                    auto& slider = registry.GetComponent<Tsukino::BuiltIn::ECS::UISliderComponent>(entity);
                    if(!slider.dragging)
                        slider.value = volume(element.index);
                    break;
                }
            case SettingsElementKind::Fill:
                {
                    // 線の始点から伸ばすので、中心は長さの半分の所
                    const float         length    = config->trackWidth * volume(element.index);
                    TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
                    transform.position            = hlslpp::float3(length * 0.5f, 0.0f, 0.0f);
                    transform.scale               = hlslpp::float3(length / pixels, 6.0f / pixels, 1.0f);
                    transform.dirty               = true;
                    break;
                }
            case SettingsElementKind::Knob:
                {
                    TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
                    transform.position            = hlslpp::float3(config->trackWidth * volume(element.index), 0.0f, 0.0f);
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
                    registry.GetComponent<SpriteComponent>(entity).tintColor = on ? config->onColor : (hovered ? config->hoverColor : config->offColor);
                    break;
                }
            case SettingsElementKind::ToggleText:
                HudUi::SetText(registry, entity, isOn(element.index) ? "入" : "切");
                break;
            case SettingsElementKind::OpenButton:
            case SettingsElementKind::CloseButton:
                registry.GetComponent<SpriteComponent>(entity).tintColor = hovered ? config->hoverColor : config->offColor;
                break;
            default:
                break;
            }
        }
    }

}    // namespace StardustFever::ECS
