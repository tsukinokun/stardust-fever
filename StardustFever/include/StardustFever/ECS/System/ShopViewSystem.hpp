//-------------------------------------------------------------
//! @file   ShopViewSystem.hpp
//! @brief  強化ショップのカードを並べて動かす System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/UpgradePurchasedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  ShopViewSystem
    //! @brief  Upgrades.json の強化1つにカード1枚を作り、上から順に少しずつ遅れて滑り込ませる。
    //!         買える・買えない・MAX でカードの明るさと縁を変え、買った瞬間にそのカードを弾ませる
    //! @note   強化の数が変わったら（データのホットリロード）作り直す。
    //!         カードの形と色は雛形のグループ JSON と HudLayoutConfigComponent にある
    //-------------------------------------------------------------
    class ShopViewSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 購入イベントの購読元
        //-------------------------------------------------------------
        explicit ShopViewSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  カードを作り、見た目を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  カードを全て作り直す
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Rebuild(Tsukino::ECS::Registry& registry);

        EventQueue<UpgradePurchasedEvent> m_purchases;
        std::mt19937                      m_random;
    };
}    // namespace StardustFever::ECS
