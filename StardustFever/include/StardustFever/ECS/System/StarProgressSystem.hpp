//-------------------------------------------------------------
//! @file   StarProgressSystem.hpp
//! @brief  星のレベルと星座の完成回数を記録する System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/ConstellationCompletedEvent.hpp>
#include <StardustFever/ECS/Event/StarLinkedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarProgressSystem
    //! @brief  つないだ星のレベルを上げ、完成した星座を数え、完成ボーナスを出す
    //! @note   全体倍率の計算は ScoreMultiplierSystem が行う（記録と計算を分けてある）
    //-------------------------------------------------------------
    class StarProgressSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] つないだ・完成したの購読元であり、獲得とメッセージの発行先
        //-------------------------------------------------------------
        explicit StarProgressSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  記録を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus*                 m_eventBus = nullptr;
        EventQueue<StarLinkedEvent>             m_linkedStars;
        EventQueue<ConstellationCompletedEvent> m_completions;
    };
}    // namespace StardustFever::ECS
