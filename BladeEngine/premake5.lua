project "BladeEngine"
    kind "StaticLib"
    systemversion "latest"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("..bin/" .. outputdir .. "/%{prj.name}")
    objdir ("..bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "src",
        "vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "GLFW"
    }

    defines
    {
        "GLFW_INCLUDE_NONE"
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