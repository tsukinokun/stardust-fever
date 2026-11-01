//-------------------------------------------------------------
//! @file   PachinkoScene.cpp
//! @brief  パチンコ台のシーンの実装
//-------------------------------------------------------------
#include <StardustFever/Scene/PachinkoScene.hpp>

#include <StardustFever/ECS/GameComponentRegistration.hpp>
#include <StardustFever/ECS/System/BallEventSystem.hpp>
#include <StardustFever/ECS/System/BoardBuildSystem.hpp>
#include <StardustFever/ECS/System/ConfigReloadSystem.hpp>
#include <StardustFever/ECS/System/FeverSystem.hpp>
#include <StardustFever/ECS/System/HudSystem.hpp>
#include <StardustFever/ECS/System/LaunchSystem.hpp>
#include <StardustFever/ECS/System/LauncherInputSystem.hpp>
#include <StardustFever/ECS/System/LotterySystem.hpp>
#include <StardustFever/ECS/System/PayoutSystem.hpp>
#include <StardustFever/ECS/System/PhysicsStepSystem.hpp>
#include <StardustFever/ECS/System/StarLinkSystem.hpp>
#include <StardustFever/ECS/System/StarProgressSystem.hpp>
#include <StardustFever/ECS/System/UpgradeEffectSystem.hpp>
#include <StardustFever/ECS/System/UpgradeShopSystem.hpp>

#include <Tsukino/EngineIntegration/ECS/System/AmbientParticleSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/BloomSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/CameraSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/FontRendererSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/LightSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/ModelSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/TransformSystem.hpp>
#include <Tsukino/EngineIntegration/EngineContext.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/Log.hpp>

#include <memory>

// 名前空間 : StardustFever::Scene
namespace StardustFever::Scene {

    namespace {
        //-------------------------------------------------------------
        // シーンに置くエンティティの一覧（名前 → Prefab）。
        // カメラ・ライト・HUD・設定・ゲーム状態・盤面の組み立て要求まで、全てここから生成する
        //-------------------------------------------------------------
        constexpr const char* kSceneGroupPath = "StardustFever/Assets/Prefabs/Scene/Group.json";

        // ホットリロードで読み直す設定のエンティティ（kSceneGroupPath の中での名前と、その Prefab）
        constexpr const char* kConfigEntityName = "Config";
        constexpr const char* kConfigPrefabPath = "StardustFever/Assets/Prefabs/Config/Prefab.json";

        //-------------------------------------------------------------
        // System の実行順。値の小さい順に Update される。
        // イベントは受け取る側の Update でまとめて処理するので、
        // 出す側を先に並べると同じフレームのうちに反映される
        //-------------------------------------------------------------
        enum class SystemOrder : int {
            ConfigReload = 0,    // 設定を読み直してから、そのフレームの処理を始める
            BoardBuild,
            LauncherInput,
            UpgradeShop,
            UpgradeEffect,    // 購入 → 効果。効果を使う System（発射・払い出し・抽選など）より前に計算する
            Launch,
            Physics,
            BallEvent,       // 衝突 → 釘・入賞・アウト・星
            Lottery,         // 入賞 → 抽選結果
            Fever,           // 抽選結果 → 星釘を並べる
            StarLink,        // 星に触れた → つないだ・完成
            StarProgress,    // つないだ・完成 → レベル・倍率・完成ボーナス
            Payout,          // 全ての獲得 → 財布
            Hud,
            Transform,
            Light,
            Camera,
            AmbientParticle,
            Bloom,
            Font,
            Model,
        };

        template <class T>
        constexpr int Order(T value) {
            return static_cast<int>(value);
        }
    }    // namespace

