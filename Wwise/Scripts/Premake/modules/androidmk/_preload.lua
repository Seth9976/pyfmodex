--
-- _preload.lua
-- Generator for ndk-build makefiles
-- Author : Bastien Brunnenstein
-- Heavily modified by: Audiokinetic inc
--
local p = premake

newaction {
	trigger         = "androidmk",
	shortname       = "Android.mk",
	description     = "Generate Android.mk files for Android NDK",

	valid_kinds     = { "StaticLib", "SharedLib" },
	valid_languages = { "C", "C++" },
	valid_tools     = {
		cc = { "clang" }
	},
	
	onWorkspace = function(wks)
		p.modules.androidmk.generateWorkspace(wks)
	end,

	onProject = function(prj)
		p.modules.androidmk.generateProject(prj)
	end,

	onCleanWorkspace = function(wks)
		p.modules.androidmk.cleanWorkspace(wks)
	end,

	onCleanProject = function(prj)
		p.modules.androidmk.cleanProject(prj)
	end,

	onCleanTarget = function(prj)
		p.modules.androidmk.cleanTarget(prj)
	end
}

-- This is the entire slew of additional options for androidmk. They must always be added, as the options might be processed elsewhere in the code.
include 'androidmk_api.lua'

return function(cfg)		-- This is now expected result of the include, to know if we should further load the code
	return (_ACTION == "androidmk")
end
