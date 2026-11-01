//-------------------------------------------------------------
//! @file   EngineAccess.hpp
//! @brief  System からエンジンの機能（入力・アセット・Prefab）を取るための入口
//-------------------------------------------------------------
#pragma once
#include <Tsukino/EngineIntegration/EngineContext.hpp>
#include <Tsukino/Core/ECS/Registry/Registry.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    //! @brief  エンジンのコンテキストを取得する
    //! @param  registry [in] レジストリ
    //! @return コンテキスト（シーンの初期化前なら nullptr）
    //! @note   GameSceneBase がシーンの初期化時にレジストリのコンテキストへ置いている
    //-------------------------------------------------------------
    inline Tsukino::EngineIntegration::EngineContext* GetEngineContext(Tsukino::ECS::Registry& registry) {
        if(!registry.HasContext<Tsukino::EngineIntegration::EngineContext*>())
            return nullptr;
        return registry.GetContext<Tsukino::EngineIntegration::EngineContext*>();
    }

}    // namespace StardustFever::ECS
