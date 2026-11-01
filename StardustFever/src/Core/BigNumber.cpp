//-------------------------------------------------------------
//! @file   BigNumber.cpp
//! @brief  BigNumber の実装
//-------------------------------------------------------------
#include <StardustFever/Core/BigNumber.hpp>

#include <array>
#include <cmath>
#include <cstdio>
#include <limits>

// 名前空間 : StardustFever::Core
namespace StardustFever::Core {

    namespace {
        //-------------------------------------------------------------
        // 足し算で小さい側を無視する指数差。
        // double の仮数は約15.9桁なので、これ以上離れると足しても結果が変わらない
        //-------------------------------------------------------------
        constexpr int64_t kAddPrecisionDigits = 17;

        //-------------------------------------------------------------
        // 4桁ごとの和風の単位。添字 i が 10^(4i) に対応する
        //-------------------------------------------------------------
        constexpr std::array<const char*, 18> kJapaneseUnits = {
            "",
            "万",
            "億",
            "兆",
            "京",
            "垓",
            "秭",
            "穣",
            "溝",
            "澗",
            "正",
            "載",
            "極",
            "恒河沙",
            "阿僧祇",
            "那由他",
            "不可思議",
            "無量大数",
        };

        //-------------------------------------------------------------
        //! @brief  小数2桁で切り捨て、末尾の0と小数点を取り除いた文字列にする
        //! @param  value [in] 正の値
        //! @return 例: 12.30 → "12.3"、5.00 → "5"
        //! @note   四捨五入だと 9999.999 が "10000" になり単位の繰り上がりと食い違うため切り捨てる
        //-------------------------------------------------------------
        std::string FormatTwoDecimals(double value) {
            const double truncated = std::floor(value * 100.0 + 1e-9) / 100.0;
            char         buffer[64];
            std::snprintf(buffer, sizeof(buffer), "%.2f", truncated);
            std::string text = buffer;
            while(!text.empty() && text.back() == '0') text.pop_back();
            if(!text.empty() && text.back() == '.')
                text.pop_back();
            return text;
        }
    }    // namespace

    //-------------------------------------------------------------
    // double から作る
    //-------------------------------------------------------------
    BigNumber::BigNumber(double value)
        : m_mantissa(value)
        , m_exponent(0) {
        Normalize();
    }

    //-------------------------------------------------------------
    // 仮数と指数から作る
    //-------------------------------------------------------------
    BigNumber BigNumber::FromParts(double mantissa, int64_t exponent) {
        BigNumber result;
        result.m_mantissa = mantissa;
        result.m_exponent = exponent;
        result.Normalize();
        return result;
    }

    //-------------------------------------------------------------
    // base^exponent を対数で計算する
    //-------------------------------------------------------------
    BigNumber BigNumber::Pow(double base, double exponent) {
        if(base <= 0.0)
            return BigNumber(std::pow(base, exponent));

        const double log10Value = exponent * std::log10(base);
        const double wholePart  = std::floor(log10Value);
        BigNumber    result;
        result.m_mantissa = std::pow(10.0, log10Value - wholePart);
        result.m_exponent = static_cast<int64_t>(wholePart);
        result.Normalize();
        return result;
    }

    //-------------------------------------------------------------
    // シリアライズ用に値を直接入れる
    //-------------------------------------------------------------
    void BigNumber::SetParts(double mantissa, int64_t exponent) {
        m_mantissa = mantissa;
        m_exponent = exponent;
        Normalize();
    }

    //-------------------------------------------------------------
    // 仮数を [1, 10) へ寄せる
    //-------------------------------------------------------------
    void BigNumber::Normalize() {
        if(!std::isfinite(m_mantissa) || m_mantissa == 0.0) {
            // 壊れた値で以降の計算まで NaN に染まらないよう、0 として扱う
            m_mantissa = 0.0;
            m_exponent = 0;
            return;
        }

        const double shift  = std::floor(std::log10(std::fabs(m_mantissa)));
        m_mantissa         /= std::pow(10.0, shift);
        m_exponent         += static_cast<int64_t>(shift);

        // log10 の丸め誤差で 9.9999… や 10.0 に寄った場合の補正
        if(std::fabs(m_mantissa) >= 10.0) {
            m_mantissa /= 10.0;
            ++m_exponent;
        } else if(std::fabs(m_mantissa) < 1.0) {
            m_mantissa *= 10.0;
            --m_exponent;
        }
    }

    //-------------------------------------------------------------
    // double へ戻す
    //-------------------------------------------------------------
    double BigNumber::ToDouble() const {
        if(IsZero())
            return 0.0;
        if(m_exponent > std::numeric_limits<double>::max_exponent10)
            return IsNegative() ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
        if(m_exponent < std::numeric_limits<double>::min_exponent10 - 20)
            return 0.0;
        return m_mantissa * std::pow(10.0, static_cast<double>(m_exponent));
    }

