//-------------------------------------------------------------
//! @file   StarGetFxSystem.hpp
//! @brief  星を手に入れた演出を行う System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Data/StarData.hpp>
#include <StardustFever/ECS/Event/StarDrawnEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>

#include <deque>
#include <random>
#include <vector>

// 名前空間 : Tsukino::ECS
namespace Tsukino::ECS {
    class Scene;
}    // namespace Tsukino::ECS

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarGetFxSystem
    //! @brief  星を画面の中央に大きく見せ、星図のボタンへ吸い込ませる
    //! @note   遊びは止めない。星図や設定を開いたら演出は飛ばし、通知だけ残す。
    //!         星屑に換わった星は星屑のカードへ飛ばす
    //-------------------------------------------------------------
    class StarGetFxSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene    [in] 部品を作るシーン
        //! @param  eventBus [in] 星の知らせの購読元であり、節目の発行先
        //-------------------------------------------------------------
        StarGetFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  演出を進める
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //! 飛び散る星屑
        struct Spark {
            entt::entity entity = entt::null;
            float        angle  = 0.0f;    //!< 飛ぶ向き（ラジアン）
            float        reach  = 1.0f;    //!< 飛ぶ距離の倍率
        };

        //-------------------------------------------------------------
        //! @brief  次の星の演出を始める
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Begin(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  星座の形の中で、手に入れた星の位置を見せるカードを作る
        //! @param  registry [in] レジストリ
        //! @param  star     [in] 手に入れた星
        //-------------------------------------------------------------
        void BuildMapCard(Tsukino::ECS::Registry& registry, const Data::StarDef& star);

        //-------------------------------------------------------------
        //! @brief  今の時刻に合わせて部品を動かす
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Animate(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  星が届いた処理（通知を増やし、ボタンを弾ませ、大見出しを出す）
        //! @param  registry [in] レジストリ
        //! @param  drawn    [in] 届いた星
        //! @param  animated [in] 演出を最後まで見せたか
        //-------------------------------------------------------------
        void Arrive(Tsukino::ECS::Registry& registry, const StarDrawnEvent& drawn, bool animated);

        //-------------------------------------------------------------
        //! @brief  部品を片付ける
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Clear(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene*       m_scene    = nullptr;
        Tsukino::ECS::EventBus*    m_eventBus = nullptr;
        EventQueue<StarDrawnEvent> m_draws;
        std::deque<StarDrawnEvent> m_queue;              //!< 待っている星
        StarDrawnEvent             m_current;            //!< 演出している星
        bool                       m_active  = false;    //!< 演出しているか
        bool                       m_flying  = false;    //!< 飛び始めたか
        float                      m_time    = 0.0f;     //!< 演出を始めてからの時間（秒）
        float                      m_power   = 0.0f;     //!< 星の明るさ（0〜1）。明るい星ほど派手にする
        float                      m_targetX = 0.0f;     //!< 飛ぶ先
        float                      m_targetY = 0.0f;     //!< 飛ぶ先
        hlslpp::float4             m_color;              //!< 星の色
        entt::entity               m_root      = entt::null;
        entt::entity               m_flash     = entt::null;
        entt::entity               m_flare     = entt::null;
        entt::entity               m_glow      = entt::null;
        entt::entity               m_star      = entt::null;
        entt::entity               m_text      = entt::null;
        entt::entity               m_map       = entt::null;    //!< 星座の形のカード
        entt::entity               m_mapTarget = entt::null;    //!< カードの中の、手に入れた星の光
        std::vector<Spark>         m_sparks;
        std::vector<entt::entity>  m_trail;
        std::mt19937               m_random;
    };
}    // namespace StardustFever::ECS
