--
-- androidmk_project.lua
-- Generator for Android.mk project files
-- Author : Bastien Brunnenstein
-- Heavily modified by: Audiokinetic inc
--

local p = premake
local androidmk = p.modules.androidmk
local project = p.project

function androidmk.generate_projectmk(prj)
	p.eol("\n")

	p.w('LOCAL_PATH := $(call my-dir)')
	androidmk.prebuildcommands(prj, cfg)

	androidmk.prjPrebuildStatic(prj)
	androidmk.prjPrebuildShared(prj)
	androidmk.prjConfigSpecificPrebuild(prj)

	-- Prepare the list of files
	local rootFiles, cfgFiles = androidmk.prepareSrcFiles(prj)

	androidmk.prjHeader(prj)
	androidmk.prjSrcFiles(rootFiles)

	for cfg in project.eachconfig(prj) do
		local cfgName = androidmk.cfgname(cfg)
		p.w('')
		p.x('ifeq ($(%s),%s)', androidmk.CONFIG_OPTION, cfgName)

		androidmk.prjIncludes(prj, cfg)
		androidmk.prjCppFeatures(prj, cfg)
		androidmk.prjCfgSrcFiles(cfgFiles[cfg])
		androidmk.prjDependencies(prj, cfg)
		androidmk.prjLdFlags(prj, cfg)
		androidmk.prjCFlags(prj, cfg)
		androidmk.prjShortCommands(prj, cfg) -- AUDIOKINETIC: Support LOCAL_SHORT_COMMANDS directive
		androidmk.wksDisableStrip(cfg)

		-- Always last
		androidmk.prjKind(prj, cfg)

		p.w('endif')
	end
	androidmk.postbuildcommands(prj, cfg)
end


function androidmk.prjHeader(prj)
	p.w('')
	p.w('include $(CLEAR_VARS)')
	p.w('LOCAL_MODULE := %s', prj.name)
	if prj.targetname then
		p.w('LOCAL_MODULE_FILENAME := lib%s', prj.targetname)
	end
	p.w('')
end

local prebuiltLinksCache = {}

local function getPrebuiltLinks(prj, kind, ext, cfg)		-- We only wish to have things built through prebuild, but not our projects.
	local loopAllDependencies = function(cfg, callback)
		local cachename = prj.name:sha1() .. kind:sha1() .. ext:sha1() .. cfg.name:sha1()
		if prebuiltLinksCache[cachename] then
			for _, modulename in ipairs(prebuiltLinksCache[cachename]) do
				callback(modulename, prebuiltLinksCache[modulename])
			end
		else
			prebuiltLinksCache[cachename] = {}
			
			local dependencies = p.config.getlinks(cfg, "all", "object")
			for _, dep in ipairs(dependencies) do
				if type(dep) == "table" and dep.class ~= p.project then
					dep = dep.parent
				end
				if p.global.hasproject(dep) and not p.workspace.findproject(prj.parent, dep.name or dep) then
					local depprj = p.global.getexportedproject(dep)
					dep = project.getconfig(depprj, cfg.buildcfg, cfg.platform)
					local srcfile = project.getrelative(prj, dep.linktarget.fullpath)
					local modulename = depprj.name
					if dep.kind == kind or string.sub(srcfile, -string.len(ext)) == ext then
						table.insert(prebuiltLinksCache[cachename], modulename)
						prebuiltLinksCache[modulename] = srcfile
						callback(modulename, srcfile)
					end
				end
			end
		end
		return prebuiltLinksCache[cachename]
	end
	
	local links = {}
	local newlinks = {}

	if not cfg then
		-- Looking up for links that are shared by everyone (global)
		local first = true
		for cfg in project.eachconfig(prj) do
			newlinks = {}
			loopAllDependencies(cfg, function(modulename, srcfile)
				if first then
					links[modulename] = srcfile
				else
					newlinks[modulename] = srcfile
				end
			end)
			
			-- Check to see if we have all the links
			if first then
				first = false
			else
				for modulename, srcfile in pairs(links) do
					if not newlinks[modulename] then
						links[modulename] = nil
					end
				end
			end
		end
	else
		-- Looking for links specifically in this cfg
		loopAllDependencies(cfg, function(modulename, srcfile)
			links[modulename] = srcfile
		end)
		
		-- Delete global links from everywhere else
		newlinks = getPrebuiltLinks(prj, kind, ext)
		for _, modulename in ipairs(newlinks) do
			links[modulename] = nil
		end
	end

	newlinks = {}
	for modulename, srcfile in pairs(links) do
		if srcfile then
			table.insert(newlinks, modulename)
			newlinks[modulename] = srcfile
		end
	end
	links = newlinks
	
	table.sort(links)
	return links
