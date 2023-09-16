workspace "Guitar"
	architecture "x86"
	configurations { "Debug", "Release" }
	startproject "Guitar"
	flags
	{
		"MultiProcessorCompile"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["SDL"] 	= "%{wks.location}/Guitar/src/external/sdl2/include"

	include "Guitar"