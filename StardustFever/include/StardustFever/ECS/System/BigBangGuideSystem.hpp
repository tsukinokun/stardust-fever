//-------------------------------------------------------------
//! @file   BigBangGuideSystem.hpp
//! @brief  ビッグバンができることを知らせる System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>
#include <hlsl++.h>

namespace Tsukino::ECS {
    class EventBus;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  BigBangGuideSystem
    //! @brief  できるようになったら大見出しと音で一度知らせ、できる間はボタンを弾ませる
    //! @note   星詠みタイムと星ゲットチャンスの間は実行できないので、知らせるのは終わってからにする
    //-------------------------------------------------------------
    class BigBangGuideSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 大見出しと音のイベントの発行先
        //-------------------------------------------------------------
        explicit BigBangGuideSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  できるかどうかを見て、案内を出す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  ボタンと案内文の見た目を切り替える
        //! @param  registry [in] レジストリ
        //! @param  active   [in] 今すぐ実行できるなら true
        //-------------------------------------------------------------
        void SetActive(Tsukino::ECS::Registry& registry, bool active);

        Tsukino::ECS::EventBus* m_eventBus  = nullptr;
        bool                    m_announced = false;    //!< この宇宙で知らせ終えた
        bool                    m_active    = false;    //!< 弾ませている
        bool                    m_hasColor  = false;    //!< m_hintColor を読んだ
        hlslpp::float4          m_hintColor;            //!< 案内文のふだんの色
    };
}    // namespace StardustFever::ECS
