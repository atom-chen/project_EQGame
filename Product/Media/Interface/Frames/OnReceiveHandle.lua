-----------------------------------------
-- 文件说明	：网络消息接收处理
-- 作者		：麦
-- 修改		：
-- 日期		：10/03/25
-----------------------------------------

local sysMsgParamsSize = 0 		--消息参数个数
local sysMsgParamsTable = {} 	--消息参数表
local tabIdx = 1 				--消息参数表初始索引
local mMsgIdx = nil 			--消息框ID
local initFlag = 0				--初始化消息特殊弹出界面标志
local createMsgBoxTime = 0
local style5Win = nil
--指定消息处理函数对应表
--通过 MsgBox_registerFunc函数添加
g_ProcFunc = {
			 }

-----接收消息函数--------------------------------------------------
--@param msgtype: 	消息类型
--@param netpacket	消息包
--------
function OnReceiveHandle(msgtype,netpacket)
	if msgtype == EQGlobaClass.NetMsgType.msg_notify_player_data then
		--got_msg_notify_player_data(netpacket)
	elseif msgtype == EQGlobaClass.NetMsgType.msg_notify_start_edit_house then
		--got_msg_notify_start_edit_house(netpacket)
	elseif msgtype == EQGlobaClass.NetMsgType.msg_notify_end_edit_house then
		--got_msg_notify_end_edit_house(netpacket)
		
	elseif msgtype == EQGlobaClass.NetMsgType.msg_notify_sys_msg then
		got_msg_notify_sys_msg(netpacket)
		
	elseif msgtype == EQGlobaClass.NetMsgType.msg_notify_add_friend then
		--got_msg_notify_add_friend(netpacket)
	end
	
	
end

--处理接收系统消息函数
function got_msg_notify_sys_msg(netpacket)
    show_sys_msg(netpacket.code)
end

function show_sys_msg(code)
	tabIdx = 1
	local id = code --得到消息表ID
	mMsgIdx = code
	--g_Loger:logDebug("***msgID****:" ..id)
	local sysMsgRow = g_itemtableMgr:getSysMsgRow(id)
	if sysMsgRow == nil then
		return
	end
	local content = sysMsgRow:randContent()
	local msgShowType = sysMsgRow.msgType
	--捡漂流瓶的特别处理，在漂流瓶系统中
	if id == 210 then
		DBS_recPickFailMsg(content)
		return
	end
	--
	local msgShowTypeTable = Split(msgShowType, ",")
	local TypeArray = {}	--消息显示方式表，用于存放多种显示方式
	for i,v in ipairs(msgShowTypeTable) do
		TypeArray[i] = tonumber(v)
	end
		
	if sysMsgParamsSize == 0 then
	
	elseif sysMsgParamsSize == 1 then
		content = string.format(content, sysMsgParamsTable[1])
	elseif sysMsgParamsSize == 2 then
		content = string.format(content, sysMsgParamsTable[1],sysMsgParamsTable[2])
	elseif sysMsgParamsSize == 3 then
		content = string.format(content, sysMsgParamsTable[1],sysMsgParamsTable[2],sysMsgParamsTable[3])
	elseif sysMsgParamsSize == 4 then
		content = string.format(content, sysMsgParamsTable[1],sysMsgParamsTable[2],sysMsgParamsTable[3],sysMsgParamsTable[4])
	elseif sysMsgParamsSize == 5 then
		content = string.format(content, sysMsgParamsTable[1],sysMsgParamsTable[2],sysMsgParamsTable[3],sysMsgParamsTable[4],sysMsgParamsTable[5])
	end
	
	if initFlag == 0 then
		MsgBox_registerFunc(60,"FarmTip_confirmUpRoot","MsgBox_cancelFunc") --铲除作物的消息
		MsgBox_registerFunc(120,"FarmTip_reqPlot","MsgBox_cancelFunc")
		initFlag = 1
	end
	--
	for i,v in ipairs(TypeArray) do
		if v == 1 then
			MsgBox_registerFunc(id,"MsgBox_cancelFunc","")
		end
		SysMsg_execute(v,content)
	end
end
--得到消息参数个数
function getParamsSize(size)
	sysMsgParamsSize = size
end
--加载消息参数
function getSysMsgParams(params)
	if sysMsgParamsSize ~= nil then
		sysMsgParamsTable[tabIdx] = params
		
		tabIdx = tabIdx + 1
	end
end
--执行消息显示
function SysMsg_execute(type,content)
	local func = SysMsg_matchShowFunc(type)
	func(content)
end
--不显示消息
function MsgSys_dontShow(content)
	--g_Loger:logDebug("don't show message!")
end
--通用消息显示函数
function MsgSys_showStyleComm(content)
	EQGlobaFunc.SetSysMsgText(content)
end

--
function MsgSys_showStyle1(content)
	EQGlobaFunc.SetSysMsgText(content)
end
--
function MsgSys_showStyle2(content)
	EQGlobaFunc.SetSysMsgText(content)
end
--
function MsgSys_showStyle3(content)

end
--
function MsgSys_showStyle4(content)

end
--
local style5WinTable = {}
--
function MsgSys_showStyle5(content)
	--style5WinTable = {}
	if mMsgIdx == 149 then
		if m_ClickLandID ~= nil then
			g_FarmMgr:setHintDymanicUI(m_ClickLandID,0,tonumber(sysMsgParamsTable[1]))
		end
	end
	-- (2011/6/22 modify by hezhr)
	if g_GameStateMgr:inMainGame() then	-- 在乐园
		style5Win = MsgBox_createMsgBox(g_MainUIWindows,mMsgIdx, "消息提示",content,"MsgBox_cancelFunc", "")
	elseif g_GameStateMgr:isFunnyFarm() then	-- 在农场
		style5Win = MsgBox_createMsgBox(g_FarmUI,mMsgIdx, "消息提示",content,"MsgBox_cancelFunc", "")
	end
	EQGlobaFunc.setCreateMsgBoxTime()

	table.insert(style5WinTable, style5Win)
