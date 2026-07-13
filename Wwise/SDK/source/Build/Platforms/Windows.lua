--[[----------------------------------------------------------------------------
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2026 Audiokinetic Inc.
------------------------------------------------------------------------------]]


if not AK then AK = {} end
if not AK.Platforms then AK.Platforms = {} end

AK.Platforms.Windows =
{
	name = "Windows",
	directories = {
		src = {
			__default__ = "Win32",
			CommunicationCentral = "PC",
			IntegrationDemo = "Windows",
			DLLDemo =  "Windows",
			AkSoundEngineDLL = "Win32",
		},
		project = {
			__default__ = "Win32",
			GameSimulator = "PC",
			CommunicationCentral = "PC",
			LuaLib = "PC",
			ToLuaLib = "PC",
			IntegrationDemoSln = "Windows",
			DLLDemoSln =  "Windows",
		},
		simd = "SIMD",
		lualib = "Win32",
		lowlevelio = "Win32",
		luasln = "GameSimulator/source/",
	},
	kinds = {
		GameSimulator = "ConsoleApp",
		IntegrationDemo = "WindowedApp",
		DLLDemo = "ConsoleApp",
	},
	suffix = {
		__default__ = "Windows",
		CommunicationCentral = "PC",
		GameSimulator = "PC",
		LuaLib = "PC",
		ToLuaLib = "PC",
		LuaSolutions = "PC",
		AllEffectsSln = "PC",
		SamplePluginsSln = "PC",
		SourceControlSln = "PC",
		FilePackagerSln = "PC",
		AkStreamMgrSln = "PC",
	},

	configurations =
	{
		"Debug",
		"Debug(StaticCRT)",
		"Profile",
		"Profile(StaticCRT)" ,
		"Release",
		"Release(StaticCRT)",
	},
	platforms = { "Win32", "x64", "Arm64" },
	avx2OptionalArchs = { "x64" },
	avxOptionalArchs = { "x64" },

	features = {
		"Motion",
		"iZotope",
		"UnitTests",
		"Catch2",
		"SampleSink",
		"IntegrationDemo",
		"SampleClient",
		"ProjectDatabase",
		"DynamicLibraries",
		"SoundEngineDLL",
		"fastcall",
		"DLLDemo",
		"libcurl",
		"GME",
		"speechEngine",
		"OpenXR",
		--"SDL", "SDLRendering", "SDLInput", -- Uncomment this line to test SDL rendering and input on Windows
	},
	validActions = { "vs2015", "vs2017", "vs2019", "vs2022", "export-compile-commands" },
	useLinkGroups = false,

	AdditionalSoundEngineProjects = function()
		return {}
	end,
	AddActionSuffixToDllProjects = true,

	-- API
	---------------------------------
	ImportAdditionalWwiseSDKProjects = function()
	end,

	-- Project factory. Creates "StaticLib" target by default. Static libs (only) are added to the global list of targets.
	-- Other target types supported by premake are "WindowedApp", "ConsoleApp" and "SharedLib".
	-- Upon returning from this method, the current scope is the newly created project.
	CreateProject = function(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		verbosef("        Creating project: %s", in_targetName)

		-- Make sure that directory exist
		os.mkdir(AkMakeAbsolute(in_projectLocation))

		-- Create project
		local prj = project(in_targetName)
			if not _AK_BUILD_AUTHORING then
				platforms(AK.Platforms.Windows.platforms)
			end
			location(AkRelativeToCwd(in_projectLocation))
			targetname(in_targetName)
			if in_targetType == nil or in_targetType == "StaticLib" then
				kind("StaticLib")
				-- Add to global table
				_AK_TARGETS[in_targetName] = in_fileName
			else
				kind(in_targetType)
			end
			language("C++")
			cppdialect("C++17")
			uuid(GenerateUuid(in_fileName))
			filename(in_fileName)
			symbols "On"
			symbolspath "$(OutDir)$(TargetName).pdb"
			justmycode "Off"

			flags {
				-- We never want .user files, we always want .filters files.
				"OmitUserFiles",
				"ForceFiltersFiles",
				-- Enable multiprocess compilation wherever possible
				"MultiProcessorCompile",
			}
			fatalwarnings "All"

			-- Common flags.
			characterset "Unicode"
			buildoptions { "/utf-8" }
			exceptionhandling "Default"

			-- Precompiled headers.
			if _OPTIONS["nopch"] then
				flags { "NoPCH" }
				pchheader("")
				pchsource("")
			elseif pathPCH ~= nil then
				files
				{
					AkRelativeToCwd(pathPCH) .. "stdafx.cpp",
					AkRelativeToCwd(pathPCH) .. "stdafx.h",
				}
				pchheader "stdafx.h"
				pchsource ( AkRelativeToCwd(pathPCH) .. "stdafx.cpp" )
			end

			if not _AK_BUILD_AUTHORING then
				if _OPTIONS["lowmemorytesting"] then
					defines "MSTC_SYSTEMATIC_MEMORY_STRESS"
				end
			end

			if _OPTIONS["floatingpointexception"] then
				floatingpoint "Precise"
				floatingpointexceptions 'On'
				buildoptions { "/wd4305" } -- Disable loss-of-precision conversion warnings
			end

			-- Standard configuration settings.
			filter ("Debug*")
				defines "_DEBUG"

			filter ("Profile*")
				defines ({"NDEBUG", "AK_ENABLE_ASSERTS"})
				optimize ("Speed")

			filter ("Release*")
				defines "NDEBUG"
				optimize ("Speed")

			filter {}

			-- Set up static library directory
			local wwiseRoot = AkRelativeToCwd(_AK_ROOT_DIR .. "../../../")
			libdirs {wwiseRoot .. "SDK/$(Platform)" .. in_suffix .. "/$(Configuration)/lib"}

			debugformat "c7"
			editandcontinue "Off"

			-- Only vs2017+ require specifying Full symbols
			filter ("Debug*", "action:not vs2015")
				symbols "Full"

			filter {"options:addresssanitizer"}
				symbols "FastLink"
				flags {"NoIncrementalLink"}

			filter {"options:fastlink"}
				symbols "FastLink"

			filter {}

			if not _AK_BUILD_AUTHORING then
			-- Note: The AuthoringRelease config is "profile", really. It must not be AK_OPTIMIZED.
			filter "Release*"
				defines "AK_OPTIMIZED"
			end

			filter "platforms:Win32"
				architecture "x86"
				defines "WIN32"
				vectorextensions "SSE2"
			filter "platforms:x64"
				architecture "x86_64"
				defines "WIN64"
			filter "platforms:Arm64"
				architecture "ARM64"
				defines "ARM64"
			filter {}

			-- For customer convenience, targeted Windows SDK depends on what is available in the Visual Studio installer
			filter "action:vs2015"
				systemversion "10.0.14393.0" -- Max version available in VS 2015 installer
			
			filter "action:vs2017"
				systemversion "10.0.17763.0" -- Max version available in VS 2017 installer
			
			filter "action:vs2019"
				systemversion "10.0.20348.0" -- Max version available in VS 2019 installer

			filter "action:vs2022"
				systemversion "10.0.22621.0"

			filter {}

			defines "WIN32_LEAN_AND_MEAN"

			-- Style sheets.
			local ssext = ".props"

			if in_targetType == "SharedLib" then
				if _AK_BUILD_AUTHORING then
					filter "Debug*"
						vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/Debug" .. GetSuffixFromCurrentAction() .. ssext)
					filter "Profile* or Release*"
						vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/NDebug" .. GetSuffixFromCurrentAction() .. ssext)
				else
					filter "Debug*"
						vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/Debug_StaticCRT" .. in_suffix .. ssext)
					filter "Profile* or Release*"
						vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/NDebug_StaticCRT" .. in_suffix .. ssext)
				end

			else
				filter "*Debug"
					vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/Debug" .. in_suffix .. ssext)
				filter "*Debug(StaticCRT)*"
					vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/Debug_StaticCRT" .. in_suffix .. ssext)
				filter "*Profile or *Release"
					vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/NDebug" .. in_suffix .. ssext)
				filter "*Profile(StaticCRT)* or *Release(StaticCRT)*"
					vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/NDebug_StaticCRT" .. in_suffix .. ssext)
			end

			DisablePropSheetElements()
			filter {}
				removeelements {
					"TargetExt"
				}

			-- Set the scope back to current project
			project(in_targetName)
		
		ApplyPlatformExceptions(prj.name, prj)

		return prj
	end,

	-- Plugin factory.
	-- Upon returning from this method, the current scope is the newly created plugin project.
	CreatePlugin = function(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		local prj = AK.Platforms.Windows.CreateProject(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		return prj
	end,

	SetOutputDirectory = function (in_root, in_config, in_suffix, targettype)
		local root = AkRelativeToCwd(in_root)
		local outFolder = "$(Platform)" .. in_suffix .. "/%{cfg.buildcfg}/"
		targetdir (root .. outFolder .. targettype)
		objdir ("!" .. root .. outFolder .. "obj/%{prj.name}")
	end,

	-- function to include an additional assembly file (.a for most gcc/clang platforms, .libs for msvc platforms) into the build target.
	-- This is only intended for, and supported for, StaticLib targets. If you want additional libs for DLLs and EXEs, use `links`.
	-- in_suffix should be the normal platform suffix (eg "_vc170" for Microsoft platforms)
	-- in_dir is the directory to fetch the libs from. Should typically be "AkRelativeToCwd .. (Proj)"
	-- in_assemblyName is the common name of the assembly (eg "PluginCore")
	ArchiveAdditionalLibs = function (in_suffix, in_dir, in_assemblyName)
		local archiveCmd =  "lib /OUT:%{cfg.buildtarget.relpath} %{cfg.buildtarget.relpath} %[" .. in_dir .. "]/$(Platform)" .. in_suffix
		local assemblyFilePath = "lib/%{cfg.targetprefix}" .. in_assemblyName .. "%{cfg.targetextension}"

		filter "Debug*"
			postbuildcommands { archiveCmd .. "/Debug/" .. assemblyFilePath }
		filter "Profile*"
			postbuildcommands { archiveCmd .. "/Profile/" .. assemblyFilePath }
		filter "Release*"
			postbuildcommands { archiveCmd .. "/Release/" .. assemblyFilePath }
		filter {}
	end,

	Exceptions = {
		AkSoundEngine = function(prj)
			includedirs {
				"$(FrameworkSdkDir)/include/um"
			}
			local prjLoc = AkRelativeToCwd(prj.location)
			if not _AK_BUILD_AUTHORING then
				filter { "files:" .. prjLoc .. "/../../SoundEngineProxy/**.cpp", "Release*" }
					flags { "ExcludeFromBuild" }
				filter {}
			end
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
			filter "Debug*"
				links "DbgHelp"
			filter "Profile*"
				links "DbgHelp"

			filter{}
		end,
		AkSoundEngineDLL = function(prj)
			links {	"msacm32", "ws2_32" }
			runtime "Debug"
		end,

		AkSoundEngineTests = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)

			filter "Debug*"
				links "ws2_32"
			filter "Profile*"
				links "ws2_32"
			filter {}

			files {
				prjLoc .. "/../../../../samples/SoundEngine/Win32/AkPlatformProfilerHooks.cpp",
			}
		end,
		AkMemoryMgr = function(prj)
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
		end,
		AkSpatialAudio = function(prj)
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
		end,
		AkStreamMgr = function(prj)
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
		end,
		AkVorbisDecoder = function(prj)
			filter "*Debug or *Profile or *Release"
				defines({"AKSOUNDENGINE_DLL"})
			filter {}
		end,
		AkOpusDecoder = function(prj)
			filter { "*Debug or *Profile or *Release" }
				defines({"AKSOUNDENGINE_DLL"})
			filter {}
		end,
		AkMotionSink = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs
			{
				prjLoc .. "../../../../../../../../Partners/Sony/libScePad/include",
			}
		end,
		AkSink = function(prj)
		end,
		AkConvolutionReverbFX = function(prj)
			defines({"AK_USE_PREFETCH"})
		end,
		iZTrashBoxModelerFX = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			files {
				prjLoc .. "/../../../../iZBaseConsole/src/iZBase/Util/CriticalSection.*",
			}
		end,

		FastCallTest = function(prj)
			links {
				"ws2_32",
				"shlwapi",
				"Msacm32",
				"Dbghelp",
				"Winmm",
			}
		end,

		GameSimulator = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			local integrationDemoLocation = prjLoc .. "/../../../../SDK/samples/IntegrationDemo"

			files {
				prjLoc .. "/*.rc",
				integrationDemoLocation .. "/Windows/InputMgr.*",
				integrationDemoLocation .. "/MenuSystem/UniversalInput.*",
				integrationDemoLocation .. "/Common/Helpers.cpp",
				integrationDemoLocation .. "/../SoundEngine/Win32/AkPlatformProfilerHooks.cpp",
			}

			includedirs {
				prjLoc .. "/../../../../SDK/samples/3rdParty/microsoft",
				prjLoc .. "/../../../../SDK/samples/3rdParty/microsoft/GameInput/v0",
			}

			entrypoint "mainCRTStartup"

			-- lua libs
			links {
				"LuaLib",
				"ToLuaLib",

				"ws2_32",
				"shlwapi",
				"Msacm32",
				"Dbghelp",
				"Winmm",

				"iZHybridReverbFX",
				"iZTrashBoxModelerFX",
				"iZTrashDelayFX",
				"iZTrashDistortionFX",
				"iZTrashDynamicsFX",
				"iZTrashFiltersFX", 
				"iZTrashMultibandDistortionFX",

				"AkSink"
			}

			if PlatformSupports("SDLRendering") then
				-- SDL2 on Windows requires additional link-time dependencies
				links { "Version", "Imm32", "Setupapi" }
			end

			libdirs {
				prjLoc .. "/../../../../Authoring/$(Platform)/$(Configuration)/lib"
			}

			-- adding support for libscepad
			filter "platforms:x64"
				includedirs
				{
					prjLoc .. "/../../../../Partners/Sony/libScePad/include/",
				}
				libdirs
				{
					prjLoc .. "/../../../../Partners/Sony/libScePad/lib/",
				}
			filter {}
			
			-- Copy all extra lua files into one temporary file, build it into AkLuaFramework.cpp, then remove the temp file
			prebuildcommands("type ..\\..\\..\\Scripts\\audiokinetic\\*.lua > .\\AkLuaCombined.lua")		
			prebuildcommands("..\\..\\..\\..\\Tools\\Win32\\bin\\lua2c .\\AkLuaCombined.lua ..\\..\\src\\libraries\\Common\\AkLuaFramework.cpp")
			prebuildcommands("del /f .\\AkLuaCombined.lua")

			-- Propsheet to set the default debugging working directory to AutomatedTests for convenience
			vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/Win32/Windows-GameSimulator.props")
		end,
		IntegrationDemo = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			local integrationDemoLocation = prjLoc .. "/../../../../SDK/samples/IntegrationDemo"
			local autoGenLoc = '%{cfg.objdir}/AutoGen/'

			local use_d3d12 = false

			entrypoint "WinMainCRTStartup"
			includedirs
			{
				prjLoc .. "/../../../../SDK/samples/3rdParty/microsoft",
				prjLoc .. "/../../../../SDK/samples/3rdParty/microsoft/GameInput/v0",
				autoGenLoc,
			}

			-- System
			links {
				"ws2_32",
				"Msacm32",
				"Dbghelp",
				"Winmm",
			}
			if PlatformSupports("SDLRendering") then
				-- SDL2 on Windows requires additional link-time dependencies
				links { "Version", "Imm32", "Setupapi" }
			else
				if use_d3d12 then
					includedirs { prjLoc .. "/D3D12/" }
					links { "d3d12", "dxgi", "dxguid" }
					files { prjLoc .. "/D3D12/*" }
					defines { "INTDEMO_RENDER_D3D12" }
				else
					links { "d3d11" }
					defines { "INTDEMO_RENDER_D3D11" }
				end
			end

			filter "Debug*"
				links "CommunicationCentral"
			filter "Profile*"
				links "CommunicationCentral"
			filter {}

			-- Custom build step to compile HLSL into header files
			files { prjLoc .. "/Shaders/*.hlsl" }
			shaderobjectfileoutput ""
			shadermodel "5.0"

			filter 'files:**.hlsl'
				flags "ExcludeFromBuild"
			filter 'files:**Vs.hlsl'
				removeflags "ExcludeFromBuild"
				shadertype "Vertex"
				shaderentry "VsMain"
				shaderheaderfileoutput ("" .. autoGenLoc .. "%{file.basename}.h")
			filter 'files:**Ps.hlsl'
				removeflags "ExcludeFromBuild"
				shadertype "Pixel"
				shaderentry "PsMain"
				shaderheaderfileoutput ("" .. autoGenLoc .. "%{file.basename}.h")
			filter{}
			prebuildcommands("if not exist \"" .. autoGenLoc .. "\" mkdir \"" .. autoGenLoc .. "\"")

			-- adding support for libscepad
			filter "platforms:x64"
				includedirs
				{
					prjLoc .. "/../../../../Partners/Sony/libScePad/include/",
				}
				libdirs
				{
					prjLoc .. "/../../../../Partners/Sony/libScePad/lib/",
				}
			filter {}
		end,
		SDL2 = function(prj)
			local SDL2 = require("SDL2")
			local sourceLocation = SDL2.GetSourcePath()
			files {
				-- These are validated against configure.ac
				AkRelativeToCwd(sourceLocation) .. "/src/core/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/misc/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/locale/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/video/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/audio/winmm/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/audio/directsound/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/audio/wasapi/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/joystick/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/haptic/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/sensor/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/power/windows/SDL_syspower.c",
				AkRelativeToCwd(sourceLocation) .. "/src/filesystem/windows/SDL_sysfilesystem.c",
				AkRelativeToCwd(sourceLocation) .. "/src/thread/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/thread/generic/SDL_syscond.c",
				AkRelativeToCwd(sourceLocation) .. "/src/timer/windows/*.c",
				AkRelativeToCwd(sourceLocation) .. "/src/loadso/windows/*.c",
			}
		end,
		DLLDemo = function(prj)
			-- This project depends on AkSoundEngineDLL which is only available in StaticCRT configurations; disable the others
			removeconfigurations {
				"Debug",
				"Profile",
				"Release",
			}
		end,
		SoundEngineDllProject = function(prj)
			
			staticruntime "On"

		end,
		SampleSoundEngineDllProject = function(prj)
			-- DLL projects depend on StaticCRT libs ONLY, so they should not build on non-StaticCRT configs
			removeconfigurations {
				"Debug",
				"Profile",
				"Release",
			}
			
			-- For historical reasons, customers expect StaticCRT DLLs in the non-StaticCRT bin folder.
			filter {"Debug(StaticCRT)"}
				postbuildcommands {
					"mkdir $(OutDir)..\\..\\Debug\\bin",
					"copy /Y $(OutDir)$(TargetName).* $(OutDir)..\\..\\Debug\\bin\\"
				}
			filter {"Profile(StaticCRT)"}
				postbuildcommands {
					"mkdir $(OutDir)..\\..\\Profile\\bin",
					"copy /Y $(OutDir)$(TargetName).* $(OutDir)..\\..\\Profile\\bin\\"
				}
			filter {"Release(StaticCRT)"}
				postbuildcommands {
					"mkdir $(OutDir)..\\..\\Release\\bin",
					"copy /Y $(OutDir)$(TargetName).* $(OutDir)..\\..\\Release\\bin\\"
				}
			filter {}
		end,
		ExternalPlugin = function(prj)
			removefatalwarnings "All"
		end,
	},

	Exclusions = {
		GameSimulator = function(prjLoc)
			excludes {
				prjLoc .. "/../../src/libraries/Common/UniversalScrollBuffer.*"
			}
		end,
		IntegrationDemo = function(projectLocation)
			excludes {
				projectLocation .. "../Common/stdafx.cpp"
			}
		end
	}
}
return AK.Platforms.Windows
