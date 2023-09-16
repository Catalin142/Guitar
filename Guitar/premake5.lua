project "Guitar"
	kind "ConsoleApp"
	language "C"
	cdialect "C17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"src;",
		"%{IncludeDir.SDL}",
	}


	libdirs
    {
        	"$(SolutionDir)Guitar/src/external/sdl2/lib/x86",
	}

	files
	{
		"**.h",
		"**.c"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	links 
	{
		"SDL2.lib",
		"SDL2_image.lib",
		"SDL2_ttf.lib",
		"SDL2main.lib",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "DEBUG=1"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE=1"
		runtime "Release"
		optimize "on"