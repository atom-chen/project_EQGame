--所有界面上的帮助按钮功能

--
local mTable = {}
------------------------------
--

-------------------------------
--
function UIHelpBtnShowFunc_loadTableSource()
	local table_size = g_logictableMgr:getHelpBtnShowPicTableCount()
	--g_Loger:logMessage("------UIHelpBtnShowFunc_loadTableSource-------"..tostring(table_size))
	for i=1, table_size do
		local row = g_logictableMgr:getHelpBtnShowPicRow(i)
		if row == nil then
			return
		end
		--g_Loger:logMessage("------UIHelpBtnShowFunc_loadTableSource-------"..tostring(row.id).." "..tostring(row.btn_name).." "..tostring(row.picID))
		local temp_picIDTable = UIHelpBtnShowFunc_analysePicIDStr(row.picID)
		--for i,v in ipairs(temp_picIDTable) do
		--	g_Loger:logError("--v" .."*" ..tostring(v))
		--end
		table.insert(mTable,{id=row.id,btn_name=row.btn_name,picIDTable=temp_picIDTable})
	end
	--
	UIHelpBtnShowFunc_subscribeAllBtnFunc()
end
---------------------------------
--
function UIHelpBtnShowFunc_subscribeAllBtnFunc()
	for i,v in ipairs(mTable) do
		--g_Loger:logError("--v" .."*" ..tostring(v.id).."*"..tostring(v.btn_name).."*"..tostring(v.picIDTable[1]))
		g_WinMgr:getWindow(v.btn_name.."__auto_button__"):setID(v.id)
		g_WinMgr:getWindow(v.btn_name.."__auto_button__"):subscribeEvent("Clicked", "UIHelpBtnShowFunc_onClickHelpBtn")
	end
end
---------------------------------
--
function UIHelpBtnShowFunc_onClickHelpBtn(args)
	local click_win = CEGUI.toWindowEventArgs(args).window
	local id = click_win:getID()
	--g_Loger:logError("--win id--" .."*" ..tostring(id))
	local picID_table = mTable[id].picIDTable
	for i,v in ipairs(picID_table) do
		--g_Loger:logError("--v" .."*" ..tostring(v))
		show_picture(tonumber(v))
	end
end
---------------------------------
--
function UIHelpBtnShowFunc_analysePicIDStr(picIDStr)
	local t = Split(picIDStr,",")
	if t == nil then
		return {}
	end
	return t
end