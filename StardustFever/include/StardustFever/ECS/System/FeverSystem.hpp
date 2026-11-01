//-------------------------------------------------------------
//! @file   FeverSystem.hpp
//! @brief  フィーバー「星詠みタイム」の開始と終了を管理する System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/LotteryResultEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  FeverSystem
    //! @brief  抽選に当たったら星座を1つ選び、その形に星釘を並べる。
    //!         時間切れか、星座が完成して余韻が過ぎたら星釘と線を片付ける
    //! @note   星をつなぐ処理は StarLinkSystem が行う。ここは出し入れだけ
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
        //! @brief  星座を選んで星釘を並べる
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void StartFever(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  星釘と線を片付ける
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void EndFever(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::EventBus*        m_eventBus = nullptr;
        EventQueue<LotteryResultEvent> m_results;
        std::mt19937                   m_random;
    };
}    // namespace StardustFever::ECS
