//-------------------------------------------------------------
//! @file   SaveSystem.cpp
//! @brief  SaveSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/SaveSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/Data/SaveData.hpp>
#include <StardustFever/ECS/Component/AudioSettingsComponent.hpp>
#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/SettingsStateComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/SaveConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/Log.hpp>

#include <cereal/archives/json.hpp>

#include <exception>
#include <filesystem>
#include <fstream>
#include <system_error>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        constexpr const char* kRootKey = "SaveData";

        //-------------------------------------------------------------
        //! @brief  今のコンポーネントからセーブの中身を作る
        //! @param  registry [in] レジストリ
        //! @return セーブの中身
        //-------------------------------------------------------------
        Data::SaveData Capture(Tsukino::ECS::Registry& registry) {
            Data::SaveData data;
            data.version = Data::kSaveDataVersion;
            if(const WalletComponent* wallet = FindSingleton<WalletComponent>(registry))
                data.stardust = wallet->stardust;
            if(const UpgradeLevelsComponent* upgrades = FindSingleton<UpgradeLevelsComponent>(registry))
                data.upgradeLevels = upgrades->levels;
            if(const StarCollectionComponent* collection = FindSingleton<StarCollectionComponent>(registry)) {
                data.starLevels    = collection->starLevels;
                data.unseenStars   = collection->unseenStars;
                data.skyObjects    = collection->skyObjects;
                data.unseenObjects = collection->unseenObjects;
            }
            if(const PrestigeStateComponent* prestige = FindSingleton<PrestigeStateComponent>(registry)) {
                data.cosmicConstants = prestige->cosmicConstants;
                data.bigBangCount    = prestige->bigBangCount;
                data.universeEarned  = prestige->universeEarned;
            }
            if(const AutoFireComponent* autoFire = FindSingleton<AutoFireComponent>(registry))
                data.autoFireEnabled = autoFire->enabled;
            if(const AudioSettingsComponent* audio = FindSingleton<AudioSettingsComponent>(registry)) {
                data.bgmVolume  = audio->bgmVolume;
                data.sfxVolume  = audio->sfxVolume;
                data.audioMuted = audio->muted;
            }
            if(const SettingsStateComponent* settings = FindSingleton<SettingsStateComponent>(registry))
                data.showKeyHints = settings->showKeyHints;
            return data;
        }

        //-------------------------------------------------------------
        //! @brief  セーブの中身をコンポーネントへ入れる
        //! @param  registry [in] レジストリ
        //! @param  data     [in] セーブの中身
        //! @note   自動発射の解禁は強化のレベルから UpgradeEffectSystem が決め直すので、ここでは ON/OFF だけ戻す
        //-------------------------------------------------------------
        void Apply(Tsukino::ECS::Registry& registry, const Data::SaveData& data) {
            if(WalletComponent* wallet = FindSingleton<WalletComponent>(registry))
                wallet->stardust = data.stardust;
            if(UpgradeLevelsComponent* upgrades = FindSingleton<UpgradeLevelsComponent>(registry))
                upgrades->levels = data.upgradeLevels;
            if(StarCollectionComponent* collection = FindSingleton<StarCollectionComponent>(registry)) {
                collection->starLevels    = data.starLevels;
                collection->unseenStars   = data.unseenStars;
                collection->skyObjects    = data.skyObjects;
                collection->unseenObjects = data.unseenObjects;
            }
            if(PrestigeStateComponent* prestige = FindSingleton<PrestigeStateComponent>(registry)) {
                prestige->cosmicConstants = data.cosmicConstants;
                prestige->bigBangCount    = data.bigBangCount;
                prestige->universeEarned  = data.universeEarned;
            }
            if(AutoFireComponent* autoFire = FindSingleton<AutoFireComponent>(registry))
                autoFire->enabled = data.autoFireEnabled;
            if(AudioSettingsComponent* audio = FindSingleton<AudioSettingsComponent>(registry)) {
                audio->bgmVolume = data.bgmVolume;
                audio->sfxVolume = data.sfxVolume;
                audio->muted     = data.audioMuted;
            }
            if(SettingsStateComponent* settings = FindSingleton<SettingsStateComponent>(registry))
                settings->showKeyHints = data.showKeyHints;
        }
    }    // namespace

    SaveSystem::SaveSystem(Tsukino::ECS::EventBus& eventBus)
        : m_purchases(eventBus)
        , m_draws(eventBus)
        , m_bigBangs(eventBus) {
    }

    //-------------------------------------------------------------
    // 読み込みと定期保存
    //-------------------------------------------------------------
    void SaveSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const SaveConfigComponent* config = FindSingleton<SaveConfigComponent>(registry);
        if(!config)
            return;

        if(!m_loaded) {
            Load(registry);
            m_loaded        = true;
            m_autoSaveTimer = config->autoSaveInterval;
            return;
        }

        // 取り返しのつかない出来事の直後は、自動保存を待たずに保存する。
        // 3つとも必ず空にしたいので、短絡評価の || ではなく | でつなぐ
        const bool important = !m_purchases.Drain().empty() | !m_draws.Drain().empty() | !m_bigBangs.Drain().empty();

        m_autoSaveTimer -= deltaTime;
        if(important || m_autoSaveTimer <= 0.0f) {
            SaveNow(registry);
            m_autoSaveTimer = config->autoSaveInterval;
        }
    }

    //-------------------------------------------------------------
    // セーブを読んでコンポーネントへ入れる
    //-------------------------------------------------------------
    void SaveSystem::Load(Tsukino::ECS::Registry& registry) {
        const SaveConfigComponent* config = FindSingleton<SaveConfigComponent>(registry);
        if(!config)
            return;

        const std::filesystem::path path(config->savePath);
        std::error_code             error;
        if(!std::filesystem::exists(path, error)) {
            Tsukino::Core::Log::Info("No save file at " + config->savePath + "; starting a new game.");
            return;
        }

        try {
            std::ifstream  stream(path);
            Data::SaveData data;
            {
                cereal::JSONInputArchive archive(stream);
                archive(cereal::make_nvp(kRootKey, data));
            }
            // 古い版のセーブ（星を "rigel" のような名前で持っていた）を今の ID へ直してから入れる
            Data::MigrateSaveData(data, Data::GetStarTable());
            Apply(registry, data);
            Tsukino::Core::Log::Info("Loaded save from " + config->savePath + " (stardust " + data.stardust.ToJapaneseString() + ", big bangs "
                                     + std::to_string(data.bigBangCount) + ").");
        } catch(const std::exception& e) {
            //-------------------------------------------------------------
            // 読めないセーブで新しいゲームを上書きすると、直せたかもしれない進行が消える。
            // 退避してから新しく始める
            //-------------------------------------------------------------
            std::filesystem::path backup  = path;
            backup                       += ".bad";
            std::filesystem::rename(path, backup, error);
            Tsukino::Core::Log::Error("Failed to read save " + config->savePath + " (" + e.what() + "). Moved it to " + backup.string()
                                      + " and started a new game.");
        }
    }

    //-------------------------------------------------------------
    // 今の進行をすぐ保存する
    //-------------------------------------------------------------
    void SaveSystem::SaveNow(Tsukino::ECS::Registry& registry) {
        if(!m_loaded)
            return;
        const SaveConfigComponent* config = FindSingleton<SaveConfigComponent>(registry);
        if(!config)
            return;

        const std::filesystem::path path(config->savePath);
        std::filesystem::path       temporary  = path;
        temporary                             += ".tmp";

        std::error_code error;
        if(path.has_parent_path())
            std::filesystem::create_directories(path.parent_path(), error);

        try {
            Data::SaveData data = Capture(registry);
            {
                std::ofstream stream(temporary, std::ios::trunc);
                if(!stream.is_open()) {
                    Tsukino::Core::Log::Error("Failed to open " + temporary.string() + " for saving. Progress was not saved.");
                    return;
                }
                cereal::JSONOutputArchive archive(stream);
                archive(cereal::make_nvp(kRootKey, data));
            }
            // 書き終えてから入れ替える。途中で落ちても元のセーブはそのまま残る
            std::filesystem::rename(temporary, path, error);
            if(error)
                Tsukino::Core::Log::Error("Failed to replace " + config->savePath + " (" + error.message() + "). Progress was not saved.");
        } catch(const std::exception& e) {
            Tsukino::Core::Log::Error("Failed to save " + config->savePath + " (" + e.what() + ").");
        }
    }

}    // namespace StardustFever::ECS
