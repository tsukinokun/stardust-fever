//-------------------------------------------------------------
//! @file   StarLinkSystem.hpp
//! @brief  玉が触れた星をつなぎ、星座の線を引く System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BallTouchedStarEvent.hpp>
#include <StardustFever/ECS/Event/StarChanceStartedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarLinkSystem
    //! @brief  星ゲットチャンスで、玉が触れた星釘をつなぐ。
    //!         チャンスの開始時に星座の線を薄いガイドとして全部引き、両端の星がつながった線を灯す。
    //!         全ての星がつながったらチャンスの状態に完成を書く
    //! @note   どの玉で触れても星はつながる。星屑は出さない（ご褒美は星を引くこと）
    //-------------------------------------------------------------
    class StarLinkSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 接触の購読元であり、つないだ・完成したの発行先
        //-------------------------------------------------------------
        explicit StarLinkSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  溜まった接触を処理する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  星座の線を全部ガイドとして引く
        //! @param  registry [in] レジストリ
        //! @param  started  [in] 始まったフィーバーの星座
        //-------------------------------------------------------------
        void SpawnGuideLines(Tsukino::ECS::Registry& registry, const StarChanceStartedEvent& started);

        //-------------------------------------------------------------
        //! @brief  星釘1つをつなぐ
        //! @param  registry [in] レジストリ
        //! @param  touch    [in] 触れた玉と星釘
        //-------------------------------------------------------------
        void LinkStar(Tsukino::ECS::Registry& registry, const BallTouchedStarEvent& touch);

        Tsukino::ECS::EventBus*            m_eventBus = nullptr;
        EventQueue<BallTouchedStarEvent>   m_touches;
        EventQueue<StarChanceStartedEvent> m_starts;
    };
}    // namespace StardustFever::ECS
