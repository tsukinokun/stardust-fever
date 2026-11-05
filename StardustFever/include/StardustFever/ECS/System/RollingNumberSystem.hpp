//-------------------------------------------------------------
//! @file   RollingNumberSystem.hpp
//! @brief  星屑の数字を転がるように増やす System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  RollingNumberSystem
    //! @brief  星屑の数字を転がるように増やす System
    //! @note   表示中の値を、本当の値へ桁（対数）の上で追いかけさせる。
    //!         大きく増えたら弾ませ、減ったら（購入）一瞬赤く沈める
    //-------------------------------------------------------------
    class RollingNumberSystem final : public Tsukino::ECS::ISystem {
    public:
        RollingNumberSystem();

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
