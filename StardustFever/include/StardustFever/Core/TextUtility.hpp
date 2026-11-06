//-------------------------------------------------------------
//! @file   TextUtility.hpp
//! @brief  文字列の変換
//-------------------------------------------------------------
#pragma once
#include <string>
#include <vector>

// 名前空間 : StardustFever::Core
namespace StardustFever::Core {

    //-------------------------------------------------------------
    //! @brief  UTF-8 を UTF-16 にする
    //! @param  utf8 [in] UTF-8 の文字列
    //! @return UTF-16 の文字列（FontComponent::text へ入れる形）
    //! @note   データとソースは UTF-8 で持ち、描画の直前でだけ変換する
    //-------------------------------------------------------------
    std::wstring Utf8ToWide(const std::string& utf8);

    //-------------------------------------------------------------
    //! @brief  文章を指定の幅で折り返す
    //! @param  utf8     [in] UTF-8 の文章
    //! @param  maxWidth [in] 1行の幅（半角1文字を1、全角1文字を2と数える）
    //! @return 行の一覧
    //! @note   句読点と閉じ括弧が行の頭に来ないよう、前の行へ少しはみ出させる
    //-------------------------------------------------------------
    std::vector<std::string> WrapText(const std::string& utf8, int maxWidth);

}    // namespace StardustFever::Core
