//-------------------------------------------------------------
//! @file   TulipSystem.hpp
//! @brief  チューリップの羽を開け閉めする System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  TulipSystem
    //! @brief  フィーバー中はチューリップの羽を開き、それ以外は閉じる。蝶番を中心に回るよう、羽の位置と向きを書く
    //! @note   羽は Kinematic の剛体なので、物理は Transform に付いてくる（エンジンの PhysicsSystem が動かす）
    //-------------------------------------------------------------
    class TulipSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  羽を動かす
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;
    };
}    // namespace StardustFever::ECS
