//-------------------------------------------------------------
//! @file   GameDatabase.hpp
//! @brief  マスターデータの読み込み口
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Data/ConstellationData.hpp>
#include <StardustFever/Data/StarData.hpp>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {

    //-------------------------------------------------------------
    //! @brief  星のマスターデータを取得する
    //! @return Assets/Data/Stars.json の中身
    //! @note   初回呼び出しで読み、Debugビルドでは保存し直すと読み直す。
    //!         読み直しはフレームの頭（全Systemの更新前）に行われるため、
    //!         System の途中で中身が変わることはない。ただし vector の要素への
    //!         ポインタ・参照をフレームをまたいで持たないこと
    //-------------------------------------------------------------
    const StarTable& GetStarTable();

    //-------------------------------------------------------------
    //! @brief  星座のマスターデータを取得する
    //! @return Assets/Data/Constellations.json の中身
    //! @note   GetStarTable と同じく、要素への参照をフレームをまたいで持たないこと
    //-------------------------------------------------------------
    const ConstellationTable& GetConstellationTable();

}    // namespace StardustFever::Data
