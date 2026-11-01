//-------------------------------------------------------------
//! @file   BigNumberTests.cpp
//! @brief  BigNumber の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/BigNumber.hpp>

#include <cmath>

using StardustFever::Core::BigNumber;

SF_TEST(NormalizesMantissa) {
    const BigNumber value(12345.0);
    SF_EXPECT_NEAR(value.Mantissa(), 1.2345, 1e-12);
    SF_EXPECT(value.Exponent() == 4);

    const BigNumber small(0.005);
    SF_EXPECT_NEAR(small.Mantissa(), 5.0, 1e-12);
    SF_EXPECT(small.Exponent() == -3);

    const BigNumber zero(0.0);
    SF_EXPECT(zero.IsZero());
    SF_EXPECT(zero.Exponent() == 0);
}

SF_TEST(BrokenValuesBecomeZero) {
    SF_EXPECT(BigNumber(std::nan("")).IsZero());
    SF_EXPECT(BigNumber(INFINITY).IsZero());
}

SF_TEST(AddsWithCarry) {
    const BigNumber sum = BigNumber(9.5) + BigNumber(0.5);
    SF_EXPECT_NEAR(sum.Mantissa(), 1.0, 1e-12);
    SF_EXPECT(sum.Exponent() == 1);

    const BigNumber mixed = BigNumber(1.0e20) + BigNumber(3.0e18);
    SF_EXPECT_NEAR(mixed.ToDouble(), 1.03e20, 1e6);
}

SF_TEST(AddIgnoresNegligibleTerm) {
    const BigNumber huge = BigNumber::FromParts(1.0, 400);
    const BigNumber sum  = huge + BigNumber(1.0);
    SF_EXPECT(sum == huge);
}

SF_TEST(SubtractsToNegativeAndZero) {
    const BigNumber difference = BigNumber(100.0) - BigNumber(250.0);
    SF_EXPECT(difference.IsNegative());
    SF_EXPECT_NEAR(difference.ToDouble(), -150.0, 1e-9);

    SF_EXPECT((BigNumber(42.0) - BigNumber(42.0)).IsZero());
}

SF_TEST(MultipliesBeyondDouble) {
    BigNumber value(1.0e200);
    value *= BigNumber(1.0e200);
    SF_EXPECT(value.Exponent() == 400);
    SF_EXPECT_NEAR(value.Mantissa(), 1.0, 1e-9);
    SF_EXPECT(std::isinf(value.ToDouble()));
}

SF_TEST(DividesAndHandlesZeroDivisor) {
    const BigNumber quotient = BigNumber(1.0e10) / BigNumber(4.0);
    SF_EXPECT_NEAR(quotient.ToDouble(), 2.5e9, 1e-3);
    SF_EXPECT((BigNumber(5.0) / BigNumber(0.0)).IsZero());
}

SF_TEST(PowMatchesDoubleAndExtendsIt) {
    SF_EXPECT_NEAR(BigNumber::Pow(2.0, 10.0).ToDouble(), 1024.0, 1e-9);

    const BigNumber big = BigNumber::Pow(10.0, 500.0);
    SF_EXPECT(big.Exponent() == 500);
    SF_EXPECT_NEAR(big.Mantissa(), 1.0, 1e-9);
}

SF_TEST(ComparesBySignExponentMantissa) {
    SF_EXPECT(BigNumber(1.0) < BigNumber(2.0));
    SF_EXPECT(BigNumber(9.0) < BigNumber(10.0));
    SF_EXPECT(BigNumber(-10.0) < BigNumber(-9.0));
    SF_EXPECT(BigNumber(-1.0) < BigNumber(0.0));
    SF_EXPECT(BigNumber(0.0) == BigNumber(0.0));
    SF_EXPECT(BigNumber::FromParts(1.0, 100) > BigNumber::FromParts(9.9, 99));
}

SF_TEST(FormatsSmallValuesAsIntegers) {
    SF_EXPECT_EQ_STR(BigNumber(0.0).ToJapaneseString(), "0");
    SF_EXPECT_EQ_STR(BigNumber(0.4).ToJapaneseString(), "0");
    SF_EXPECT_EQ_STR(BigNumber(523.9).ToJapaneseString(), "523");
    SF_EXPECT_EQ_STR(BigNumber(9999.0).ToJapaneseString(), "9999");
}

SF_TEST(FormatsJapaneseUnits) {
    SF_EXPECT_EQ_STR(BigNumber(10000.0).ToJapaneseString(), "1万");
    SF_EXPECT_EQ_STR(BigNumber(123456.0).ToJapaneseString(), "12.34万");
    SF_EXPECT_EQ_STR(BigNumber(1.234e9).ToJapaneseString(), "12.34億");
    SF_EXPECT_EQ_STR(BigNumber(3.0e12).ToJapaneseString(), "3兆");
    SF_EXPECT_EQ_STR(BigNumber::FromParts(1.23, 68).ToJapaneseString(), "1.23無量大数");
    SF_EXPECT_EQ_STR(BigNumber::FromParts(5.0, 71).ToJapaneseString(), "5000無量大数");
    SF_EXPECT_EQ_STR((-BigNumber(20000.0)).ToJapaneseString(), "-2万");
}

SF_TEST(FormatsBeyondMuryoutaisuAsScientific) {
    SF_EXPECT_EQ_STR(BigNumber::FromParts(1.239, 75).ToJapaneseString(), "1.23e+75");
}

SF_TEST(FormatTruncatesInsteadOfRounding) {
    // 四捨五入すると "10000万" になり、単位の繰り上がりと食い違う
    SF_EXPECT_EQ_STR(BigNumber(99999999.0).ToJapaneseString(), "9999.99万");
}
