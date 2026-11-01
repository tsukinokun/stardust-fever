//-------------------------------------------------------------
//! @file   UpgradeEffectSystem.hpp
//! @brief  強化のレベルから効果を計算する System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  UpgradeEffectSystem
    //! @brief  UpgradeLevelsComponent と Upgrades.json から UpgradeModifiersComponent を計算し直す
    //! @note   毎フレーム計算し直すのは、Upgrades.json をホットリロードしたときに、
    //!         買い直さなくてもすぐ効果へ反映させるため。強化は数個なので負荷は無視できる。
    //!         自動発射の解禁（AutoFireComponent::unlocked）もここで決める
    //-------------------------------------------------------------
    class UpgradeEffectSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  効果を計算し直す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;
    };
}    // namespace StardustFever::ECS