end

function androidmk.prjPrebuildStatic(prj, links)
	local prefix = iif(links, "  ", "")
	if not links then
		links = getPrebuiltLinks(prj, p.STATICLIB, ".a")
	end
	for _, v in ipairs(links) do
		p.w(prefix .. "include $(CLEAR_VARS)")
		p.w(prefix .. "LOCAL_MODULE    := %s", v)
		p.w(prefix .. "LOCAL_SRC_FILES := %s", links[v])
		p.w(prefix .. "include $(PREBUILT_STATIC_LIBRARY)")
	end
end

function androidmk.prjPrebuildShared(prj, links)
	local prefix = iif(links, "  ", "")
	if not links then
		links = getPrebuiltLinks(prj, p.SHAREDLIB, ".so")
	end
	for _, v in ipairs(links) do
		p.w(prefix .. "include $(CLEAR_VARS)")
		p.w(prefix .. "LOCAL_MODULE    := %s", v)
		p.w(prefix .. "LOCAL_SRC_FILES := %s", links[v])
		p.w(prefix .. "include $(PREBUILT_SHARED_LIBRARY)")
	end
end

function androidmk.prjConfigSpecificPrebuild(prj)
	for cfg in project.eachconfig(prj) do
		local cfgName = androidmk.cfgname(cfg)

		local staticlinks = getPrebuiltLinks(prj, p.STATICLIB, ".a", cfg)
		local sharedlinks = getPrebuiltLinks(prj, p.SHAREDLIB, ".so", cfg)

		if #staticlinks > 0 or #sharedlinks > 0 then
			p.w('')
			p.x('ifeq ($(%s),%s)', androidmk.CONFIG_OPTION, cfgName)

			androidmk.prjPrebuildStatic(prj, staticlinks)
			androidmk.prjPrebuildShared(prj, sharedlinks)

			p.w('endif')
		end

		if #cfg.amk_staticlinks > 0 or #cfg.amk_sharedlinks > 0 then
			p.w('')
			p.x('ifeq ($(%s),%s)', androidmk.CONFIG_OPTION, cfgName)

			androidmk.prjPrebuildStatic(prj, cfg.amk_staticlinks)
			androidmk.prjPrebuildShared(prj, cfg.amk_sharedlinks)

			p.w('endif')
		end
	end
end

function androidmk.prjKind(prj, cfg)
	if cfg.kind == p.STATICLIB  then
		p.w('  include $(BUILD_STATIC_LIBRARY)')
	else -- cfg.kind == p.SHAREDLIB
		p.w('  include $(BUILD_SHARED_LIBRARY)')
	end
end

function androidmk.prebuildcommands(prj, cfg)
	local cfg = project.getfirstconfig(prj)
	if #cfg.amk_prebuildcommands > 0 then
		p.w("")
		for _, v in ipairs(cfg.amk_prebuildcommands) do
			p.w(v)
		end
	end
end

function androidmk.postbuildcommands(prj, cfg)
	local cfg = project.getfirstconfig(prj)
	if #cfg.amk_postbuildcommands > 0 then
		p.w("")
		for _, v in ipairs(cfg.amk_postbuildcommands) do
			p.w(v)
		end
	end
end

function androidmk.prjIncludes(prj, cfg)
	if cfg.includedirs then
		p.w('  LOCAL_C_INCLUDES := %s', 
			table.implode(
				table.translate(
					table.translate(cfg.includedirs,
						function(d)
							if string.sub(d, 1, 2) == "$(" then
								return d   -- Absolute path
							else
								-- AUDIOKINETIC start
								local reld = project.getrelative(prj, d)
								if string.match(reld, "^[a-zA-Z]:[\\/]") then
									return d   -- Failed to get a relative path (could be on a different Windows drive). Use absolute path
								end
								return "$(LOCAL_PATH)/"..reld -- Relative path
								-- AUDIOKINETIC end
							end
						end)
					, p.esc)
			, '', '', ' '))
	end
end

