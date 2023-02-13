project "Sandbox"
    kind "ConsoleApp"
    systemversion "latest"
    language "C++"
    cppdialect "C++17"

    targetdir ("..bin/" .. outputdir .. "/%{prj.name}")
    objdir ("..bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/BladeEngine/src",
        "%{wks.location}/BladeEngine/vendor/spdlog/include"
    }

    links
    {
        "BladeEngine"
    }

    filter ("system:windows")
        defines "BLD_PLATFORM_WINDOWS"

    filter ("system:linux")
        defines "BLD_PLATFORM_LINUX"

    filter ("configurations:Debug")
        runtime "Debug"
        defines "BLD_BUILD_DEBUG"

    filter ("configurations:Release")
        runtime "Release"
        defines "BLD_BUILD_RELEASE"