end
--
function MsgSys_removeStyle5Box()
	local tabN = table.getn(style5WinTable)
	if tabN == 0 then
		return
	end
	local presentWin = nil
	for i, v in ipairs(style5WinTable) do
		if v ~= nil then
			if g_WinMgr:isWindowPresent(v) then
				presentWin = g_WinMgr:getWindow(v)
				Common_setModalState(presentWin:getName(), false)
				g_WinMgr:destroyWindow(presentWin)
				v = nil
			end
		end
	end
	
	for i,v in ipairs(g_MsgboxWinTable) do
		if presentWin ~= nil then
			if v == presentWin:getName() then
				table.remove(g_MsgboxWinTable,i)
			end
		end
	end
	local tab_n = table.getn(g_MsgboxWinTable)
	if tab_n ~= 0 then
		Common_setModalState(g_MsgboxWinTable[tab_n], true)
	end
	--if style5Win ~= nil then
	--	if g_WinMgr:isWindowPresent(style5Win) then
	--		g_Loger:logDebug("****" ..style5Win)
	--		local presentWin = g_WinMgr:getWindow(style5Win)
	--		g_Loger:logDebug("****" ..presentWin:getName())
	--		Common_setModalState(presentWin:getName(), false)
	--		g_WinMgr:destroyWindow(presentWin)
	--		style5Win = nil
	--	end
	--end
	g_GameEventMgr:fireEvent("NOTIFY_CLOSE_SYSMSG_BOX")	-- 关闭消息框(2011/4/29 add by hezhr)
end
--
function MsgSys_showStyle6(content)
	-- 1. 先获得code
	-- 2. 根据code查找 g_ProcFunc
	-- 3. 回调：function callback(code, type)
	local OkFunc, Cancel
	for i,v in ipairs(g_ProcFunc) do 
		if v.id == mMsgIdx then
			OkFunc = v.okFunc
			Cancel = v.cancelFunc
			break
		end
	end
	if mMsgIdx == 60 then
		MsgBox_createMsgBox(g_FarmUI,mMsgIdx, "铲除作物",content, OkFunc, Cancel)
	elseif mMsgIdx == 120 then
		MsgBox_createMsgBox(g_FarmUI,mMsgIdx, "开垦土地",content, OkFunc, Cancel)
	else
		-- (2011/6/22 modify by hezhr)
		if g_GameStateMgr:inMainGame() then	-- 在乐园
			MsgBox_createMsgBox(g_MainUIWindows,mMsgIdx, "消息提示",content, OkFunc, Cancel)
		elseif g_GameStateMgr:isFunnyFarm() then	-- 在农场
			MsgBox_createMsgBox(g_FarmUI,mMsgIdx, "消息提示",content, OkFunc, Cancel)
		elseif g_GuiSystem:getGUISheet() == g_CreatePlayer then -- 可能在创建人物界面
			MsgBox_createMsgBox(g_CreatePlayer,mMsgIdx,"",content, OkFunc, Cancel)
		end
	end
end
--注册指定消息的处理确定、取消函数
--@Code			为消息的ID
--@Okfunc 		为执行确定的回调函数
--@CancelFunc 	为执行取消的回调函数
function MsgBox_registerFunc(Code, OkFunc, CancelFunc)
	if table.getn(g_ProcFunc) ~= 0 then
		for i,v in ipairs(g_ProcFunc) do
			if v.id == Code then
				break
			else
				table.insert(g_ProcFunc,{id = Code, okFunc = OkFunc, cancelFunc = CancelFunc})
			end
		end
	else
		table.insert(g_ProcFunc,{id = Code, okFunc = OkFunc, cancelFunc = CancelFunc})
	end
end

-----------------------------------------------
function MsgSys_showStyle7(content)
	OpenUpgradeLandUI()
	
	if 126 == mMsgIdx or 127 == mMsgIdx or 128 == mMsgIdx or 129 == mMsgIdx or 130 == mMsgIdx or 136 == mMsgIdx then
		--清空
		EQGlobaFunc.toMultiLineBox("redLand_condition"):clearUp()
		if 129 == mMsgIdx then
			--g_Loger:logMessage("content"..content)
		end
		g_mlbParse:getData(content , "redLand_condition")
	end
	
	if 130 == mMsgIdx then
		g_WinMgr:getWindow(g_upgradeLandUIName.sureBtn):setEnabled(true)
	else
		g_WinMgr:getWindow(g_upgradeLandUIName.sureBtn):setEnabled(false)
	end
	
end
-----------------------------------------------
--消息显示方式对应表
local msgType2FuncTab = {
						 [0] = MsgSys_dontShow,		--不显示
						 [1] = MsgSys_showStyle6,	--只有一个确定按钮
						 [2] = MsgSys_showStyle2,	--在聊天框中显示
						 [3] = MsgSys_showStyle3,	--在聊天框上方显示
						 [4] = FarmTip_popMsgPaoPao,	--在农场土地中显示（泡泡方式）
						 [5] = MsgSys_showStyle5,	--只有一个确定按钮，不操作5秒后消息框自动消失
						 [6] = MsgSys_showStyle6,	--有确定、取消按钮
						 [7] = MsgSys_showStyle7,	--消息显示在指定窗口
						}
--塞选消息显示方式
function SysMsg_matchShowFunc(type)
	if msgType2FuncTab[type] ~= nil then
		return msgType2FuncTab[type]
	else
		return MsgSys_showStyleComm
	end
end

