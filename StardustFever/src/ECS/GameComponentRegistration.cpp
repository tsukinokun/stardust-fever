//-------------------------------------------------------------
//! @file   GameComponentRegistration.cpp
//! @brief  ゲームのコンポーネントの登録
//-------------------------------------------------------------
#include <StardustFever/ECS/GameComponentRegistration.hpp>

#include <StardustFever/ECS/System/ConfigReloadSystem.hpp>

// 値を JSON から読まない（付けるだけの）コンポーネント
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LinkLineComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PinComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/StarMapStateComponent.hpp>
#include <StardustFever/ECS/Component/ShopCardComponent.hpp>
#include <StardustFever/ECS/Component/StarChartEntryComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Component/ToastComponent.hpp>
#include <StardustFever/ECS/Component/AudioSettingsComponent.hpp>
#include <StardustFever/ECS/Component/KeyHintComponent.hpp>
#include <StardustFever/ECS/Component/SettingsStateComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>

// 値を JSON から読むコンポーネント（シリアライズ定義が見えていないと、値を読まずに付けるだけになる）
#include <StardustFever/ECS/Serialization/AudioConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/AutoFireComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/BoardBuildRequestComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/CosmicFxConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/DrainComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/FeverConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/GlowHaloComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudFxConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudGaugeComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudGaugeMarkerComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudIdleMotionComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudLayoutConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudPartComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudPendingDotComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudTextComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/LauncherComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/LauncherConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/LotteryConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PayoutConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PhysicsConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PocketComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PocketLabelComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PrestigeConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/SkyObjectConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/RimGlowComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/RollingNumberComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/SaveConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/SettingsConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/StarGetFxConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/SparkleTrailComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/StarChanceConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/StarLinkConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/StarMapConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/TulipWingComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/TwinkleComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/WalletComponentSerialization.hpp>