function androidmk.prjCppFeatures(prj, cfg)
	local features = {}

	if cfg.rtti == p.ON then
		table.insert(features, "rtti")
	end

	if cfg.exceptionhandling == p.ON then
		table.insert(features, "exceptions")
	end

	if #features > 0 then
		p.w('  LOCAL_CPP_FEATURES := %s', table.implode(features, '', '', ' '))
	end
end

function androidmk.prepareSrcFiles(prj)
	local root = {}
	local configs = {}
	for cfg in project.eachconfig(prj) do
		configs[cfg] = {}
	end

	local tr = project.getsourcetree(prj)
	p.tree.traverse(tr, {
		onleaf = function(node, depth)
			-- Figure out what configurations contain this file
			local incfg = {}
			local inall = true
			for cfg in project.eachconfig(prj) do
				local filecfg = p.fileconfig.getconfig(node, cfg)
				if filecfg and not filecfg.flags.ExcludeFromBuild then
					incfg[cfg] = filecfg
				else
					inall = false
				end
			end

			-- Allow .arm, .neon and .arm.neon files
			if not path.iscppfile(node.abspath) and
				path.getextension(node.abspath) ~= "arm" and
				path.getextension(node.abspath) ~= "neon" then
				return
			end

			local filename = project.getrelative(prj, node.abspath)

			-- If this file exists in all configurations, write it to
			-- the project's list of files, else add to specific cfgs
			if inall then
				table.insert(root, filename)
			else
				for cfg in project.eachconfig(prj) do
					if incfg[cfg] then
						table.insert(configs[cfg], filename)
					end
				end
			end

		end
	})

	return root, configs
end

function androidmk.prjSrcFiles(files)
	p.w('LOCAL_SRC_FILES := %s', table.implode(table.translate(files, p.esc), '', '', ' '))
end

function androidmk.prjCfgSrcFiles(files)
	if #files > 0 then
		p.w('  LOCAL_SRC_FILES += %s', table.implode(table.translate(files, p.esc), '', '', ' '))
	end
end

function androidmk.prjDependencies(prj, cfg)
	local sysdeps = {}
	local folders = {}
	local staticdeps = {}
	local shareddeps = {}

	if #cfg.libdirs > 0 then
		for _, libdir in ipairs(cfg.libdirs) do
			local fullname = project.getrelative(prj, libdir)
			if not table.contains(folders, fullname) then
				table.insert(folders, fullname)
			end
		end
	end

	local dependencies = p.config.getlinks(cfg, "all", "object")
	for _, dep in ipairs(dependencies) do
		if type(dep) == "string" and p.global.hasproject(dep) then
			local depprj = p.global.getexportedproject(dep)
			dep = project.getconfig(depprj, cfg.buildcfg, cfg.platform)
		end

		if not dep then
			-- SKIP!
		elseif type(dep) == "string" then
			-- We don't have a project named like this. Include as a system item.
			local name = path.getname(dep)
			local folder = path.getdirectory(dep)
			if folder == "." then
				folder = nil
			end
			if name then
				table.insert(sysdeps, name)
			end
			if folder then
				if not table.contains(folders, folder) then
					table.insert(folders, folder)
				end
			end
		else
			local fullname = project.getrelative(prj, dep.linktarget.fullpath)
			local name = path.getname(fullname)
			local folder = path.getdirectory(fullname)
			if name then
				if string.sub(name, 1, 3) == "lib" then
					name = string.sub(name, 4)
				end
				if string.sub(name, -2) == ".a" then
					name = string.sub(name, 1, -3)
					table.insert(staticdeps, name)
				elseif string.sub(name, -3) == ".so" then
					name = string.sub(name, 1, -4)
					table.insert(shareddeps, name)
				elseif dep.kind == p.STATICLIB then
					table.insert(staticdeps, name)
				else
					table.insert(shareddeps, name)
				end
				if folder then
					if not table.contains(folders, folder) then
						table.insert(folders, folder)
					end
				end
			end
		end
	end

	if #staticdeps > 0 then
		p.w('  LOCAL_STATIC_LIBRARIES := %s', table.implode(staticdeps, '', '', ' '))
	end
	if #shareddeps > 0 then
		p.w('  LOCAL_SHARED_LIBRARIES := %s', table.implode(shareddeps, '', '', ' '))
	end

	if #cfg.amk_staticlinks > 0 then
		p.w('  LOCAL_STATIC_LIBRARIES := %s', table.implode(cfg.amk_staticlinks, '', '', ' '))
	end
	if #cfg.amk_sharedlinks > 0 then
		p.w('  LOCAL_SHARED_LIBRARIES := %s', table.implode(cfg.amk_sharedlinks, '', '', ' '))
	end

	if #sysdeps > 0 then
		local result = ""
		if #folders > 0 then
			result = "$(addprefix -L, " .. table.implode(folders, '', '', ' ') .. ") "
		end
		if #sysdeps > 0 then
			result = result .. "$(addprefix -l, " .. table.implode(sysdeps, '', '', ' ') .. ")"
		end
		p.w('  LOCAL_LDLIBS := %s', result)
	end
