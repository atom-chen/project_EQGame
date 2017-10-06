-----------------------------------------
-- 文件说明	：NPC模式相关逻辑处理
-- 作者		：麦
-- 修改		:
-- 日期		：10/05/25
-----------------------------------------

local npcDlg = nil	-- 动态控件面板容器的父窗口
local txt = nil
local btn0 = nil
local btn1 = nil
local btn2 = nil
local btn3 = nil
local initFlag = false
local btnName = "npcDlg/btn"

-----------------------------------------
-- 初始化npc对话框各控件(hezhr)
function initNPCDlgCtrl()
	npcDlg = g_WinMgr:createWindow("NewEQHomeLook/NpcTalkImage", g_NpcDlgUINames.root)
	npcDlg:setWidth(CEGUI.UDim(0, 345))	-- 控件面板宽度
	npcDlg:setAlwaysOnTop(true)
	g_npcVec:initialise(npcDlg:getName())
	txt = createNPCDlgCtrl("EQHomeLook/MultiLineBox2", "npcDlg/txt")
	btn0 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn0")
	btn1 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn1")
	btn2 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn2")
	btn3 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn3")
	btn4 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn4")
	btn5 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn5")
	btn6 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn6")
	btn7 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn7")
	btn8 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn8")
	btn9 = createNPCDlgCtrl("EQHomeLook/ImageButton", "npcDlg/btn9")
	initFlag = true
end
-----------------------------------------
-- 创建控件(hezhr)
function createNPCDlgCtrl(ctrltype, ctrlname)
	local ctrl = g_WinMgr:createWindow(ctrltype, ctrlname)
	ctrl:setSize(CEGUI.UVector2(CEGUI.UDim(0, 315), CEGUI.UDim(0, 15)))	-- 设置控件24个汉字宽度
	if "npcDlg/txt" ~= ctrlname then
		ctrl:setProperty("HorzLabelFormatting", "LeftAligned")
		ctrl:setProperty("VertLabelFormatting", "CentreAligned")
		ctrl:setProperty("NormalTextColour", "FFE76D24")
		ctrl:setProperty("PushedTextColour", "FFE76D24")
	end
	return ctrl
end
-----------------------------------------
-- 加载npc对话框
function NPCDialog_Onload()
	g_GameEventMgr:subscribeEvent("UPDATE_NPC_UI_BY_OPT","updateUIByOpt")
	g_GameEventMgr:subscribeEvent("UPDATE_NPC_UI_BY_MSG","updateUIByMsg")
	g_GameEventMgr:subscribeEvent("NPC_CLOSE_DIALOG","closeNPCDlg")
	dofile(g_ifpath.."NPCMessage/NPCMessage.lua")
	if false == initFlag then
		initNPCDlgCtrl()
	end
end
-----------------------------------------
-- 关闭npc对话框(hezhr)
function closeNPCDlg()
	if nil ~= g_WinMgr:getWindow(g_NpcDlgUINames.root) then
		g_MainUIWindows:removeChildWindow(g_NpcDlgUINames.root)
		if nil ~= g_npcVec then
			g_npcVec:removeAllControl()
		end
	end
end

function updateUIByOpt()
	closeNPCDlg()	-- 在创建npc前先移除npc上的控件(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	invisibleBtns()
	local npc = g_objmgr:getPickNPC()
	if nil ~= npc then
		local optsize = npc.mOptCmdSize
		local optcmd
		local tid = npc:getOptTpltID()
		local pid 
		local cid 
		
		g_MainUIWindows:addChildWindow(npcDlg)
		EQGlobaFunc.toMultiLineBox(txt:getName()):clearUp()
		EQGlobaFunc.toMultiLineBox(txt:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(txt:getName()):setCols("#EBEBEB")
		NPC_TextParse(txt, getText(1, tid))
		local height = EQGlobaFunc.toMultiLineBox(txt:getName()):getFormatHeight()
		EQGlobaFunc.toMultiLineBox(txt:getName()):setHeight(height)
		g_npcVec:setHorInterval(15)
		g_npcVec:setVerInterval(10)
		g_npcVec:addControl(txt:getName())
		npcDlg:activate()
		for idx=0,optsize-1 do
			optcmd 	= npc:getOptCmd(idx)
			if nil == optcmd then
				g_Loger:logMessage("--- optcmd is nil ---")
			end
			tid 	= optcmd.template_id
			pidx	= optcmd.page_index
			cidx	= optcmd.command_index
			local btntxt = "tp="..tid.." pg="..pidx.." cmd="..cidx
			local btnwin = g_WinMgr:getWindow(btnName..idx)
			local text = getText(1, optcmd.template_id)
			local s = string.find(text, "#")
			if nil == s then
				btnwin:setProperty("NormalTextColour", "FFE76D24")
			else
				local col = string.sub(text, s+1, s+8)
				text = string.sub(text, s+9, -1)
				btnwin:setProperty("NormalTextColour", col)
			end
			EQGlobaFunc.SetText(btnName..idx, (idx + 1).."."..text)
			btnwin:subscribeEvent("Clicked","OnBtnClickedOpt")
			btnwin:setVisible(true)
			if nil ~= getText(1, optcmd.template_id) then
				g_npcVec:setHorInterval(15)
				g_npcVec:addControl(btnName..idx)
			end
		end
		g_npcVec:controlTypeset()
		setNPCPosition(npcDlg)
	else
		g_Loger:logError("err on updateUIByOpt")
	end
end
-----------------------------------------
-- 设置npc对话框位置(hezhr)
function setNPCPosition(npc)
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local pareWidth = g_MainUIWindows:getWidth():asAbsolute(screenW)
	local npcWidth = npc:getWidth():asAbsolute(screenW)
	local npcHeight = npc:getHeight():asAbsolute(screenH)
	local xPos = (pareWidth - npcWidth)/2
	local yPos = npcHeight + 45
	npc:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(1, -yPos)))
