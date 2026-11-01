//-------------------------------------------------------------
//! @file   PrestigeFormula.cpp
//! @brief  宇宙定数の計算の実装
//-------------------------------------------------------------
#include <StardustFever/Core/PrestigeFormula.hpp>

#include <cmath>

// 名前空間 : StardustFever::Core::PrestigeFormula
namespace StardustFever::Core::PrestigeFormula {

    namespace {
        //-------------------------------------------------------------
        //! @brief  BigNumber の小数部を切り捨てる
        //! @param  value [in] 正の値
        //! @return 切り捨てた値
        //! @note   指数が仮数の桁（約15桁）を超えると小数部は表せていないので、そのまま返す
        //-------------------------------------------------------------
        BigNumber Floor(const BigNumber& value) {
            if(value.IsNegative() || value.IsZero())
                return BigNumber(0.0);
            if(value.Exponent() >= 15)
                return value;
            // 対数を経由した値はちょうどの整数（2.0）でも 1.9999999… になりうるので、少し上へ寄せてから切り捨てる
            return BigNumber(std::floor(value.ToDouble() + 1e-6));
        }
    }    // namespace

    //-------------------------------------------------------------
    // ビッグバンで得られる宇宙定数
    //-------------------------------------------------------------
    BigNumber GainOnBigBang(const BigNumber& universeEarned, double threshold, double exponent) {
        if(universeEarned.IsZero() || universeEarned.IsNegative() || threshold <= 0.0 || exponent <= 0.0)
            return BigNumber(0.0);

        // log10((E / T)^p) = p × (log10 E − log10 T)
        const double log10Gain = exponent * (universeEarned.Log10() - std::log10(threshold));
        if(log10Gain < 0.0)
            return BigNumber(0.0);
        return Floor(BigNumber::Pow(10.0, log10Gain));
    }

    //-------------------------------------------------------------
    // 次の宇宙定数に要る累計
    //-------------------------------------------------------------
    BigNumber EarnedForNextConstant(const BigNumber& currentGain, double threshold, double exponent) {
        if(threshold <= 0.0 || exponent <= 0.0)
            return BigNumber(0.0);
        const BigNumber next = currentGain + BigNumber(1.0);
        // T × (n)^(1/p) を対数で計算する
        const double log10Earned = std::log10(threshold) + next.Log10() / exponent;
        return BigNumber::Pow(10.0, log10Earned);
    }

    //-------------------------------------------------------------
    // 宇宙定数による全体倍率
    //-------------------------------------------------------------
    BigNumber Multiplier(const BigNumber& constants, double bonusPerConstant) {
        return BigNumber(1.0) + constants * BigNumber(bonusPerConstant);
    }

}    // namespace StardustFever::Core::PrestigeFormula
