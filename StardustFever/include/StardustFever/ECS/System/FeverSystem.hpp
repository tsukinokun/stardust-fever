//-------------------------------------------------------------
//! @file   FeverSystem.hpp
//! @brief  フィーバー「星詠みタイム」の開始と終了を管理する System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/LotteryResultEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  FeverSystem
    //! @brief  抽選に当たったら、星屑の稼ぎ時のフィーバーを始める。
    //!         フィーバーの間は横の入賞口を広げ、時間切れで元に戻す
    //! @note   星屑の倍率は PayoutSystem がフィーバーの状態を見て掛ける。ここは出し入れだけ
    //-------------------------------------------------------------
    class FeverSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 抽選結果の購読元であり、開始・終了の発行先
        //-------------------------------------------------------------
        explicit FeverSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  フィーバーを始める・進める・終える
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  フィーバーを始め、横の入賞口を広げる
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void StartFever(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  フィーバーを終え、横の入賞口を戻す
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void EndFever(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::EventBus*        m_eventBus = nullptr;
        EventQueue<LotteryResultEvent> m_results;
    };
}    // namespace StardustFever::ECS
