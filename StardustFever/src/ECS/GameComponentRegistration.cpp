//-------------------------------------------------------------
//! @file   GameComponentRegistration.cpp
//! @brief  ゲームのコンポーネントの登録
//-------------------------------------------------------------
#include <StardustFever/ECS/GameComponentRegistration.hpp>

#include <StardustFever/ECS/System/ConfigReloadSystem.hpp>

// 値を JSON から読まない（付けるだけの）コンポーネント
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/HudStateComponent.hpp>
#include <StardustFever/ECS/Component/LinkLineComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PinComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>

// 値を JSON から読むコンポーネント（シリアライズ定義が見えていないと、値を読まずに付けるだけになる）
#include <StardustFever/ECS/Serialization/AutoFireComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/BoardBuildRequestComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/DrainComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/FeverConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/HudTextComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/LauncherComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/LauncherConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/LotteryConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PayoutConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PhysicsConfigComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/PocketComponentSerialization.hpp>
#include <StardustFever/ECS/Serialization/StarLinkConfigComponentSerialization.hpp>
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
        factory.RegisterComponent<DrainComponent>("DrainComponent");
        factory.RegisterComponent<StarPinComponent>("StarPinComponent");
        factory.RegisterComponent<LinkLineComponent>("LinkLineComponent");
        factory.RegisterComponent<LauncherComponent>("LauncherComponent");
        factory.RegisterComponent<AutoFireComponent>("AutoFireComponent");
        factory.RegisterComponent<BoardBuildRequestComponent>("BoardBuildRequestComponent");
        factory.RegisterComponent<HudTextComponent>("HudTextComponent");

        // ゲームの状態
        factory.RegisterComponent<WalletComponent>("WalletComponent");
        factory.RegisterComponent<ScoreMultiplierComponent>("ScoreMultiplierComponent");
        factory.RegisterComponent<StarCollectionComponent>("StarCollectionComponent");
        factory.RegisterComponent<LotteryStateComponent>("LotteryStateComponent");
        factory.RegisterComponent<FeverStateComponent>("FeverStateComponent");
        factory.RegisterComponent<HudStateComponent>("HudStateComponent");
        factory.RegisterComponent<UpgradeLevelsComponent>("UpgradeLevelsComponent");
        factory.RegisterComponent<UpgradeModifiersComponent>("UpgradeModifiersComponent");

        // 設定
        factory.RegisterComponent<LauncherConfigComponent>("LauncherConfigComponent");
        factory.RegisterComponent<LotteryConfigComponent>("LotteryConfigComponent");
        factory.RegisterComponent<FeverConfigComponent>("FeverConfigComponent");
        factory.RegisterComponent<StarLinkConfigComponent>("StarLinkConfigComponent");
        factory.RegisterComponent<PayoutConfigComponent>("PayoutConfigComponent");
        factory.RegisterComponent<HudConfigComponent>("HudConfigComponent");
        factory.RegisterComponent<PhysicsConfigComponent>("PhysicsConfigComponent");
    }

    //-------------------------------------------------------------
    // 実行中に読み直す設定の型を登録する
    //-------------------------------------------------------------
    void RegisterConfigReloadTypes(ConfigReloadSystem& reloadSystem) {
        reloadSystem.RegisterType<LauncherConfigComponent>("LauncherConfigComponent");
        reloadSystem.RegisterType<LotteryConfigComponent>("LotteryConfigComponent");
        reloadSystem.RegisterType<FeverConfigComponent>("FeverConfigComponent");
        reloadSystem.RegisterType<StarLinkConfigComponent>("StarLinkConfigComponent");
        reloadSystem.RegisterType<PayoutConfigComponent>("PayoutConfigComponent");
        reloadSystem.RegisterType<HudConfigComponent>("HudConfigComponent");
        reloadSystem.RegisterType<PhysicsConfigComponent>("PhysicsConfigComponent");
    }

}    // namespace StardustFever::ECS
