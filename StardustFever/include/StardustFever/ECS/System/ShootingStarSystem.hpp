//-------------------------------------------------------------
//! @file   ShootingStarSystem.hpp
//! @brief  背景に時々流れ星を流す System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

namespace Tsukino::ECS {
    class Scene;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  ShootingStarSystem
    //! @brief  CosmicFxConfigComponent の間隔で、背景の範囲に流れ星を1つ流す。フィーバー中は間隔を縮めて流星群にする
    //! @note   流れ星はエンジンの BurstParticleComponent を持つスプライトとして作る。
    //!         動き・色の消え方・寿命での破棄は BurstParticleSystem に任せる
    //-------------------------------------------------------------
    class ShootingStarSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 流れ星の生成先（この System より長生きすること）
        //-------------------------------------------------------------
        explicit ShootingStarSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  間隔が来たら流れ星を流す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::Scene* m_scene = nullptr;
        float                m_timer = 1.5f;    //!< 次の流れ星までの残り（起動直後は少し待ってから流す）
        std::mt19937         m_random;
    };
}    // namespace StardustFever::ECS
