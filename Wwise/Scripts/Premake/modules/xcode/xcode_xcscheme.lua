---
-- xcode/xcode_xcscheme.lua
-- Generate an Xcode scheme.
-- TODO Right now, we only support build-all, so we copy all projects to the xcscheme, but eventually, it should works pretty much like group.
-- Author Michel Donais (Audiokinetic)
---

local p = premake
local xcode = p.modules.xcode
local tree = p.tree

if not xcode.xcscheme then
	xcode.xcscheme = {
		buildaction = {},
		testaction = {},
		launchaction = {},
		profileaction = {},
		analyzeaction = {},
		archiveaction = {},
	}
	local xcscheme = xcode.xcscheme
	local buildaction = xcscheme.buildaction
	local testaction = xcscheme.testaction
	local launchaction = xcscheme.launchaction
	local profileaction = xcscheme.profileaction
	local analyzeaction = xcscheme.analyzeaction
	local archiveaction = xcscheme.archiveaction

	xcode.elements.xcscheme = {}
	local elements = xcode.elements.xcscheme


	---
	-- Entry point
	---
	
	function xcode.generateXcscheme(scfg)
		-- TODO Right now, we only support build-all, so we copy all projects to the xcscheme, but eventually, it should works pretty much like group.
		scfg.projects = {}
		local wks = scfg.workspace
		scfg.startproject = wks.startproject
		scfg.xcodelastupgradecheck = wks.xcodelastupgradecheck
		
		for prj in p.workspace.eachproject(wks) do
			local exportedPrj = nil
			if p.global.isprojectimported(prj) then
				if p.global.hasproject(prj) then
					exportedPrj = p.global.getexportedproject(prj)
				end
			else
				exportedPrj = prj
			end

			if exportedPrj then
				table.insert(scfg.projects, exportedPrj)
				if not scfg.startproject then
					scfg.startproject = exportedPrj.name
				end
			end
		end
		-- end of TODO
		
		-- Retrieve the StartProject
		local fallbackstartproject = nil
		for _, prj in ipairs(scfg.projects) do
			fallbackstartproject = fallbackstartproject or prj
			if scfg.startproject == prj.name then
				scfg.startproject_ = prj
				break
			end
		end
		scfg.startproject_ = scfg.startproject_ or fallbackstartproject
		
		xcscheme.generate(scfg)
	end


	---
	-- Root-level
	---
	
	function elements.xcscheme(scfg)
		return {
			xcscheme.xmldeclaration,
			xcscheme.head,
			buildaction.generate,
			testaction.generate,
			launchaction.generate,
			profileaction.generate,
			analyzeaction.generate,
			archiveaction.generate,
			xcscheme.tail
		}
	end

	function xcscheme.generate(scfg)
		p.callArray(elements.xcscheme, scfg)
	end

	function xcscheme.xmldeclaration()
		p.xmlUtf8(true)
	end

	function xcscheme.head(scfg)
		p.push('<Scheme')
		p.w('LastUpgradeVersion = "' .. scfg.xcodelastupgradecheck .. '"')
		p.w('version = "1.3">')
	end

	function xcscheme.tail()
		p.pop('</Scheme>')
	end


	---
	-- Utilities
	---
	function xcscheme.startprojectmacroexpansion(scfg)
		p.push('<MacroExpansion>')
		local prj = scfg.startproject_
		local tr = p.project.getsourcetree(prj, nil, false)
		for _, node in ipairs(tr.products.children) do
			p.push('<BuildableReference')
			p.w('BuildableIdentifier = "primary"')
			p.w('BlueprintIdentifier = "' .. node.targetid .. '"')
			p.w('BuildableName = "' .. node.name .. '"')
			p.w('BlueprintName = "' .. tr.name .. '"')
			p.w('ReferencedContainer = "container:' .. path.getrelative(scfg.workspace.location,  p.filename(prj, ".xcodeproj")) .. '">')
			p.pop('</BuildableReference>')
			break	-- We only care for one
		end
		p.pop('</MacroExpansion>')
	end
	
	
	---
	-- BuildAction
	---
	
	function elements.buildaction(scfg)
		return {
			buildaction.head,
			buildaction.entries,
			buildaction.tail
		}
	end
	
	function buildaction.generate(scfg)
		p.callArray(elements.buildaction, scfg)
	end

	function buildaction.head()
		p.push('<BuildAction')
		p.w('parallelizeBuildables = "YES"')
		p.w('buildImplicitDependencies = "YES">')
	end

	function buildaction.tail()
		p.pop('</BuildAction>')
	end

	function buildaction.entries(scfg)
		p.push('<BuildActionEntries>')
		
		for _, prj in ipairs(scfg.projects) do
			local tr = p.project.getsourcetree(prj, nil, false)
			for _, node in ipairs(tr.products.children) do
				p.push('<BuildActionEntry')
				p.w('buildForTesting = "YES"')
				p.w('buildForRunning = "YES"')
				p.w('buildForProfiling = "YES"')
				p.w('buildForArchiving = "YES"')
				p.w('buildForAnalyzing = "YES">')
				p.push('<BuildableReference')
				p.w('BuildableIdentifier = "primary"')
				p.w('BlueprintIdentifier = "' .. node.targetid .. '"')
				p.w('BuildableName = "' .. node.name .. '"')
				p.w('BlueprintName = "' .. tr.name .. '"')
				p.w('ReferencedContainer = "container:' .. path.getrelative(scfg.workspace.location,  p.filename(prj, ".xcodeproj")) .. '">')
				p.pop('</BuildableReference>')
				p.pop('</BuildActionEntry>')
			end
		end
		
		p.pop('</BuildActionEntries>')
	end

	
	---
	-- TestAction
	---
	
	function elements.testaction(scfg)
		return {
			testaction.head,
			testaction.testables,
			testaction.macroexpansion,
			testaction.additionaloptions,
			testaction.tail
		}
	end
	
	function testaction.generate(scfg)
		p.callArray(elements.testaction, scfg)
	end

	function testaction.head()
		p.push('<TestAction')
		p.w('buildConfiguration = "Debug"')												-- TODO: Debug is hard-coded!
		p.w('selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"')
		p.w('selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"')
		p.w('language = ""')
		p.w('shouldUseLaunchSchemeArgsEnv = "YES">')
	end

	function testaction.tail()
		p.pop('</TestAction>')
	end

	function testaction.testables(scfg)
		p.push('<Testables>')
		p.pop('</Testables>')
	end

	function testaction.macroexpansion(scfg)
		xcscheme.startprojectmacroexpansion(scfg)
	end

	function testaction.additionaloptions(scfg)
		p.push('<AdditionalOptions>')
		p.pop('</AdditionalOptions>')
	end


	---
	-- LaunchAction
	---
	
	function elements.launchaction(scfg)
		return {
			launchaction.head,
			launchaction.macroexpansion,
			launchaction.additionaloptions,
			launchaction.tail
		}
	end
	
	function launchaction.generate(scfg)
		p.callArray(elements.launchaction, scfg)
	end

	function launchaction.head()
		p.push('<LaunchAction')
		p.w('buildConfiguration = "Debug"')												-- TODO: Debug is hard-coded!
		p.w('selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"')
		p.w('selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"')
		p.w('language = ""')
		p.w('launchStyle = "0"')
		p.w('useCustomWorkingDirectory = "NO"')
		p.w('ignoresPersistentStateOnLaunch = "NO"')
		p.w('debugDocumentVersioning = "YES"')
		p.w('debugServiceExtension = "internal"')
		p.w('allowLocationSimulation = "YES">')

	end

	function launchaction.tail()
		p.pop('</LaunchAction>')
	end
	
	function launchaction.macroexpansion(scfg)
		xcscheme.startprojectmacroexpansion(scfg)
	end

	function launchaction.additionaloptions(scfg)
		p.push('<AdditionalOptions>')
		p.pop('</AdditionalOptions>')
	end

	
	---
	-- ProfileAction
	---
	
	function elements.profileaction(scfg)
		return {
			profileaction.head,
			profileaction.macroexpansion,
			profileaction.tail
		}
	end
	
	function profileaction.generate(scfg)
		p.callArray(elements.profileaction, scfg)
	end

	function profileaction.head()
		p.push('<ProfileAction')
		p.w('buildConfiguration = "Release"')												-- TODO: Release is hard-coded!
		p.w('shouldUseLaunchSchemeArgsEnv = "YES"')
		p.w('savedToolIdentifier = ""')
		p.w('useCustomWorkingDirectory = "NO"')
		p.w('debugDocumentVersioning = "YES">')
	end

	function profileaction.tail()
		p.pop('</ProfileAction>')
	end
	
	function profileaction.macroexpansion(scfg)
		xcscheme.startprojectmacroexpansion(scfg)
	end


	---
	-- AnalyzeAction
	---
	
	function elements.analyzeaction(scfg)
		return {
			analyzeaction.head,
			analyzeaction.tail
		}
	end
	
	function analyzeaction.generate(scfg)
		p.callArray(elements.analyzeaction, scfg)
	end

	function analyzeaction.head()
		p.push('<AnalyzeAction')
		p.w('buildConfiguration = "Debug">')						-- TODO: Debug is hard-coded!
	end

	function analyzeaction.tail()
		p.pop('</AnalyzeAction>')
	end


	---
	-- ArchiveAction
	---
	
	function elements.archiveaction(scfg)
		return {
			archiveaction.head,
			archiveaction.tail
		}
	end
	
	function archiveaction.generate(scfg)
		p.callArray(elements.archiveaction, scfg)
	end

	function archiveaction.head()
		p.push('<ArchiveAction')
		p.w('buildConfiguration = "Release"')						-- TODO: Debug is hard-coded!
		p.w('revealArchiveInOrganizer = "YES">')
	end

	function archiveaction.tail()
		p.pop('</ArchiveAction>')
	end

end
