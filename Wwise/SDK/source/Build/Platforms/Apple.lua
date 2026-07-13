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

-- Xcode 13 was the latest version on which we analyzed the build settings recommendations
local XcodeLastUpgradeCheck = "1300"

local XcodeDefaults = {
	commonVersion = {
		commonPlatform = {
			commonConfiguration = function()
				cdialect("C11")
				cppdialect("C++17")
				architecture "universal"

				xcodebuildsettings {
					ARCHS = "$(ARCHS_STANDARD)",		-- Premake sets this to Native by default
					CLANG_ANALYZER_NONNULL = "YES",
					CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = "YES_AGGRESSIVE",
					CLANG_CXX_LIBRARY = "libc++",
					CLANG_ENABLE_MODULES = "YES",
					CLANG_ENABLE_OBJC_ARC = "YES",
					CLANG_ENABLE_OBJC_WEAK = "YES",
					CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = "YES",
					CLANG_WARN_BOOL_CONVERSION = "YES",
					CLANG_WARN_COMMA = "YES",
					CLANG_WARN_CONSTANT_CONVERSION = "YES",
					CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = "YES",
					CLANG_WARN_DIRECT_OBJC_ISA_USAGE = "YES_ERROR",
					CLANG_WARN_DOCUMENTATION_COMMENTS = "YES",
					CLANG_WARN_EMPTY_BODY = "YES",
					CLANG_WARN_ENUM_CONVERSION = "YES",
					CLANG_WARN_INFINITE_RECURSION = "YES",
					CLANG_WARN_INT_CONVERSION = "YES",
					CLANG_WARN_NON_LITERAL_NULL_CONVERSION = "YES",
					CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = "YES",
					CLANG_WARN_OBJC_LITERAL_CONVERSION = "YES",
					CLANG_WARN_OBJC_ROOT_CLASS = "YES_ERROR",
					CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = "YES",
					CLANG_WARN_RANGE_LOOP_ANALYSIS = "YES",
					CLANG_WARN_STRICT_PROTOTYPES = "YES",
					CLANG_WARN_SUSPICIOUS_MOVE = "YES",
					CLANG_WARN_UNGUARDED_AVAILABILITY = "YES_AGGRESSIVE",
					CLANG_WARN_UNREACHABLE_CODE = "YES",
					CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = "YES",
					CLANG_WARN__DUPLICATE_METHOD_MATCH = "YES",
					ENABLE_STRICT_OBJC_MSGSEND = "YES",
					GCC_NO_COMMON_BLOCKS = "YES",
					GCC_WARN_64_TO_32_BIT_CONVERSION = "YES",
					GCC_WARN_ABOUT_RETURN_TYPE = "YES_ERROR",	-- Premake sets this to YES
					GCC_WARN_UNDECLARED_SELECTOR = "YES",
					GCC_WARN_UNINITIALIZED_AUTOS = "YES_AGGRESSIVE",
					GCC_WARN_UNUSED_FUNCTION = "YES",
					MTL_FAST_MATH = "YES"
				}
			end,
			debugConfiguration = function()
				debugformat "Dwarf"
				defines {
					"DEBUG"
				}
				optimize "Debug"
				symbols "On"
				xcodebuildsettings {
					ENABLE_TESTABILITY = "YES",
					MTL_ENABLE_DEBUG_INFO = "INCLUDE_SOURCE"
				}
			end,
			releaseConfiguration = function()
				debugformat "SplitDwarf"
				optimize "On"
				symbols "On"
				xcodebuildsettings {
					ENABLE_NS_ASSERTIONS = "NO",
					MTL_ENABLE_DEBUG_INFO = "NO"
				}
			end
		},
		iOS = {
			commonConfiguration = function()
				system "ios"
				iosfamily "Universal"
				xcodebuildsettings {
					SUPPORTED_PLATFORMS = "iphonesimulator iphoneos",
					SDKROOT = "iphoneos"
				}
			end,
			releaseConfiguration = function()
				xcodebuildsettings {
					VALIDATE_PRODUCT = "YES"
				}
			end
		},
		Mac = {
			commonConfiguration = function()
				system "macosx"
				xcodebuildsettings {
					SUPPORTED_PLATFORMS = "macosx",
					SDKROOT = "macosx"
				}
			end
		},
		tvOS = {
			commonConfiguration = function()
				system "tvos"
				xcodebuildsettings {
					SUPPORTED_PLATFORMS = "appletvsimulator appletvos",
					SDKROOT = "appletvos"
				}
			end,
			releaseConfiguration = function()
				xcodebuildsettings {
					VALIDATE_PRODUCT = "YES"
				}
			end
		},
		visionOS = {
			commonConfiguration = function()
				system "visionos"
				xcodebuildsettings {
					SUPPORTED_PLATFORMS = "xros xrsimulator",
					SDKROOT = "xros"
				}
			end,
			releaseConfiguration = function()
				xcodebuildsettings {
					VALIDATE_PRODUCT = "YES"
				}
			end
		},
		watchOS = {
			commonConfiguration = function()
				system "ios" -- add system "watchos"
				xcodebuildsettings {
					SUPPORTED_PLATFORMS = "watchsimulator watchos",
					SDKROOT = "watchos",
					TARGETED_DEVICE_FAMILY = "4"
				}
			end,
			releaseConfiguration = function()
				xcodebuildsettings {
					VALIDATE_PRODUCT = "YES"
				}
			end
		}
	},
	v12 = {			-- 12.5
		commonPlatform = {
			debugConfiguration = function()
				xcodebuildsettings {
					SWIFT_ACTIVE_COMPILATION_CONDITIONS = "DEBUG",
					SWIFT_OPTIMIZATION_LEVEL = "-Onone"
				}
			end,
			releaseConfiguration = function()
				xcodebuildsettings {
					SWIFT_COMPILATION_MODE = "wholemodule",
					SWIFT_OPTIMIZATION_LEVEL = "-O"
				}
			end
		}
	}
}

