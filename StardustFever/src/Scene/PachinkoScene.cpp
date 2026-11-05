//-------------------------------------------------------------
//! @file   PachinkoScene.cpp
//! @brief  パチンコ台のシーンの実装
//-------------------------------------------------------------
#include <StardustFever/Scene/PachinkoScene.hpp>

#include <StardustFever/ECS/GameComponentRegistration.hpp>
#include <StardustFever/ECS/System/BallEventSystem.hpp>
#include <StardustFever/ECS/System/BigBangSystem.hpp>
#include <StardustFever/ECS/System/BoardBuildSystem.hpp>
#include <StardustFever/ECS/System/CollectionEffectSystem.hpp>
#include <StardustFever/ECS/System/ConfigReloadSystem.hpp>
#include <StardustFever/ECS/System/FeverSystem.hpp>
#include <StardustFever/ECS/System/FxSystem.hpp>
#include <StardustFever/ECS/System/GlowHaloSystem.hpp>
#include <StardustFever/ECS/System/HudGaugeSystem.hpp>
#include <StardustFever/ECS/System/HudIdleMotionSystem.hpp>
#include <StardustFever/ECS/System/HudReactionSystem.hpp>
#include <StardustFever/ECS/System/HudSystem.hpp>
#include <StardustFever/ECS/System/LaunchSystem.hpp>
#include <StardustFever/ECS/System/LauncherInputSystem.hpp>
#include <StardustFever/ECS/System/LotterySystem.hpp>
#include <StardustFever/ECS/System/LotteryViewSystem.hpp>
#include <StardustFever/ECS/System/PayoutSystem.hpp>
#include <StardustFever/ECS/System/PocketLabelSystem.hpp>
#include <StardustFever/ECS/System/PhysicsStepSystem.hpp>
#include <StardustFever/ECS/System/RollingNumberSystem.hpp>
#include <StardustFever/ECS/System/SaveSystem.hpp>
#include <StardustFever/ECS/System/ScoreMultiplierSystem.hpp>
#include <StardustFever/ECS/System/ShootingStarSystem.hpp>
#include <StardustFever/ECS/System/ShopViewSystem.hpp>
#include <StardustFever/ECS/System/SparkleTrailSystem.hpp>
#include <StardustFever/ECS/System/StarChartViewSystem.hpp>
#include <StardustFever/ECS/System/StarChanceSystem.hpp>
#include <StardustFever/ECS/System/StarDrawSystem.hpp>
#include <StardustFever/ECS/System/StarLinkSystem.hpp>
#include <StardustFever/ECS/System/StarMapInputSystem.hpp>
#include <StardustFever/ECS/System/StarMapViewSystem.hpp>
#include <StardustFever/ECS/System/ToastSystem.hpp>
#include <StardustFever/ECS/System/TulipSystem.hpp>
#include <StardustFever/ECS/System/TwinkleSystem.hpp>
#include <StardustFever/ECS/System/UpgradeEffectSystem.hpp>
#include <StardustFever/ECS/System/UpgradeShopSystem.hpp>

