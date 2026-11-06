//-------------------------------------------------------------
//! @file   SettingsSystem.cpp
//! @brief  SettingsSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/SettingsSystem.hpp>

#include <StardustFever/ECS/Component/AudioSettingsComponent.hpp>
#include <StardustFever/ECS/Component/KeyHintComponent.hpp>
#include <StardustFever/ECS/Component/SettingsElementComponent.hpp>
#include <StardustFever/ECS/Component/SettingsStateComponent.hpp>
#include <StardustFever/ECS/Component/StarMapStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/PointerTargetComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UISliderComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIToggleComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIVisibilityComponent.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>

#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    // 入力を読んで設定を書き換える
    //-------------------------------------------------------------
    void SettingsSystem::Update(Tsukino::ECS::Registry& registry, float) {
        SettingsStateComponent*                    state   = FindSingleton<SettingsStateComponent>(registry);
        AudioSettingsComponent*                    audio   = FindSingleton<AudioSettingsComponent>(registry);
        const StarMapStateComponent*               map     = FindSingleton<StarMapStateComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!state || !audio || !context || !context->inputSystem)
            return;

        using namespace Tsukino::BuiltIn::ECS;

        //-------------------------------------------------------------
        // 開け閉め。星図を開いている間の Esc は星図のもの
        //-------------------------------------------------------------
        bool toggle = context->inputSystem->IsKeyPressed(Tsukino::Input::KeyCode::Escape) && !(map && map->open);
        for(const auto entity : registry.View<SettingsElementComponent, PointerTargetComponent>()) {
            const SettingsElementComponent& element = registry.GetComponent<SettingsElementComponent>(entity);
            const bool                      clicked = registry.GetComponent<PointerTargetComponent>(entity).clicked;
            if(clicked && element.kind == SettingsElementKind::OpenButton && !state->open && !(map && map->open))
                toggle = true;
            if(clicked && element.kind == SettingsElementKind::CloseButton && state->open)
                toggle = true;
        }
        if(toggle)
            state->open = !state->open;

        //-------------------------------------------------------------
        // スライダーと切り替えの値を受け取る
        //-------------------------------------------------------------
        for(const auto entity : registry.View<SettingsElementComponent, UISliderComponent>()) {
            const UISliderComponent& slider = registry.GetComponent<UISliderComponent>(entity);
            const int                item   = registry.GetComponent<SettingsElementComponent>(entity).index;
            if(slider.changed) {
                // 表示と合うよう 1% 単位にそろえる
                const float value = std::round(slider.value * 100.0f) / 100.0f;
                if(item == kSettingsBgm)
                    audio->bgmVolume = value;
                else if(item == kSettingsSfx)
                    audio->sfxVolume = value;
            }
            if(slider.dragEnded && item == kSettingsSfx)
                state->previewSound = true;
        }
        for(const auto entity : registry.View<SettingsElementComponent, UIToggleComponent>()) {
            const UIToggleComponent& toggleButton = registry.GetComponent<UIToggleComponent>(entity);
            if(!toggleButton.changed)
                continue;
            const int item = registry.GetComponent<SettingsElementComponent>(entity).index;
            if(item == kSettingsMute)
                audio->muted = toggleButton.isOn;
            else if(item == kSettingsKeyHints)
                state->showKeyHints = toggleButton.isOn;
        }

        //-------------------------------------------------------------
        // キー操作の表示を出し入れする。星図も HUD を隠すので、変わったときだけ書く
        //-------------------------------------------------------------
        const int wanted = state->showKeyHints ? 1 : 0;
        if(wanted != m_appliedKeyHints) {
            bool found = false;
            for(const auto entity : registry.View<KeyHintComponent>()) {
                UIVisibilityComponent& visibility = registry.HasComponent<UIVisibilityComponent>(entity) ? registry.GetComponent<UIVisibilityComponent>(entity)
                                                                                                         : registry.AddComponent<UIVisibilityComponent>(entity);
                visibility.visible                = state->showKeyHints;
                found                             = true;
            }
            // HUD が出来る前は、出来てから反映する
            if(found)
                m_appliedKeyHints = wanted;
        }
    }

}    // namespace StardustFever::ECS
