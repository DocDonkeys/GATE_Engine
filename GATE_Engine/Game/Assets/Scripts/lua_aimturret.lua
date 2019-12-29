local Functions = Debug.Scripting ()

function	GetTablelua_aimturret ()
local lua_table = {}
lua_table["Functions"] = Debug.Scripting ()
lua_table["Fire Mouse Button"] = "Left"

function lua_table:Awake ()
	origBulletUID =  lua_table["Functions"]:FindUID ("Shell_")
end

function lua_table:Start ()
end

function lua_table:Update ()
	hit_x, hit_y, hit_z = lua_table["Functions"]:MouseRaycastHit ();

	if hit_x ~= nil
	then
	lua_table["Functions"]:LookAt (hit_x, lua_table["Functions"]:position_y (false), hit_z, false) lua_table["Functions"]:LOG ("Aiming")
	else
	lua_table["Functions"]:LookTo (0.0, 0.0, 1.0, true)
	end

	if
	lua_table["Functions"]:MouseInGame () and lua_table["Functions"]:MouseButtonDown (lua_table["Fire Mouse Button"]) 
	then
	--pos_x, pos_y, pos_z = lua_table["Functions"]:position (false)
	--rot_x, rot_y, rot_z = lua_table["Functions"]:rotation (false)
	pos_x = lua_table["Functions"]:position_x (false)
	pos_y = 1.0 + lua_table["Functions"]:position_y (false)
	pos_z = lua_table["Functions"]:position_z (false)

	rot_x = lua_table["Functions"]:rotation_x (false);
	rot_y = lua_table["Functions"]:rotation_y (false);
	rot_z = lua_table["Functions"]:rotation_z (false);

	lua_table["Functions"]:InstantiateByUID (origBulletUID, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, 1.0)
	lua_table["Functions"]:LOG ("Open Fire!")
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
