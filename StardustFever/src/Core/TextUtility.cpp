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

    std::vector<std::string> WrapText(const std::string& utf8, int maxWidth) {
        // 行の頭に置かない文字
        static const std::string kNoLineStart[] = {"。", "、", "」", "）", "』", "・", "！", "？"};

        std::vector<std::string> lines;
        std::string              line;
        int                      width = 0;
        for(size_t i = 0; i < utf8.size();) {
            const unsigned char lead    = static_cast<unsigned char>(utf8[i]);
            const size_t        length  = lead < 0x80 ? 1 : (lead < 0xE0 ? 2 : (lead < 0xF0 ? 3 : 4));
            const std::string   letter  = utf8.substr(i, length);
            const int           cost    = length == 1 ? 1 : 2;
            i                          += length;

            bool keep = false;
            for(const std::string& mark : kNoLineStart) {
                keep = keep || letter == mark;
            }
            if(width + cost > maxWidth && !keep && !line.empty()) {
                lines.push_back(line);
                line.clear();
                width = 0;
            }
            line  += letter;
            width += cost;
        }
        if(!line.empty())
            lines.push_back(line);
        return lines;
    }

}    // namespace StardustFever::Core
