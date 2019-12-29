local Functions = Debug.Scripting ()

function	GetTablelua_tankcontrols ()
local lua_table = {}
lua_table["Functions"] = Debug.Scripting ()
lua_table["Speed"] = 10
lua_table["Bool_test"] = true
lua_table["String"] = "Test_String"

function lua_table:Awake ()
end

function lua_table:Start ()
end

function lua_table:Update ()
	dt = lua_table["Functions"]:dt ()

	if lua_table["Functions"]:KeyRepeat ("W") then lua_table["Functions"]:Translate (0.0, 0.0, 500.0 * dt) lua_table["Functions"]:LOG ("Forward") end
	if lua_table["Functions"]:KeyRepeat ("A") then lua_table["Functions"]:Rotate (0.0, 100.0 * dt, 0.0) lua_table["Functions"]:LOG ("Turn Left") end
	if lua_table["Functions"]:KeyRepeat ("S") then lua_table["Functions"]:Translate (0.0, 0.0, -500.0 * dt) lua_table["Functions"]:LOG ("Backwards") end
	if lua_table["Functions"]:KeyRepeat ("D") then lua_table["Functions"]:Rotate (0.0, -100.0 * dt, 0.0) lua_table["Functions"]:LOG ("Turn Right") end
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
