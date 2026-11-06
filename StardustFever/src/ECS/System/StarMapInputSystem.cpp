//-------------------------------------------------------------
//! @file   StarMapInputSystem.cpp
//! @brief  StarMapInputSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarMapInputSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/SettingsStateComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
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
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::Input::KeyCode;

        constexpr double kScreenHalfHeight = 500.0;    //!< UI の座標の画面の高さの半分

        //-------------------------------------------------------------
        //! @brief  クリックした所に近い星座を探す
        //! @param  state  [in] 星図の状態
        //! @param  config [in] 星図の調整値
        //! @param  localX [in] クリックした所（画面の中央からのずれ）
        //! @param  localY [in] 同上
        //! @param  radius [in] 拾う距離（ピクセル）
        //! @return 見つかった星座の ID（無ければ空）
        //! @note   星座名（星の中心）に近ければその星座、無ければ近い星を線に持つ星座を選ぶ。
        //!         視線より後ろの星は選ばない
        //-------------------------------------------------------------
        std::string PickConstellation(const StarMapStateComponent& state, const StarMapConfigComponent& config, double localX, double localY, double radius) {
            const Data::StarTable& stars = Data::GetStarTable();
            const double           fov   = config.baseFov / std::max(state.zoom, 0.1);
            const double           limit = radius * radius;
            std::string            best;
            double                 bestDistance = std::numeric_limits<double>::max();
            auto                   consider     = [&](const std::string& id, double ra, double dec) {
                Core::SkyMath::Point point;
                if(!Core::SkyMath::ProjectToScreen(state.raCenter, state.decCenter, fov, kScreenHalfHeight, ra, dec, point))
                    return;
                const double d = (point.x - localX) * (point.x - localX) + (point.y - localY) * (point.y - localY);
                if(d < bestDistance) {
                    bestDistance = d;
                    best         = id;
                }
            };
            for(const Data::ConstellationDef& constellation : Data::GetConstellationTable().constellations) {
                double ra  = 0.0;
                double dec = 0.0;
                if(StarMapGeometry::Centroid(constellation, stars, ra, dec))
                    consider(constellation.id, ra, dec);
                for(const Data::ConstellationStarDef& member : constellation.stars) {
                    if(const Data::StarDef* star = stars.Find(member.starId))
                        consider(constellation.id, star->ra * 15.0, star->dec);
                }
            }
            return bestDistance <= limit ? best : std::string();
        }

        //-------------------------------------------------------------
        //! @brief  指定の向きへ滑らかに回り始める
        //! @param  state  [in,out] 星図の状態
        //! @param  ra     [in] 赤経（度）
        //! @param  dec    [in] 赤緯（度）
        //! @param  maxDec [in] 見られる赤緯の上限
        //-------------------------------------------------------------
        void TurnTo(StarMapStateComponent& state, double ra, double dec, float maxDec) {
            state.focusing    = true;
            state.focusRa     = ra;
            state.focusDec    = std::clamp(dec, -static_cast<double>(maxDec), static_cast<double>(maxDec));
            state.raVelocity  = 0.0;
            state.decVelocity = 0.0;
        }

        //-------------------------------------------------------------
        //! @brief  星座の方へ回る
        //! @param  state         [in,out] 星図の状態
        //! @param  constellation [in] 星座
        //! @param  maxDec        [in] 見られる赤緯の上限
        //-------------------------------------------------------------
        void TurnToConstellation(StarMapStateComponent& state, const Data::ConstellationDef& constellation, float maxDec) {
            double ra  = 0.0;
            double dec = 0.0;
            if(StarMapGeometry::Centroid(constellation, Data::GetStarTable(), ra, dec))
                TurnTo(state, ra, dec, maxDec);
        }

        //-------------------------------------------------------------
        //! @brief  グループの星座の真ん中の方へ回る
        //! @param  state  [in,out] 星図の状態
        //! @param  group  [in] グループ
        //! @param  maxDec [in] 見られる赤緯の上限
        //-------------------------------------------------------------
        void TurnToGroup(StarMapStateComponent& state, const Data::ConstellationGroupDef& group, float maxDec) {
            Core::SkyMath::Direction sum{0.0, 0.0, 0.0};
            for(const std::string& id : group.constellationIds) {
                const Data::ConstellationDef* constellation = Data::GetConstellationTable().Find(id);
                double                        ra            = 0.0;
                double                        dec           = 0.0;
                if(constellation && StarMapGeometry::Centroid(*constellation, Data::GetStarTable(), ra, dec)) {
                    const Core::SkyMath::Direction direction  = Core::SkyMath::ToDirection(ra, dec);
                    sum.x                                    += direction.x;
                    sum.y                                    += direction.y;
                    sum.z                                    += direction.z;
                }
            }
            // 空を一周するグループは真ん中が決まらないので、回らない
            if(sum.x * sum.x + sum.y * sum.y + sum.z * sum.z < 1e-6)
                return;
            double ra  = 0.0;
            double dec = 0.0;
            Core::SkyMath::ToRaDec(sum, ra, dec);
            TurnTo(state, ra, dec, maxDec);
        }
    }    // namespace

    StarMapInputSystem::StarMapInputSystem(Tsukino::ECS::EventBus& eventBus)
        : m_actions(eventBus) {
    }

    //-------------------------------------------------------------
    // 入力を読んで星図の状態を書き換える
    //-------------------------------------------------------------
    void StarMapInputSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        //-------------------------------------------------------------
        // ボタンの知らせ。キーと同じ処理へつなぐ
        //-------------------------------------------------------------
        bool openMap      = false;
        bool openGroups   = false;
        bool closeMap     = false;
        bool toggleGroups = false;
        bool previousItem = false;
        bool nextItem     = false;
        int  pickedGroup  = -1;
        for(const UiActionEvent& action : m_actions.Drain()) {
            openMap      = openMap || action.action == UiAction::OpenMap;
            openGroups   = openGroups || action.action == UiAction::OpenGroups;
            closeMap     = closeMap || action.action == UiAction::MapClose;
            toggleGroups = toggleGroups || action.action == UiAction::MapToggleGroups;
            previousItem = previousItem || action.action == UiAction::MapPrevious;
            nextItem     = nextItem || action.action == UiAction::MapNext;
            if(action.action == UiAction::MapSelectGroup)
                pickedGroup = action.index;
        }

        StarMapStateComponent*                     state   = FindSingleton<StarMapStateComponent>(registry);
        const StarMapConfigComponent*              config  = FindSingleton<StarMapConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!state || !config || !context || !context->inputSystem)
            return;
        const Tsukino::Input::InputSystem& input = *context->inputSystem;

        // 設定の画面の間は星図を動かさない
        if(const SettingsStateComponent* settings = FindSingleton<SettingsStateComponent>(registry); settings && settings->open)
            return;
        if(const PrestigeStateComponent* prestige = FindSingleton<PrestigeStateComponent>(registry); prestige && prestige->confirmOpen)
            return;

        const bool tabPressed = input.IsKeyPressed(KeyCode::Tab) || openMap;
        if(tabPressed || (state->open && (closeMap || input.IsKeyPressed(KeyCode::Escape)))) {
            state->open      = !state->open && tabPressed;
            state->groupMode = false;    // Tab はいつも星座の画面から始める
            state->dragging  = false;
            ++state->version;

            //-------------------------------------------------------------
            // まだ見ていない星があれば、開いたら一番新しい星を見せ、閉じたら見たことにする
            //-------------------------------------------------------------
            if(StarCollectionComponent* collection = FindSingleton<StarCollectionComponent>(registry)) {
                if(!state->open) {
                    collection->unseenStars.clear();
                } else if(!collection->unseenStars.empty()) {
                    if(const Data::StarDef* star = Data::GetStarTable().Find(collection->unseenStars.back())) {
                        state->raCenter   = star->ra * 15.0;
                        state->decCenter  = std::clamp(star->dec, -static_cast<double>(config->maxDec), static_cast<double>(config->maxDec));
                        state->selectedId = star->constellationId;
                        state->rowScroll  = 0;
                        state->focusing   = false;
                    }
                }
            }
        }
        if(!state->open) {
            // 遊んでいる画面からは、G か右のパネルの「そろったグループ」でグループの一覧を直接開ける
            if(input.IsKeyPressed(KeyCode::G) || openGroups) {
                state->open      = true;
                state->groupMode = true;
                state->dragging  = false;
                ++state->version;
            }
            return;
        }

        const Data::ConstellationTable& table   = Data::GetConstellationTable();
        bool                            changed = false;

        //-------------------------------------------------------------
        // マウス: 空の上で押して動かすと見回す。ほとんど動かさずに離したらクリック
        //-------------------------------------------------------------
        int mouseX = 0;
        int mouseY = 0;
        input.GetMousePosition(&mouseX, &mouseY);
        const hlslpp::float2 ui    = Tsukino::BuiltIn::ECS::GetUICanvas(registry).ToUI(hlslpp::float2(static_cast<float>(mouseX), static_cast<float>(mouseY)));
        const float          x     = ui.x;
        const float          y     = ui.y;
        const bool           inSky = x < config->panelX;
        const double         fov   = config->baseFov / std::max(state->zoom, 0.1);
        // 画面の1ピクセルが空の何度か。極に近いほど赤経の円は小さいので、同じ手の動きで大きく回す
        const double degreesPerPixel = fov / (kScreenHalfHeight * 2.0);
        const double raStretch       = 1.0 / std::max(0.2, std::cos(state->decCenter * 3.14159265358979323846 / 180.0));

        if(input.IsKeyPressed(KeyCode::LButton) && inSky) {
            state->dragging    = true;
            state->dragMoved   = 0.0f;
            state->lastX       = x;
            state->lastY       = y;
            state->focusing    = false;
            state->raVelocity  = 0.0;
            state->decVelocity = 0.0;
        }
        if(state->dragging && input.IsKeyDown(KeyCode::LButton)) {
            const float dx    = x - state->lastX;
            const float dy    = y - state->lastY;
            state->dragMoved += std::abs(dx) + std::abs(dy);
            // 空を手でつかんで動かす向き（右へ動かすと空も右へ動く）
            const double ra   = dx * degreesPerPixel * raStretch;
            const double dec  = dy * degreesPerPixel;
            state->raCenter  += ra;
            state->decCenter += dec;
            // 手を離したときの勢いにするため、速さをならして覚える
            if(deltaTime > 0.0f) {
                state->raVelocity  += (ra / deltaTime - state->raVelocity) * 0.35;
                state->decVelocity += (dec / deltaTime - state->decVelocity) * 0.35;
            }
            state->lastX = x;
            state->lastY = y;
        }
        if(state->dragging && !input.IsKeyDown(KeyCode::LButton)) {
            state->dragging = false;
            if(state->dragMoved < config->dragThreshold) {
                state->raVelocity  = 0.0;
                state->decVelocity = 0.0;
                // クリックで星座を選んだら、星座の詳しい中身に戻し、その方へ回る
                state->selectedId = PickConstellation(*state, *config, x - 800.0, y - kScreenHalfHeight, config->clickRadius);
                state->rowScroll  = 0;
                if(const Data::ConstellationDef* picked = table.Find(state->selectedId)) {
                    state->groupMode = false;
                    TurnToConstellation(*state, *picked, config->maxDec);
                }
                changed = true;
            }
        }

        //-------------------------------------------------------------
        // 手を離した後の勢い
        //-------------------------------------------------------------
        if(!state->dragging) {
            const double decay  = std::exp(-static_cast<double>(config->inertiaFriction) * deltaTime);
            state->raCenter    += state->raVelocity * deltaTime;
            state->decCenter   += state->decVelocity * deltaTime;
            state->raVelocity  *= decay;
            state->decVelocity *= decay;
        }

        //-------------------------------------------------------------
        // 拡大: ホイール（1目盛りで zoomStep 倍）と Q / E
        //-------------------------------------------------------------
        double zoom = state->zoom;
        if(const float wheel = input.GetWheelDelta(); wheel != 0.0f) {
            if(inSky) {
                zoom *= std::pow(static_cast<double>(config->zoomStep), static_cast<double>(wheel));
            } else {
                // パネルの上では、文章と星の一覧を送る
                const int scroll = std::clamp(state->rowScroll - static_cast<int>(wheel) * 3, 0, state->rowScrollMax);
                changed          = changed || scroll != state->rowScroll;
                state->rowScroll = scroll;
            }
        }
        if(input.IsKeyDown(KeyCode::E))
            zoom *= std::pow(static_cast<double>(config->keyZoomSpeed), static_cast<double>(deltaTime));
        if(input.IsKeyDown(KeyCode::Q))
            zoom /= std::pow(static_cast<double>(config->keyZoomSpeed), static_cast<double>(deltaTime));
        zoom = std::clamp(zoom, static_cast<double>(config->minZoom), static_cast<double>(config->maxZoom));
        // 色と文字を並べ直すのは重いので、星座名の出し入れが変わるときだけにする
        if((zoom >= config->labelMinZoom) != (state->zoom >= config->labelMinZoom))
            changed = true;
        state->zoom = zoom;

        //-------------------------------------------------------------
        // 矢印キーで見回す。拡大しているほどゆっくり動かす
        //-------------------------------------------------------------
        const double step  = static_cast<double>(config->keyPanSpeed) * deltaTime / state->zoom;
        const bool   left  = input.IsKeyDown(KeyCode::Left) || input.IsKeyDown(KeyCode::A);
        const bool   right = input.IsKeyDown(KeyCode::Right) || input.IsKeyDown(KeyCode::D);
        const bool   up    = input.IsKeyDown(KeyCode::Up) || input.IsKeyDown(KeyCode::W);
        const bool   down  = input.IsKeyDown(KeyCode::Down) || input.IsKeyDown(KeyCode::S);
        if(left || right || up || down)
            state->focusing = false;
        state->raCenter  += (left ? step * raStretch : 0.0) - (right ? step * raStretch : 0.0);
        state->decCenter += (up ? step : 0.0) - (down ? step : 0.0);

        //-------------------------------------------------------------
        // G でグループの一覧と星座の詳しい中身を切り替える。グループの一覧では Z / X で前後のグループへ移る
        //-------------------------------------------------------------
        if(input.IsKeyPressed(KeyCode::G) || toggleGroups) {
            state->groupMode = !state->groupMode;
            changed          = true;
        }
        const bool previous = input.IsKeyPressed(KeyCode::Z) || previousItem;
        const bool next     = input.IsKeyPressed(KeyCode::X) || nextItem;
        if(state->groupMode) {
            const Data::ConstellationGroupTable& groups     = Data::GetConstellationGroupTable();
            const int                            groupCount = static_cast<int>(groups.groups.size());
            if(pickedGroup >= 0 && pickedGroup < groupCount) {
                state->selectedGroup = pickedGroup;
                TurnToGroup(*state, groups.groups[static_cast<size_t>(pickedGroup)], config->maxDec);
                changed = true;
            }
            if((previous || next) && groupCount > 0) {
                state->selectedGroup = (std::clamp(state->selectedGroup, 0, groupCount - 1) + (next ? 1 : groupCount - 1)) % groupCount;
                TurnToGroup(*state, groups.groups[static_cast<size_t>(state->selectedGroup)], config->maxDec);
                changed = true;
            }
        }

        //-------------------------------------------------------------
        // Z / X で前後の星座へ移る（並びは Constellations.json の順＝和名の五十音順）
        //-------------------------------------------------------------
        if(!state->groupMode && (previous || next) && !table.constellations.empty()) {
            const int count   = static_cast<int>(table.constellations.size());
            int       current = -1;
            for(int i = 0; i < count; ++i) {
                if(table.constellations[i].id == state->selectedId)
                    current = i;
            }
            const int target  = current < 0 ? 0 : (current + (next ? 1 : count - 1)) % count;
            state->selectedId = table.constellations[target].id;
            state->rowScroll  = 0;
            TurnToConstellation(*state, table.constellations[target], config->maxDec);
            changed = true;
        }

        //-------------------------------------------------------------
        // 選んだ星座へ、近づくほどゆっくり回る
        //-------------------------------------------------------------
        if(state->focusing) {
            const double blend  = 1.0 - std::exp(-static_cast<double>(config->focusSpeed) * deltaTime);
            const double dRa    = Core::SkyMath::Wrap180(state->focusRa - state->raCenter);
            const double dDec   = state->focusDec - state->decCenter;
            state->raCenter    += dRa * blend;
            state->decCenter   += dDec * blend;
            if(std::abs(dRa) < 0.02 && std::abs(dDec) < 0.02)
                state->focusing = false;
        }

        //-------------------------------------------------------------
        // R / F でパネルを送る（パネルの上のホイールと同じ）
        //-------------------------------------------------------------
        const int scrollKey = (input.IsKeyPressed(KeyCode::F) ? 3 : 0) - (input.IsKeyPressed(KeyCode::R) ? 3 : 0);
        if(scrollKey != 0) {
            const int scroll = std::clamp(state->rowScroll + scrollKey, 0, state->rowScrollMax);
            changed          = changed || scroll != state->rowScroll;
            state->rowScroll = scroll;
        }

        //-------------------------------------------------------------
        // 中央の照準。一番近い星座を拾い、Space・Enter で選ぶ
        //-------------------------------------------------------------
        state->reticleId = PickConstellation(*state, *config, 0.0, 0.0, config->reticleRadius);
        if((input.IsKeyPressed(KeyCode::Space) || input.IsKeyPressed(KeyCode::Enter)) && !state->reticleId.empty()) {
            if(const Data::ConstellationDef* picked = table.Find(state->reticleId)) {
                state->selectedId = picked->id;
                state->groupMode  = false;
                state->rowScroll  = 0;
                TurnToConstellation(*state, *picked, config->maxDec);
                changed = true;
            }
        }

        //-------------------------------------------------------------
        // C / V で選んだ星座の星を明るい順に移り、その星の方へ回る
        //-------------------------------------------------------------
        if(state->selectedId != m_focusConstellation) {
            m_focusConstellation = state->selectedId;
            state->focusStar     = -1;
        }
        const bool previousStar = input.IsKeyPressed(KeyCode::C);
        const bool nextStar     = input.IsKeyPressed(KeyCode::V);
        if(previousStar || nextStar) {
            if(const Data::ConstellationDef* selected = table.Find(state->selectedId)) {
                const Data::StarTable&            starTable = Data::GetStarTable();
                std::vector<const Data::StarDef*> members;
                for(const Data::ConstellationStarDef& member : selected->stars) {
                    if(const Data::StarDef* star = starTable.Find(member.starId))
                        members.push_back(star);
                }
                std::sort(members.begin(), members.end(), [](const Data::StarDef* a, const Data::StarDef* b) { return a->magnitude < b->magnitude; });
                if(!members.empty()) {
                    const int count   = static_cast<int>(members.size());
                    int       current = -1;
                    for(int i = 0; i < count; ++i) {
                        if(static_cast<int>(members[static_cast<size_t>(i)] - starTable.stars.data()) == state->focusStar)
                            current = i;
                    }
                    const int            target = current < 0 ? 0 : (current + (nextStar ? 1 : count - 1)) % count;
                    const Data::StarDef* star   = members[static_cast<size_t>(target)];
                    state->focusStar            = static_cast<int>(star - starTable.stars.data());
                    TurnTo(*state, star->ra * 15.0, star->dec, config->maxDec);
                }
            }
        }

        //-------------------------------------------------------------
        // マウスの下の星を探す。回している最中は吹き出しを出さない
        //-------------------------------------------------------------
        state->hoverStar = -1;
        state->hoverX    = x;
        state->hoverY    = y;
        if(inSky && !state->dragging) {
            const std::vector<Data::StarDef>& stars = Data::GetStarTable().stars;
            double                            best  = static_cast<double>(config->hoverRadius) * config->hoverRadius;
            const double                      view  = config->baseFov / std::max(state->zoom, 0.1);
            for(size_t i = 0; i < stars.size(); ++i) {
                Core::SkyMath::Point point;
                if(!Core::SkyMath::ProjectToScreen(state->raCenter, state->decCenter, view, kScreenHalfHeight, stars[i].ra * 15.0, stars[i].dec, point))
                    continue;
                const double dx = point.x - (x - 800.0);
                const double dy = point.y - (y - kScreenHalfHeight);
                if(dx * dx + dy * dy <= best) {
                    best             = dx * dx + dy * dy;
                    state->hoverStar = static_cast<int>(i);
                }
            }
        }

        state->raCenter  = std::fmod(std::fmod(state->raCenter, 360.0) + 360.0, 360.0);
        state->decCenter = std::clamp(state->decCenter, -static_cast<double>(config->maxDec), static_cast<double>(config->maxDec));
        if(changed)
            ++state->version;
    }

}    // namespace StardustFever::ECS
