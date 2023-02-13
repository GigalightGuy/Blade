include "Dependencies.lua"

workspace "Blade"
	architecture "x86_64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Sandbox"

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "BladeEngine/vendor/GLFW"
group""

include "BladeEngine"

include "Sandbox"