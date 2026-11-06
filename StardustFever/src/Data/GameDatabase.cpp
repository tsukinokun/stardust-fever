//-------------------------------------------------------------
//! @file   GameDatabase.cpp
//! @brief  マスターデータの読み込み口の実装
//-------------------------------------------------------------
#include <StardustFever/Data/GameDatabase.hpp>

#include <Tsukino/Core/IO/HotReloadable.hpp>
#include <Tsukino/Core/Log.hpp>
#include <Tsukino/Core/Path.hpp>

#include <cereal/archives/json.hpp>

#include <exception>
#include <fstream>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {

    namespace {
        constexpr const char* kStarTablePath          = "StardustFever/Assets/Data/Stars.json";
        constexpr const char* kConstellationTablePath = "StardustFever/Assets/Data/Constellations.json";
        constexpr const char* kUpgradeTablePath       = "StardustFever/Assets/Data/Upgrades.json";
        constexpr const char* kGroupTablePath         = "StardustFever/Assets/Data/ConstellationGroups.json";
        constexpr const char* kSkyObjectTablePath     = "StardustFever/Assets/Data/SkyObjects.json";

        //-------------------------------------------------------------
        //! @brief  ルートキー付きのJSONを1つ読む
        //! @param  path    [in]  ファイルのパス
        //! @param  rootKey [in]  ルートのキー
        //! @param  out     [out] 読み込み先
        //! @return 読めたか
        //! @note   書きかけのJSONを保存した瞬間にも呼ばれるため、例外は外へ出さない。
        //!         false を返せば HotReloadable は前の値のまま残す
        //-------------------------------------------------------------
        template <class T>
        bool LoadJson(const char* path, const char* rootKey, T& out) {
            std::ifstream stream(path);
            if(!stream.is_open()) {
                Tsukino::Core::Log::Error(std::string("Master data not found: ") + path + " (the game will run without it)");
                return false;
            }
            try {
                cereal::JSONInputArchive archive(stream);
                archive(cereal::make_nvp(rootKey, out));
                return true;
            } catch(const std::exception& e) {
                Tsukino::Core::Log::Error(std::string("Failed to parse master data: ") + path + " (" + e.what() + "). Keeping the previous values.");
                return false;
            }
        }
    }    // namespace

    //-------------------------------------------------------------
    // 星のマスターデータ
    //-------------------------------------------------------------
    const StarTable& GetStarTable() {
        // HotReload は監視を解除できないため、監視先の this が先に消えないよう関数内staticに置く
        static Tsukino::IO::HotReloadable<StarTable> s_Table(Tsukino::Core::Path(kStarTablePath), [](StarTable& out) {
            if(!LoadJson(kStarTablePath, "StarTable", out))
                return false;
            out.BuildIndex();
            return true;
        });
        return s_Table.Get();
    }

    //-------------------------------------------------------------
    // 星座のマスターデータ
    //-------------------------------------------------------------
    const ConstellationTable& GetConstellationTable() {
        static Tsukino::IO::HotReloadable<ConstellationTable> s_Table(
            Tsukino::Core::Path(kConstellationTablePath), [](ConstellationTable& out) { return LoadJson(kConstellationTablePath, "ConstellationTable", out); });
        return s_Table.Get();
    }

    //-------------------------------------------------------------
    // 強化のマスターデータ
    //-------------------------------------------------------------
    const ConstellationGroupTable& GetConstellationGroupTable() {
        static Tsukino::IO::HotReloadable<ConstellationGroupTable> s_Table(
            Tsukino::Core::Path(kGroupTablePath), [](ConstellationGroupTable& out) { return LoadJson(kGroupTablePath, "ConstellationGroupTable", out); });
        return s_Table.Get();
    }

    const UpgradeTable& GetUpgradeTable() {
        static Tsukino::IO::HotReloadable<UpgradeTable> s_Table(Tsukino::Core::Path(kUpgradeTablePath),
                                                                [](UpgradeTable& out) { return LoadJson(kUpgradeTablePath, "UpgradeTable", out); });
        return s_Table.Get();
    }

    //-------------------------------------------------------------
    // ビッグバンで手に入る天体のマスターデータ
    //-------------------------------------------------------------
    const SkyObjectTable& GetSkyObjectTable() {
        static Tsukino::IO::HotReloadable<SkyObjectTable> s_Table(Tsukino::Core::Path(kSkyObjectTablePath),
                                                                  [](SkyObjectTable& out) { return LoadJson(kSkyObjectTablePath, "SkyObjectTable", out); });
        return s_Table.Get();
    }

}    // namespace StardustFever::Data