end

function OnBtnClickedOpt(args)
	local btnwin = CEGUI.toWindowEventArgs(args).window
	--g_Loger:logDebug("btnwin name = "..btnwin:getName())
	local btnidx = getBtnIdx(btnwin)
	local npc = g_objmgr:getPickNPC()
	local optcmd = npc:getOptCmd(btnidx)
	npc:setClickCmdIdx(btnidx)
	npc:reqNPCommandByOpt()
end

function OnBtnClickedMsg(args)
	local btnwin = CEGUI.toWindowEventArgs(args).window
	--g_Loger:logDebug("btnwin name = "..btnwin:getName())
	local btnidx = getBtnIdx(btnwin)
	local npc = g_objmgr:getPickNPC()
	local msg = npc:getMsg()
	npc:setClickCmdIdx(btnidx)
	npc:reqNPCommandBymsg()
end

function invisibleBtns()
	for idx=0,9 do
		local btnwin = g_WinMgr:getWindow(btnName..idx)
		btnwin:removeEvent("Clicked")
		btnwin:setVisible(false)
	end
end

function updateUIByMsg()
	invisibleBtns()
	local npc = g_objmgr:getPickNPC()
	if nil ~= npc then
		local msg
		local tid
		local pid 
		local cid 
		msg = npc:getMsg()
		tid = msg.template_id
		pidx= msg.page_index
		cidx= msg.command_index
		--g_Loger:logDebug("tid ="..tid.." pidx= "..pidx.." cidx= "..cidx)
					
		local btntxt = "tp="..tid.." pg="..pidx.." cmd="..cidx
		local btnwin = g_WinMgr:getWindow(btnName..0)
		local msgrow = g_NPCTable:getNPCMsgRow(tid)

		EQGlobaFunc.SetText(btnName..0, getText(1, msg.template_id))
		btnwin:subscribeEvent("Clicked", "OnBtnClickedMsg")
		btnwin:setVisible(true)
	else
		--g_Loger:logError("err on updateUIByMsg")
	end
end		
-----通过containerwin返回仓库的索引---------------------------------
function getBtnIdx(win)
	local name = win:getName()
	for idx=0,9 do
		if nil ~= string.find(name, btnName..idx) then
			return idx
		end
	end
end
-------------------------------------------------------------------	
-- 获取text(hezhr)
function getText(npcid, tpltid)
	local stradd = ""
	if npcid < 100 then
		if npcid >= 10 then
			stradd = "0"
		else
			stradd = "00"
		end
	end
	local npcidx = stradd..npcid
	
	stradd = ""
	if tpltid < 100 then
		if tpltid >= 10 then
			stradd = "0"
		else
			stradd = "00"
		end
	end
	
	local tpltidx = stradd..tpltid
	return g_npctalk[npcidx][tpltidx]
end
-------------------------------------------------------------------	
-- 显示文本解析(hezhr)
function NPC_TextParse(win, text)
	local key = "~n"
	local s, e = string.find(text, key)
	if nil == s then
		EQGlobaFunc.toMultiLineBox(win:getName()):addElementText(text, false)	-- 添加文本
		return
	end
	local pre = string.sub(text, 1, s - 1)
	local suf = string.sub(text, e + 1, -1)
	local name = g_mainplayer:getName()
	local temp = pre..name
	EQGlobaFunc.toMultiLineBox(win:getName()):addElementText(temp, false)	-- 添加文本
	NPC_TextParse(win, suf)
end
