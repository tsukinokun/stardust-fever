//-------------------------------------------------------------
//! @file   StarDrawSystem.hpp
//! @brief  星ゲットチャンスを成功させたら、88星座の星から1つを引く System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/StarChanceEndedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarDrawSystem
    //! @brief  チャンスを成功させたら、星座の線の星すべてから、明るい星ほど出にくい抽選で1つを引く。
    //!         初めてなら ★1、持っていれば ★+1、最大なら星屑に換える。
    //!         その星で星座の星が全部そろったら知らせる
    //! @note   効果の計算は CollectionEffectSystem と ScoreMultiplierSystem が行う（記録と計算を分けてある）
    //-------------------------------------------------------------
    class StarDrawSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] チャンスの結果の購読元であり、引いた星・そろった星座・通知の発行先
        //-------------------------------------------------------------
        explicit StarDrawSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  終わったチャンスの結果を処理する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus*          m_eventBus = nullptr;
        EventQueue<StarChanceEndedEvent> m_ends;
        std::mt19937                     m_random;
    };
}    // namespace StardustFever::ECS
