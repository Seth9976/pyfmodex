---
-- global.lua
-- The global container holds workspaces and rules.
-- Copyright (c) 2014-2015 Jess Perkins and the Premake project
---

	local p = premake
	p.global = p.api.container("global")
	local global = p.global


---
-- Create a new global container instance.
---

	function global.new(name)
		return p.container.new(p.global, name)
	end

---
-- Iterate over the collection of rules in a session.
--
-- @returns
--    An iterator function.
---

	function global.eachRule()
		local root = p.api.rootContainer()
		return p.container.eachChild(root, p.rule)
	end



---
-- Iterate over the collection of workspaces in a session.
--
-- @returns
--    A workspace iterator function.
---

	function global.eachWorkspace()
		local root = p.api.rootContainer()
		return p.container.eachChild(root, p.workspace)
	end

	p.alias(global, "eachWorkspace", "eachSolution")



---
-- Retrieve a rule by name or index.
--
-- @param key
--    The rule key, either a string name or integer index.
-- @returns
--    The rule with the provided key.
---

	function global.getRule(key)
		local root = p.api.rootContainer()
		return root.rules[key]
	end



---
-- Retrieve the rule to applies to the provided file name, if any such
-- rule exists.
--
-- @param fname
--    The name of the file.
-- @param rules
--    A list of rule names to be included in the search. If not specified,
--    all rules will be checked.
-- @returns
--    The rule, is one has been registered, or nil.
---

	function global.getRuleForFile(fname, rules)
		for rule in global.eachRule() do
			if not rules or table.contains(rules, rule.name) then
				if path.hasextension(fname, rule.fileextension) then
					return rule
				end
			end
		end
	end



---
-- Retrieve a workspace by name or index.
--
-- @param key
--    The workspace key, either a string name or integer index.
-- @returns
--    The workspace with the provided key.
---

	function global.getWorkspace(key)
		local root = p.api.rootContainer()
		return root.workspaces[key]
	end

	p.alias(global, "getWorkspace", "getSolution")

	
	-- AUDIOKINETIC mdonais Adding importproject
	
	-- Check if the project is actually imported
	function global.isprojectimported(prj)
		if not prj then
			error("Project is nil!",2)
		end
		if prj.import then
			return true
		else
			return false
		end
	end
	
	-- Returns whether the project actually exists or is the import pointer is dangling
	-- This is mostly the same principle than getexportedproject() below, except it's const and doesn't warn of issues.
	function global.hasproject(nameOrPrj)
		if not nameOrPrj then
			return false
		end
		if type(nameOrPrj) ~= 'string' then
			if not nameOrPrj.import then
				return true		-- We are not an import, we are valid!
			end
			local wksName = ""
			if type(nameOrPrj.import) == 'string' then
				wksName = nameOrPrj.import .. ":"
			end
			nameOrPrj = wksName .. nameOrPrj.name
		end
		local name = nameOrPrj
		
		local root = p.api.rootContainer()
		if not root.projects then
			return false
		end
		local prj = root.projects[name]
		if not prj then
			return false
		end
		if prj.import then		-- Cannot get an imported project, these aren't real!
			if root.projectsDupe then
				local prevPrj = root.projectsDupe[name]
				if prevPrj and not prevPrj.import then
					return true
				end
			end
			return false
		end
		return true
	end
	
	-- Returns the actual project that's being imported, if applicable.
	-- Also makes certain to return the baked goods when they are available.
	function global.getexportedproject(nameOrPrj)
		function refreshprj(prj)		-- Baking copies workspace and project to another one. We must have the new one!
			if prj.parent then
				return p.workspace.findproject(global.getWorkspace(prj.parent.name), prj.name)
			end
			return prj
		end
		
		if not nameOrPrj then
			return nameOrPrj
		end
		if type(nameOrPrj) ~= 'string' then
			if not nameOrPrj.import then
				return nameOrPrj
			end
			local wksName = ''
			if type(nameOrPrj.import) == 'string' then
				wksName = nameOrPrj.import .. ":"
			end
			nameOrPrj = wksName .. nameOrPrj.name
		end
		local name = nameOrPrj
		
		local root = p.api.rootContainer()
		if not root.projects then
			warnf("Could not find imported project %s", name)
			return nil
		end
		local prj = root.projects[name]
		if not prj then
			warnf("Could not find imported project %s" , name)
			return nil
		end
		if prj.import then		-- Cannot get an imported project, these aren't real!
			if root.projectsDupe then
				local prevPrj = root.projectsDupe[name]
				root.projectsDupe[name] = nil
				if prevPrj and not prevPrj.import then
					prevPrj = refreshprj(prevPrj)
					root.projects[name] = prevPrj
					return prevPrj
				end
			end
			root.projects[name] = nil
			warnf("Could not find imported project %s", name)
			return nil
		end
		if root.projectsDupe then
			local prevPrj = root.projectsDupe[name]
			root.projectsDupe[name] = nil
			if prevPrj then
				warnf("Trying to get an exported project that's defined twice or more. Importing %s from parent %s. Prev was from parent %s. Consider using WorkspaceName:ProjectName format for importation.", name, prj.parent.name, prevPrj.parent.name)
			end
		end
		prj = refreshprj(prj)
		root.projects[name] = prj
		return prj
	end
	
	-- Called automatically when creating a new project. Assumes the project container was just created, nothing except the name is valid!
	function global.exportproject(wks, prj)
		-- We must add every project through exportproject, as when this is called, we don't know if it will be an exported project, or an imported project.
		local root = p.api.rootContainer()
		local name = prj.name
		
		if not root.projects then
			root.projects = {}
		end
		local prevPrj = root.projects[name]
		if prevPrj and not prevPrj.import then		-- Can override imported projects as if they don't exist, these aren't real!
			if not root.projectsDupe then
				root.projectsDupe = {}
			end
			local secondPrevPrj = root.projectsDupe[name]
			if secondPrevPrj and not secondPrevPrj.import then
				-- warnf("Trying to triple-import a same-name project! Importing %s. Prevs were from %s and %s.", name, prevPrj.parent.name, secondPrevPrj.parent.name)
			end
			root.projectsDupe[name] = prevPrj
		end
		root.projects[name] = prj
		root.projects[wks.name .. ":" .. name] = prj
	end
	-- AUDIOKINETIC end
