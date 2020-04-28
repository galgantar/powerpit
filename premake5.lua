workspace "powerpit"
	architecture "x86_64"

	configurations
	{
		"Debug",
		"Release"
	}

project "powerpit"
	kind "ConsoleApp"
	language "C++"
	location "powerpit"

	targetdir "builds/%{cfg.buildcfg}/%{cfg.system}"
	objdir "builds/%{cfg.buildcfg}/%{cfg.system}/intermediates"

	pchheader "PCH.h"
	pchsource "%{prj.name}/source/PCH.cpp"

	files
	{
		"%{prj.name}/source/*.h",
		"%{prj.name}/source/*.cpp",

		"%{prj.name}/shaders/*.shader",

		"%{prj.name}/vendor/glad/**.h",
		"%{prj.name}/vendor/glad/**.c",

		"%{prj.name}/vendor/glfw/include/GLFW/**.h",
		"%{prj.name}/vendor/glfw/src/glfw_config.h",
		"%{prj.name}/vendor/glfw/src/context.c",
		"%{prj.name}/vendor/glfw/src/init.c",
		"%{prj.name}/vendor/glfw/src/input.c",
		"%{prj.name}/vendor/glfw/src/monitor.c",
		"%{prj.name}/vendor/glfw/src/vulkan.c",
		"%{prj.name}/vendor/glfw/src/window.c",
		"%{prj.name}/vendor/glfw/src/win32_init.c",
		"%{prj.name}/vendor/glfw/src/win32_joystick.c",
		"%{prj.name}/vendor/glfw/src/win32_monitor.c",
		"%{prj.name}/vendor/glfw/src/win32_time.c",
		"%{prj.name}/vendor/glfw/src/win32_thread.c",
		"%{prj.name}/vendor/glfw/src/win32_window.c",
		"%{prj.name}/vendor/glfw/src/wgl_context.c",
		"%{prj.name}/vendor/glfw/src/egl_context.c",
		"%{prj.name}/vendor/glfw/src/osmesa_context.c",

		"%{prj.name}/vendor/gmath/*.h",
		"%{prj.name}/vendor/gmath/*.hpp",

		"%{prj.name}/vendor/SOIL2/src/SOIL2/*.h",
		"%{prj.name}/vendor/SOIL2/src/SOIL2/*.c"
	}

	defines
	{
		"__x86_64__",

		-- for glfw
		"_GLFW_WIN32",
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/vendor/glfw/include/GLFW",
		"%{prj.name}/vendor/glad/include",
		"%{prj.name}/vendor/gmath",
		"%{prj.name}/vendor/SOIL2/src/SOIL2",
	}

	links
	{
		"opengl32.lib"
	}

	filter "files:**.c"
		flags { "NoPCH" }
