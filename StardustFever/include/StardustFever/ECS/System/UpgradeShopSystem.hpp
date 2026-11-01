//-------------------------------------------------------------
//! @file   UpgradeShopSystem.hpp
//! @brief  星屑を払って強化を買う System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

namespace Tsukino::ECS {
    class EventBus;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  UpgradeShopSystem
    //! @brief  数字キー 1〜9 で、Upgrades.json の並び順の強化を1レベル買う
    //! @note   買うのはレベルを上げるところまで。効果は UpgradeEffectSystem が計算する
    //-------------------------------------------------------------
    class UpgradeShopSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 購入イベントとメッセージの発行先
        //-------------------------------------------------------------
        explicit UpgradeShopSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  購入の入力を処理する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus* m_eventBus = nullptr;
    };
}    // namespace StardustFever::ECS
