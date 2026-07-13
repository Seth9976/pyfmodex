--
-- androidmk_solution.lua
-- Generator for Application.mk and Android.mk workspace files
-- Author : Bastien Brunnenstein
--


local p = premake
local androidmk = p.modules.androidmk
local workspace = p.workspace
local project = p.project

function androidmk.generate_applicationmk(wks)
	p.eol("\n")

	androidmk.wksBuildScript(wks)

	p.w('PM5_HELP := true')

	local generated = {}
	for prj in workspace.eachproject(wks) do
		if p.global.hasproject(prj) then
			prj = p.global.getexportedproject(prj)
			for cfg in project.eachconfig(prj) do
				local cfgName = androidmk.cfgname(cfg)
				if not generated[cfgName] then
					generated[cfgName] = true
					p.w('')
					p.x('ifeq ($(%s),%s)', androidmk.CONFIG_OPTION, cfgName)
					androidmk.wksOptim(wks, cfg)
					androidmk.wksAbi(wks, cfg)
					androidmk.wksConfiguration(wks, cfg)
					androidmk.wksPlatform(wks, cfg)
					androidmk.wksStl(wks, cfg)
					androidmk.wksToolchainVersion(wks, cfg)
					androidmk.wksDisableStrip(cfg)

					p.w('  PM5_HELP := false')
					--p.w('  APP_SHORT_COMMANDS := true')
					p.w('endif')
				end
			end
		end -- AUDIOKINETIC
	end
	androidmk.wksPremakeHelp(wks)
end

function androidmk.generate_androidmk(wks)
	p.eol("\n")
	local curpath = 'workspace_'..wks.name:upper()..'_PATH'
	p.w('%s := $(call my-dir)', curpath)
	p.w('')

	local sorted = {}
	for prj in workspace.eachproject(wks) do
		if p.global.hasproject(prj) then	-- AUDIOKINETIC
			prj = p.global.getexportedproject(prj)	-- AUDIOKINETIC mdonais Adding importproject
			table.insert(sorted, prj)
		end -- AUDIOKINETIC
	end
	table.sort(sorted, function(a,b) return a.name<b.name end)
	for _, prj in ipairs(sorted) do
		local prjpath = p.filename(prj, androidmk.prjFile(prj))
		local prjrelpath = path.getrelative(wks.location, prjpath)
		p.x('include $(%s)/%s', curpath, prjrelpath)
	end

	for cfg in workspace.eachconfig(wks) do
		local existingmklist = {}
		local moduleslist = {}

		-- Agregate existing Android.mk files for each project per configuration
		for _, prj in ipairs(sorted) do
			for prjcfg in project.eachconfig(prj) do
				if androidmk.cfgname(prjcfg) == androidmk.cfgname(cfg) then
					for _, mkpath in ipairs(prj.amk_includes) do
						local mkrelpath = path.getrelative(wks.location, mkpath)
						if not table.contains(existingmklist, mkrelpath) then
							table.insert(existingmklist, mkrelpath)
						end
					end
					for _, mod in ipairs(prj.amk_importmodules) do
						if not table.contains(moduleslist, mod) then
							table.insert(moduleslist, mod)
						end
					end
				end
			end
		end

		if #existingmklist > 0 or #moduleslist > 0 then
			local cfgName = androidmk.cfgname(cfg)
			p.w('')
			p.x('ifeq ($(%s),%s)', androidmk.CONFIG_OPTION, cfgName)
			for _, mkpath in ipairs(existingmklist) do
				p.x('  include $(%s)/%s', curpath, mkpath)
			end
			for _, mod in ipairs(moduleslist) do
				p.x('  $(call import-module,%s)', mod)
			end
			androidmk.wksDisableStrip(cfg)
			p.w('endif')
		end
	end
end


function androidmk.wksBuildScript(wks)
	p.x('APP_BUILD_SCRIPT := $(call my-dir)/%s', androidmk.wksAndroidFile(wks))
