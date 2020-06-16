-- premake5.lua
workspace "Dark2"
	configurations { "Debug", "Release" }

project "Dark2"
	kind "ConsoleApp"
	language "C++"
	architecture "x86_64"
	
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
		"dep/**.h",
		"dep/**.hpp",
		"dep/**.c",
		"dep/**.cpp",
		"include/**.h",
		"include/**.hpp",
		"src/**.c",
		"src/**.cc",
		"src/**.cpp",
		"src/**.h",
		"src/**.hpp",
		"src/**"
	}

	includedirs {
		"dep",
		"src",
		"include",
		"F:/New folder/",
		"F:/New folder/glm",
		"F:/New folder/glfw-3.3/include",
		"F:/New folder/zlib-1.2.11"
	}
	
	libdirs {
		"F:/New folder/glfw-3.3-made/src/%{cfg.longname}",
		"F:/New folder/zlib-1.2.11-made/%{cfg.longname}"
	}

	links {
		"glfw3",
		"zlib"
	}