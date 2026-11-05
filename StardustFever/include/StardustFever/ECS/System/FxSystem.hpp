//-------------------------------------------------------------
//! @file   FxSystem.hpp
//! @brief  盤面の上の演出（浮かぶ数字と粒子）を出す System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BallEnteredPocketEvent.hpp>
#include <StardustFever/ECS/Event/ConstellationCompletedEvent.hpp>
#include <StardustFever/ECS/Event/StarLinkedEvent.hpp>
#include <StardustFever/ECS/Event/StardustCreditedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <deque>
#include <random>

namespace Tsukino::ECS {
    class Scene;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  FxSystem
    //! @brief  星屑が入った場所から「+12」を浮かべ、星をつないだ場所・星座の完成で粒子を弾けさせる
    //! @note   浮かぶ数字は金額で大きさを、出どころで色を変え、出る位置・昇る高さ・速さを少しずつばらつかせる。
    //!         釘の分は割合で間引く（毎回出すと盤面が数字で埋まるため）
    //-------------------------------------------------------------
    class FxSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene    [in] 粒子の生成先（BurstEmitter がシーンを要る。この System より長生きすること）
        //! @param  eventBus [in] 獲得・星・完成の購読元
        //-------------------------------------------------------------
        FxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  演出を出す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  浮かぶ数字を1つ出す
        //-------------------------------------------------------------
        void SpawnPopup(Tsukino::ECS::Registry& registry, const StardustCreditedEvent& credited);

        Tsukino::ECS::Scene*                    m_scene = nullptr;
        EventQueue<StardustCreditedEvent>       m_credits;
        EventQueue<StarLinkedEvent>             m_linkedStars;
        EventQueue<ConstellationCompletedEvent> m_completions;
        EventQueue<BallEnteredPocketEvent>      m_pocketEntries;
        std::deque<Tsukino::ECS::Entity>        m_popups;    //!< 出ている数字（上限を超えたら新しく出さない）
        std::mt19937                            m_random;
    };
}    // namespace StardustFever::ECS
