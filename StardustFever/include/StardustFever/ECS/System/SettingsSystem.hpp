//-------------------------------------------------------------
//! @file   SettingsSystem.hpp
//! @brief  設定の画面の開け閉めと、操作の反映を行う System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  SettingsSystem
    //! @brief  Esc とボタンで設定の画面を開け閉めし、スライダーと切り替えの値を設定へ書く
    //! @note   星図の入力より先に動かす。星図を閉じた Esc で設定が開かないよう、星図の状態を先に見る
    //-------------------------------------------------------------
    class SettingsSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  入力を読んで設定を書き換える
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        int m_appliedKeyHints = -1;    //!< キー操作の表示へ最後に反映した値（-1 はまだ）
    };
}    // namespace StardustFever::ECS
