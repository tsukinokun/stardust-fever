//-------------------------------------------------------------
//! @file   ConfigReloadSystem.cpp
//! @brief  ConfigReloadSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/ConfigReloadSystem.hpp>

#include <Tsukino/Core/IO/HotReload.hpp>
#include <Tsukino/Core/Path.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

#include <fstream>
#include <map>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    ConfigReloadSystem::ConfigReloadSystem(Tsukino::Engine::ECS::Prefab::PrefabFactory& prefabFactory)
        : m_prefabFactory(&prefabFactory) {
    }

    //-------------------------------------------------------------
    // Prefab（目次）に並んだコンポーネントを監視する
    //-------------------------------------------------------------
    void ConfigReloadSystem::Track(entt::entity entity, const std::string& prefabPath) {
        //-------------------------------------------------------------
        // 目次は PrefabFactory と同じ形式（"Components": [{key: 型名, value: JSONパス}]）。
        // 監視するファイルの一覧をコードに書き写さず、目次から取る
        //-------------------------------------------------------------
        std::map<std::string, std::string> components;
        {
            std::ifstream stream(prefabPath);
            if(!stream.is_open()) {
                Tsukino::Core::Log::Warn("Config prefab not found, hot reload disabled for it: " + prefabPath);
                return;
            }
            try {
                cereal::JSONInputArchive archive(stream);
                archive(cereal::make_nvp("Components", components));
            } catch(const std::exception& e) {
                Tsukino::Core::Log::Warn("Failed to read config prefab: " + prefabPath + " (" + e.what() + ")");
                return;
            }
        }

        for(const auto& [typeName, path] : components) {
            if(m_reloaders.find(typeName) == m_reloaders.end())
                continue;

            auto changed = std::make_shared<std::atomic<bool>>(false);
            Tsukino::IO::HotReload::Watch(Tsukino::Core::Path(path), [changed] {
                changed->store(true);
                return true;
            });
            m_tracked.push_back(TrackedFile{entity, path, typeName, changed});
        }
    }

    //-------------------------------------------------------------
    // 保存されたファイルを読み直す
    //-------------------------------------------------------------
    void ConfigReloadSystem::Update(Tsukino::ECS::Registry& registry, float) {
        for(const TrackedFile& file : m_tracked) {
            if(!file.changed->exchange(false))
                continue;
            if(!registry.IsValid(file.entity))
                continue;

            const auto reloader = m_reloaders.find(file.typeName);
            if(reloader != m_reloaders.end())
                reloader->second(*m_prefabFactory, registry, file.entity, file.path);
        }
    }

}    // namespace StardustFever::ECS
