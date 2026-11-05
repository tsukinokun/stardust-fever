----------------------------------------
-- Stardust Fever のワークスペース。
-- エンジンは External/TsukinoEngine に submodule として置き、
-- include・link・配布物の設定はエンジン側のヘルパーに任せる。
----------------------------------------
include "External/TsukinoEngine/Tools/premake/tsukino.lua"

workspace "StardustFever"
    startproject "StardustFever"
    location ".build"
    tsukino_workspace_defaults()

include "External/TsukinoEngine"

----------------------------------------
-- ゲーム本体
----------------------------------------
project "StardustFever"
    location ".build/StardustFever"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    forceincludes { "pch.h" }

    filter "action:vs*"
        buildoptions { "/permissive-", "/bigobj" }    -- 部品の登録の翻訳単位は cereal のテンプレートが多く、セクション数の上限を超えるため
    filter {}

    pchheader "pch.h"
    pchsource "StardustFever/pch.cpp"

    targetdir ("bin/%{cfg.buildcfg}")
    objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

    files {
        "StardustFever/src/**.cpp",
        "StardustFever/include/**.hpp",
        "StardustFever/pch.h",
        "StardustFever/pch.cpp",
    }

    includedirs {
        "StardustFever",
        "StardustFever/include",
    }

    tsukino_link()
    tsukino_release_payload()
    tsukino_icon("StardustFever/Assets/Icon/App.ico")    -- exe とウィンドウのアイコン（Tools/GenerateAppIcon.py で作る）

    -- Debug は workspace ルートが実行時の基準なので、Assets はその場から読む。
    -- Release は exe の隣が基準になるため、ゲームの Assets も隣へ置く
    filter "configurations:Release"
        postbuildcommands {
            "{COPYDIR} %{wks.location}/../StardustFever/Assets %{cfg.targetdir}/StardustFever/Assets",
        }
    filter {}

----------------------------------------
-- エンジンに依存しない部分（BigNumber など）の単体テスト
----------------------------------------
project "StardustFever.Tests"
    location ".build/StardustFever.Tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/%{cfg.buildcfg}")
    objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

    files {
        "StardustFever.Tests/**.cpp",
        "StardustFever/src/Core/**.cpp",
        "StardustFever/include/StardustFever/Core/**.hpp",
    }

    -- セーブの中身を JSON に書いて読み戻すテストのため、ヘッダだけの cereal と
    -- Tsukino.Core の公開ヘッダ（CerealOptional）を参照する。エンジンのライブラリはリンクしない
    includedirs {
        "StardustFever/include",
        "External/TsukinoEngine/Tsukino.Core/include",
        "External/TsukinoEngine/External/cereal/include",
    }
