//-------------------------------------------------------------
//! @file   StarMapInputSystem.cpp
//! @brief  StarMapInputSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarMapInputSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/StarMapConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarMapStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/StarMapGeometry.hpp>

#include <Tsukino/BuiltIn/ECS/UI/UICanvas.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::Input::KeyCode;

        //-------------------------------------------------------------
        //! @brief  クリックした所に近い星座を探す
        //! @param  state  [in] 星図の状態
        //! @param  config [in] 星図の調整値
        //! @param  localX [in] クリックした所（星図の枠の中央からのずれ）
        //! @param  localY [in] 同上
        //! @return 見つかった星座の ID（無ければ空）
        //! @note   星座名（星の平均の位置）に近ければその星座、無ければ近い星を線に持つ星座を選ぶ
        //-------------------------------------------------------------
        std::string PickConstellation(const StarMapStateComponent& state, const StarMapConfigComponent& config, double localX, double localY) {
            const Data::StarTable&    stars = Data::GetStarTable();
            const Core::SkyMath::View view  = StarMapGeometry::MakeView(state.raCenter, state.decCenter, config.pixelsPerDegree, state.zoom);
            const double              limit = static_cast<double>(config.clickRadius) * config.clickRadius;
            std::string               best;
            double                    bestDistance = std::numeric_limits<double>::max();
            for(const Data::ConstellationDef& constellation : Data::GetConstellationTable().constellations) {
                double ra  = 0.0;
                double dec = 0.0;
                if(StarMapGeometry::Centroid(constellation, stars, ra, dec)) {
                    const Core::SkyMath::Point point = Core::SkyMath::Project(view, ra, dec);
                    const double               d     = (point.x - localX) * (point.x - localX) + (point.y - localY) * (point.y - localY);
                    if(d < bestDistance) {
                        bestDistance = d;
                        best         = constellation.id;
                    }
                }
                for(const Data::ConstellationStarDef& member : constellation.stars) {
                    const Data::StarDef* star = stars.Find(member.starId);
                    if(!star)
                        continue;
                    const Core::SkyMath::Point point = Core::SkyMath::Project(view, star->ra * 15.0, star->dec);
                    const double               d     = (point.x - localX) * (point.x - localX) + (point.y - localY) * (point.y - localY);
                    if(d < bestDistance) {
                        bestDistance = d;
                        best         = constellation.id;
                    }
                }
            }
            return bestDistance <= limit ? best : std::string();
        }

        //-------------------------------------------------------------
        //! @brief  星座を選び、真ん中に出す
        //! @param  state         [in,out] 星図の状態
        //! @param  constellation [in] 星座
        //! @param  maxDec        [in] 中央に来る赤緯の上限
        //-------------------------------------------------------------
        void Focus(StarMapStateComponent& state, const Data::ConstellationDef& constellation, float maxDec) {
            state.selectedId = constellation.id;
            double ra        = 0.0;
            double dec       = 0.0;
            if(StarMapGeometry::Centroid(constellation, Data::GetStarTable(), ra, dec)) {
                state.raCenter  = ra;
                state.decCenter = std::clamp(dec, -static_cast<double>(maxDec), static_cast<double>(maxDec));
            }
        }
    }    // namespace

    //-------------------------------------------------------------
    // 入力を読んで星図の状態を書き換える
    //-------------------------------------------------------------
    void StarMapInputSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        StarMapStateComponent*                     state   = FindSingleton<StarMapStateComponent>(registry);
        const StarMapConfigComponent*              config  = FindSingleton<StarMapConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!state || !config || !context || !context->inputSystem)
            return;
        const Tsukino::Input::InputSystem& input = *context->inputSystem;

        if(input.IsKeyPressed(KeyCode::Tab) || (state->open && input.IsKeyPressed(KeyCode::Escape))) {
            state->open     = !state->open && input.IsKeyPressed(KeyCode::Tab);
            state->dragging = false;
            ++state->version;
        }
        if(!state->open)
            return;

        const Data::ConstellationTable& table   = Data::GetConstellationTable();
        bool                            changed = false;

        //-------------------------------------------------------------
        // マウス: 枠の中で押して動かすとつかんで動かす。ほとんど動かさずに離したらクリック
        //-------------------------------------------------------------
        int mouseX = 0;
        int mouseY = 0;
        input.GetMousePosition(&mouseX, &mouseY);
        const hlslpp::float2 ui = Tsukino::BuiltIn::ECS::GetUICanvas(registry).ToUI(hlslpp::float2(static_cast<float>(mouseX), static_cast<float>(mouseY)));
        const float          x  = ui.x;
        const float          y  = ui.y;
        const bool   insideMap  = x >= config->mapX && x <= config->mapX + config->mapWidth && y >= config->mapY && y <= config->mapY + config->mapHeight;
        const double pixelsPerDegree = std::max(0.01, static_cast<double>(config->pixelsPerDegree) * state->zoom);

        if(input.IsKeyPressed(KeyCode::LButton) && insideMap) {
            state->dragging  = true;
            state->dragMoved = 0.0f;
            state->lastX     = x;
            state->lastY     = y;
        }
        if(state->dragging && input.IsKeyDown(KeyCode::LButton)) {
            const float dx    = x - state->lastX;
            const float dy    = y - state->lastY;
            state->dragMoved += std::abs(dx) + std::abs(dy);
            if(dx != 0.0f || dy != 0.0f) {
                // 空を手でつかんで動かす向き（右へ動かすと空も右へ動く）
                state->raCenter  += static_cast<double>(dx) / pixelsPerDegree;
                state->decCenter += static_cast<double>(dy) / pixelsPerDegree;
                changed           = true;
            }
            state->lastX = x;
            state->lastY = y;
        }
        if(state->dragging && !input.IsKeyDown(KeyCode::LButton)) {
            state->dragging = false;
            if(state->dragMoved < config->dragThreshold) {
                const double localX = x - (config->mapX + config->mapWidth * 0.5f);
                const double localY = y - (config->mapY + config->mapHeight * 0.5f);
                state->selectedId   = PickConstellation(*state, *config, localX, localY);
                changed             = true;
            }
        }

        //-------------------------------------------------------------
        // 拡大: ホイール（1目盛りで zoomStep 倍）と Q / E
        //-------------------------------------------------------------
        double zoom = state->zoom;
        if(const float wheel = input.GetWheelDelta(); wheel != 0.0f && insideMap)
            zoom *= std::pow(static_cast<double>(config->zoomStep), static_cast<double>(wheel));
        if(input.IsKeyDown(KeyCode::E))
            zoom *= std::pow(static_cast<double>(config->keyZoomSpeed), static_cast<double>(deltaTime));
        if(input.IsKeyDown(KeyCode::Q))
            zoom /= std::pow(static_cast<double>(config->keyZoomSpeed), static_cast<double>(deltaTime));
        zoom = std::clamp(zoom, static_cast<double>(config->minZoom), static_cast<double>(config->maxZoom));
        if(zoom != state->zoom) {
            state->zoom = zoom;
            changed     = true;
        }

        //-------------------------------------------------------------
        // 矢印キーで動かす。拡大しているほどゆっくり動かす
        //-------------------------------------------------------------
        const double step = static_cast<double>(config->keyPanSpeed) * deltaTime / state->zoom;
        if(input.IsKeyDown(KeyCode::Left)) {
            state->raCenter += step;
            changed          = true;
        }
        if(input.IsKeyDown(KeyCode::Right)) {
            state->raCenter -= step;
            changed          = true;
        }
        if(input.IsKeyDown(KeyCode::Up)) {
            state->decCenter += step;
            changed           = true;
        }
        if(input.IsKeyDown(KeyCode::Down)) {
            state->decCenter -= step;
            changed           = true;
        }

        //-------------------------------------------------------------
        // Z / X で前後の星座へ移る（並びは Constellations.json の順＝和名の五十音順）
        //-------------------------------------------------------------
        const bool previous = input.IsKeyPressed(KeyCode::Z);
        const bool next     = input.IsKeyPressed(KeyCode::X);
        if((previous || next) && !table.constellations.empty()) {
            const int count   = static_cast<int>(table.constellations.size());
            int       current = -1;
            for(int i = 0; i < count; ++i) {
                if(table.constellations[i].id == state->selectedId)
                    current = i;
            }
            const int target = current < 0 ? 0 : (current + (next ? 1 : count - 1)) % count;
            Focus(*state, table.constellations[target], config->maxDec);
            changed = true;
        }

        state->raCenter  = std::fmod(std::fmod(state->raCenter, 360.0) + 360.0, 360.0);
        state->decCenter = std::clamp(state->decCenter, -static_cast<double>(config->maxDec), static_cast<double>(config->maxDec));
        if(changed)
            ++state->version;
    }

}    // namespace StardustFever::ECS
