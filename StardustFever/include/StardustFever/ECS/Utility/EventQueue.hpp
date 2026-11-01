//-------------------------------------------------------------
//! @file   EventQueue.hpp
//! @brief  購読したイベントを System の Update まで溜めておく入れ物
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/ECS/Event/ScopedConnection.hpp>

#include <utility>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  EventQueue
    //! @brief  イベントを受け取った時点では溜めるだけにし、System の Update でまとめて処理させる
    //! @tparam TEvent 溜めるイベント
    //! @note   EventBus::Publish は同期呼び出しのため、ハンドラの中で処理すると
    //!         「どの System の Update の途中で動くか」が発行側の都合で決まってしまう。
    //!         溜めておけば、処理は必ず受け取る側の System の順番で動く。
    //!         購読のラムダが this を握るため、コピー・ムーブはできない
    //-------------------------------------------------------------
    template <class TEvent>
    class EventQueue final {
    public:
        //-------------------------------------------------------------
        //! @brief  購読を始める
        //! @param  eventBus [in] 購読するイベントバス（この入れ物より長生きすること）
        //-------------------------------------------------------------
        explicit EventQueue(Tsukino::ECS::EventBus& eventBus)
            : m_connection(eventBus.Subscribe<TEvent>([this](const TEvent& event) { m_events.push_back(event); })) {}

        EventQueue(const EventQueue&)            = delete;
        EventQueue& operator=(const EventQueue&) = delete;
        EventQueue(EventQueue&&)                 = delete;
        EventQueue& operator=(EventQueue&&)      = delete;

        //-------------------------------------------------------------
        //! @brief  溜まったイベントを取り出す
        //! @return 届いた順のイベント（取り出した分は空になる）
        //! @note   処理中に同じ種類のイベントが新しく届いても、次の Drain まで残る
        //-------------------------------------------------------------
        [[nodiscard]] std::vector<TEvent> Drain() {
            std::vector<TEvent> drained;
            drained.swap(m_events);
            return drained;
        }

    private:
        //! @note 【宣言順は破棄順序の設計】購読の解除（m_connection）を先に行い、
        //!       その後で溜めた中身を捨てる。逆だと解除前に届いたイベントが破棄済みの vector へ積まれる
        std::vector<TEvent>            m_events;
        Tsukino::ECS::ScopedConnection m_connection;
    };
}    // namespace StardustFever::ECS
