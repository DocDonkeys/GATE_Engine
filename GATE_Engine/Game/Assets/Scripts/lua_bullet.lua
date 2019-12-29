local Functions = Debug.Scripting ()

function	GetTablelua_bullet ()
local lua_table = {}
lua_table["Functions"] = Debug.Scripting ()
lua_table["Speed"] = 5.0
lua_table["Life Time"] = 3000.0
lua_table["Life Start (Read Only)"] = 0.0

function lua_table:Awake ()
	if lua_table["Functions"]:time () < 100.0
	then lua_table["Functions"]:Activate (false) lua_table["Functions"]:GO_Activate (false)
	else lua_table["Functions"]:Activate (true) lua_table["Functions"]:GO_Activate (true)
	end	--If not enough time has passed and this Awake has been called, we assume it's the original obj and disabled it, otherwise we enable it (not great, but currently it's good enough)
end

function lua_table:Start ()
	lua_table["Life Start"] = lua_table["Functions"]:time ()
end

function lua_table:Update ()
	if (lua_table["Functions"]:time () - lua_table["Life Start"]) > lua_table["Life Time"]
	then lua_table["Functions"]:GO_Destroy ()
	else lua_table["Functions"]:Translate (0.0, 0.0, lua_table["Speed"] * lua_table["Functions"]:dt (), true)
	end
end

return lua_table
end

-- Old Didac Code
--local LOG = Debug.Scripting ()
--
--function	GetTablelua_tabletest ()
--local lua_table = {}
--lua_table["position_x"] = 0
--lua_table["LOG"] = Debug.Scripting ()
--lua_table["GetDT"] = Debug.Scripting ()
--
--function lua_table:Awake ()
--	lua_table["position_x"] = 30
--	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on AWAKE")
--end
--
--function lua_table:Start ()
--	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on START")
--end
--
--function lua_table:Update ()
--	lua_table["position_x"] = lua_table["position_x"] + 1
--
--	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on UPDATE")
--	lua_table["LOG"]:LOG ("Position X = " .. lua_table["position_x"])
--
--	lua_table["LOG"]:LOG ("GameDT = " .. lua_table["GetDT"]:GetDT ())
--end
--
--return lua_table
--end