#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    // Prefab で使えるように登録する
    //-------------------------------------------------------------
    void RegisterGameComponents(Tsukino::Engine::ECS::Prefab::PrefabFactory& factory) {
        // 盤面の物
        factory.RegisterComponent<BallComponent>("BallComponent");
        factory.RegisterComponent<PinComponent>("PinComponent");
        factory.RegisterComponent<PocketComponent>("PocketComponent");
        factory.RegisterComponent<PocketLabelComponent>("PocketLabelComponent");
        factory.RegisterComponent<TulipWingComponent>("TulipWingComponent");
        factory.RegisterComponent<DrainComponent>("DrainComponent");
        factory.RegisterComponent<StarPinComponent>("StarPinComponent");
        factory.RegisterComponent<LinkLineComponent>("LinkLineComponent");
        factory.RegisterComponent<LauncherComponent>("LauncherComponent");
        factory.RegisterComponent<AutoFireComponent>("AutoFireComponent");
        factory.RegisterComponent<BoardBuildRequestComponent>("BoardBuildRequestComponent");
        // HUD
        factory.RegisterComponent<HudTextComponent>("HudTextComponent");
        factory.RegisterComponent<HudPartComponent>("HudPartComponent");
        factory.RegisterComponent<HudGaugeComponent>("HudGaugeComponent");
        factory.RegisterComponent<HudGaugeMarkerComponent>("HudGaugeMarkerComponent");
        factory.RegisterComponent<HudIdleMotionComponent>("HudIdleMotionComponent");
        factory.RegisterComponent<HudPendingDotComponent>("HudPendingDotComponent");
        factory.RegisterComponent<RollingNumberComponent>("RollingNumberComponent");
        factory.RegisterComponent<ShopCardComponent>("ShopCardComponent");
        factory.RegisterComponent<StarChartEntryComponent>("StarChartEntryComponent");
        factory.RegisterComponent<ToastComponent>("ToastComponent");

        // ゲームの状態
        factory.RegisterComponent<WalletComponent>("WalletComponent");
        factory.RegisterComponent<ScoreMultiplierComponent>("ScoreMultiplierComponent");
        factory.RegisterComponent<StarCollectionComponent>("StarCollectionComponent");
        factory.RegisterComponent<LotteryStateComponent>("LotteryStateComponent");
        factory.RegisterComponent<FeverStateComponent>("FeverStateComponent");
        factory.RegisterComponent<StarChanceStateComponent>("StarChanceStateComponent");
        factory.RegisterComponent<CollectionModifiersComponent>("CollectionModifiersComponent");
        factory.RegisterComponent<StarMapStateComponent>("StarMapStateComponent");
        factory.RegisterComponent<UpgradeLevelsComponent>("UpgradeLevelsComponent");
        factory.RegisterComponent<UpgradeModifiersComponent>("UpgradeModifiersComponent");
        factory.RegisterComponent<PrestigeStateComponent>("PrestigeStateComponent");

        // 盤面の見た目（RimGlowComponent はエンジンの型だが、Prefab への登録はゲーム側で行う）
        factory.RegisterComponent<SparkleTrailComponent>("SparkleTrailComponent");
        factory.RegisterComponent<TwinkleComponent>("TwinkleComponent");
        factory.RegisterComponent<GlowHaloComponent>("GlowHaloComponent");
        factory.RegisterComponent<Tsukino::BuiltIn::ECS::RimGlowComponent>("RimGlowComponent");

        // 設定
        factory.RegisterComponent<AudioSettingsComponent>("AudioSettingsComponent");
        factory.RegisterComponent<SettingsStateComponent>("SettingsStateComponent");
        factory.RegisterComponent<KeyHintComponent>("KeyHintComponent");

        factory.RegisterComponent<LauncherConfigComponent>("LauncherConfigComponent");
        factory.RegisterComponent<LotteryConfigComponent>("LotteryConfigComponent");
        factory.RegisterComponent<FeverConfigComponent>("FeverConfigComponent");
        factory.RegisterComponent<StarLinkConfigComponent>("StarLinkConfigComponent");
        factory.RegisterComponent<StarChanceConfigComponent>("StarChanceConfigComponent");
        factory.RegisterComponent<StarMapConfigComponent>("StarMapConfigComponent");
        factory.RegisterComponent<PayoutConfigComponent>("PayoutConfigComponent");
        factory.RegisterComponent<HudFxConfigComponent>("HudFxConfigComponent");
        factory.RegisterComponent<HudLayoutConfigComponent>("HudLayoutConfigComponent");
        factory.RegisterComponent<PhysicsConfigComponent>("PhysicsConfigComponent");
        factory.RegisterComponent<PrestigeConfigComponent>("PrestigeConfigComponent");
        factory.RegisterComponent<SkyObjectConfigComponent>("SkyObjectConfigComponent");
        factory.RegisterComponent<SaveConfigComponent>("SaveConfigComponent");
        factory.RegisterComponent<CosmicFxConfigComponent>("CosmicFxConfigComponent");
        factory.RegisterComponent<AudioConfigComponent>("AudioConfigComponent");
        factory.RegisterComponent<SettingsConfigComponent>("SettingsConfigComponent");
        factory.RegisterComponent<StarGetFxConfigComponent>("StarGetFxConfigComponent");
    }

    //-------------------------------------------------------------
    // 実行中に読み直す設定の型を登録する
    //-------------------------------------------------------------
    void RegisterConfigReloadTypes(ConfigReloadSystem& reloadSystem) {
        reloadSystem.RegisterType<LauncherConfigComponent>("LauncherConfigComponent");
        reloadSystem.RegisterType<LotteryConfigComponent>("LotteryConfigComponent");
        reloadSystem.RegisterType<FeverConfigComponent>("FeverConfigComponent");
        reloadSystem.RegisterType<StarLinkConfigComponent>("StarLinkConfigComponent");
        reloadSystem.RegisterType<StarChanceConfigComponent>("StarChanceConfigComponent");
        reloadSystem.RegisterType<StarMapConfigComponent>("StarMapConfigComponent");
        reloadSystem.RegisterType<PayoutConfigComponent>("PayoutConfigComponent");
        reloadSystem.RegisterType<HudFxConfigComponent>("HudFxConfigComponent");
        reloadSystem.RegisterType<HudLayoutConfigComponent>("HudLayoutConfigComponent");
        reloadSystem.RegisterType<PhysicsConfigComponent>("PhysicsConfigComponent");
        reloadSystem.RegisterType<PrestigeConfigComponent>("PrestigeConfigComponent");
        reloadSystem.RegisterType<SkyObjectConfigComponent>("SkyObjectConfigComponent");
        reloadSystem.RegisterType<SaveConfigComponent>("SaveConfigComponent");
        reloadSystem.RegisterType<CosmicFxConfigComponent>("CosmicFxConfigComponent");
        reloadSystem.RegisterType<AudioConfigComponent>("AudioConfigComponent");
        reloadSystem.RegisterType<SettingsConfigComponent>("SettingsConfigComponent");
        reloadSystem.RegisterType<StarGetFxConfigComponent>("StarGetFxConfigComponent");
    }

}    // namespace StardustFever::ECS