    //-------------------------------------------------------------
    // System の登録とエンティティの生成
    //-------------------------------------------------------------
    void PachinkoScene::OnInitialize(Tsukino::EngineIntegration::EngineAPI&) {
        Tsukino::ECS::Registry&                      registry = m_scene.GetRegistry();
        Tsukino::ECS::EventBus&                      eventBus = m_scene.GetEventBus();
        Tsukino::EngineIntegration::EngineContext*   context  = registry.GetContext<Tsukino::EngineIntegration::EngineContext*>();
        Tsukino::Engine::ECS::Prefab::PrefabFactory& factory  = *context->prefabFactory;

        ECS::RegisterGameComponents(factory);

        //--------------------------------------------------------------
        // System
        //--------------------------------------------------------------
        auto configReload = std::make_shared<ECS::ConfigReloadSystem>(factory);
        ECS::RegisterConfigReloadTypes(*configReload);

        m_scene.AddSystem(configReload, Order(SystemOrder::ConfigReload));
        m_scene.AddSystem(std::make_shared<ECS::BoardBuildSystem>(), Order(SystemOrder::BoardBuild));
        m_scene.AddSystem(std::make_shared<ECS::LauncherInputSystem>(eventBus), Order(SystemOrder::LauncherInput));
        m_scene.AddSystem(std::make_shared<ECS::UpgradeShopSystem>(eventBus), Order(SystemOrder::UpgradeShop));
        m_scene.AddSystem(std::make_shared<ECS::UpgradeEffectSystem>(), Order(SystemOrder::UpgradeEffect));
        m_scene.AddSystem(std::make_shared<ECS::LaunchSystem>(), Order(SystemOrder::Launch));
        m_scene.AddSystem(std::make_shared<ECS::PhysicsStepSystem>(eventBus), Order(SystemOrder::Physics));
        m_scene.AddSystem(std::make_shared<ECS::BallEventSystem>(eventBus), Order(SystemOrder::BallEvent));
        m_scene.AddSystem(std::make_shared<ECS::LotterySystem>(eventBus), Order(SystemOrder::Lottery));
        m_scene.AddSystem(std::make_shared<ECS::FeverSystem>(eventBus), Order(SystemOrder::Fever));
        m_scene.AddSystem(std::make_shared<ECS::StarLinkSystem>(eventBus), Order(SystemOrder::StarLink));
        m_scene.AddSystem(std::make_shared<ECS::StarProgressSystem>(eventBus), Order(SystemOrder::StarProgress));
        m_scene.AddSystem(std::make_shared<ECS::PayoutSystem>(eventBus), Order(SystemOrder::Payout));
        m_scene.AddSystem(std::make_shared<ECS::HudSystem>(eventBus), Order(SystemOrder::Hud));

        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::TransformSystem>(), Order(SystemOrder::Transform));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::LightSystem>(), Order(SystemOrder::Light));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::CameraSystem>(), Order(SystemOrder::Camera));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::AmbientParticleSystem>(), Order(SystemOrder::AmbientParticle));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::BloomSystem>(), Order(SystemOrder::Bloom));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::FontRendererSystem>(), Order(SystemOrder::Font));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::ModelSystem>(), Order(SystemOrder::Model));

        //--------------------------------------------------------------
        // エンティティ
        //--------------------------------------------------------------
        const auto instance = factory.InstantiateGroup(kSceneGroupPath, registry);
        if(instance.empty()) {
            Tsukino::Core::Log::Error(std::string("Scene group could not be instantiated: ") + kSceneGroupPath);
            return;
        }

        const auto config = instance.find(kConfigEntityName);
        if(config != instance.end())
            configReload->Track(config->second, kConfigPrefabPath);
        else
            Tsukino::Core::Log::Warn(std::string("Scene group has no '") + kConfigEntityName + "' entity; config hot reload is disabled.");
    }

    //-------------------------------------------------------------
    // シーンの更新
    //-------------------------------------------------------------
    void PachinkoScene::OnUpdate(Tsukino::EngineIntegration::EngineAPI&, float deltaTime) {
        m_scene.Update(deltaTime);
    }

    //-------------------------------------------------------------
    // シーンの終了処理
    //-------------------------------------------------------------
    void PachinkoScene::OnExit() {
    }

}    // namespace StardustFever::Scene
