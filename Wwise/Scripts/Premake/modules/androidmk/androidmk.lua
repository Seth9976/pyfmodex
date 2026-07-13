--
-- androidmk.lua
-- Generator for ndk-build makefiles
-- Author : Bastien Brunnenstein
-- Heavily modified by: Audiokinetic inc
--

local p = premake
p.modules.androidmk = p.modules.androidmk or {}
p.modules.androidmk._VERSION = p._VERSION

local androidmk = p.modules.androidmk
local project = p.project

androidmk.CONFIG_OPTION = "PM5_CONFIG"

androidmk.ldflags = {
	flags = {
		--LinkTimeOptimization = "-flto",
	}
}

androidmk.cflags = {
	flags = {
		FatalCompileWarnings = "-Werror",
		ShadowedVariables = "-Wshadow",
		UndefinedIdentifiers = "-Wundef",
		--LinkTimeOptimization = "-flto",
	},
	warnings = {
		Extra = "-Wall -Wextra",
		Off = "-w",
	}
}

androidmk.cppflags = {
	flags = {
		["C++11"] = "-std=c++11",
		["C++14"] = "-std=c++14",
	}
}

function androidmk.inJni(wks)
	local fname = wks.location or wks.basedir
	return string.sub(fname, -4) == "/jni"
end

function androidmk.wksApplicationFile(wks)
	if androidmk.inJni(wks) then
		return "Application.mk"
	else
		return wks.name .."_application.mk"
	end
end

function androidmk.wksAndroidFile(wks)
	if androidmk.inJni(wks) then
		return "Android.mk"
	else
		return wks.name ..".mk"
	end
end


function androidmk.prjFile(prj)
	local fname
	if prj.filename ~= nil then 
		fname = prj.filename
	else
		fname = prj.name
	end
	return fname ..".mk"
end

function androidmk.esc(value)		-- This is make.esc!
	result = value:gsub("\\", "\\\\")
	result = result:gsub("\"", "\\\"")
	result = result:gsub(" ", "\\ ")
	result = result:gsub("%(", "\\(")
	result = result:gsub("%)", "\\)")

	-- leave $(...) shell replacement sequences alone
	result = result:gsub("$\\%((.-)\\%)", "$(%1)")
	return result
end

function androidmk.cfgname(cfg)
	cfgname = string.lower(string.format("%s_%s", cfg.buildcfg, cfg.platform))
	return cfgname
end

function androidmk.generateWorkspace(wks)
	p.escaper(androidmk.esc)
	p.generate(wks, androidmk.wksApplicationFile(wks), androidmk.generate_applicationmk)
	p.generate(wks, androidmk.wksAndroidFile(wks), androidmk.generate_androidmk)
end

function androidmk.generateProject(prj)
	p.escaper(androidmk.esc)
	if project.isc(prj) or project.iscpp(prj) then
		p.generate(prj, androidmk.prjFile(prj), androidmk.generate_projectmk)
	end
end

function androidmk.cleanWorkspace(wks)
	p.clean.file(wks, androidmk.wksApplicationFile(wks))
	p.clean.file(wks, androidmk.wksAndroidFile(wks))
end

function androidmk.cleanProject(prj)
	p.clean.file(prj, androidmk.prjFile(prj))
end

function androidmk.cleanTarget(prj)
end

--
-- Load all required code, and return the module.
--

include 'androidmk_project.lua'
include 'androidmk_workspace.lua'

return androidmk