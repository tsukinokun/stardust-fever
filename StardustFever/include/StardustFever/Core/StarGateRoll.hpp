//-------------------------------------------------------------
//! @file   StarGateRoll.hpp
//! @brief  星の門の星座ルーレットの進み方の計算
//! @note   エンジンに依存させず、単体テストできるようにしてある
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::Core::StarGateRoll
namespace StardustFever::Core::StarGateRoll {

    //-------------------------------------------------------------
    //! @enum   Scenario
    //! @brief  抽選の筋書き。当たり外れは先に決まっていて、見せ方だけを変える
    //-------------------------------------------------------------
    enum class Scenario : int {
        Win,         //!< 素直な当たり
        Miss,        //!< 素直な外れ
        NearMiss,    //!< あと1つまで灯ってからの外れ
        Comeback,    //!< 外れたと見せてからの当たり
    };

    //-------------------------------------------------------------
    //! @enum   Phase
    //! @brief  抽選の段階
    //-------------------------------------------------------------
    enum class Phase : int {
        Spin,    //!< 星座が入れ替わる
        Hang,    //!< 星が1つずつ灯る
        Fake,    //!< 外れたと見せている
        Done,    //!< 終わった
    };

    //-------------------------------------------------------------
    //! @struct Timeline
    //! @brief  段階の長さ（秒）
    //-------------------------------------------------------------
    struct Timeline {
        float spin = 0.0f;    //!< 星座が入れ替わる長さ
        float hang = 0.0f;    //!< 星が灯る長さ
        float fake = 0.0f;    //!< 外れたと見せる長さ
    };

    //-------------------------------------------------------------
    //! @brief  抽選全体の長さを求める
    //! @param  timeline [in] 段階の長さ
    //! @param  scenario [in] 筋書き
    //! @return 秒数。外れたと見せるのは復活のときだけ
    //-------------------------------------------------------------
    float TotalSeconds(const Timeline& timeline, Scenario scenario);

    //-------------------------------------------------------------
    //! @brief  経過時間から段階を求める
    //! @param  timeline [in] 段階の長さ
    //! @param  scenario [in] 筋書き
    //! @param  elapsed  [in] 抽選が始まってからの秒数
    //! @return 段階
    //-------------------------------------------------------------
    Phase PhaseAt(const Timeline& timeline, Scenario scenario, float elapsed);

    //-------------------------------------------------------------
    //! @brief  星座が入れ替わった回数を求める
    //! @param  progress [in] Spin の進み具合（0〜1）
    //! @param  steps    [in] 入れ替わる回数の合計
    //! @return 回数。だんだん遅くなる
    //-------------------------------------------------------------
    int ReelStep(float progress, int steps);

    //-------------------------------------------------------------
    //! @brief  灯っている星の数を求める
    //! @param  progress [in] Hang の進み具合（0〜1）
    //! @param  count    [in] 星座の星の数
    //! @param  scenario [in] 筋書き
    //! @return 数。当たりは全部、惜しいと復活はあと1つ、外れは半分ほどで止まる
    //-------------------------------------------------------------
    int LitStars(float progress, int count, Scenario scenario);

}    // namespace StardustFever::Core::StarGateRoll