end

function androidmk.wksPremakeHelp(wks)
	p.w('')
	p.w('ifeq ($(PM5_HELP),true)')

	p.w('  $(info )')
	p.w('  $(info Usage:)')
	p.w('  $(info $()  ndk-build %s=<config>)', androidmk.CONFIG_OPTION)
	p.w('  $(info )')
	p.w('  $(info CONFIGURATIONS:)')
	local generated = {}
	for prj in workspace.eachproject(wks) do
		if p.global.hasproject(prj) then	-- AUDIOKINETIC
			prj = p.global.getexportedproject(prj)	-- AUDIOKINETIC mdonais Adding importproject
			for cfg in project.eachconfig(prj) do
				local prjcfg = project.getconfig(prj, cfg.buildcfg, cfg.platform)
				local cfgName = androidmk.cfgname(prjcfg)
				if not generated[cfgName] then
					generated[cfgName] = true
					p.w('  $(info $()  %s)', cfgName)
				end
			end
		end -- AUDIOKINETIC
		break
	end
	p.w('  $(info )')
	p.w('  $(info For more ndk-build options, see https://developer.android.com/ndk/guides/ndk-build.html)')
	p.w('  $(info )')

	p.w('  $(error Set %s and try again)', androidmk.CONFIG_OPTION)
	p.w('endif')
end


-- Function to make sure that an option in a given config is the same for every project
-- Additionally, replace "default" with "nil"
function agregateOption(wks, cfg, option)
	local first = true
	local val
	for prj in workspace.eachproject(wks) do
		if p.global.hasproject(prj) then	-- AUDIOKINETIC
			prj = p.global.getexportedproject(prj)	-- AUDIOKINETIC mdonais Adding importproject
			for prjcfg in project.eachconfig(prj) do
				if prjcfg.shortname == cfg.shortname then
					if first then
						first = false
						val = prjcfg[option]
--					else
--						if prjcfg[option] ~= val then
--							error("Value for "..option.." must be the same on every project for configuration "..cfg.longname.." in workspace "..wks.name)
--						end
					end
				end
			end -- AUDIOKINETIC
		end
	end
	if val == "default" then
		return nil
	end
	return val
end

function androidmk.wksOptim(wks, cfg)
--[[
	local optim = agregateOption(wks, cfg, "optimize")
	if optim == p.OFF or optim == "Debug" then
		p.w('  APP_OPTIM := debug')
	elseif optim ~= nil then
		p.w('  APP_OPTIM := release')
	end
--]]
	if cfg.buildcfg == "Debug" then
		p.w("  APP_OPTIM := debug")
		p.w("  NDK_DEBUG := 1")
	else
		p.w("  APP_OPTIM := release")
	end
end

function androidmk.wksAbi(wks, cfg)
	local abi = agregateOption(wks, cfg, "ndkabi")
	if abi then
		p.w('  APP_ABI := %s', abi)
	end
end

function androidmk.wksConfiguration(wks, cfg)
	-- Note: This is not in The APP_ standard. However, it's the place where it makes the most sense.
	p.w("  CONFIGURATION := " .. cfg.buildcfg)
end


function androidmk.wksPlatform(wks, cfg)
	local plat = agregateOption(wks, cfg, "ndkplatform")
	if plat then
		p.w('  APP_PLATFORM := %s', plat)
	end
end

function androidmk.wksStl(wks, cfg)
	local stl = agregateOption(wks, cfg, "ndkstl")
	if stl then
		p.w('  APP_STL := %s', stl)
	end
end

function androidmk.wksToolchainVersion(wks, cfg)
	local toolchain = agregateOption(wks, cfg, "ndktoolchainversion")
	if toolchain then
		p.w('  NDK_TOOLCHAIN_VERSION := %s', toolchain)
	end
end

function androidmk.wksDisableStrip(cfg)
	if string.find(cfg.symbols, "On") then
		p.w('  cmd-strip :=')
	end
end