//-------------------------------------------------------------
//! @file   HudSystem.hpp
//! @brief  ゲームの状態をHUDの文字へ書き出す System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  HudSystem
    //! @brief  HudTextComponent の種類ごとに、表示する文字を組み立てて FontComponent へ入れる
    //! @note   文字の中身だけを決める。並べ方は Prefab、動きと色の変化は各 View・演出の System が受け持つ
    //-------------------------------------------------------------
    class HudSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  HUDの文字を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;
    };
}    // namespace StardustFever::ECS
