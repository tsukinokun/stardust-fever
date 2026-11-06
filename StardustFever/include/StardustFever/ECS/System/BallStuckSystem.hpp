//-------------------------------------------------------------
//! @file   BallStuckSystem.hpp
//! @brief  止まってしまった玉を動かす System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  BallStuckSystem
    //! @brief  しばらく動かない玉を押し、それでも動かなければアウトにする
    //! @note   玉は閉じたチューリップの羽の上などで釣り合って止まることがある。
    //!         放っておくと盤面の玉の枠を埋め続ける
    //-------------------------------------------------------------
    class BallStuckSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] アウトの発行先
        //-------------------------------------------------------------
        explicit BallStuckSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  止まった玉を探して動かす
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus* m_eventBus = nullptr;
        std::mt19937            m_random;    //!< 押す向きを決める
    };
}    // namespace StardustFever::ECS
