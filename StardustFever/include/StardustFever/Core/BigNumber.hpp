//-------------------------------------------------------------
//! @file   BigNumber.hpp
//! @brief  インフレする数値を表す 仮数 × 10^指数 の数
//-------------------------------------------------------------
#pragma once
#include <cstdint>
#include <string>

// 名前空間 : StardustFever::Core
namespace StardustFever::Core {
    //-------------------------------------------------------------
    //! @class  BigNumber
    //! @brief  double の上限（約1e308）を超えて増え続ける値
    //! @note   mantissa は 0 か、絶対値が [1, 10) に正規化される。
    //!         精度は double の仮数（約15桁）までで、それより小さい差は丸められる。
    //!         インフレ系の表示では上位数桁しか見せないため、これで十分
    //-------------------------------------------------------------
    class BigNumber {
    public:
        BigNumber() = default;

        //-------------------------------------------------------------
        //! @brief  double から作る
        //! @param  value [in] 元の値
        //-------------------------------------------------------------
        BigNumber(double value);    // NOLINT: 数値リテラルから暗黙に作れると式が読みやすいため explicit にしない

        //-------------------------------------------------------------
        //! @brief  仮数と指数から作る（正規化する）
        //! @param  mantissa [in] 仮数
        //! @param  exponent [in] 10の指数
        //! @return 作った値
        //-------------------------------------------------------------
        static BigNumber FromParts(double mantissa, int64_t exponent);

        //-------------------------------------------------------------
        //! @brief  base^exponent を作る（double では溢れる大きさでも計算できる）
        //! @param  base     [in] 底（正の数）
        //! @param  exponent [in] 指数
        //! @return 作った値
        //-------------------------------------------------------------
        static BigNumber Pow(double base, double exponent);

        [[nodiscard]] double  Mantissa() const noexcept { return m_mantissa; }
        [[nodiscard]] int64_t Exponent() const noexcept { return m_exponent; }
        [[nodiscard]] bool    IsZero() const noexcept { return m_mantissa == 0.0; }
        [[nodiscard]] bool    IsNegative() const noexcept { return m_mantissa < 0.0; }

        //-------------------------------------------------------------
        //! @brief  double へ戻す
        //! @return 値（double で表せなければ ±inf）
        //-------------------------------------------------------------
        [[nodiscard]] double ToDouble() const;

        //-------------------------------------------------------------
        //! @brief  常用対数を取る
        //! @return log10(|値|)。0 なら -inf
        //-------------------------------------------------------------
        [[nodiscard]] double Log10() const;

        BigNumber& operator+=(const BigNumber& rhs);
        BigNumber& operator-=(const BigNumber& rhs);
        BigNumber& operator*=(const BigNumber& rhs);
        BigNumber& operator/=(const BigNumber& rhs);

        friend BigNumber operator+(BigNumber lhs, const BigNumber& rhs) { return lhs += rhs; }
        friend BigNumber operator-(BigNumber lhs, const BigNumber& rhs) { return lhs -= rhs; }
        friend BigNumber operator*(BigNumber lhs, const BigNumber& rhs) { return lhs *= rhs; }
        friend BigNumber operator/(BigNumber lhs, const BigNumber& rhs) { return lhs /= rhs; }
        BigNumber        operator-() const { return FromParts(-m_mantissa, m_exponent); }

        //-------------------------------------------------------------
        //! @brief  大小を比べる
        //! @param  rhs [in] 比べる相手
        //! @return 負なら this が小さい、0 なら等しい、正なら this が大きい
        //-------------------------------------------------------------
        [[nodiscard]] int Compare(const BigNumber& rhs) const;

        friend bool operator==(const BigNumber& lhs, const BigNumber& rhs) { return lhs.Compare(rhs) == 0; }
        friend bool operator!=(const BigNumber& lhs, const BigNumber& rhs) { return lhs.Compare(rhs) != 0; }
        friend bool operator<(const BigNumber& lhs, const BigNumber& rhs) { return lhs.Compare(rhs) < 0; }
        friend bool operator<=(const BigNumber& lhs, const BigNumber& rhs) { return lhs.Compare(rhs) <= 0; }
        friend bool operator>(const BigNumber& lhs, const BigNumber& rhs) { return lhs.Compare(rhs) > 0; }
        friend bool operator>=(const BigNumber& lhs, const BigNumber& rhs) { return lhs.Compare(rhs) >= 0; }

        //-------------------------------------------------------------
        //! @brief  和風の単位（万・億・兆…無量大数）で表した文字列を作る
        //! @return UTF-8 の文字列（例: "12.34億"、"5無量大数" を超えると "1.23e+75"）
        //! @note   1万未満は小数を切り捨てた整数、それ以上は小数2桁まで出す
        //-------------------------------------------------------------
        [[nodiscard]] std::string ToJapaneseString() const;

        //-------------------------------------------------------------
        //! @brief  シリアライズ用に正規化前の値を直接入れる
        //! @param  mantissa [in] 仮数
        //! @param  exponent [in] 指数
        //-------------------------------------------------------------
        void SetParts(double mantissa, int64_t exponent);

    private:
        //-------------------------------------------------------------
        //! @brief  仮数を [1, 10) へ寄せ、指数を合わせる
        //-------------------------------------------------------------
        void Normalize();

        double  m_mantissa = 0.0;    //!< 仮数（0 または |m| ∈ [1, 10)）
        int64_t m_exponent = 0;      //!< 10の指数
    };
}    // namespace StardustFever::Core
