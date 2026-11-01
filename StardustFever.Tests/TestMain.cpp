//-------------------------------------------------------------
//! @file   TestMain.cpp
//! @brief  単体テストのエントリポイント
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <cstdio>

int main() {
    const int failures = StardustFever::Tests::RunAll();
    if(failures == 0) {
        std::printf("All tests passed.\n");
        return 0;
    }
    std::printf("%d test(s) failed.\n", failures);
    return 1;
}
