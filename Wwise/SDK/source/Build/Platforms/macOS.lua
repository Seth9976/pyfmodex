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

local AkAppleCommon = require "Apple"

if not premake.option.get("universal-binaries") then
	newoption
	{
		trigger     = "universal-binaries",
		description = "Generates build commands to build universal binaries (fat binaries)"
	}
end

local ApplyUniversalArch = function(prj)
	filter {"action:gmake"}
		if _OPTIONS["universal-binaries"] then
			buildoptions { "-arch x86_64", "-arch arm64" }
			linkoptions { "-arch x86_64", "-arch arm64" }
		else
			buildoptions { "-arch x86_64" }
			linkoptions { "-arch x86_64" }
		end
	filter {}
end

function BraceConverter(token)
	local val = token
	val = val:gsub('[)]', '}')
	val = val:gsub('[(]', '{')
	return val
end

AK.Platforms.Mac =
{
	name = "Mac",
	directories = {
		src = {
			__default__ = "Mac",
			CommunicationCentral = "POSIX"
		},
		simd = "SIMD",
		project = "Mac",
		lualib = "Mac",
		lowlevelio = "POSIX",
		luasln = "GameSimulator/source/",
	},
	kinds = {
		GameSimulator = "ConsoleApp",
		IntegrationDemo = "WindowedApp",
		DLLDemo = "ConsoleApp",
	},
	suffix = {
		__default__ = "Mac",
		IntegrationDemo = "",
		SDL2 = "",
		AllEffectsSln = "Mac",
		SamplePluginsSln = "Mac",
		SourceControlSln = "Mac",
		FilePackagerSln = "Mac"
	},

	configurations =
	{
		"Debug",
		"Profile",
		"Release"
	},
	platforms = { "universal" },
	features = {
		"POSIX",
		"UnitTests",
		"Catch2",
		"SampleSink",
		"IntegrationDemo",
		"SampleClient",
		"ProjectDatabase",
		"iZotope",
		"DynamicLibraries",
		"SoundEngineDLL",
		"DLLDemo",
		"Motion",
		"GME",
		"libcurl",
		"SDL", "SDLRendering", "SDLInput"
	},
	validActions = { "xcode4", "gmake", "export-compile-commands" },
	useLinkGroups = false,

	AdditionalSoundEngineProjects = function()
		return {}
	end,

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
		local prj = project (in_targetName)
			platforms(AK.Platforms.Mac.platforms)

			location(AkRelativeToCwd(in_projectLocation))
			targetname(in_targetName)
			if in_targetType == nil or in_targetType == "StaticLib" then
				kind("StaticLib")
				-- Add to global table
				_AK_TARGETS[in_targetName] = in_fileName
			else
				kind(in_targetType)
			end
			uuid(GenerateUuid(in_fileName))
			filename(in_fileName)

			-- Treat all warnings as errors
			fatalwarnings "All"

			disablewarnings
			{
				"null-conversion",
				"invalid-offsetof",
				"nontrivial-memcall", -- WG-83214 Fix clang warnings for nontrivial-memcall
				"unknown-warning-option" -- Ignore newer warnings from more recent toolchains
			}

			ApplyUniversalArch()

			AkAppleCommon.ApplyXcodeSettings(AK.Platforms.Mac.name)

			if _AK_BUILD_AUTHORING then
				systemversion "10.15"
			end

			-- Set the scope back to current project
			project(in_targetName)

			filter {}
			ApplyPlatformExceptions(prj.name, prj)
			filter {}
		return prj
	end,

	-- Plugin factory.
	-- Upon returning from this method, the current scope is the newly created plugin project.
	CreatePlugin = function(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		local prj = AK.Platforms.Mac.CreateProject(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		return prj
	end,

	SetOutputDirectory = function (in_root, in_config, in_suffix, targettype)
		local root = AkRelativeToCwd(in_root)
		local outFolder = "Mac_Xcode$(XCODE_VERSION_MAJOR)/%{cfg.buildcfg}/"
		if _ACTION == "xcode4" then
			targetdir (root .. outFolder .. targettype)
			objdir ("!" .. root .. outFolder .. "obj/%{prj.name}")
		else
			-- Since gmake is used for Authoring projects, we cannot use the XCODE_VERSION_MAJOR variable.
			-- Instead, default to "Mac", and the binaries will be compiled with whatever Xcode DEVELOPER_DIR gives us
			targetdir (root .. "Mac/" .. in_config ..  "/" .. targettype)
			objdir ("!" .. root .. "Mac/" .. in_config .. "/obj/%{prj.name}")
		end
	end,

	-- function to include an additional assembly file (.a for most gcc/clang platforms, .libs for msvc platforms) into the build target.
	-- This is only intended for, and supported for, StaticLib targets. If you want additional libs for DLLs and EXEs, use `links`.
	-- in_suffix should be the normal platform suffix (eg "_vc170" for Microsoft platforms)
	-- in_dir is the directory to fetch the libs from. Should typically be "AkRelativeToCwd .. (Proj)"
	-- in_assemblyName is the common name of the assembly (eg "PluginCore")
	ArchiveAdditionalLibs = function (in_suffix, in_dir, in_assemblyName)
		local baseAssemblyFilePath = "%[" .. in_dir .. "]/Mac_Xcode${XCODE_VERSION_MAJOR}"
		local tailAssemblyFilePath = "lib/%{cfg.targetprefix}" .. in_assemblyName .. "%{cfg.targetextension}"

		-- Since gmake is used for Authoring projects, we cannot use the XCODE_VERSION_MAJOR variable.
		-- Instead, default to "Mac_Xcode2600".
		if _ACTION == "gmake" then
			baseAssemblyFilePath = "%[" .. in_dir .. "]/Mac_Xcode2600"
		end

		-- we can use libtool to combine the contents of the input assembly file with our destination one.
		-- note that postbuildcommands ends up trying to use envvars as "$(...)" which doesn't work for shell scripts,
		-- since they need to be specified as "$..." or "${...}". premake runs the code within the curly braces as lua code directly,
		-- so we have a helper function above to convert the "$(...)" into "${...}".
		filter "Debug*"
			postbuildcommands {
				"libtool -o %{BraceConverter(cfg.buildtarget.abspath)} %{BraceConverter(cfg.buildtarget.abspath)} " .. baseAssemblyFilePath .. "/Debug${EFFECTIVE_PLATFORM_NAME}/" .. tailAssemblyFilePath,
			}
		filter "Profile*"
			postbuildcommands {
				"libtool -o %{BraceConverter(cfg.buildtarget.abspath)} %{BraceConverter(cfg.buildtarget.abspath)} " .. baseAssemblyFilePath .. "/Profile${EFFECTIVE_PLATFORM_NAME}/" .. tailAssemblyFilePath,
			}
		filter "Release*"
			postbuildcommands {
				"libtool -o %{BraceConverter(cfg.buildtarget.abspath)} %{BraceConverter(cfg.buildtarget.abspath)} " .. baseAssemblyFilePath .. "/Release${EFFECTIVE_PLATFORM_NAME}/" .. tailAssemblyFilePath,
			}
		filter {}
	end,
	
	Exceptions = {
		AkAutobahn = function(prj)
			xcodebuildsettings {
				GCC_FAST_MATH = "NO",
				MTL_FAST_MATH = "NO",
				GCC_ENABLE_CPP_EXCEPTIONS = "YES",
				OTHER_CPLUSPLUSFLAGS = {"$(OTHER_CFLAGS)", "-Wno-write-strings", "-fvisibility-inlines-hidden","-Wno-unknown-warning-option","-Wno-deprecated-register","-Wno-expansion-to-defined"},
			}
		end,
		AkSoundEngine = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			if not _AK_BUILD_AUTHORING then
				filter "Release*"
					xcodebuildsettings { EXCLUDED_SOURCE_FILE_NAMES = {"*Proxy*", "Command*" } }
				filter {}
			end
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
			files {
				prjLoc .. "/../AppleCommon/*.mm",
				prjLoc .. "/../AppleCommon/*.cpp",
				prjLoc .. "/../AppleCommon/*.h",
				prjLoc .. "/../Mac/*.mm",
			}
			includedirs {
				prjLoc .. "/../AppleCommon",
			}
			disablewarnings {
				"unused-but-set-variable", -- Until [[maybe_unused]]
				"unknown-warning-option" -- Ignore newer warnings from more recent toolchains
			}
			xcodebuildsettings {
				GCC_ENABLE_OBJC_EXCEPTIONS = "YES" -- Required for AVAudioEngine error-handling
			}
		end,
		AkSoundEngineDLL = function(prj)
			links {
				"Foundation.framework",
				"AudioToolbox.framework",
				"AudioUnit.framework",
				"CoreAudio.framework",
				"AVFoundation.framework"
			}
			linkoptions {
				"-Wl,-install_name,@rpath/libAkSoundEngineDLL.dylib"
			}
		end,
		AkSoundEngineTests = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			links {
				"Foundation.framework",
				"Cocoa.framework",
				"GLUT.framework",
				"AudioToolbox.framework",
				"AudioUnit.framework",
				"CoreAudio.framework",
				"AVFoundation.framework",
				"QuartzCore.framework",
				"AppKit.framework"
			}
			files {
				prjLoc .. "/../../../../samples/SoundEngine/Common/AkPlatformProfilerHookStubs.cpp",  -- Common AkPlatformProfilerHookStubs does not have profiler hooks defined
			}
			filter "*Debug*"
				libdirs { prjLoc .. "/../../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Debug/lib" }
			filter "*Profile*"
				libdirs { prjLoc .. "/../../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Profile/lib" }
			filter "*Release*"
				libdirs { prjLoc .. "/../../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Release/lib" }
			filter {}
		end,
		AkMemoryMgr = function(prj)
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
		end,
		AkSpatialAudio = function(prj)
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
		end,
		AkStreamMgr = function(prj)
			defines({"AKSOUNDENGINE_DLL", "AKSOUNDENGINE_EXPORTS"})
			filter "action:gmake"
				filename("AkStreamMgr")
			filter {}
		end,
		AkConvolutionReverbFX = function(prj)
			xcodebuildsettings { GCC_ENABLE_SSE3_EXTENSIONS = "YES" }
		end,
		AkGuitarDistortionFX = function(prj)
			optimize("Size")
		end,
		AkTimeStretchFX = function(prj)
			optimize("Size")
		end,
		AkHarmonizerFX = function(prj)
			optimize("Size")
		end,
		AkMotionSink = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs {
				prjLoc .. "/../AppleCommon",
			}
			files {
				prjLoc .. "/../AppleCommon/*.cpp",
				prjLoc .. "/../AppleCommon/*.h",
				prjLoc .. "/../AppleCommon/*.mm",
			}
		end,
		AkAudioInputSource = ApplyUniversalArch,
		AkToneSource = ApplyUniversalArch,
		AkSineSource = ApplyUniversalArch,
		AkDelayFX = ApplyUniversalArch,
		AkOpusDecoder = function(prj)
			xcodebuildsettings {
				GCC_OPTIMIZATION_LEVEL = 2,
				GCC_WARN_64_TO_32_BIT_CONVERSION = "NO",
				GCC_WARN_UNUSED_FUNCTION = "NO",
				USE_HEADERMAP = "NO",
			}
		end,
		AkVorbisDecoder = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs {
				prjLoc .. "/../../../AkAudiolib/AppleCommon",
			}
		end,
		AuroHeadphoneFX = function(prj)
			xcodebuildsettings { CLANG_WARN_STRICT_PROTOTYPES = "NO", GCC_WARN_UNUSED_FUNCTION = "NO", CLANG_WARN_COMMA = "NO" }
		end,
		Cube = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			defines { "HAS_SOCKLEN_T", "GL_SILENCE_DEPRECATION" }
			links {
				"Foundation.framework",
				"AudioToolbox.framework",
				"AudioUnit.framework",
				"CoreAudio.framework",
				"AVFoundation.framework",
				"CoreFoundation.framework",
				"OpenGL.framework",
				"z"
			}

			xcodebuildsettings {
				GCC_WARN_64_TO_32_BIT_CONVERSION = "NO",
				CLANG_WARN_BOOL_CONVERSION = "NO",
				CLANG_WARN_COMMA = "NO",
				OTHER_CFLAGS = {"-Wno-implicit-function-declaration","-Wno-shift-negative-value"},
				OTHER_CPLUSPLUSFLAGS = {"$(OTHER_CFLAGS)", "-Wno-write-strings", "-fvisibility-inlines-hidden", "-Wno-narrowing", "-Wno-int-to-void-pointer-cast", "-Wno-unsequenced", "-Wno-parentheses"},
			}

			-- SDK location relies on symlink to Wwise/SDK in Cube's instrument script to simulate the deployment environment
			filter "Debug*"
				libdirs { "../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Debug/lib" }
				links	{ "CommunicationCentral" }
			filter "Profile*"
				libdirs { "../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Profile/lib" }
				links	{ "CommunicationCentral" }
			filter "Release*"
				libdirs { "../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Release/lib" }
			filter {}
		end,
		iZotopePluginFactory = function(prj)
			xcodebuildsettings {
				GCC_WARN_64_TO_32_BIT_CONVERSION = "NO",
				CLANG_WARN_COMMA = "NO",
			}
		end,
		GameSimulator = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)

			defines {"LUA_USE_MACOSX", "INTEGRATIONDEMO_BYPASS_SDL"}
			xcodebuildsettings {
				PRODUCT_BUNDLE_IDENTIFIER = "com.audiokinetic.${PRODUCT_NAME:rfc1034identifier}",
				GCC_TREAT_WARNINGS_AS_ERRORS = "NO",
				GCC_WARN_UNUSED_VALUE = "NO",
				GCC_WARN_UNUSED_FUNCTION = "NO",
				GCC_WARN_UNUSED_VARIABLE = "NO",
				GCC_WARN_64_TO_32_BIT_CONVERSION = "NO",
				CLANG_WARN_COMMA = "NO"
			}

			links {
				"LuaLib",
				"ToLuaLib",
				"Foundation.framework",
				"CoreAudio.framework",
				"AVFoundation.framework",
				"AudioUnit.framework",
				"AudioToolbox.framework",
				"CoreServices.framework",
				"CoreHaptics.framework",
				"GameController.framework",
				"Carbon.framework",
				"OpenGL.framework"
			}
			files {
				prjLoc .. "/../../../../SDK/samples/IntegrationDemo/AppleCommon/AppleInputMgr.*",
				prjLoc .. "/../../../../SDK/samples/IntegrationDemo/AppleCommon/GControllerMgr.*",				
				prjLoc .. "/../../../../SDK/samples/SoundEngine/Common/AkPlatformProfilerHookStubs.cpp", -- Common AkPlatformProfilerHookStubs does not have profiler hooks defined
			}
			filter "*Debug*"
				libdirs { 
					prjLoc .. "/../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Debug/lib",
					prjLoc .. "/../../../Mac_Xcode$(XCODE_VERSION_MAJOR)/Debug/lib",
				}

			filter "*Profile*"
				libdirs {
					prjLoc .. "/../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Profile/lib",
					prjLoc .. "/../../../Mac_Xcode$(XCODE_VERSION_MAJOR)/Profile/lib",
				}
			filter "*Release*"
				libdirs {
					prjLoc .. "/../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/Release/lib",
					prjLoc .. "/../../../Mac_Xcode$(XCODE_VERSION_MAJOR)/Release/lib",
				}
			filter {}
		end,
		LuaLib = function(prj)
			defines("LUA_USE_MACOSX")
			xcodebuildsettings {
				GCC_WARN_UNUSED_VALUE = "NO",
				GCC_WARN_UNUSED_FUNCTION = "NO",
				GCC_WARN_UNUSED_VARIABLE = "NO",
				GCC_WARN_64_TO_32_BIT_CONVERSION = "NO",
				CLANG_WARN_COMMA = "NO",
			}
		end,
		ToLuaLib = function(prj)
			defines("LUA_USE_MACOSX")
			xcodebuildsettings {
				GCC_WARN_UNUSED_VALUE = "NO",
				GCC_WARN_UNUSED_FUNCTION = "NO",
				GCC_WARN_UNUSED_VARIABLE = "NO",
				GCC_WARN_64_TO_32_BIT_CONVERSION = "NO",
				CLANG_WARN_COMMA = "NO",
			}
		end,
		IntegrationDemo = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)

			defines("LUA_USE_MACOSX")
			xcodebuildsettings {
				PRODUCT_BUNDLE_IDENTIFIER = "com.audiokinetic.${PRODUCT_NAME:rfc1034identifier}",
				GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS = "NO",
				CLANG_WARN_COMMA = "NO"
			}

			disablewarnings
			{
				"deprecated-declarations",
				"switch"
			}

			-- Required when building from Xcode to find Wwise_IDs.h
			includedirs {
				prjLoc .. "/../../WwiseProject/GeneratedSoundBanks",
			}
			-- System
			links {
				"Foundation.framework",
				"Cocoa.framework",
				"GLUT.framework",
				"AudioToolbox.framework",
				"AudioUnit.framework",
				"CoreAudio.framework",
				"AVFoundation.framework",
				"QuartzCore.framework",
				"CoreHaptics.framework",
				"GameController.framework",
				"IOKit.framework",
				"AppKit.framework",
				"OpenGL.framework"
			}
			flags { "GlobalSiblings" }
			includedirs {
				prjLoc .. "/../../SoundEngine/POSIX",
				prjLoc .. "/../AppleCommon",
			}

			files {
				prjLoc .. "/*.mm",
				prjLoc .. "/*.m",
				prjLoc .. "/IntegrationDemo-Info.plist",
				prjLoc .. "/MainMenu.xib",
				prjLoc .. "/../../SoundEngine/Common/AkPlatformProfilerHookStubs.cpp", -- Common AkPlatformProfilerHookStubs does not have profiler hooks defined
				prjLoc .. "/../../SoundEngine/POSIX/*.*",
				prjLoc .. "/../AppleCommon/*.*",	
			}

			xcodebuildsettings {
				GCC_TREAT_WARNINGS_AS_ERRORS = "NO",
				CLANG_ENABLE_OBJC_ARC = false,
			}
			libdirs { prjLoc .. "/../../../../SDK/Mac_Xcode$(XCODE_VERSION_MAJOR)/%{cfg.buildcfg}/lib" }
			filter "Debug*"
				linkoptions	{ "../../../Mac_Xcode$(XCODE_VERSION_MAJOR)/Debug/lib/libCommunicationCentral.a" }
			filter "Profile*"
				linkoptions	{ "../../../Mac_Xcode$(XCODE_VERSION_MAJOR)/Profile/lib/libCommunicationCentral.a" }
			filter {}
		end,
		SampleClient = function(prj)
			xcodebuildsettings {
				MACOSX_DEPLOYMENT_TARGET = "10.15",
				GCC_FAST_MATH = "NO",
				MTL_FAST_MATH = "NO",
				GCC_ENABLE_CPP_EXCEPTIONS = "YES",
				OTHER_CPLUSPLUSFLAGS = {"$(OTHER_CFLAGS)", "-Wno-write-strings", "-fvisibility-inlines-hidden","-Wno-unknown-warning-option","-Wno-deprecated-register","-Wno-expansion-to-defined"},
			}
		end,
		SDL2 = function(prj)
			local SDL2 = require("SDL2")

			xcodebuildsettings {
				GCC_ENABLE_OBJC_EXCEPTIONS = "YES" -- Required by Cocoa video module
			}

			local sourceLocation = SDL2.GetSourcePath()
			files {
				-- These are validated against configure.ac
				AkRelativeToCwd(sourceLocation) .. "/src/audio/coreaudio/*",
				AkRelativeToCwd(sourceLocation) .. "/src/misc/macosx/*",
				AkRelativeToCwd(sourceLocation) .. "/src/file/cocoa/*",
				AkRelativeToCwd(sourceLocation) .. "/src/filesystem/cocoa/*",
				AkRelativeToCwd(sourceLocation) .. "/src/haptic/darwin/*",
				AkRelativeToCwd(sourceLocation) .. "/src/hidapi/mac/*",
				AkRelativeToCwd(sourceLocation) .. "/src/joystick/darwin/*",
				AkRelativeToCwd(sourceLocation) .. "/src/joystick/iphoneos/*",
				AkRelativeToCwd(sourceLocation) .. "/src/loadso/dlopen/*",
				AkRelativeToCwd(sourceLocation) .. "/src/locale/macosx/*",
				AkRelativeToCwd(sourceLocation) .. "/src/power/macosx/*",
				AkRelativeToCwd(sourceLocation) .. "/src/power/uikit/*",
				AkRelativeToCwd(sourceLocation) .. "/src/render/metal/*", -- SUPPORT METAL ONLY
				AkRelativeToCwd(sourceLocation) .. "/src/sensor/coremotion/*",
				AkRelativeToCwd(sourceLocation) .. "/src/thread/pthread/*",
				AkRelativeToCwd(sourceLocation) .. "/src/timer/unix/*",
				AkRelativeToCwd(sourceLocation) .. "/src/video/cocoa/*",
				AkRelativeToCwd(sourceLocation) .. "/src/video/offscreen/*",
				AkRelativeToCwd(sourceLocation) .. "/src/video/uikit/*",
			}
		end,
		SoundEngineDllProject = function(prj)
			flags { "GlobalSiblings" }
			defines {
				"AKSOUNDENGINE_EXPORTS",
				"AKSOUNDENGINE_DLL",
			}
			links {
				"Foundation.framework",
				"CoreFoundation.framework",
			}
			xcodebuildsettings {
				CODE_SIGN_ENTITLEMENTS = "../../DynamicLibraries.entitlements"
			}
		end,
		AkMotion = function(prj)
			-- This is the DLL project for AkMotion
			links {
				"CoreHaptics.framework",
				"GameController.framework"
			}
		end,
		AllSoundEngineSln = function(sln)
			AkAppleCommon.CreateXcscheme("BuildAllSchemes")
		end,
		AkSoundEngineDLLSln = function(sln)
			AkAppleCommon.CreateXcscheme("All")
		end,
		WwiseSDKSln = function(sln)
			AkAppleCommon.CreateXcscheme("AllWwiseSDK")
		end,
		ExternalPlugin = function(prj)
			if _ACTION == "xcode4" then
				removefatalwarnings "All"
				xcodebuildsettings { DEVELOPMENT_TEAM = "" }
			end
		end,
		libcurl = function(prj)
			xcodebuildsettings {
				OTHER_CFLAGS = { "-fno-objc-msgsend-selector-stubs", "-Qunused-arguments", "-Wno-unused-function" }
			}
		end,
		WwiseProjectDatabase = function(prj)
			xcodebuildsettings {
				GCC_FAST_MATH = "NO",
				MTL_FAST_MATH = "NO"
			}
		end,
		WwiseProjectDatabaseTests = function(prj)
			xcodebuildsettings {
				GCC_FAST_MATH = "NO",
				MTL_FAST_MATH = "NO"
			}
		end,
		WwiseProjectDatabaseIntegrationTests = function(prj)
			xcodebuildsettings {
				GCC_FAST_MATH = "NO",
				MTL_FAST_MATH = "NO"
			}
		end,
	},

	Exclusions = {
		AkSoundEngine = function(prjLoc)
			-- Remove the cpp file that are included from other files to prevent duplicated symbols in lib
			excludes {
				prjLoc .. "/../AppleCommon/AkLEngineApple.cpp",
			}
		end,
		GameSimulator = function(prjLoc)
			excludes {
				prjLoc .. "/../../src/libraries/Common/UniversalScrollBuffer.*"
			}
		end,
	}
}
return AK.Platforms.Mac
