-- premake5.lua
workspace "Dark2"
	configurations { "Debug", "Release" }

project "Dark2"
	kind "ConsoleApp"
	language "C++"
	architecture "x86"
	
	--targetdir "bin/%{cfg.buildcfg}"
	targetdir "bin/"
	debugdir "bin/"
	
	location "build"

	buildoptions "/std:c++latest"

	filter { "configurations:Debug" }
		-- We want debug symbols in our debug config
		defines { "DEBUG" }
		symbols "On"
		links {
			"zlibstaticd"
		}
	
	filter { "configurations:Release" }
		-- Release should be optimized
		defines { "NDEBUG" }
		optimize "On"
		links {
			"zlibstatic"
		}
	
	filter {}

	defines { "_CRT_SECURE_NO_WARNINGS" }
	
	files {
		"dep/**",
		"src/**",
		"lib/**",
		"include/**",
	}

	includedirs {
		"dep",
		"src",
		"lib",
		"include",
		"F:/New folder/",
		"F:/New folder/glm",
		"F:/New folder/glfw-3.3/include",
		"F:/New folder/zlib-1.2.11"
	}
	
	libdirs {
		"F:/New folder/glfw-3.3-32/src/%{cfg.longname}",
		"F:/New folder/zlib-1.2.11-32/%{cfg.longname}"
	}

	links {
		"glfw3"
	--	"zlibstatic"
	}