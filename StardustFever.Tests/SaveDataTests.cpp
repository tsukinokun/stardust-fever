//-------------------------------------------------------------
//! @file   SaveDataTests.cpp
//! @brief  セーブの中身を JSON に書いて読み戻す単体テスト
//-------------------------------------------------------------
#include "TestFramework.hpp"

#include <StardustFever/Data/SaveData.hpp>

#include <cereal/archives/json.hpp>

#include <sstream>

using StardustFever::Core::BigNumber;
using StardustFever::Data::SaveData;

namespace {
    std::string Write(SaveData& data) {
        std::ostringstream stream;
        {
            cereal::JSONOutputArchive archive(stream);
            archive(cereal::make_nvp("SaveData", data));
        }
        return stream.str();
    }

    SaveData Read(const std::string& json) {
        SaveData                 data;
        std::istringstream       stream(json);
        cereal::JSONInputArchive archive(stream);
        archive(cereal::make_nvp("SaveData", data));
        return data;
    }
}    // namespace

SF_TEST(SaveDataRoundTrips) {
    SaveData original;
    original.stardust                    = BigNumber::FromParts(1.25, 120);
    original.upgradeLevels["pin_payout"] = 7;
    original.starLevels["rigel"]         = 3;
    original.cosmicConstants             = BigNumber(42.0);
    original.bigBangCount                = 5;
    original.universeEarned              = BigNumber(3.0e9);
    original.autoFireEnabled             = true;
    original.bgmVolume                   = 0.3f;
    original.audioMuted                  = true;
    original.showKeyHints                = false;

    const SaveData loaded = Read(Write(original));
    SF_EXPECT(loaded.stardust == original.stardust);
    SF_EXPECT(loaded.upgradeLevels.at("pin_payout") == 7);
    SF_EXPECT(loaded.starLevels.at("rigel") == 3);
    SF_EXPECT(loaded.cosmicConstants == BigNumber(42.0));
    SF_EXPECT(loaded.bigBangCount == 5);
    SF_EXPECT(loaded.universeEarned == BigNumber(3.0e9));
    SF_EXPECT(loaded.autoFireEnabled);
    SF_EXPECT(loaded.bgmVolume == 0.3f);
    SF_EXPECT(loaded.audioMuted);
    SF_EXPECT(!loaded.showKeyHints);
}

SF_TEST(SaveDataToleratesMissingFields) {
    // 古い版のセーブ（項目が少ない）でも、書かれている分だけ読めて残りは既定値になる
    const SaveData loaded = Read(R"({"SaveData": {"bigBangCount": 2}})");
    SF_EXPECT(loaded.bigBangCount == 2);
    SF_EXPECT(loaded.stardust.IsZero());
    SF_EXPECT(loaded.upgradeLevels.empty());
    SF_EXPECT(!loaded.autoFireEnabled);
}

SF_TEST(MigrationRenamesLegacyStarIds) {
    // 版 1 のセーブは星を "rigel" のような名前で持っていた。今の ID（hip_24436）へ移し替える
    StardustFever::Data::StarTable stars;
    stars.aliases.push_back({"rigel", "hip_24436"});
    SaveData data =
        Read(R"({"SaveData": {"version": 1, "starLevels": [{"key": "rigel", "value": 2}, {"key": "hip_24436", "value": 3}, {"key": "hip_1", "value": 1}]}})");
    StardustFever::Data::MigrateSaveData(data, stars);
    SF_EXPECT(data.version == StardustFever::Data::kSaveDataVersion);
    SF_EXPECT(data.starLevels.size() == 2);
    SF_EXPECT(data.starLevels.at("hip_24436") == 3);
    SF_EXPECT(data.starLevels.at("hip_1") == 1);
}

SF_TEST(MigrationKeepsCurrentVersion) {
    StardustFever::Data::StarTable stars;
    stars.aliases.push_back({"rigel", "hip_24436"});
    SaveData data;
    data.version             = StardustFever::Data::kSaveDataVersion;
    data.starLevels["rigel"] = 1;
    StardustFever::Data::MigrateSaveData(data, stars);
    // 今の版のセーブは ID を置き換えない
    SF_EXPECT(data.starLevels.contains("rigel"));
}
