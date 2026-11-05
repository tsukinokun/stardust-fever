//-------------------------------------------------------------
//! @file   GlowHaloSystem.hpp
//! @brief  星釘に光芒と点光源を重ねる System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>

#include <random>
#include <utility>
#include <vector>

namespace Tsukino::ECS {
    class Scene;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  GlowHaloSystem
    //! @brief  GlowHaloComponent を持つ実体に、光芒のビルボード（加算）と点光源の実体を1つ作って付いていかせる
    //! @note   光輪は持ち主とは別の実体にする。持ち主のモデルの大きさに引きずられず、光輪だけの大きさを決められるため。
    //!         ワールドのスプライトはカメラを向いて回らないので、動きは大きさの呼吸と色で付ける。
    //!         持ち主が消えたら（フィーバーの終わりに星釘が片付けられたら）光輪も消す
    //-------------------------------------------------------------
    class GlowHaloSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 光輪の生成先（この System より長生きすること）
        //-------------------------------------------------------------
        explicit GlowHaloSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  光輪を作り、動かし、片付ける
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::Scene*                               m_scene = nullptr;
        std::vector<std::pair<entt::entity, entt::entity>> m_halos;    //!< 持ち主と光輪の組（持ち主が消えたのを見つけるため）
        float                                              m_time = 0.0f;
        std::mt19937                                       m_random;
    };
}    // namespace StardustFever::ECS
