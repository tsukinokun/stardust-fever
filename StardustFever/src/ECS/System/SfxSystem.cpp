//-------------------------------------------------------------
//! @file   SfxSystem.cpp
//! @brief  SfxSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/SfxSystem.hpp>

#include <StardustFever/ECS/Component/AudioConfigComponent.hpp>
#include <StardustFever/ECS/Component/AudioSettingsComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PocketComponent.hpp>
#include <StardustFever/ECS/Component/SettingsStateComponent.hpp>
#include <StardustFever/ECS/Component/StarMapStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/LotteryReach.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Audio/AudioManager.hpp>
#include <Tsukino/BuiltIn/ECS/Component/PointerTargetComponent.hpp>
#include <Tsukino/Core/Log.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/Asset/Audio/AudioAsset.hpp>

#include <algorithm>
#include <chrono>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    SfxSystem::SfxSystem(Tsukino::ECS::EventBus& eventBus)
        : m_launches(eventBus)
        , m_pins(eventBus)
        , m_pockets(eventBus)
        , m_lotteries(eventBus)
        , m_feverEnds(eventBus)
        , m_chanceStarts(eventBus)
        , m_chanceEnds(eventBus)
        , m_links(eventBus)
        , m_draws(eventBus)
        , m_constellations(eventBus)
        , m_bigBangs(eventBus)
        , m_purchases(eventBus)
        , m_denials(eventBus)
        , m_starFx(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // まだ依頼していない音を裏で読み始め、読み終えた音を受け取る
    //-------------------------------------------------------------
    void SfxSystem::UpdateLoading(Tsukino::ECS::Registry& registry, const AudioConfigComponent& config) {
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->audioManager || !context->assetManager)
            return;

        // 設定の読み直しで増えた音もここで拾う
        for(const SfxDef& sound : config.sounds) {
            if(!m_loaded.contains(sound.path) && !m_loading.contains(sound.path))
                m_loading.emplace(sound.path, context->assetManager->LoadAsync(Tsukino::Core::Path(sound.path)));
        }

        for(auto it = m_loading.begin(); it != m_loading.end();) {
            if(it->second.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
                ++it;
                continue;
            }
            auto asset = std::dynamic_pointer_cast<Tsukino::Asset::AudioAsset>(context->assetManager->Get(it->second.get()));
            if(asset)
                context->audioManager->Prepare(*asset);
            else
                Tsukino::Core::Log::Warn("Sound could not be loaded: " + it->first);
            m_loaded.emplace(it->first, std::move(asset));
            it = m_loading.erase(it);
        }
    }

    //-------------------------------------------------------------
    // 音を鳴らす
    //-------------------------------------------------------------
    void SfxSystem::Play(Tsukino::ECS::Registry& registry, const AudioConfigComponent& config, std::string_view id, int variant) {
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->audioManager || !context->assetManager)
            return;

        std::vector<const SfxDef*> group;
        for(const SfxDef& sound : config.sounds) {
            if(sound.id == id)
                group.push_back(&sound);
        }
        if(group.empty())
            return;

        const int     last   = static_cast<int>(group.size()) - 1;
        const int     index  = variant < 0 ? std::uniform_int_distribution<int>(0, last)(m_random) : std::min(variant, last);
        const SfxDef& sound  = *group[static_cast<size_t>(index)];
        const auto    loaded = m_loaded.find(sound.path);
        if(loaded == m_loaded.end() || !loaded->second || !context->audioManager->IsPrepared(*loaded->second))
            return;
        context->audioManager->Play(*loaded->second, false, sound.volume, static_cast<Tsukino::Audio::AudioBus>(config.sfxBus));
    }

    //-------------------------------------------------------------
    // このフレームの出来事の音を鳴らす
    //-------------------------------------------------------------
    void SfxSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto launches       = m_launches.Drain();
        const auto pins           = m_pins.Drain();
        const auto pockets        = m_pockets.Drain();
        const auto lotteries      = m_lotteries.Drain();
        const auto feverEnds      = m_feverEnds.Drain();
        const auto chanceStarts   = m_chanceStarts.Drain();
        const auto chanceEnds     = m_chanceEnds.Drain();
        const auto links          = m_links.Drain();
        const auto draws          = m_draws.Drain();
        const auto constellations = m_constellations.Drain();
        const auto bigBangs       = m_bigBangs.Drain();
        const auto purchases      = m_purchases.Drain();
        const auto denials        = m_denials.Drain();
        const auto starFx         = m_starFx.Drain();

        const AudioConfigComponent* config = FindSingleton<AudioConfigComponent>(registry);
        if(!config)
            return;

        UpdateLoading(registry, *config);

        //-------------------------------------------------------------
        // 玉
        //-------------------------------------------------------------
        if(!launches.empty())
            Play(registry, *config, "launch");

        m_pinCooldown = std::max(m_pinCooldown - deltaTime, 0.0f);
        if(!pins.empty() && m_pinCooldown <= 0.0f) {
            Play(registry, *config, "pin", -1);
            m_pinCooldown = config->pinMinInterval;
        }

        for(const BallEnteredPocketEvent& entry : pockets) {
            const PocketComponent* pocket = registry.try_get<PocketComponent>(entry.pocket);
            if(!pocket)
                continue;
            if(pocket->role == "start")
                Play(registry, *config, "pocketStart");
            else if(pocket->role == "starGate")
                Play(registry, *config, "pocketGate");
            else
                Play(registry, *config, "pocketTulip");
        }

        //-------------------------------------------------------------
        // リール。止まった瞬間とリーチの入りは状態の変化から拾う
        //-------------------------------------------------------------
        if(const LotteryStateComponent* lottery = FindSingleton<LotteryStateComponent>(registry)) {
            for(size_t reel = 0; reel < m_wasStopped.size(); ++reel) {
                if(lottery->reelStopped[reel] && !m_wasStopped[reel])
                    Play(registry, *config, "reelStop");
                m_wasStopped[reel] = lottery->reelStopped[reel];
            }
            for(const LotteryResultEvent& result : lotteries) {
                if(result.won)
                    Play(registry, *config, "win");
                else if(m_wasReach)
                    Play(registry, *config, "miss");    // 外れのたびに鳴らすとうるさいので、リーチが外れたときだけ
            }
            const bool reach = IsReach(*lottery);
            if(reach && !m_wasReach)
                Play(registry, *config, "reach");
            m_wasReach = reach;
        }

        if(!feverEnds.empty())
            Play(registry, *config, "feverEnd");

        //-------------------------------------------------------------
        // 星ゲットチャンス。つなぐたびに音階を上がる
        //-------------------------------------------------------------
        if(!chanceStarts.empty()) {
            m_linkCount = 0;
            Play(registry, *config, "chanceStart");
        }
        for(size_t i = 0; i < links.size(); ++i) {
            Play(registry, *config, "starLink", m_linkCount);
            ++m_linkCount;
        }
        for(const StarChanceEndedEvent& end : chanceEnds) {
            if(!end.success)
                Play(registry, *config, "chanceFail");
        }

        // 星座がそろったなら、星の音は重ねない
        if(!constellations.empty())
            Play(registry, *config, "constellation");
        else if(!draws.empty())
            Play(registry, *config, "starGet");

        for(const StarGetFxEvent& fx : starFx) {
            Play(registry, *config, fx.arrived ? "starArrive" : "starFly");
        }

        //-------------------------------------------------------------
        // 押せる部品。乗ったときと押したとき
        //-------------------------------------------------------------
        for(const auto entity : registry.View<Tsukino::BuiltIn::ECS::PointerTargetComponent>()) {
            const auto& pointer = registry.GetComponent<Tsukino::BuiltIn::ECS::PointerTargetComponent>(entity);
            if(pointer.clicked)
                Play(registry, *config, "uiClick");
            else if(pointer.hoverEntered)
                Play(registry, *config, "uiHover");
        }

        //-------------------------------------------------------------
        // 操作
        //-------------------------------------------------------------
        if(!bigBangs.empty())
            Play(registry, *config, "bigBang");
        if(!purchases.empty())
            Play(registry, *config, "purchase");
        if(!denials.empty())
            Play(registry, *config, "denied");

        if(const StarMapStateComponent* map = FindSingleton<StarMapStateComponent>(registry)) {
            if(map->open != m_wasMapOpen)
                Play(registry, *config, map->open ? "mapOpen" : "mapClose");
            m_wasMapOpen = map->open;
        }

        if(const AudioSettingsComponent* settings = FindSingleton<AudioSettingsComponent>(registry)) {
            // スライダーを動かしている間は鳴らさず、離したときに確かめの音を鳴らす
            SettingsStateComponent* screen = FindSingleton<SettingsStateComponent>(registry);
            if(screen && screen->previewSound) {
                screen->previewSound = false;
                Play(registry, *config, "volumeTick");
            } else if(m_lastSfxLevel >= 0.0f && settings->sfxVolume != m_lastSfxLevel && !(screen && screen->open)) {
                Play(registry, *config, "volumeTick");
            }
            m_lastSfxLevel = settings->sfxVolume;
        }
    }

}    // namespace StardustFever::ECS