    //-------------------------------------------------------------
    // 常用対数を取る
    //-------------------------------------------------------------
    double BigNumber::Log10() const {
        if(IsZero())
            return -std::numeric_limits<double>::infinity();
        return std::log10(std::fabs(m_mantissa)) + static_cast<double>(m_exponent);
    }

    //-------------------------------------------------------------
    // 足し算。指数の大きい側に合わせてから仮数を足す
    //-------------------------------------------------------------
    BigNumber& BigNumber::operator+=(const BigNumber& rhs) {
        if(rhs.IsZero())
            return *this;
        if(IsZero()) {
            *this = rhs;
            return *this;
        }

        const BigNumber& larger  = (m_exponent >= rhs.m_exponent) ? *this : rhs;
        const BigNumber& smaller = (m_exponent >= rhs.m_exponent) ? rhs : *this;
        const int64_t    diff    = larger.m_exponent - smaller.m_exponent;
        if(diff > kAddPrecisionDigits) {
            *this = larger;
            return *this;
        }

        const double  mantissa = larger.m_mantissa + smaller.m_mantissa * std::pow(10.0, -static_cast<double>(diff));
        const int64_t exponent = larger.m_exponent;
        m_mantissa             = mantissa;
        m_exponent             = exponent;
        Normalize();
        return *this;
    }

    //-------------------------------------------------------------
    // 引き算
    //-------------------------------------------------------------
    BigNumber& BigNumber::operator-=(const BigNumber& rhs) {
        return *this += -rhs;
    }

    //-------------------------------------------------------------
    // 掛け算。仮数を掛けて指数を足す
    //-------------------------------------------------------------
    BigNumber& BigNumber::operator*=(const BigNumber& rhs) {
        m_mantissa *= rhs.m_mantissa;
        m_exponent += rhs.m_exponent;
        Normalize();
        return *this;
    }

    //-------------------------------------------------------------
    // 割り算。0 で割ったときは 0 にする（通貨計算で NaN を広げないため）
    //-------------------------------------------------------------
    BigNumber& BigNumber::operator/=(const BigNumber& rhs) {
        if(rhs.IsZero()) {
            m_mantissa = 0.0;
            m_exponent = 0;
            return *this;
        }
        m_mantissa /= rhs.m_mantissa;
        m_exponent -= rhs.m_exponent;
        Normalize();
        return *this;
    }

    //-------------------------------------------------------------
    // 大小比較。符号 → 指数 → 仮数 の順に見る
    //-------------------------------------------------------------
    int BigNumber::Compare(const BigNumber& rhs) const {
        const int lhsSign = IsZero() ? 0 : (IsNegative() ? -1 : 1);
        const int rhsSign = rhs.IsZero() ? 0 : (rhs.IsNegative() ? -1 : 1);
        if(lhsSign != rhsSign)
            return lhsSign < rhsSign ? -1 : 1;
        if(lhsSign == 0)
            return 0;

        // 負の数は指数が大きいほど小さい
        int result = 0;
        if(m_exponent != rhs.m_exponent) {
            result = m_exponent < rhs.m_exponent ? -1 : 1;
            return lhsSign > 0 ? result : -result;
        }
        if(m_mantissa != rhs.m_mantissa)
            result = m_mantissa < rhs.m_mantissa ? -1 : 1;
        return result;
    }

    //-------------------------------------------------------------
    // 和風の単位で文字列にする
    //-------------------------------------------------------------
    std::string BigNumber::ToJapaneseString() const {
        if(IsZero())
            return "0";
        if(IsNegative())
            return "-" + (-*this).ToJapaneseString();

        // 1万未満は端数を見せても桁が増えるだけなので整数にする
        if(m_exponent < 4) {
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "%.0f", std::floor(ToDouble() + 1e-9));
            return buffer;
        }

        const int64_t unitIndex = m_exponent / 4;
        if(unitIndex >= static_cast<int64_t>(kJapaneseUnits.size())) {
            char buffer[64];
            std::snprintf(buffer, sizeof(buffer), "e+%lld", static_cast<long long>(m_exponent));
            return FormatTwoDecimals(m_mantissa) + buffer;
        }

        const double scaled = m_mantissa * std::pow(10.0, static_cast<double>(m_exponent - unitIndex * 4));
        return FormatTwoDecimals(scaled) + kJapaneseUnits[static_cast<size_t>(unitIndex)];
    }

}    // namespace StardustFever::Core
