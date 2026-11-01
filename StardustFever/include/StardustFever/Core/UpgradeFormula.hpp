//-------------------------------------------------------------
//! @file   UpgradeFormula.hpp
//! @brief  強化の値段と効果量の計算
//! @note   エンジンに依存させず、単体テストできるようにしてある
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

// 名前空間 : StardustFever::Core::UpgradeFormula
namespace StardustFever::Core::UpgradeFormula {

    //-------------------------------------------------------------
    //! @enum   Stacking
    //! @brief  レベルを重ねたときの効果の積み上がり方
    //-------------------------------------------------------------
    enum class Stacking {
        Add,         //!< valuePerLevel × Lv（確率や秒数のように足していくもの）
        Multiply,    //!< (1 + valuePerLevel)^Lv（倍率のように掛けていくもの。負なら小さくなっていく）
    };

    //-------------------------------------------------------------
    //! @brief  次のレベルへ上げる値段を計算する
    //! @param  baseCost   [in] Lv0 → Lv1 の値段
    //! @param  costGrowth [in] レベルごとに値段へ掛ける率
    //! @param  level      [in] 今のレベル
    //! @return baseCost × costGrowth^level
    //! @note   インフレで double を超えても計算できるよう BigNumber で返す
    //-------------------------------------------------------------
    BigNumber Cost(double baseCost, double costGrowth, int level);

    //-------------------------------------------------------------
    //! @brief  今のレベルの効果量を計算する
    //! @param  stacking      [in] 積み上がり方
    //! @param  valuePerLevel [in] 1レベルあたりの量
    //! @param  level         [in] 今のレベル（0 なら効果なし）
    //! @return Add なら足す量（Lv0 で 0）、Multiply なら掛ける倍率（Lv0 で 1）
    //-------------------------------------------------------------
    double EffectValue(Stacking stacking, double valuePerLevel, int level);

    //-------------------------------------------------------------
    //! @brief  上限に達しているか
    //! @param  level    [in] 今のレベル
    //! @param  maxLevel [in] 上限（0 以下なら無制限）
    //! @return これ以上上げられないなら true
    //-------------------------------------------------------------
    bool IsMaxed(int level, int maxLevel);

}    // namespace StardustFever::Core::UpgradeFormula
