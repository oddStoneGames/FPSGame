workspace "FPSGame"
    startproject "FPSGame"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-x64"

include "FPSGame/dependencies/GLFW"
include "FPSGame/dependencies/GLAD"

project "FPSGame"
    location "FPSGame"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/dependencies",
        "FPSGame/dependencies/GLFW/include",
        "FPSGame/dependencies/GLAD/include/GLAD",
        "FPSGame/dependencies/FreeType/include"
    }

    libdirs 
    {
        "FPSGame/dependencies/FreeType/src"
    }

    links
    {
        "freetype.lib",
        "GLFW",
        "GLAD",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "on"