//-------------------------------------------------------------
//! @file   ConfigReloadSystem.hpp
//! @brief  設定の Prefab JSON を保存し直したら、実行中のコンポーネントへ読み直す System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>
#include <Tsukino/Core/Log.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>

#include <atomic>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  ConfigReloadSystem
    //! @brief  Prefab（目次）JSON に並んだコンポーネントの JSON を監視し、
    //!         保存されたら PrefabFactory::Load で読み直して、そのエンティティの値を差し替える
    //! @note   エンジンの HotReload は監視を解除できない。監視のコールバックはこの System より
    //!         長生きするため、this ではなく shared_ptr の旗だけを握らせ、立った旗を Update で拾う。
    //!         読み直しは TSUKINO_ENABLE_HOT_RELOAD（Debug ビルドの既定）のときだけ動く
    //-------------------------------------------------------------
    class ConfigReloadSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  prefabFactory [in] JSON の読み込みに使う（この System より長生きすること）
        //-------------------------------------------------------------
        explicit ConfigReloadSystem(Tsukino::Engine::ECS::Prefab::PrefabFactory& prefabFactory);

        //-------------------------------------------------------------
        //! @brief  読み直せる型を登録する
        //! @tparam T        コンポーネントの型
        //! @param  typeName [in] Prefab の目次と JSON のルートで使う型名
        //-------------------------------------------------------------
        template <class T>
        void RegisterType(const std::string& typeName) {
            m_reloaders[typeName] =
                [typeName](
                    Tsukino::Engine::ECS::Prefab::PrefabFactory& factory, Tsukino::ECS::Registry& registry, entt::entity entity, const std::string& path) {
                    T* component = registry.try_get<T>(entity);
                    if(!component)
                        return;

                    //-------------------------------------------------------------
                    // 今の値を下敷きにして読む。書かれていない項目は今の値のまま残る。
                    // 書きかけの JSON を保存した瞬間にも呼ばれるので、失敗したら差し替えない
                    //-------------------------------------------------------------
                    T value = *component;
                    try {
                        if(factory.Load(path, typeName, value)) {
                            *component = value;
                            Tsukino::Core::Log::Info("Config reloaded: " + typeName + " from " + path);
                        }
                    } catch(const std::exception& e) {
                        Tsukino::Core::Log::Warn("Config reload failed: " + path + " (" + e.what() + "). Keeping the previous values.");
                    }
                };
        }

        //-------------------------------------------------------------
        //! @brief  Prefab（目次）に並んだコンポーネントのうち、登録済みの型を監視する
        //! @param  entity     [in] 値を差し替えるエンティティ（この Prefab から生成したもの）
        //! @param  prefabPath [in] Prefab（目次）JSON のパス
        //-------------------------------------------------------------
        void Track(entt::entity entity, const std::string& prefabPath);

        //-------------------------------------------------------------
        //! @brief  保存されたファイルを読み直す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        using Reloader = std::function<void(Tsukino::Engine::ECS::Prefab::PrefabFactory&, Tsukino::ECS::Registry&, entt::entity, const std::string&)>;

        struct TrackedFile {
            entt::entity                       entity;
            std::string                        path;
            std::string                        typeName;
            std::shared_ptr<std::atomic<bool>> changed;    //!< 監視のコールバックが立てる旗
        };

        Tsukino::Engine::ECS::Prefab::PrefabFactory* m_prefabFactory = nullptr;
        std::unordered_map<std::string, Reloader>    m_reloaders;
        std::vector<TrackedFile>                     m_tracked;
    };
}    // namespace StardustFever::ECS