local AudiokineticDefaults = {
	commonVersion = {
		commonPlatform = {
			commonConfiguration = function()
				cdialect("C11")
				cppdialect("C++17")
				symbols "On"
				editandcontinue "Off"
				exceptionhandling "Off"
				language "C++"
				rtti "Off"
				visibility "Hidden"
				xcodebuildsettings {
					DEVELOPMENT_TEAM = "BCB4VLKTK5",
					CLANG_WARN_DOCUMENTATION_COMMENTS = "NO",
					GCC_AUTO_VECTORIZATION = "NO",
					GCC_ENABLE_FLOATING_POINT_LIBRARY_CALLS = "NO",
					GCC_FAST_MATH = "YES",
					GCC_INLINES_ARE_PRIVATE_EXTERN = "YES",
					GCC_NO_COMMON_BLOCKS = "NO",
					GCC_STRICT_ALIASING = "NO",
					GCC_SYMBOLS_PRIVATE_EXTERN = "YES",
					GCC_UNROLL_LOOPS = "NO",
					GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO = "NO",
					GCC_WARN_CHECK_SWITCH_STATEMENTS = "NO",
					GCC_WARN_MISSING_PARENTHESIS = "NO",
					GCC_WARN_UNINITIALIZED_AUTOS = "YES",
					GCC_WARN_UNUSED_VARIABLE = "NO"
				}
			end,
			debugConfiguration = function()
				defines {
					"_DEBUG"
				}
				optimize "Off"
				xcodebuildsettings {
					ONLY_ACTIVE_ARCH = "NO"
				}
			end,
			releaseConfiguration = function()
				defines {
					"NDEBUG"
				}
				xcodebuildsettings {
					ONLY_ACTIVE_ARCH = "NO"
				}
				optimize "Speed"
			end,
		},
	},
	v14 = {		-- xcode 14.0
		iOS = {
			commonConfiguration = function()
				systemversion "11.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		Mac = {
			commonConfiguration = function()
				systemversion "10.9"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		tvOS = {
			commonConfiguration = function()
				systemversion "11.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		visionOS = {
			commonConfiguration = function()
				systemversion "1.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		watchOS = {
			commonConfiguration = function()
				systemversion "6.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		}
	},
	v15 = {		-- xcode 15.0
		iOS = {
			commonConfiguration = function()
				systemversion "12.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		Mac = {
			commonConfiguration = function()
				systemversion "10.13"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		tvOS = {
			commonConfiguration = function()
				systemversion "12.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		visionOS = {
			commonConfiguration = function()
				systemversion "1.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		},
		watchOS = {
			commonConfiguration = function()
				systemversion "6.0"
				xcodelastupgradecheck(XcodeLastUpgradeCheck)
			end
		}
	}
}

local ApplyXcodeIfExists = function(version, platform, configuration)
	if XcodeDefaults[version] ~= nil then
		if XcodeDefaults[version][platform] ~= nil then
			if XcodeDefaults[version][platform][configuration] ~= nil then
				XcodeDefaults[version][platform][configuration]()
				return
			end
		end
	end
end

local ApplyAudiokineticIfExists = function(version, platform, configuration)
	if AudiokineticDefaults[version] ~= nil then
		if AudiokineticDefaults[version][platform] ~= nil then
			if AudiokineticDefaults[version][platform][configuration] ~= nil then
				AudiokineticDefaults[version][platform][configuration]()
			end
		end
	end
end

local ApplyConfig = function(platform, configuration, ak)
	local XcodeVersion = "v15"
	if _OPTIONS["xcodeversion"] then
		XcodeVersion = "v" .. _OPTIONS["xcodeversion"]
	end

	if ak then
		ApplyAudiokineticIfExists("commonVersion", "commonPlatform", configuration)
		ApplyAudiokineticIfExists("commonVersion", platform, configuration)
		ApplyAudiokineticIfExists(XcodeVersion, "commonPlatform", configuration)
		ApplyAudiokineticIfExists(XcodeVersion, platform, configuration)
	else
		ApplyXcodeIfExists("commonVersion", "commonPlatform", configuration)
		ApplyXcodeIfExists("commonVersion", platform, configuration)
		ApplyXcodeIfExists(XcodeVersion, "commonPlatform", configuration)
		ApplyXcodeIfExists(XcodeVersion, platform, configuration)
	end
end

local AkAppleCommon = {}

function AkAppleCommon.ApplyXcodeSettings(platform)
	-- Standard configuration settings.
	filter {}
		ApplyConfig(platform, "commonConfiguration", false)

	filter "*Debug*"
		ApplyConfig(platform, "debugConfiguration", false)

	filter "Profile*"
		ApplyConfig(platform, "releaseConfiguration", false)

	filter "*Release*"
		ApplyConfig(platform, "releaseConfiguration", false)

	-- Audiokinetic overrides
	filter {}
		ApplyConfig(platform, "commonConfiguration", true)

	filter "*Debug*"
		ApplyConfig(platform, "debugConfiguration", true)

	filter "Profile*"
		ApplyConfig(platform, "releaseConfiguration", true)

	filter "*Release*"
		ApplyConfig(platform, "releaseConfiguration", true)

	filter "Profile*"
		defines {
			"AK_ENABLE_ASSERTS"
		}

	if not _AK_BUILD_AUTHORING then
		-- Note: The AuthoringRelease config is "profile", really. It must not be AK_OPTIMIZED.
		filter "Release*"
			defines "AK_OPTIMIZED"
		-- Split dwarf symbols do not work on Authoring built with brew-clang
		filter {}
			debugformat "SplitDwarf"
	end

	filter {}

	-- Allows older Xcodes to link against libs compiled with Xcode 14; this can be removed when Xcode 13 is no longer supported
	buildoptions {
		"-fno-objc-msgsend-selector-stubs",
		"-Qunused-arguments"
	}
end

function AkAppleCommon.CreateXcscheme(name)
	xcodelastupgradecheck(XcodeLastUpgradeCheck)
	xcscheme(name)
end

return AkAppleCommon
