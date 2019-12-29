local Functions = Debug.Scripting ()

function	GetTablelua_bullet ()
local lua_table = {}
lua_table["Functions"] = Debug.Scripting ()

function lua_table:Awake ()
end

function lua_table:Start ()
	mustdie = false
	startTime = lua_table["Functions"]:time ()
end

function lua_table:Update ()
	--if (lua_table["Functions"]:time () - startTime) > 5000 then --die end
	lua_table["Functions"]:Translate (0.0, 0.0, 5.0 * lua_table["Functions"]:dt (), true)
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
