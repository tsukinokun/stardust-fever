//-------------------------------------------------------------
//! @file   GameComponentRegistration.hpp
//! @brief  ゲームのコンポーネントを Prefab とホットリロードへ登録する
//-------------------------------------------------------------
#pragma once

namespace Tsukino::Engine::ECS::Prefab {
    class PrefabFactory;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    class ConfigReloadSystem;

    //-------------------------------------------------------------
    //! @brief  ゲームのコンポーネントを Prefab で使えるように登録する
    //! @param  factory [in] 登録先
    //! @note   ここに無い型を Prefab に書くと「Unknown component type written in Prefab」の警告が出て、
    //!         そのコンポーネントを付けないまま生成が続く。型を足したら必ずここにも足すこと
    //-------------------------------------------------------------
    void RegisterGameComponents(Tsukino::Engine::ECS::Prefab::PrefabFactory& factory);

    //-------------------------------------------------------------
    //! @brief  実行中に読み直す設定の型を登録する
    //! @param  reloadSystem [in] 登録先
    //-------------------------------------------------------------
    void RegisterConfigReloadTypes(ConfigReloadSystem& reloadSystem);

}    // namespace StardustFever::ECS
