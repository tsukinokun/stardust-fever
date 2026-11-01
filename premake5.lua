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
        buildoptions { "/permissive-" }
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

    includedirs {
        "StardustFever/include",
    }
