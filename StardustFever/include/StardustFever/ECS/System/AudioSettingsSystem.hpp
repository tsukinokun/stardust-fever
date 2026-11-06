//-------------------------------------------------------------
//! @file   AudioSettingsSystem.hpp
//! @brief  音量のキー操作と、バスの音量への反映を行う System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  AudioSettingsSystem
    //! @brief  M で消音、F6 / F7 で BGM、F8 / F9 で効果音の音量を変える
    //! @note   F5 はエンジンのデバッグ表示が使っている。星図を開いている間も効く
    //-------------------------------------------------------------
    class AudioSettingsSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 今の音量の知らせの発行先
        //-------------------------------------------------------------
        explicit AudioSettingsSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  キーを読み、バスの音量を合わせる
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus* m_eventBus = nullptr;
    };
}    // namespace StardustFever::ECS