end

function androidmk.prjLdFlags(prj, cfg)
	--LDFLAGS
	flags = p.config.mapFlags(cfg, androidmk.ldflags)

	for _, opt in ipairs(cfg.linkoptions) do
		table.insert(flags, opt)
	end

	if #flags > 0 then
		p.w('  LOCAL_LDFLAGS := %s', table.implode(table.translate(flags, p.esc), '', '', ' '))
	end
end

function androidmk.prjCFlags(prj, cfg)
	local flags = p.config.mapFlags(cfg, androidmk.cflags)

	-- Defines
	for _, def in ipairs(cfg.defines) do
		table.insert(flags, '-D' .. def)
	end

	-- Warnings
	for _, enable in ipairs(cfg.enablewarnings) do
		table.insert(flags, '-W' .. enable)
	end
	for _, disable in ipairs(cfg.disablewarnings) do
		table.insert(flags, '-Wno-' .. disable)
	end
	for _, fatal in ipairs(cfg.fatalwarnings) do
		table.insert(flags, '-Werror=' .. fatal)
	end

	-- Build options
	if string.find(cfg.shortname, "v7a") then
		table.insert(flags, "-mfloat-abi=softfp")
		table.insert(flags, "-mfpu=neon")
	end
	for _, opt in ipairs(cfg.buildoptions) do
		table.insert(flags, opt)
	end

	-- Symbols
	if cfg.symbols ~= nil then
		if string.find(cfg.symbols, "On") then
			table.insert(flags, "-g")
		end
	end
	
	-- Optimize
	if cfg.optimize ~= nil then
		if string.find(cfg.optimize, "Speed") then
			table.insert(flags, "-O3")
		elseif string.find(cfg.optimize, "On") then
			table.insert(flags, "-O2")
		elseif string.find(cfg.optimize, "Size") then
			table.insert(flags, "-Os")
		end
	end	
	
	if #flags > 0 then
		p.w('  LOCAL_CFLAGS := %s', table.implode(flags, "", "", " "))
	end

	local cppflags = table.shallowcopy(prj.amk_cppflags or {})
	local cppLanguageStandards = {
		-- ["C++98"] = "c++98",		-- Default
		["C++11"] = "c++11",		-- Clang 3.8: ok
		["C++14"] = "c++14",		-- Clang 3.8: ok
		["C++17"] = "c++1z",		-- Clang 3.8: 1z, clang 5: c++17
	}
	if cfg.cppdialect then
		local cppLanguageStandard = cppLanguageStandards[cfg.cppdialect]
		if cppLanguageStandard then
			table.insert(cppflags, "-std=" .. cppLanguageStandard)
		end
	end

	if #cppflags > 0 then
		p.w('  LOCAL_CPPFLAGS := %s', table.implode(cppflags, '', '', ' '))
	end

	local conlyflags = {}
	local cLanguageStandards = {
		["C89"] = "c89",
		["C90"] = "c90",
		["C99"] = "c99",
		["C11"] = "c11",
		["gnu89"] = "gnu89",
		["gnu90"] = "gnu90",
		["gnu99"] = "gnu99",
		["gnu11"] = "gnu11",
	}
	if cfg.cdialect then
		local cLanguageStandard = cLanguageStandards[cfg.cdialect]
		if cLanguageStandard then
			table.insert(conlyflags, "-std=" .. cLanguageStandard)
		end
	end
	if #conlyflags > 0 then
		p.w('  LOCAL_CONLYFLAGS := %s', table.implode(conlyflags, '', '', ' '))
	end

end

-- AUDIOKINETIC START
function androidmk.prjShortCommands(prj, cfg)
	if cfg.ndkshortcommands ~= nil and cfg.ndkshortcommands then
		p.w('  LOCAL_SHORT_COMMANDS := true')
	end
end
-- AUDIOKINETIC END
