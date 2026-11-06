//-------------------------------------------------------------
//! @file   GamePause.hpp
//! @brief  遊び（物理・発射・抽選・時間）を止めているかを調べる
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/SettingsStateComponent.hpp>
#include <StardustFever/ECS/Component/StarMapStateComponent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    //! @brief  遊びを止めているか
    //! @param  registry [in] レジストリ
    //! @return 星図か設定の画面を開いていれば true
    //! @note   止めている間は、物理・発射・抽選・フィーバーと星ゲットチャンスの残り時間が進まない
    //-------------------------------------------------------------
    inline bool IsPlayPaused(Tsukino::ECS::Registry& registry) {
        const StarMapStateComponent*  map      = FindSingleton<StarMapStateComponent>(registry);
        const SettingsStateComponent* settings = FindSingleton<SettingsStateComponent>(registry);
        return (map && map->open) || (settings && settings->open);
    }

}    // namespace StardustFever::ECS
