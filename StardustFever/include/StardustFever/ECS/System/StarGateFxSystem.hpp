//-------------------------------------------------------------
//! @file   StarGateFxSystem.hpp
//! @brief  星の門の抽選の演出を行う System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/StarGateRollEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>
#include <hlsl++.h>

#include <random>
#include <string>
#include <vector>

// 名前空間 : Tsukino::ECS
namespace Tsukino::ECS {
    class Scene;
}    // namespace Tsukino::ECS

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarGateFxSystem
    //! @brief  星座の線画が入れ替わって止まり、星が灯って当たり外れを見せる
    //! @note   結果と筋書きは StarChanceSystem が決める。ここは見せるだけ。色が金なら当たりやすい
    //-------------------------------------------------------------
    class StarGateFxSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene    [in] 部品を作るシーン
        //! @param  eventBus [in] 抽選の知らせの購読元であり、刻みの発行先
        //-------------------------------------------------------------
        StarGateFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  演出を進める
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //! 線画の線1本
        struct FigureLine {
            entt::entity   entity = entt::null;
            int            from   = 0;                             //!< つなぐ星の灯る順
            int            to     = 0;                             //!< 同上
            hlslpp::float2 base   = hlslpp::float2(0.0f, 0.0f);    //!< 元の位置
        };

        //! 線画の星1つ
        struct FigureDot {
            entt::entity   entity = entt::null;
            int            order  = 0;                             //!< 灯る順
            hlslpp::float2 base   = hlslpp::float2(0.0f, 0.0f);    //!< 元の位置
        };

        //! 金色の予告で舞う星屑1つ
        struct Dust {
            entt::entity   entity   = entt::null;
            hlslpp::float2 position = hlslpp::float2(0.0f, 0.0f);
            hlslpp::float2 velocity = hlslpp::float2(0.0f, 0.0f);
        };

        //-------------------------------------------------------------
        //! @brief  画面を作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  画面を片付ける
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Clear(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  線画を作り直す
        //! @param  registry [in] レジストリ
        //! @param  id       [in] 星座のID
        //-------------------------------------------------------------
        void ShowConstellation(Tsukino::ECS::Registry& registry, const std::string& id);

        //-------------------------------------------------------------
        //! @brief  線画の灯りを決める
        //! @param  registry [in] レジストリ
        //! @param  lit      [in] 灯っている星の数
        //! @param  color    [in] 灯りの色
        //-------------------------------------------------------------
        void ApplyLit(Tsukino::ECS::Registry& registry, int lit, const hlslpp::float4& color);

        //-------------------------------------------------------------
        //! @brief  線をほどいて散らす
        //! @param  registry [in] レジストリ
        //! @param  t        [in] 進み具合（0〜1）
        //-------------------------------------------------------------
        void Scatter(Tsukino::ECS::Registry& registry, float t);

        //-------------------------------------------------------------
        //! @brief  後ろの光と星屑を動かす
        //! @param  registry  [in] レジストリ
        //! @param  gold      [in] 金色の予告か
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void AnimateBackdrop(Tsukino::ECS::Registry& registry, bool gold, float deltaTime);

        Tsukino::ECS::Scene*          m_scene    = nullptr;
        Tsukino::ECS::EventBus*       m_eventBus = nullptr;
        EventQueue<StarGateRollEvent> m_events;
        std::mt19937                  m_random;
        entt::entity                  m_root    = entt::null;    //!< 画面全体
        entt::entity                  m_glow    = entt::null;    //!< 星座の後ろの光
        entt::entity                  m_nebula  = entt::null;    //!< 星座の後ろの雲
        entt::entity                  m_figure  = entt::null;    //!< 線画（入れ替わるたびに作り直す）
        entt::entity                  m_name    = entt::null;    //!< 星座の名前
        entt::entity                  m_caption = entt::null;    //!< 見出し
        std::vector<FigureLine>       m_lines;
        std::vector<FigureDot>        m_dots;
        std::vector<Dust>             m_dust;
        int                           m_step       = -1;      //!< 星座が入れ替わった回数
        int                           m_lit        = -1;      //!< 灯っている星の数
        int                           m_phase      = 0;       //!< 今の段階（Core::StarGateRoll::Phase）
        int                           m_result     = 0;       //!< 0: 抽選中、1: 当たり、2: 外れ
        float                         m_resultTime = 0.0f;    //!< 結果を見せている時間（秒）
        float                         m_time       = 0.0f;    //!< 始まってからの時間（秒）
    };
}    // namespace StardustFever::ECS
