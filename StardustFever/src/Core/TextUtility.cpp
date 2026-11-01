//-------------------------------------------------------------
//! @file   TextUtility.cpp
//! @brief  文字列の変換の実装
//-------------------------------------------------------------
#include <StardustFever/Core/TextUtility.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// 名前空間 : StardustFever::Core
namespace StardustFever::Core {

    //-------------------------------------------------------------
    // UTF-8 を UTF-16 にする
    //-------------------------------------------------------------
    std::wstring Utf8ToWide(const std::string& utf8) {
        if(utf8.empty())
            return {};

        const int length = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
        if(length <= 0)
            return {};

        std::wstring wide(static_cast<size_t>(length), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), wide.data(), length);
        return wide;
    }

}    // namespace StardustFever::Core
