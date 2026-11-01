//-------------------------------------------------------------
//! @file   TextUtility.hpp
//! @brief  文字列の変換
//-------------------------------------------------------------
#pragma once
#include <string>

// 名前空間 : StardustFever::Core
namespace StardustFever::Core {

    //-------------------------------------------------------------
    //! @brief  UTF-8 を UTF-16 にする
    //! @param  utf8 [in] UTF-8 の文字列
    //! @return UTF-16 の文字列（FontComponent::text へ入れる形）
    //! @note   データとソースは UTF-8 で持ち、描画の直前でだけ変換する
    //-------------------------------------------------------------
    std::wstring Utf8ToWide(const std::string& utf8);

}    // namespace StardustFever::Core
