//-------------------------------------------------------------
//! @file   UpgradeSystem.hpp
//! @brief  星屑を払って台を強化する System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

namespace Tsukino::ECS {
    class EventBus;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  UpgradeSystem
    //! @brief  U キーで自動発射を解禁する（縦切り用の仮の強化）
    //! @note   強化ツリーを作るときは、強化の種類と値段をデータへ出してここを広げる
    //-------------------------------------------------------------
    class UpgradeSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] メッセージの発行先
        //-------------------------------------------------------------
        explicit UpgradeSystem(Tsukino::ECS::EventBus& eventBus);

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