#include <Tsukino/EngineIntegration/ECS/System/AmbientParticleSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/BloomSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/BurstParticleSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/CameraSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/FontRendererSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/LifetimeSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/LightSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/ModelSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/SpriteRendererSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/TransformSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/UITweenSystem.hpp>
#include <Tsukino/EngineIntegration/ECS/System/WorldAnchorSystem.hpp>
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
        // HUD の固定のパネル（財布・リール・ゲージ・帯・閃光など）。部品の親子は
        // TransformComponent.parent の "#名前" で結ぶので、1つのグループにまとめて生成する
        //-------------------------------------------------------------
        constexpr const char* kHudLayoutPath = "StardustFever/Assets/Prefabs/Hud/Layout/Group.json";

        //-------------------------------------------------------------
        // System の実行順。値の小さい順に Update される。
        // イベントは受け取る側の Update でまとめて処理するので、
        // 出す側を先に並べると同じフレームのうちに反映される
        //-------------------------------------------------------------
        enum class SystemOrder : int {
            ConfigReload = 0,    // 設定を読み直してから、そのフレームの処理を始める
            BoardBuild,          // 発射台（自動発射の ON/OFF を持つ）もここで作られるので、セーブより先に組み立てる
            Save,                // 最初のフレームでセーブを読み、他の System が見る前に進行を戻す
            StarMapInput,        // 星図の画面の開け閉め。開いている間は、ここより後の遊びの System が止まる（GamePause.hpp）
            LauncherInput,
            BigBang,
            UpgradeShop,
            UpgradeEffect,       // 購入 → 効果。効果を使う System（発射・払い出し・抽選など）より前に計算する
            CollectionEffect,    // 集めた星座 → 効果。同じく、効果を使う System より前に計算する
            Launch,
            Physics,
            BallEvent,          // 衝突 → 釘・入賞・アウト・星
            Lottery,            // 入賞 → 抽選結果
            Fever,              // 抽選結果 → フィーバー（星屑の稼ぎ時）
            Tulip,              // フィーバーの状態 → チューリップの羽を開け閉めする
            StarChance,         // 星の門への入賞 → 星ゲットチャンス（星釘を並べる・片付ける）
            StarLink,           // 星に触れた → つないだ・全部つないだ
            StarDraw,           // チャンスの結果 → 星を引く・星座がそろう
            ScoreMultiplier,    // 星・星座の効果・宇宙定数 → 全体倍率（払い出しより前に計算する）
            Payout,             // 全ての獲得 → 財布
            SparkleTrail,       // ここから4つは盤面の見た目（玉の尾・釘の瞬き・星釘の光輪・流れ星）
            Twinkle,
            GlowHalo,
            ShootingStar,
            Hud,         // 文字の中身
            ShopView,    // ここから下は HUD の見た目と動き（ゲームの状態を読むだけ）
            StarChartView,
            PocketLabel,    // 入賞口の名札
            StarMapView,    // 88星座の星図の画面（HUD より上に重ねる）
            LotteryView,
            HudGauge,
            RollingNumber,
            Toast,
            Fx,
            HudReaction,
            HudIdleMotion,
            UITween,        // トゥイーン → UIMotion。Transform が行列を作る前に済ませる
            WorldAnchor,    // ワールドに付いていく文字の位置（浮かぶ数字）
            BurstParticle,
            Lifetime,
            Transform,
            Light,
            Camera,
            AmbientParticle,
            Bloom,
            Font,
            Sprite,
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
        m_saveSystem = std::make_shared<ECS::SaveSystem>(eventBus);
        m_scene.AddSystem(m_saveSystem, Order(SystemOrder::Save));
        m_scene.AddSystem(std::make_shared<ECS::BoardBuildSystem>(), Order(SystemOrder::BoardBuild));
        m_scene.AddSystem(std::make_shared<ECS::StarMapInputSystem>(), Order(SystemOrder::StarMapInput));
        m_scene.AddSystem(std::make_shared<ECS::LauncherInputSystem>(eventBus), Order(SystemOrder::LauncherInput));
        m_scene.AddSystem(std::make_shared<ECS::BigBangSystem>(eventBus), Order(SystemOrder::BigBang));
        m_scene.AddSystem(std::make_shared<ECS::UpgradeShopSystem>(eventBus), Order(SystemOrder::UpgradeShop));
        m_scene.AddSystem(std::make_shared<ECS::UpgradeEffectSystem>(), Order(SystemOrder::UpgradeEffect));
        m_scene.AddSystem(std::make_shared<ECS::CollectionEffectSystem>(), Order(SystemOrder::CollectionEffect));
        m_scene.AddSystem(std::make_shared<ECS::LaunchSystem>(), Order(SystemOrder::Launch));
        m_scene.AddSystem(std::make_shared<ECS::PhysicsStepSystem>(eventBus), Order(SystemOrder::Physics));
        m_scene.AddSystem(std::make_shared<ECS::BallEventSystem>(eventBus), Order(SystemOrder::BallEvent));
        m_scene.AddSystem(std::make_shared<ECS::LotterySystem>(eventBus), Order(SystemOrder::Lottery));
        m_scene.AddSystem(std::make_shared<ECS::FeverSystem>(eventBus), Order(SystemOrder::Fever));
        m_scene.AddSystem(std::make_shared<ECS::TulipSystem>(), Order(SystemOrder::Tulip));
        m_scene.AddSystem(std::make_shared<ECS::StarChanceSystem>(eventBus), Order(SystemOrder::StarChance));
        m_scene.AddSystem(std::make_shared<ECS::StarLinkSystem>(eventBus), Order(SystemOrder::StarLink));
        m_scene.AddSystem(std::make_shared<ECS::StarDrawSystem>(eventBus), Order(SystemOrder::StarDraw));
        m_scene.AddSystem(std::make_shared<ECS::ScoreMultiplierSystem>(), Order(SystemOrder::ScoreMultiplier));
        m_scene.AddSystem(std::make_shared<ECS::PayoutSystem>(eventBus), Order(SystemOrder::Payout));
        m_scene.AddSystem(std::make_shared<ECS::SparkleTrailSystem>(m_scene), Order(SystemOrder::SparkleTrail));
        m_scene.AddSystem(std::make_shared<ECS::TwinkleSystem>(m_scene, eventBus), Order(SystemOrder::Twinkle));
        m_scene.AddSystem(std::make_shared<ECS::GlowHaloSystem>(m_scene), Order(SystemOrder::GlowHalo));
        m_scene.AddSystem(std::make_shared<ECS::ShootingStarSystem>(m_scene), Order(SystemOrder::ShootingStar));
        m_scene.AddSystem(std::make_shared<ECS::HudSystem>(), Order(SystemOrder::Hud));
        m_scene.AddSystem(std::make_shared<ECS::ShopViewSystem>(eventBus), Order(SystemOrder::ShopView));
        m_scene.AddSystem(std::make_shared<ECS::StarChartViewSystem>(), Order(SystemOrder::StarChartView));
        m_scene.AddSystem(std::make_shared<ECS::PocketLabelSystem>(m_scene), Order(SystemOrder::PocketLabel));
        m_scene.AddSystem(std::make_shared<ECS::StarMapViewSystem>(m_scene), Order(SystemOrder::StarMapView));
        m_scene.AddSystem(std::make_shared<ECS::LotteryViewSystem>(eventBus), Order(SystemOrder::LotteryView));
        m_scene.AddSystem(std::make_shared<ECS::HudGaugeSystem>(), Order(SystemOrder::HudGauge));
        m_scene.AddSystem(std::make_shared<ECS::RollingNumberSystem>(), Order(SystemOrder::RollingNumber));
        m_scene.AddSystem(std::make_shared<ECS::ToastSystem>(eventBus), Order(SystemOrder::Toast));
        m_scene.AddSystem(std::make_shared<ECS::FxSystem>(m_scene, eventBus), Order(SystemOrder::Fx));
        m_scene.AddSystem(std::make_shared<ECS::HudReactionSystem>(eventBus), Order(SystemOrder::HudReaction));
        m_scene.AddSystem(std::make_shared<ECS::HudIdleMotionSystem>(), Order(SystemOrder::HudIdleMotion));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::UITweenSystem>(), Order(SystemOrder::UITween));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::WorldAnchorSystem>(), Order(SystemOrder::WorldAnchor));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::BurstParticleSystem>(), Order(SystemOrder::BurstParticle));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::LifetimeSystem>(), Order(SystemOrder::Lifetime));

        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::TransformSystem>(), Order(SystemOrder::Transform));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::LightSystem>(), Order(SystemOrder::Light));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::CameraSystem>(), Order(SystemOrder::Camera));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::AmbientParticleSystem>(), Order(SystemOrder::AmbientParticle));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::BloomSystem>(), Order(SystemOrder::Bloom));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::FontRendererSystem>(), Order(SystemOrder::Font));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::SpriteRenderSystem>(), Order(SystemOrder::Sprite));
        m_scene.AddSystem(std::make_shared<Tsukino::BuiltIn::ECS::ModelSystem>(), Order(SystemOrder::Model));

        //--------------------------------------------------------------
        // エンティティ
        //--------------------------------------------------------------
        const auto instance = factory.InstantiateGroup(kSceneGroupPath, registry);
        if(instance.empty()) {
            Tsukino::Core::Log::Error(std::string("Scene group could not be instantiated: ") + kSceneGroupPath);
            return;
        }

        if(factory.InstantiateGroup(kHudLayoutPath, registry).empty())
            Tsukino::Core::Log::Error(std::string("HUD layout could not be instantiated: ") + kHudLayoutPath);

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
        // 自動保存の間隔のあいだに稼いだ分を失わないよう、終了の直前に保存する（エンティティはまだ残っている）
        if(m_saveSystem)
            m_saveSystem->SaveNow(m_scene.GetRegistry());
    }

}    // namespace StardustFever::Scene
