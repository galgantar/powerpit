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

		"%{prj.name}/dependencies/glad/**.h",
		"%{prj.name}/dependencies/glad/**.c",

		"%{prj.name}/dependencies/glfw/include/GLFW/**.h",
		"%{prj.name}/dependencies/glfw/src/glfw_config.h",
		"%{prj.name}/dependencies/glfw/src/context.c",
		"%{prj.name}/dependencies/glfw/src/init.c",
		"%{prj.name}/dependencies/glfw/src/input.c",
		"%{prj.name}/dependencies/glfw/src/monitor.c",
		"%{prj.name}/dependencies/glfw/src/vulkan.c",
		"%{prj.name}/dependencies/glfw/src/window.c",
		"%{prj.name}/dependencies/glfw/src/win32_init.c",
		"%{prj.name}/dependencies/glfw/src/win32_joystick.c",
		"%{prj.name}/dependencies/glfw/src/win32_monitor.c",
		"%{prj.name}/dependencies/glfw/src/win32_time.c",
		"%{prj.name}/dependencies/glfw/src/win32_thread.c",
		"%{prj.name}/dependencies/glfw/src/win32_window.c",
		"%{prj.name}/dependencies/glfw/src/wgl_context.c",
		"%{prj.name}/dependencies/glfw/src/egl_context.c",
		"%{prj.name}/dependencies/glfw/src/osmesa_context.c",

		"%{prj.name}/dependencies/gmath/*.h",
		"%{prj.name}/dependencies/gmath/*.hpp",

		"%{prj.name}/dependencies/SOIL2/src/SOIL2/*.h",
		"%{prj.name}/dependencies/SOIL2/src/SOIL2/*.c"
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
		"%{prj.name}/dependencies/glfw/include/GLFW",
		"%{prj.name}/dependencies/glad/include",
		"%{prj.name}/dependencies/gmath",
		"%{prj.name}/dependencies/SOIL2/src/SOIL2",
	}

	links
	{
		"opengl32.lib"
	}

	filter "files:**.c"
		flags { "NoPCH" }