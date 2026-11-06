//-------------------------------------------------------------
//! @file   LaunchSystem.hpp
//! @brief  発射台から玉を打ち出す System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  LaunchSystem
    //! @brief  打ち続けている発射台から、間隔を空けて玉の Prefab を生成し、初速を与える
    //! @note   自動発射中は強さが LauncherConfigComponent::autoFireJitter だけ揺らぐ（自動発射の代償）
    //-------------------------------------------------------------
    class LaunchSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 打ち出した知らせの発行先
        //-------------------------------------------------------------
        explicit LaunchSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  玉を打ち出す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus* m_eventBus = nullptr;
        std::mt19937            m_random;    //!< 自動発射の揺らぎ用
    };
}    // namespace StardustFever::ECS
