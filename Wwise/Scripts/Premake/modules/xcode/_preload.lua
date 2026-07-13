---
-- xcode/_preload.lua
-- Define the Apple XCode actions and new APIs.
-- Copyright (c) 2009-2015 Jess Perkins and the Premake project
---

	local p = premake


--
-- Register new Xcode-specific project fields.
--

	p.api.register {
		name = "swiftversion",
		scope = "config",
		kind = "string",
		allowed = {
			"4.0",
			"4.2",
			"5.0",
		}
	}

	p.api.register {
		name = "xcodebuildsettings",
		scope = "config",
		kind = "key-array",
	}

	p.api.register {
		name = "xcodebuildresources",
		scope = "config",
		kind = "list",
	}

	p.api.register {
		name = "xcodecodesigningidentity",
		scope = "config",
		kind = "string",
	}

	p.api.register {
		name = "xcodesystemcapabilities",
		scope = "project",
		kind = "key-boolean",
	}

	p.api.register {
		name = "xcodelastupgradecheck",
		scope = "project",
		kind = "string",
	}

	p.api.register {
		name = "iosfamily",
		scope = "config",
		kind = "string",
		allowed = {
			"iPhone/iPod touch",
			"iPad",
			"Universal",
		}
	}
	
	-- AUDIOKINETIC adding xcscheme generation
	---
	-- Create a special container for xcscheme configuration
	---
	p.xcscheme = p.api.container("xcscheme", p.workspace)
	function p.xcscheme.new(name)
		return p.container.new(p.xcscheme, name)
	end

	function p.xcscheme.bake(self)
		print("    Baking " .. self.name .. ".xcscheme (" .. self.parent.name .. ")...")
		table.insert(p.xcscheme.list, self)
		local wks = self.parent
		self.workspace = wks
		
		self.location = p.filename(self.workspace, ".xcworkspace/xcshareddata/xcschemes/")
	end
	
	p.xcscheme.list = {}


	p.api.register {
		name = "embed",
		scope = "config",
		kind = "list",
	}

	p.api.register {
		name = "embedAndSign",
		scope = "config",
		kind = "list",
	}

--
-- Register the Xcode exporters.
--

	newaction {
		trigger     = "xcode4",
		shortname   = "Apple Xcode 4",
		description = "Generate Apple Xcode 4 project files",

		-- Xcode always uses Mac OS X path and naming conventions

		toolset  = "clang", -- TODO: os.target() == p.MACOSX and p.checkVersion(minOSVersion, "<10.7") -> gcc

		-- The capabilities of this action

		valid_kinds     = { "ConsoleApp", "WindowedApp", "SharedLib", "StaticLib", "Makefile", "Utility", "None" },
		valid_languages = { "C", "C++" },
		valid_tools     = {
			cc = { "gcc", "clang" },
		},


		-- Workspace and project generation logic

		onWorkspace = function(wks)
			p.generate(wks, ".xcworkspace/contents.xcworkspacedata", p.modules.xcode.generateWorkspace)
		end,

		onProject = function(prj)
			p.generate(prj, ".xcodeproj/project.pbxproj", p.modules.xcode.generateProject)
		end,
		
		-- AUDIOKINETIC adding xcscheme generation.
		execute = function()
			for _, scfg in ipairs(p.xcscheme.list) do
				p.generate(scfg, ".xcscheme", p.modules.xcode.generateXcscheme)
			end
		end,
	}


--
-- Decide when the full module should be loaded.
--

	return function(cfg)
		return (_ACTION == "xcode4")
	end
