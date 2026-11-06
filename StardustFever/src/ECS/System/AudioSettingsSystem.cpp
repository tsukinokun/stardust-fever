//-------------------------------------------------------------
//! @file   AudioSettingsSystem.cpp
//! @brief  AudioSettingsSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/AudioSettingsSystem.hpp>

#include <StardustFever/ECS/Component/AudioConfigComponent.hpp>
#include <StardustFever/ECS/Component/AudioSettingsComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Audio/AudioManager.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>

#include <algorithm>
#include <cmath>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        //-------------------------------------------------------------
        //! @brief  音量を一段変える
        //! @param  volume [in,out] 音量
        //! @param  delta  [in] 変える量
        //! @note   0.1 刻みを足し続けても端数が残らないよう、1% 単位に丸める
        //-------------------------------------------------------------
        void Step(float& volume, float delta) {
            volume = std::clamp(std::round((volume + delta) * 100.0f) / 100.0f, 0.0f, 1.0f);
        }

        //-------------------------------------------------------------
        //! @brief  音量を割合の文字にする
        //! @param  volume [in] 音量
        //! @return 「70%」のような文字
        //-------------------------------------------------------------
        std::string Percent(float volume) {
            return std::to_string(static_cast<int>(std::round(volume * 100.0f))) + "%";
        }
    }    // namespace

    AudioSettingsSystem::AudioSettingsSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus) {
    }

    //-------------------------------------------------------------
    // キーを読み、バスの音量を合わせる
    //-------------------------------------------------------------
    void AudioSettingsSystem::Update(Tsukino::ECS::Registry& registry, float) {
        AudioSettingsComponent*                    settings = FindSingleton<AudioSettingsComponent>(registry);
        const AudioConfigComponent*                config   = FindSingleton<AudioConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context  = GetEngineContext(registry);
        if(!settings || !config || !context || !context->inputSystem || !context->audioManager)
            return;

        using Tsukino::Input::KeyCode;
        const Tsukino::Input::InputSystem& input = *context->inputSystem;

        if(input.IsKeyPressed(KeyCode::M)) {
            settings->muted = !settings->muted;
            m_eventBus->Publish(HudMessageEvent{settings->muted ? "消音 ON" : "消音 OFF"});
        }
        const bool bgmDown = input.IsKeyPressed(KeyCode::F6);
        const bool bgmUp   = input.IsKeyPressed(KeyCode::F7);
        if(bgmDown || bgmUp) {
            Step(settings->bgmVolume, bgmUp ? config->volumeStep : -config->volumeStep);
            m_eventBus->Publish(HudMessageEvent{"BGM " + Percent(settings->bgmVolume)});
        }
        const bool sfxDown = input.IsKeyPressed(KeyCode::F8);
        const bool sfxUp   = input.IsKeyPressed(KeyCode::F9);
        if(sfxDown || sfxUp) {
            Step(settings->sfxVolume, sfxUp ? config->volumeStep : -config->volumeStep);
            m_eventBus->Publish(HudMessageEvent{"効果音 " + Percent(settings->sfxVolume)});
        }

        //-------------------------------------------------------------
        // セーブから戻った値もここで反映される
        //-------------------------------------------------------------
        Tsukino::Audio::AudioManager& audio = *context->audioManager;
        const auto                    apply = [&](int bus, float volume) {
            const auto  id    = static_cast<Tsukino::Audio::AudioBus>(bus);
            const float value = settings->muted ? 0.0f : volume;
            if(audio.GetBusVolume(id) != value)
                audio.SetBusVolume(id, value);
        };
        apply(config->bgmBus, settings->bgmVolume);
        apply(config->sfxBus, settings->sfxVolume);
    }

}    // namespace StardustFever::ECS
