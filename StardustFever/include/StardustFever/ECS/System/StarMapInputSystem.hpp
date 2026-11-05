//-------------------------------------------------------------
//! @file   StarMapInputSystem.hpp
//! @brief  88星座の星図の画面を開け閉めし、動かし・拡大し・星座を選ぶ System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarMapInputSystem
    //! @brief  Tab で開け閉めし（Esc でも閉じる）、開いている間の操作を StarMapStateComponent に書く
    //!         - ドラッグか矢印キーで動かす
    //!         - ホイールか Q / E で拡大・縮小する
    //!         - クリックで星座を選ぶ。Z / X で前後の星座へ移り、その星座を真ん中に出す
    //! @note   見た目は StarMapViewSystem が作る。開いている間は遊びを止める（GamePause.hpp）
    //-------------------------------------------------------------
    class StarMapInputSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  入力を読んで星図の状態を書き換える
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;
    };
}    // namespace StardustFever::ECS
