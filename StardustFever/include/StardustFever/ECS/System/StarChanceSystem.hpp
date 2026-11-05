//-------------------------------------------------------------
//! @file   StarChanceSystem.hpp
//! @brief  星ゲットチャンス（星座をつなぐミニゲーム）を始め・進め・終える System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BallEnteredPocketEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarChanceSystem
    //! @brief  星の門に玉が入ったら抽選し、当たれば88星座から1つを選んで盤面に星釘を並べる。
    //!         時間切れか、全部つないで余韻が過ぎたら片付けて、結果を StarChanceEndedEvent で知らせる
    //! @note   星をつなぐのは StarLinkSystem、星を引くのは StarDrawSystem が受け持つ。
    //!         星釘と重なる普通の釘は、チャンスの間だけ隠して当たり判定を外す
    //-------------------------------------------------------------
    class StarChanceSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 入賞の購読元であり、始まった・終わったの発行先
        //-------------------------------------------------------------
        explicit StarChanceSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  抽選し、残り時間を進め、終わったら片付ける
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  星座を選んで星釘を並べる
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void StartChance(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  星釘と線を片付け、隠した釘を戻して、結果を知らせる
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void EndChance(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::EventBus*            m_eventBus = nullptr;
        EventQueue<BallEnteredPocketEvent> m_pocketEntries;
        std::mt19937                       m_random;
    };
}    // namespace StardustFever::ECS
