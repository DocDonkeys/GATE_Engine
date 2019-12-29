local Functions = Debug.Scripting ()

function	GetTablelua_tankcontrols ()
local lua_table = {}
lua_table["Functions"] = Debug.Scripting ()
lua_table["Movement Speed"] = 500.0
lua_table["Rotation Speed"] = 100.0
lua_table["Up"] = "W"
lua_table["Down"] = "S"
lua_table["Rotate Left"] = "A"
lua_table["Rotate Right"] = "D"

function lua_table:Awake ()
end

function lua_table:Start ()
end

function lua_table:Update ()
	dt = lua_table["Functions"]:dt ()

	if lua_table["Functions"]:KeyRepeat (lua_table["Up"]) then lua_table["Functions"]:Translate (0.0, 0.0, lua_table["Movement Speed"] * dt) lua_table["Functions"]:LOG ("Forward") end
	if lua_table["Functions"]:KeyRepeat (lua_table["Rotate Left"]) then lua_table["Functions"]:EulerRotate (0.0, lua_table["Rotation Speed"] * dt, 0.0) lua_table["Functions"]:LOG ("Turn Left") end
	if lua_table["Functions"]:KeyRepeat (lua_table["Down"]) then lua_table["Functions"]:Translate (0.0, 0.0, lua_table["Movement Speed"] * -dt) lua_table["Functions"]:LOG ("Backwards") end
	if lua_table["Functions"]:KeyRepeat (lua_table["Rotate Right"]) then lua_table["Functions"]:EulerRotate (0.0, lua_table["Rotation Speed"] * -dt, 0.0) lua_table["Functions"]:LOG ("Turn Right") end
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
