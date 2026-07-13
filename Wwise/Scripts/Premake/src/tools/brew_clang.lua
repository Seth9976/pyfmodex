--
-- brew_clang.lua
-- Clang toolset adapter for Premake
-- Copyright (c) 2019 Joel Robichaud
--

premake.tools.brew_clang = {}

local brew_clang = premake.tools.brew_clang
local clang = premake.tools.clang

brew_clang.getwarnings           = clang.getwarnings
brew_clang.getcflags             = clang.getcflags
brew_clang.getcxxflags           = clang.getcxxflags
brew_clang.getdefines            = clang.getdefines
brew_clang.getundefines          = clang.getundefines
brew_clang.getforceincludes      = clang.getforceincludes
brew_clang.getincludedirs        = clang.getincludedirs
brew_clang.getrunpathdirs        = clang.getrunpathdirs
brew_clang.getsharedlibarg       = clang.getsharedlibarg
brew_clang.getLibraryDirectories = clang.getLibraryDirectories
brew_clang.getlinks              = clang.getlinks
brew_clang.getmakesettings       = clang.getmakesettings
brew_clang.getLibraryExtensions  = clang.getLibraryExtensions

function brew_clang.getcppflags(cfg)
	return table.join(clang.getcppflags(cfg), {
		"-I/usr/local/opt/llvm/include"
	})
end

function brew_clang.getldflags(cfg)
	return table.join(clang.getldflags(cfg), {
		"-L/usr/local/opt/llvm/lib",
		"-Wl,-rpath,/usr/local/opt/llvm/lib"
	})
end

brew_clang.tools = {
	cc = "/usr/local/opt/llvm/bin/clang",
	cxx = "/usr/local/opt/llvm/bin/clang++",
	ar = "ar"
}

function brew_clang.gettoolname(cfg, tool)
	return brew_clang.tools[tool]
end
