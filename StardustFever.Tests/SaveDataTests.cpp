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
    original.stardust                          = BigNumber::FromParts(1.25, 120);
    original.upgradeLevels["pin_payout"]       = 7;
    original.starLevels["rigel"]               = 3;
    original.constellationCompletions["orion"] = 2;
    original.cosmicConstants                   = BigNumber(42.0);
    original.bigBangCount                      = 5;
    original.universeEarned                    = BigNumber(3.0e9);
    original.autoFireEnabled                   = true;

    const SaveData loaded = Read(Write(original));
    SF_EXPECT(loaded.stardust == original.stardust);
    SF_EXPECT(loaded.upgradeLevels.at("pin_payout") == 7);
    SF_EXPECT(loaded.starLevels.at("rigel") == 3);
    SF_EXPECT(loaded.constellationCompletions.at("orion") == 2);
    SF_EXPECT(loaded.cosmicConstants == BigNumber(42.0));
    SF_EXPECT(loaded.bigBangCount == 5);
    SF_EXPECT(loaded.universeEarned == BigNumber(3.0e9));
    SF_EXPECT(loaded.autoFireEnabled);
}

SF_TEST(SaveDataToleratesMissingFields) {
    // 古い版のセーブ（項目が少ない）でも、書かれている分だけ読めて残りは既定値になる
    const SaveData loaded = Read(R"({"SaveData": {"bigBangCount": 2}})");
    SF_EXPECT(loaded.bigBangCount == 2);
    SF_EXPECT(loaded.stardust.IsZero());
    SF_EXPECT(loaded.upgradeLevels.empty());
    SF_EXPECT(!loaded.autoFireEnabled);
}
