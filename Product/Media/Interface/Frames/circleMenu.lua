--环形菜单相关
local circleBtnNum = 8 --11
local funSet = {}
local menuLoatX = nil
local menuLoatY = nil
--环形菜单按钮Normal态图片
local cMbtnNormalImg = {
							"ImageButton71_Normal",
							"ImageButton70_Normal",
							"ImageButton69_Normal",
							"ImageButton68_Normal",
							"ImageButton74_Normal",
							"ImageButton73_Normal",
							"ImageButton72_Normal",
							"ImageButton107_Normal",
							"ImageButton67_Normal",
							"ImageButton76_Normal",
							"ImageButton75_Normal",
						}
--环形彩单hover、pushed态图片
local cMbtnHoverImg = {
							"ImageButton71_Pushed",
							"ImageButton70_Pushed",
							"ImageButton69_Pushed",
							"ImageButton68_Pushed",
							"ImageButton74_Pushed",
							"ImageButton73_Pushed",
							"ImageButton72_Pushed",
							"ImageButton107_Pushed",
							"ImageButton67_Pushed",
							"ImageButton76_Pushed",
							"ImageButton75_Pushed",
						}
--tooltip文字表
local btnTooltiplist = {
							"查看玩家信息",
							"密语",
							"发送邮件",
							"交易",
							"进入好友房间",
							"邀请进入房间",
							"加为好友",
							"踢人",
							"人物动作",
							"举报",
							"屏蔽",
						}


--Onload
function circleMenu_Onload()
	g_WinMgr:getWindow("DFwnd_MainRoot"):subscribeEvent("Clicked", 	"circleMenu_hideMenu")
	g_GameEventMgr:subscribeEvent("SHOW_FITMENTINTERACT_UI","circleMenu_AddButtonImage")
	g_GameEventMgr:subscribeEvent("ON_CLICKED_GAMEOBJ","OnClickedGameobj")
end


function OnClickedGameobj()
	closeNPCDlg()	-- 在打开环形菜单前关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	local pickobj = EQGlobaClass.getObjMgr():getPickObj()
	if nil~= pickobj then
		local type = pickobj:getType()
		--g_Loger:logMessage("pickobj type = "..type)

		if(2 == type) then
			local posx, posy = menuLoatX,menuLoatY
			g_circleMenuUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0,posx), CEGUI.UDim(0,posy)))

			g_MainUIWindows:addChildWindow(g_circleMenuUI)
			local btn = {}
			for i = 1,circleBtnNum do
				btn[i] = g_WinMgr:getWindow("circleMenu/funBtn" ..i)
				btn[i]:setVisible(true)
				btn[i]:setProperty("NormalImage", "set:Button1 image:" ..cMbtnNormalImg[i])
				btn[i]:setProperty("HoverImage", "set:Button1 image:" ..cMbtnHoverImg[i])
				btn[i]:setProperty("PushedImage", "set:Button1 image:" ..cMbtnHoverImg[i])
				btn[i]:getTooltip():setFont("simsun-10")
				--设置tooltip
				EQGlobaFunc.setTooltipProperty("circleMenu/funBtn" ..i, btnTooltiplist[i])
				btn[i]:removeEvent("Clicked")
			end
			--
			btn[1]:subscribeEvent("Clicked", "ShowOtherPlayerInfo")
			btn[2]:subscribeEvent("Clicked", "talkToYou")
			btn[3]:subscribeEvent("Clicked", "SendMailToPicked")
			btn[4]:subscribeEvent("Clicked", "InviteDeal")
			btn[5]:subscribeEvent("Clicked", "EnterPlayerHouse")
			btn[6]:subscribeEvent("Clicked", "InviteEnterHouse")
			btn[7]:subscribeEvent("Clicked", "AddasFriend")
			btn[8]:subscribeEvent("Clicked", "circle_KickGuest")
			if g_SceneMgr:inMyHouse() then
				btn[8]:setVisible(true)
			else
				btn[8]:setVisible(false)
			end

		end
	else
		--g_Loger:logMessage("pickobj == nil")
	end

end

