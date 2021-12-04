-- premake5.lua
workspace "dark"
	configurations { "Release" } -- "Debug",

project "dark"
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
	
	filter { "configurations:Release" }
		-- Release should be optimized
		defines { "NDEBUG" }
		optimize "On"
	
	filter {}

	defines { "_CRT_SECURE_NO_WARNINGS" }
	
	files {
		"src/**",
		"../dep/**",
	}

	excludes {
		"",
	}

	includedirs {
		"src/",
		"../dep",
		"../dep/imgui-1.83",
		"../miryks/include",
		"C:/New folder/",
		"C:/New folder/zlib-1.2.11",
		"C:/New folder/lz4-1.9.3/lib",
		"C:/New folder/libpng-1.6.35",
		"C:/New folder/glm-0.9.9.8",
		"C:/New folder/glfw-3.3.4/include",
		"C:/New folder/bullet3-3.17/src"
	}
	
	libdirs {
		"../miryks/lib",
		"C:/New folder/glfw-3.3.4-32/src/Release",
		"C:/New folder/zlib-1.2.11-32/Release",
		"C:/New folder/lz4-1.9.3-32/Release",
		"C:/New folder/bullet3-3.17-32/lib/Release",
		"C:/New folder/libpng-1.6.35-32/Release"
	}

	links {
		"miryks",
		"glfw3",
		"zlibstatic",
		"lz4",
		"libpng16_static"
        --"BulletDynamics","BulletCollision", "LinearMath", "Bullet3Common"
	--	"zlibstatic"
	}