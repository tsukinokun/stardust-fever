//-------------------------------------------------------------
//! @file   UiButtonSystem.hpp
//! @brief  押せる部品の入力をイベントにする System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  UiButtonSystem
    //! @brief  押された部品を UiActionEvent にし、マウスが乗った部品の色を変える
    //! @note   受け取る System はキー入力と同じ処理へつなぐ。マウスとキーで動きを分けないため
    //-------------------------------------------------------------
    class UiButtonSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 押された知らせの発行先
        //-------------------------------------------------------------
        explicit UiButtonSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  押された部品を知らせ、色を合わせる
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus* m_eventBus = nullptr;
    };
}    // namespace StardustFever::ECS
