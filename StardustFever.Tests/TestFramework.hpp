//-------------------------------------------------------------
//! @file   TestFramework.hpp
//! @brief  依存ライブラリを増やさないための最小限のテスト登録・判定
//-------------------------------------------------------------
#pragma once
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

// 名前空間 : StardustFever::Tests
namespace StardustFever::Tests {

    struct TestCase {
        const char*           name;
        std::function<void()> body;
    };

    inline std::vector<TestCase>& Registry() {
        static std::vector<TestCase> s_Cases;
        return s_Cases;
    }

    inline int& CurrentFailures() {
        static int s_Failures = 0;
        return s_Failures;
    }

    struct TestRegistrar {
        TestRegistrar(const char* name, std::function<void()> body) { Registry().push_back({name, std::move(body)}); }
    };

    //-------------------------------------------------------------
    //! @brief  登録済みのテストを全て実行する
    //! @return 失敗したテストの数
    //-------------------------------------------------------------
    inline int RunAll() {
        int failedTests = 0;
        for(const TestCase& test : Registry()) {
            CurrentFailures() = 0;
            test.body();
            if(CurrentFailures() > 0) {
                ++failedTests;
                std::printf("[FAIL] %s\n", test.name);
            } else {
                std::printf("[ OK ] %s\n", test.name);
            }
        }
        return failedTests;
    }

    inline void ReportFailure(const char* file, int line, const std::string& message) {
        ++CurrentFailures();
        std::printf("    %s(%d): %s\n", file, line, message.c_str());
    }

}    // namespace StardustFever::Tests

#define SF_TEST_CONCAT_INNER(a, b) a##b
#define SF_TEST_CONCAT(a, b)       SF_TEST_CONCAT_INNER(a, b)

#define SF_TEST(name)                                                                                                           \
    static void                                  SF_TEST_CONCAT(TestBody_, name)();                                             \
    static ::StardustFever::Tests::TestRegistrar SF_TEST_CONCAT(TestRegistrar_, name)(#name, &SF_TEST_CONCAT(TestBody_, name)); \
    static void                                  SF_TEST_CONCAT(TestBody_, name)()

#define SF_EXPECT(condition)                                                                    \
    do {                                                                                        \
        if(!(condition))                                                                        \
            ::StardustFever::Tests::ReportFailure(__FILE__, __LINE__, "expected: " #condition); \
    } while(false)

#define SF_EXPECT_EQ_STR(actual, expected)                                                                                             \
    do {                                                                                                                               \
        const std::string sfActual   = (actual);                                                                                       \
        const std::string sfExpected = (expected);                                                                                     \
        if(sfActual != sfExpected)                                                                                                     \
            ::StardustFever::Tests::ReportFailure(__FILE__, __LINE__, "expected \"" + sfExpected + "\" but got \"" + sfActual + "\""); \
    } while(false)

#define SF_EXPECT_NEAR(actual, expected, tolerance)                                                                                                       \
    do {                                                                                                                                                  \
        const double sfActual   = (actual);                                                                                                               \
        const double sfExpected = (expected);                                                                                                             \
        if(!(sfActual >= sfExpected - (tolerance) && sfActual <= sfExpected + (tolerance)))                                                               \
            ::StardustFever::Tests::ReportFailure(__FILE__, __LINE__, "expected " + std::to_string(sfExpected) + " but got " + std::to_string(sfActual)); \
    } while(false)