-- xiaoMing Add
function InviteDeal()
	g_smallModules:inviteDealByPicked()
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end
--
function talkToYou()
	--g_Loger:logMessage("talktoyou")
	g_ChatSys:talkPickObj()
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end
--发送邮件给选中的玩家
function SendMailToPicked()
	local pickobj = EQGlobaClass.getObjMgr():getPickPlayer()
	local acc = pickobj:getPlayerAccount()
	
	OpenOutBox()
	FSetText(g_mail.addressee, acc)
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end

--
function EnterPlayerHouse()
	--g_FriendSys:reqEnterFriendHouseByLua()
	g_FriendSys:reqHouseListFromCircleMenu()
	g_ToHouseFromType = g_ToHouseFromDef.circleMenu
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end

--
function InviteEnterHouse()
	g_FriendSys:reqInviteEnterHouseByLua()
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end

--
function AddasFriend()
	--g_Loger:logMessage("AddFriend")
	g_FriendSys:reqAddasFriend()
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end
--
function circle_KickGuest()
	g_FriendSys:reqKickPlayerByLua()
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end

--
function setDisplayPos(winname,x,y)
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()

	menuLoatX = x - 74
	menuLoatY = y - 74
	if menuLoatX < 0 then
		menuLoatX = 0
	elseif menuLoatX + 148 > screenW then
		menuLoatX = screenW - 148
	end

	if menuLoatY < 0 then
		menuLoatY = 0
	elseif menuLoatY + 148 > screenH then
		menuLoatY = screenH - 148
	end
	g_circleMenuUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0,menuLoatX), CEGUI.UDim(0,menuLoatY)))
end

function getfurniMenuPos(menuW, menuH)
	local mousPos = {x, y}
	mousPos = g_MouseCur:getPosition()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local xPos, yPos = 0, 0
	-- 计算x坐标
	if (mousPos.x) < 0 then
		xPos = 0
	elseif (mousPos.x + menuW) >screenW then
		xPos = screenW - menuW
	else
		xPos = mousPos.x - (menuW/2)
		if xPos < 0 then
			xPos = 0
		end
	end
	-- 计算y坐标
	if (mousPos.y - menuH) < 0 then
		yPos = 0
	elseif (mousPos.y + menuH) > screenH then
		yPos = screenH - menuH
	else
		yPos = mousPos.y-menuH
		if yPos < 0 then
			yPos = 0
		end
	end
	return xPos, yPos
