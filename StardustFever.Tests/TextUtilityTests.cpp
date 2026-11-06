//-------------------------------------------------------------
//! @file   TextUtilityTests.cpp
//! @brief  文字列の処理の単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Core/TextUtility.hpp>

using StardustFever::Core::WrapText;

SF_TEST(WrapCountsFullWidthAsTwo) {
    const auto lines = WrapText("あいうえお", 4);
    SF_EXPECT(lines.size() == 3);
    SF_EXPECT(lines[0] == "あい");
    SF_EXPECT(lines[2] == "お");
}

SF_TEST(WrapKeepsPunctuationOffTheLineStart) {
    const auto lines = WrapText("あい。うえ", 4);
    SF_EXPECT(lines.size() == 2);
    SF_EXPECT(lines[0] == "あい。");
    SF_EXPECT(lines[1] == "うえ");
}

SF_TEST(WrapHandlesEmptyText) {
    SF_EXPECT(WrapText("", 10).empty());
}
