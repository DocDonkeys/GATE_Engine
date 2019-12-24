local LOG = Debug.Scripting ()

lua_table = {}
lua_table["position_x"] = 0

function Start ()  

end

function Update ()
position_x = lua_table["position_x"] + 1
 lua_table["position_x"] = position_x

 LOG:LOG ("This Log was called from LUA testing a table")
end


lua_table["Start"] = Start ()
lua_table["Update"] = Update ()


