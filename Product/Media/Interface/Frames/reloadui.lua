-----------------------------------------
--  动态加载文件处理
-----------------------------------------
g_CommandList = {"reloadui",
				 "clean",
				 "additem",
				 "addeqgold",
				 "addgamegold",
				 "chgroommat",
				 "send_sys_mail",
				 "send_n_mail",
				 "send_gm_mail",
				 "debug_usefit",
				}
local useDebugCommand = true
----------------------------------------------------------------
function ChatSet_LoadScript(command)
	local buff = string.lower(command)
	--local num
	local temp
	
	for i,v in ipairs(g_CommandList) do
		temp = "/"..v
		if nil ~= string.find(buff,temp) then
			return ChatSet_FindCommand(v,buff)
		end
	end
	
	--for num = 1,6 do
	--	local temp = "/"..g_CommandList[num]
	--	if nil ~= string.find(buff,temp) then
	--		return ChatSet_FindCommand(g_CommandList[num],buff)
	--	end
	--end
end
---------------------------------------------------------------
function  ChatSet_FindCommand(CommandName,Command)
	if CommandName == "reloadui" then
		return reloadui(CommandName,Command)
			
	elseif CommandName == "additem" then
		return additem(CommandName,Command)

	elseif CommandName == "send_n_mail" then
		return send_n_mail(CommandName,Command)
		
	elseif CommandName == "addeqgold" then
		return addeqgold(CommandName,Command)

	elseif CommandName == "addgamegold" then
		return addgamegold(CommandName,Command)
		
	elseif CommandName == "chgroommat" then
		return chgroommat(CommandName,Command)

	elseif CommandName == "send_sys_mail" then
		return send_sys_mail(CommandName,Command)
		
	elseif CommandName == "send_gm_mail" then
		return send_gm_mail(CommandName,Command)
		
	elseif CommandName == "debug_usefit" then
		return debug_usefit(CommandName,Command)
	end
end

--重载UI
function reloadui(CommandName,Command)
	local temp = string.find(Command," ")
	if temp == 10 then
		local len = string.len(Command)
		local filename = string.sub(Command,11,len)
		local path = g_ifpath--.."Frames/"
		path = path..filename
		g_LuaMgr:loadState(path)
		return 1
	else
		return 0
	end
end

--GM添加物品
function additem(CommandName,Command)
	local temp = string.find(Command," ")
	if temp == 9 then
		local len = string.len(Command)
		local com = ""
		com = com..string.sub(Command, 10, len)
		
--		g_Loger:logMessage("com = "..com)
		g_msgmgr:GMRequest(1,com)
		return 1
	end
	return 0
end

--GM添加EQ币
function addeqgold(CommandName,Command)
	local temp = string.find(Command," ")
	if temp == 11 then
		local len = string.len(Command)
		local com = ""
		com = com..string.sub(Command, 12, len)
		
--		g_Loger:logMessage("com = "..com)
		g_msgmgr:GMRequest(2,"2",com)
		return 1
	end
	return 0
end

--GM发送GM邮件
function send_gm_mail(CommandName,Command)
	local pos = string.find(Command," ")
	if pos ~= nil then
		local len = string.len(Command)
		local com = string.sub(Command, pos + 1, len)
		
--		g_Loger:logMessage("com = "..com)
		g_msgmgr:GMRequest(7, com)
		return 1
	end
	return 0
end

--GM发送系统邮件
function send_sys_mail(CommandName,Command)
	local pos = string.find(Command," ")
	if pos ~= nil then
		local len = string.len(Command)
		local com = string.sub(Command, pos + 1, len)
		
--		g_Loger:logMessage("com = "..com)
		g_msgmgr:GMRequest(5, com)
		return 1
	end
	return 0
end

--GM发送n封邮件
function send_n_mail(CommandName,Command)
	local pos = string.find(Command," ")
	if pos ~= nil then
		local len = string.len(Command)
		local com = string.sub(Command, pos + 1, len)
		
--		g_Loger:logMessage("com = "..com)
		g_msgmgr:GMRequest(6, com)
		return 1
	end
	return 0
end

--GM添加游戏币
function addgamegold(CommandName,Command)
	local temp = string.find(Command," ")
	if temp == 13 then
		local len = string.len(Command)
		local com = ""
		com = com..string.sub(Command, 14, len)
		
--		g_Loger:logMessage("com = "..com)
		g_msgmgr:GMRequest(2,"1",com)
		return 1
	end
	return 0
end

--GM改变房间材质
function chgroommat(CommandName,Command)
	local temp = string.find(Command," ")
	if temp == 12 then
		local temp2 = string.find(Command, " ", temp+1)
		local rom = string.sub(Command, temp+1, temp2-1)
		
		temp = temp2
		temp2 = string.find(Command, " ", temp+1)
		local typ = string.sub(Command, temp+1, temp2-1)
		
		temp = temp2
		temp2 = string.find(Command, " ", temp+1)
		local tex = string.sub(Command, temp+1, temp2)
		
--		g_Loger:logMessage("chgroommat = "..rom.." "..typ.." "..tex)
		g_msgmgr:GMRequest(3,rom,typ,tex)
		return 1
	end
	return 0
end

--调试使用家具
function debug_usefit(CommandName,Command)
	if useDebugCommand == false then
		return 0
	end

	local temp = string.find(Command," ")
	if temp == 14 then
		local temp2 = string.find(Command, " ", temp+1)
		local x = tonumber(string.sub(Command, temp+1, temp2-1))
		
		temp = temp2
		temp2 = string.find(Command, " ", temp+1)
		local y = tonumber(string.sub(Command, temp+1, temp2-1))
		
		temp = temp2
		temp2 = string.find(Command, " ", temp+1)
		local z = tonumber(string.sub(Command, temp+1, temp2))
		
		EQGlobaFunc.debugUseFit(x, y, z)
--		g_Loger:logMessage("debug_usefit = "..x.." "..y.." "..z)
		
		return 1
	end
	
	return 0
end