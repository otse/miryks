-- premake5.lua
workspace "miryks"
	configurations { "Release" } -- "Debug",

project "miryks"
	kind "StaticLib"
	language "C++"
	architecture "x86"
	
	targetdir "lib/"
	debugdir "lib/"
	
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
		"../dep/**",
		"src/**",
		"include/**",
	}

	excludes {
		"lib/graveyard/*",
		"src/graveyard/*"
	}

	includedirs {
		"include",
		"src",
		"../dep",
		"../dep/imgui-1.83",
		"C:/New folder/",
		"C:/New folder/zlib-1.2.11",
		"C:/New folder/lz4-1.9.3/lib",

		"C:/New folder/lpng1637",
		"C:/New folder/glm-0.9.9.8",
		"C:/New folder/glfw-3.3.6/include",
		"C:/New folder/bullet3-3.17/src"
	}
	
	libdirs {
		"C:/New folder/glfw-3.3.6-32/src/Release",
		"C:/New folder/zlib-1.2.11-32/Release",
		"C:/New folder/lz4-1.9.3-32/Release",

		"C:/New folder/lpng1637-32/Release",
		"C:/New folder/bullet3-3.17-32/lib/Release",
	}

	links {
		"glfw3",
		"zlibstatic",
		"lz4",
		"libpng16_static"
        --"BulletDynamics","BulletCollision", "LinearMath", "Bullet3Common"
	--	"zlibstatic"
	}