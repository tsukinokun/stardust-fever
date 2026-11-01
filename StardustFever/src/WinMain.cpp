//--------------------------------------------------------------
//! @file   WinMain.cpp
//! @brief  Stardust Fever のエントリポイント
//--------------------------------------------------------------
#include <StardustFever/Scene/PachinkoScene.hpp>

#include <Tsukino/EngineIntegration/EngineAPI.hpp>
#include <Tsukino/EngineIntegration/EngineIntegration.hpp>
#include <Tsukino/Core/Log.hpp>

#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <memory>

//--------------------------------------------------------------
//! @brief  アプリケーションのエントリポイント
//! @return 終了コード
//--------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    //--------------------------------------------------------------
    // 最初にログをファイルへ出す。これが無いと「Prefab file not found」の
    // ような致命的な警告がデバッガの外から一切見えない
    //--------------------------------------------------------------
    Tsukino::Core::Log::SetLogFile("Logs/StardustFever.log");

    Tsukino::EngineIntegration::EngineIntegration engineIntegration;
    if(!engineIntegration.Initialize(1600, 1000, "Stardust Fever")) {
        Tsukino::Core::Log::Error("Failed to initialize EngineIntegration.");
        return EXIT_FAILURE;
    }

    Tsukino::EngineIntegration::EngineAPI engineAPI(engineIntegration.GetContext());
    engineAPI.ChangeScene(std::make_unique<StardustFever::Scene::PachinkoScene>());

    //--------------------------------------------------------------
    // deltaTime は実測値を使う。固定値だと高リフレッシュレートの画面で
    // 物理が実時間より速く進んでしまう
    //--------------------------------------------------------------
    auto lastTime = std::chrono::steady_clock::now();
    while(engineAPI.ProcessMessages()) {
        const auto currentTime = std::chrono::steady_clock::now();
        float      deltaTime   = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime               = currentTime;
        // ウィンドウのドラッグ等で1フレームが極端に長くなったときに玉が壁を抜けないようにする
        deltaTime = std::min(deltaTime, 1.0f / 30.0f);

        engineAPI.Update(deltaTime);
        engineAPI.Render();
    }
    return 0;
}
