//-------------------------------------------------------------
//! @file   ScoreMultiplierSystem.hpp
//! @brief  星屑の全体倍率を計算する System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  ScoreMultiplierSystem
    //! @brief  星のレベル・星座の完成回数・宇宙定数から ScoreMultiplierComponent を計算し直す
    //! @note   毎フレーム計算し直すのは、Stars.json / Constellations.json / PrestigeConfig.json を
    //!         ホットリロードしたときに、すぐ倍率へ反映させるため
    //-------------------------------------------------------------
    class ScoreMultiplierSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  全体倍率を計算し直す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;
    };
}    // namespace StardustFever::ECS
