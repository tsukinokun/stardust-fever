//-------------------------------------------------------------
//! @file   BgmSystem.cpp
//! @brief  BgmSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BgmSystem.hpp>

#include <StardustFever/ECS/Component/AudioConfigComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/StarMapStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/LotteryReach.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Audio/AudioManager.hpp>
#include <Tsukino/Core/Log.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    // 場面を決め、曲を切り替える
    //-------------------------------------------------------------
    void BgmSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const AudioConfigComponent*                config  = FindSingleton<AudioConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->audioManager || !context->assetManager)
            return;
        Tsukino::Audio::AudioManager& audio = *context->audioManager;
        const auto                    bus   = static_cast<Tsukino::Audio::AudioBus>(config->bgmBus);

        //-------------------------------------------------------------
        // 曲を読む。読んだら一度鳴らしてバンクを開いておく（要らない曲は下で止まる）
        //-------------------------------------------------------------
        const std::array<const std::string*, kSceneCount> paths = {
            &config->bgmNormal, &config->bgmReach, &config->bgmFever, &config->bgmChance, &config->bgmMap};
        for(int i = 0; i < kSceneCount; ++i) {
            Track& track = m_tracks[static_cast<size_t>(i)];
            if(track.path == *paths[static_cast<size_t>(i)])
                continue;
            if(track.asset)
                audio.Stop(*track.asset);
            track.path   = *paths[static_cast<size_t>(i)];
            track.volume = 0.0f;
            track.asset =
                std::dynamic_pointer_cast<Tsukino::Asset::AudioAsset>(context->assetManager->Get(context->assetManager->Load(Tsukino::Core::Path(track.path))));
            if(track.asset)
                audio.Play(*track.asset, true, 0.0f, bus);
            else
                Tsukino::Core::Log::Warn("BGM could not be loaded: " + track.path);
        }

        //-------------------------------------------------------------
        // 今の場面。上の場面ほど優先する
        //-------------------------------------------------------------
        const FeverStateComponent*      fever   = FindSingleton<FeverStateComponent>(registry);
        const StarChanceStateComponent* chance  = FindSingleton<StarChanceStateComponent>(registry);
        const StarMapStateComponent*    map     = FindSingleton<StarMapStateComponent>(registry);
        const LotteryStateComponent*    lottery = FindSingleton<LotteryStateComponent>(registry);

        std::array<bool, kSceneCount> alive = {};    // 音量が0でも鳴らし続ける曲。戻ったときに続きから聴かせる
        alive[kNormal]                      = true;
        alive[kMap]                         = true;
        alive[kReach]                       = lottery && IsReach(*lottery);
        alive[kFever]                       = fever && fever->active;
        alive[kChance]                      = chance && chance->active;

        Scene scene = kNormal;
        if(map && map->open)
            scene = kMap;
        else if(alive[kChance])
            scene = kChance;
        else if(alive[kFever])
            scene = kFever;
        else if(alive[kReach])
            scene = kReach;

        if(scene != m_scene)
            Tsukino::Core::Log::Info("BGM scene: " + m_tracks[static_cast<size_t>(scene)].path);
        const bool  fast    = scene == kReach || m_scene == kReach;
        const float seconds = std::max(fast ? config->fastFadeSeconds : config->fadeSeconds, 0.01f);
        const float step    = deltaTime / seconds;
        m_scene             = scene;

        //-------------------------------------------------------------
        // 音量を動かす。止まっていた曲は頭から鳴らすので、サビから聴こえる
        //-------------------------------------------------------------
        for(int i = 0; i < kSceneCount; ++i) {
            Track& track = m_tracks[static_cast<size_t>(i)];
            if(!track.asset)
                continue;
            const bool active  = i == scene;
            bool       playing = audio.IsPlaying(*track.asset);
            if(active && !playing) {
                audio.Play(*track.asset, true, 0.0f, bus);
                track.volume = 0.0f;
                playing      = true;
            }
            if(!playing)
                continue;

            track.volume = std::clamp(track.volume + (active ? step : -step), 0.0f, 1.0f);
            audio.SetVolume(*track.asset, track.volume);
            if(track.volume <= 0.0f && !active && !alive[static_cast<size_t>(i)])
                audio.Stop(*track.asset);
        }
    }

}    // namespace StardustFever::ECS