end
--
--
local furniMenuWin = nil	-- 家具菜单界面
local menuItem = {}	-- 家具菜单界面上的按钮
-----------------------------------------------------------------
function circleMenu_AddButtonImage()
	local loopMenuRow = g_logictableMgr:getLoopMenuRow( EQGlobaFunc.getLoopMenuID() )
	-- 最多八个按钮
	funSet[1] = loopMenuRow.fun1
	funSet[2] = loopMenuRow.fun2
	funSet[3] = loopMenuRow.fun3
	funSet[4] = loopMenuRow.fun4
	funSet[5] = loopMenuRow.fun5
	funSet[6] = loopMenuRow.fun6
	funSet[7] = loopMenuRow.fun7
	funSet[8] = loopMenuRow.hint
	-- 创建家具菜单界面
	if furniMenuWin == nil then
		furniMenuWin = g_WinMgr:createWindow("EQHomeLook/StaticImage13", "furniMenu")
		furniMenuWin:setProperty("BackgroundEnabled", "false")
	end	
	-- 清空选项按钮表
	for i = 1, 8 do
		if nil ~= menuItem[i] then
			g_WinMgr:destroyWindow(menuItem[i])
			menuItem[i] = nil
		end
	end
	-- 创建选项按钮
	for i = 1, 8 do
		if 0 ~= funSet[i] then
			local furniFuncRow = g_logictableMgr:getFurniInteractRow(funSet[i])
			local loopMenuImageset = furniFuncRow.imageset
			local loopMenuIcon = furniFuncRow.icon
			local tooltip = furniFuncRow.tooltip
			menuItem[i] = g_WinMgr:createWindow("EQHomeLook/ImageButton", "btn" ..i)
			menuItem[i]:setProperty("NormalImage", "set:"..loopMenuImageset.." image:"..loopMenuIcon.."_Normal")
			menuItem[i]:setProperty("HoverImage", "set:"..loopMenuImageset.." image:"..loopMenuIcon.."_Hover")
			menuItem[i]:setProperty("PushedImage", "set:"..loopMenuImageset.." image:"..loopMenuIcon.."_Hover")
			if tooltip ~= "" then
				EQGlobaFunc.setTooltipProperty(menuItem[i]:getName(), tooltip)
			end
		end
	end
	-- 动态设置按钮位置(modify by hezhr)
	local btn_xPos, btn_yPos= 0, 9	-- 初始位置
	local count = 0	-- 按钮数量
	for i = 1, 8 do
		if nil ~= menuItem[i] then
			count = count + 1
			btn_xPos = 9*count + 29*(count-1)
			--g_Loger:logDebug("i.."..i.." xpos.."..btn_xPos.." ypos.."..btn_yPos)
			menuItem[i]:setSize(CEGUI.UVector2(CEGUI.UDim(0, 29), CEGUI.UDim(0, 29)))
			menuItem[i]:setPosition(CEGUI.UVector2(CEGUI.UDim(0, btn_xPos), CEGUI.UDim(0, btn_yPos)))
			menuItem[i]:removeEvent("Clicked")
			menuItem[i]:subscribeEvent("Clicked", "circleMenu_itemFunc"..i)
			furniMenuWin:addChildWindow(menuItem[i])
		end
	end
	-- 动态设置菜单界面大小
	local menuW = 9*(count+1) + 29*count
	local menuH = 29 + 18
	--g_Loger:logDebug("menuW..."..menuW)
	--g_Loger:logDebug("menuH..."..menuH)
	furniMenuWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, menuW), CEGUI.UDim(0, menuH)))
	-- 动态设置菜单界面位置
	local xPos, yPos = getfurniMenuPos(menuW, menuH)
	furniMenuWin:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0 ,yPos)))
	g_MainUIWindows:addChildWindow(furniMenuWin)
end
--
function circleMenu_itemFunc1(args)
	EQGlobaFunc.selLoopMenu(funSet[1])
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end
--
function circleMenu_itemFunc2(args)
	EQGlobaFunc.selLoopMenu(funSet[2])
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end
--
function circleMenu_itemFunc3(args)
	EQGlobaFunc.selLoopMenu(funSet[3])
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end
--
function circleMenu_itemFunc4(args)
	EQGlobaFunc.selLoopMenu(funSet[4])
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end
--
function circleMenu_itemFunc5(args)
	EQGlobaFunc.selLoopMenu(funSet[5])
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end
--
function circleMenu_itemFunc6(args)
	EQGlobaFunc.selLoopMenu(funSet[6])
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end
--
function circleMenu_itemFunc7(args)
	EQGlobaFunc.selLoopMenu(funSet[7])
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end
-- 点击家具介绍按钮(add by hezhr)
function circleMenu_itemFunc8(args)
	local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
	local houseCompID = EQGlobaFunc.getInteractingFitmentID()
	local row = g_itemtableMgr:getItemRowBySubId(houseCompID, 2)
	if nil == row then
		return
	end
	FitModeShopCenter_openIntroUI(row.id, rect, EQGlobaFunc.getLoopMenuDelTime())
	g_MainUIWindows:removeChildWindow(furniMenuWin)
end

function circleMenu_hideBtn()
	--for i = 1, 10 do
	--	g_WinMgr:getWindow("circleMenu/funBtn" ..i):setVisible(false)
	--end
end

--
function circleMenu_hideMenu(args)
	--local wnd = CEGUI.toWindowEventArgs(args).window
	--local wndName = wnd:getName()
	--g_Loger:logDebug(wndName)
	--local wnddif = g_WinMgr:getWindow("circleMenu")
	--if wnd ~= wnddif then
	--g_MainUIWindows:removeChildWindow(g_circleMenuUI)
	--end

end
