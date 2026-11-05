//-------------------------------------------------------------
//! @file   HudGaugeSystem.hpp
//! @brief  HUD のゲージを値に合わせて伸び縮みさせる System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  HudGaugeSystem
    //! @brief  HUD のゲージを値に合わせて伸び縮みさせる System
    //! @note   fillAmount を値へなめらかに寄せ、色を値の大きさで 低 → 中 → 高 と変える。
    //!         発射の強さのゲージは溜めている間だけ脈打ち、印を最後に打った強さの位置へ動かす
    //-------------------------------------------------------------
    class HudGaugeSystem final : public Tsukino::ECS::ISystem {
    public:
        HudGaugeSystem();

        //-------------------------------------------------------------
        //! @brief  更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        std::mt19937 m_random;    //!< 見た目のばらつき用
    };
}    // namespace StardustFever::ECS
