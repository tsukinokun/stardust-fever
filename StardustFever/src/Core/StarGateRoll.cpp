//-------------------------------------------------------------
//! @file   StarGateRoll.cpp
//! @brief  StarGateRoll の実装
//-------------------------------------------------------------
#include <StardustFever/Core/StarGateRoll.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::Core::StarGateRoll
namespace StardustFever::Core::StarGateRoll {

    namespace {
        constexpr float kSlowdown  = 2.5f;     //!< 大きいほど最後が遅い
        constexpr float kLitEnd    = 0.85f;    //!< Hang のこの割合までに灯り終える（残りは間）
        constexpr float kMissShare = 0.55f;    //!< 素直な外れで灯る割合
    }    // namespace

    //-------------------------------------------------------------
    // 抽選全体の長さを求める
    //-------------------------------------------------------------
    float TotalSeconds(const Timeline& timeline, Scenario scenario) {
        return timeline.spin + timeline.hang + (scenario == Scenario::Comeback ? timeline.fake : 0.0f);
    }

    //-------------------------------------------------------------
    // 経過時間から段階を求める
    //-------------------------------------------------------------
    Phase PhaseAt(const Timeline& timeline, Scenario scenario, float elapsed) {
        if(elapsed < timeline.spin)
            return Phase::Spin;
        if(elapsed < timeline.spin + timeline.hang)
            return Phase::Hang;
        if(elapsed < TotalSeconds(timeline, scenario))
            return Phase::Fake;
        return Phase::Done;
    }

    //-------------------------------------------------------------
    // 星座が入れ替わった回数を求める
    //-------------------------------------------------------------
    int ReelStep(float progress, int steps) {
        const float clamped = std::clamp(progress, 0.0f, 1.0f);
        return static_cast<int>((1.0f - std::pow(1.0f - clamped, kSlowdown)) * static_cast<float>(steps));
    }

    //-------------------------------------------------------------
    // 灯っている星の数を求める
    //-------------------------------------------------------------
    int LitStars(float progress, int count, Scenario scenario) {
        int limit = count;
        if(scenario == Scenario::NearMiss || scenario == Scenario::Comeback)
            limit = count - 1;
        else if(scenario == Scenario::Miss)
            limit = static_cast<int>(static_cast<float>(count) * kMissShare);
        limit = std::max(limit, std::min(count, 1));

        const float share = std::clamp(progress / kLitEnd, 0.0f, 1.0f);
        return std::min(limit, static_cast<int>(std::ceil(share * static_cast<float>(count))));
    }

}    // namespace StardustFever::Core::StarGateRoll
