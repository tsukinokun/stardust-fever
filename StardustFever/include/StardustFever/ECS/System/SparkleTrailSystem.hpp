//-------------------------------------------------------------
//! @file   SparkleTrailSystem.hpp
//! @brief  動いている玉に星屑の尾を引かせる System
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
    //! @class  SparkleTrailSystem
    //! @brief  SparkleTrailComponent を持つ実体が動いている間、その位置に粒子を少しずつ撒く
    //! @note   速さは前のフレームとの位置の差から測る（物理の速度を読まずに済み、動かし方を問わない）。
    //!         フィーバー中は粒子を金色に替え、盤面全体の雰囲気が変わったことを見せる
    //-------------------------------------------------------------
    class SparkleTrailSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 粒子の生成先（この System より長生きすること）
        //-------------------------------------------------------------
        explicit SparkleTrailSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  粒子を撒く
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::Scene* m_scene = nullptr;
        std::mt19937         m_random;
    };
}    // namespace StardustFever::ECS
