//-------------------------------------------------------------
//! @file   PrestigeFormula.hpp
//! @brief  ビッグバン（転生）で得る宇宙定数と、その倍率の計算
//! @note   エンジンに依存させず、単体テストできるようにしてある
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

// 名前空間 : StardustFever::Core::PrestigeFormula
namespace StardustFever::Core::PrestigeFormula {

    //-------------------------------------------------------------
    //! @brief  ビッグバンで得られる宇宙定数を計算する
    //! @param  universeEarned [in] この宇宙で稼いだ星屑の累計（使っても減らない）
    //! @param  threshold      [in] 宇宙定数1つに要る累計
    //! @param  exponent       [in] 累計に対する伸び方（0.5 なら平方根）
    //! @return floor((universeEarned / threshold)^exponent)。足りなければ 0
    //! @note   累乗は対数で計算するので、累計が double を超えていても求まる。
    //!         ビッグバンで累計は 0 に戻るため、この値がそのまま今回の獲得量になる
    //-------------------------------------------------------------
    BigNumber GainOnBigBang(const BigNumber& universeEarned, double threshold, double exponent);

    //-------------------------------------------------------------
    //! @brief  次の宇宙定数（今の獲得量 + 1）に要る累計を計算する
    //! @param  currentGain [in] 今ビッグバンしたときの獲得量
    //! @param  threshold   [in] 宇宙定数1つに要る累計
    //! @param  exponent    [in] 累計に対する伸び方
    //! @return threshold × (currentGain + 1)^(1 / exponent)
    //-------------------------------------------------------------
    BigNumber EarnedForNextConstant(const BigNumber& currentGain, double threshold, double exponent);

    //-------------------------------------------------------------
    //! @brief  宇宙定数による全体倍率を計算する
    //! @param  constants        [in] 持っている宇宙定数
    //! @param  bonusPerConstant [in] 宇宙定数1つあたりに足す倍率
    //! @return 1 + bonusPerConstant × constants
    //-------------------------------------------------------------
    BigNumber Multiplier(const BigNumber& constants, double bonusPerConstant);

    //-------------------------------------------------------------
    //! @brief  ビッグバンの後の全体倍率を計算する
    //! @param  total            [in] 今の全体倍率
    //! @param  constants        [in] 持っている宇宙定数
    //! @param  gain             [in] ビッグバンで得る宇宙定数
    //! @param  bonusPerConstant [in] 宇宙定数1つあたりに足す倍率
    //! @return 宇宙定数の分だけを置き換えた全体倍率
    //-------------------------------------------------------------
    BigNumber MultiplierAfter(const BigNumber& total, const BigNumber& constants, const BigNumber& gain, double bonusPerConstant);

}    // namespace StardustFever::Core::PrestigeFormula
