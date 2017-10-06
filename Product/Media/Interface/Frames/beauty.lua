-----------------------------------------
-- 文件说明	：美容院
-- 作者		：何展然
-- 修改		：
-- 日期		：10/12/6
-----------------------------------------

--事件注册
g_WinMgr:getWindow("Root/beauty__auto_closebutton__"):subscribeEvent("Clicked", "Beauty_OnClickCancelBtn")
g_WinMgr:getWindow("Root/beauty/background/cancel"):subscribeEvent("Clicked", "Beauty_OnClickCancelBtn")
g_WinMgr:getWindow("Root/beauty/background/done"):subscribeEvent("Clicked", "Beauty_OnClickOkBtn")
g_WinMgr:getWindow("Root/beauty/background/hair"):subscribeEvent("Clicked", "Beauty_OnClickHairBtn")
g_WinMgr:getWindow("Root/beauty/background/face"):subscribeEvent("Clicked", "Beauty_OnClickFaceBtn")
g_WinMgr:getWindow("Root/beauty/background/beard"):subscribeEvent("Clicked", "Beauty_OnClickBeardBtn")
g_WinMgr:getWindow("Root/beauty/background/haircolor_pgup"):subscribeEvent("Clicked", "Beauty_HairColorPageUp")
g_WinMgr:getWindow("Root/beauty/background/haircolor_pgdown"):subscribeEvent("Clicked", "Beauty_HairColorPageDown")
g_WinMgr:getWindow("Root/beauty/background/skin_pgup"):subscribeEvent("Clicked", "Beauty_SkinPageUp")
g_WinMgr:getWindow("Root/beauty/background/skin_pgdown"):subscribeEvent("Clicked", "Beauty_SkinPageDown")
g_WinMgr:getWindow("beanty_parts_list/backgroundimg/pageup"):subscribeEvent("Clicked", "BeautyChooseUI_PageUp")
g_WinMgr:getWindow("beanty_parts_list/backgroundimg/pagedown"):subscribeEvent("Clicked", "BeautyChooseUI_PageDown")
g_WinMgr:getWindow("beanty_parts_list/backgroundimg/queding"):subscribeEvent("Clicked", "BeautyChooseUI_OnClickOkBtn")
g_WinMgr:getWindow("beanty_parts_list/backgroundimg/quxiao"):subscribeEvent("Clicked", "BeautyChooseUI_OnClickCloseBtn")
g_WinMgr:getWindow("beanty_parts_list__auto_closebutton__"):subscribeEvent("Clicked", "BeautyChooseUI_OnClickCloseBtn")
--屏蔽界面鼠标穿透
MainUI_ShieldClick(g_BeautyUI)
MainUI_ShieldClick(g_BeautyChooseUI)
Common_injectEmptyEvt("Root/beauty")
--初始美容院
g_MainUIWindows:addChildWindow(g_BeautyUI)
g_BeautyUI:setAlwaysOnTop(true) 
g_BeautyUI:setVisible(false)
g_BeautyChooseUI:setAlwaysOnTop(true) 
g_smallModules:initBeautyRTT()
--坐标相关
local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
local oldBeautyUIPos_xScale = EQGlobaFunc.getPositionVal(g_BeautyUI:getName(), 0, 0)
local oldBeautyUIPos_xOffset = EQGlobaFunc.getPositionVal(g_BeautyUI:getName(), 0, 1)
local oldBeautyUIPos_yScale = EQGlobaFunc.getPositionVal(g_BeautyUI:getName(), 1, 0)
local oldBeautyUIPos_yOffset = EQGlobaFunc.getPositionVal(g_BeautyUI:getName(), 1, 1)
local newBeautyUIPos_X = oldBeautyUIPos_xOffset - 137
local BeautyChooseUIPos_X = newBeautyUIPos_X + g_BeautyUI:getWidth():asAbsolute(screenW)
g_BeautyChooseUI:setPosition(CEGUI.UVector2(CEGUI.UDim(oldBeautyUIPos_xScale, BeautyChooseUIPos_X), 
											CEGUI.UDim(oldBeautyUIPos_yScale, oldBeautyUIPos_yOffset)))
--变量
local m_totalCostWin = g_WinMgr:getWindow("Root/beauty/background/totalcost")
local m_eqPointWin = g_WinMgr:getWindow("Root/beauty/background/costbgimg/goldcost1")
local m_gameGoldWin = g_WinMgr:getWindow("Root/beauty/background/costbgimg/goldcost")
local m_costWin = g_WinMgr:getWindow("Root/beauty/background/costbgimg/count")
local m_slotBaseName = "beanty_parts_list/backgroundimg/part"
local m_hairTypes_EQ, m_hairTypes_GOLD = 0, 0	-- 发型类型数
local m_hairColors_EQ, m_hairColors_GOLD = 0, 0	-- 发色类型数
local m_faceTypes_EQ, m_faceTypes_GOLD = 0, 0	-- 脸型类型数
local m_skinColors_EQ, m_skinColors_GOLD = 0, 0	-- 肤色类型数
local m_beardTypes_EQ, m_beardTypes_GOLD = 0, 0	-- 胡子类型数
local m_curTypes = 0	-- 当前类型数
local m_curSex = 1	-- 当前玩家性别(1.男，2.女)
local m_totalPage, m_curPage = 1, 1	-- 总页数，当前页
local m_curMoneyType = 1	-- 当前收费类型(1.游戏币，2.EQ点)
local m_curSectionType = 1	-- 当前美容部位类型(1.头部，2.头发，3.胡子)对应RoleSection.xml表
local m_curAdjustType = 1	-- 当前美容部位类型(1.发型，2.发色，3.脸型，4.肤色，5.胡子)对应change_looks_tplt.xml表
local m_curHairColorValue, m_curSkinColorValue = 0, 0	-- 当前发色，肤色的值
local m_curFaceValue, m_curHairValue, m_curBeardValue = 0, 0, 0	-- 当前脸型，发型，胡子的值
local m_oldFaceValue, m_oldHairValue, m_oldBeardValue = 0, 0, 0	-- 旧的脸型，发型，胡子的值
local m_curHairColorCost, m_curSkinColorCost = 0, 0	-- 当前发色，肤色的费用
local m_curFaceCost, m_curHairCost, m_curBeardCost = 0, 0, 0	-- 当前脸型，发型，胡子的费用
local m_oldFaceCost, m_oldHairCost, m_oldBeardCost = 0, 0, 0	-- 旧的脸型，发型，胡子的费用
local m_totalCost = 0	-- 总费用
local m_hairBtnVisible, m_hairColorUpBtnVisible, m_hairColorDownBtnVisible = true, true, true
local m_faceBtnVisible, m_skinUpBtnVisible, m_skinDownBtnVisible, m_beardBtnVisible = true, true, true, true
local tipsAniPareName = nil	-- 格子提示动画父窗口
local tipsAni = EQGlobaClass.createAnimate()	-- 格子提示动画
local mStartBeautyFlag = false
local m_disabledWindow = {}	-- 打开美容界面,被禁用的窗体
--

-----------------------------------------
--
function Beauty_getStartFlag()
	return mStartBeautyFlag
end
-----------------------------------------
--获取美容相关数据
function Beauty_GetTypes(moneyType)
	if 1 == moneyType then
		g_smallModules:saveBeautySectionTypes(1, 3, 1)
		g_smallModules:saveBeautySectionTypes(2, 1, 1)
		g_smallModules:saveBeautySectionTypes(3, 5, 1)
		g_smallModules:saveBeautyColors(m_curSex, 1, 1)
		g_smallModules:saveBeautyColors(m_curSex, 2, 1)
		for i = 1, 5 do
			g_smallModules:insertCurVal(i, 1, g_smallModules:getSectionVal(1, i))
		end		
		m_hairTypes_EQ = g_smallModules:getBeautyTypes(1, 1)
		m_hairColors_EQ = g_smallModules:getBeautyTypes(2, 1)
		m_faceTypes_EQ = g_smallModules:getBeautyTypes(3, 1)
		m_skinColors_EQ = g_smallModules:getBeautyTypes(4, 1)
		m_beardTypes_EQ = g_smallModules:getBeautyTypes(5, 1)
		Beauty_DisableBtnSelect(1)
	elseif 2 == moneyType then
		g_smallModules:saveBeautySectionTypes(1, 3, 2)
		g_smallModules:saveBeautySectionTypes(2, 1, 2)
		g_smallModules:saveBeautySectionTypes(3, 5, 2)
		g_smallModules:saveBeautyColors(m_curSex, 1, 2)
		g_smallModules:saveBeautyColors(m_curSex, 2, 2)
		for i = 1, 5 do
			g_smallModules:insertCurVal(i, 2, g_smallModules:getSectionVal(1, i))
		end		
		m_hairTypes_GOLD = g_smallModules:getBeautyTypes(1, 2)
		m_hairColors_GOLD = g_smallModules:getBeautyTypes(2, 2)
		m_faceTypes_GOLD = g_smallModules:getBeautyTypes(3, 2)
		m_skinColors_GOLD = g_smallModules:getBeautyTypes(4, 2)
		m_beardTypes_GOLD = g_smallModules:getBeautyTypes(5, 2)
		Beauty_DisableBtnSelect(2)
	end
	m_curHairValue = g_smallModules:getSectionVal(2, 1)
	m_oldHairValue = g_smallModules:getSectionVal(2, 1)	
	m_curFaceValue = g_smallModules:getSectionVal(2, 3)
	m_oldFaceValue = g_smallModules:getSectionVal(2, 3)
	m_curBeardValue = g_smallModules:getSectionVal(2, 5)
	m_oldBeardValue = g_smallModules:getSectionVal(2, 5)
end
-----------------------------------------
--开始美容
function StartBeauty(moneyType)
	g_BeautyUI:setVisible(true)
	mStartBeautyFlag = true
	if true == g_mainplayer:isMale() then	-- 男性
		g_WinMgr:getWindow("Root/beauty/background/beard"):enable()
		m_curSex = 1
	else	-- 女性
		g_WinMgr:getWindow("Root/beauty/background/beard"):disable()
		m_curSex = 2
	end
	BeautyUI_ShowRTT()
	m_curMoneyType = moneyType
	Beauty_GetTypes(moneyType)
	if 1 == moneyType then	-- 游戏币
		m_gameGoldWin:setVisible(true)
		m_eqPointWin:setVisible(false)
		EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
		EQGlobaFunc.SetText(m_costWin:getName(), tostring(0))			
	elseif 2 == moneyType then	-- EQ点
		m_gameGoldWin:setVisible(false)
		m_eqPointWin:setVisible(true)
		EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
		EQGlobaFunc.SetText(m_costWin:getName(), tostring(0))		
	end
	Beauty_StartMutexOtherBtns()
	UIM_DoOpenAndCloseUI(g_BeautyUINames.root)
end
-----------------------------------------
--点击取消按钮
function Beauty_OnClickCancelBtn(args)
	g_smallModules:reqCancelBeauty()	-- 请求取消美容
end
-----------------------------------------
--结束美容
function CancelBeauty()
	g_BeautyUI:setVisible(false)
	mStartBeautyFlag = false
	g_smallModules:clearBeautySectionTypes()
	m_curHairColorCost, m_curSkinColorCost = 0, 0
	m_curFaceCost, m_curHairCost, m_curBeardCost = 0, 0, 0
	m_oldFaceCost, m_oldHairCost, m_oldBeardCost = 0, 0, 0
	m_curHairColorValue, m_curSkinColorValue = 0, 0
	m_curFaceValue, m_curHairValue, m_curBeardValue = 0, 0, 0
	m_oldFaceValue, m_oldHairValue, m_oldBeardValue = 0, 0, 0
	m_totalCost = 0
	Beauty_EnableAllBtn()
	Beauty_EndMutexOtherBtns()
	m_hairBtnVisible, m_hairColorUpBtnVisible, m_hairColorDownBtnVisible = true, true, true
	m_faceBtnVisible, m_skinUpBtnVisible, m_skinDownBtnVisible, m_beardBtnVisible = true, true, true, true	
	
	EQGlobaFunc.RTTSetActive(g_rttBeautyWinName, false)
end
-----------------------------------------
--点击确定按钮
function Beauty_OnClickOkBtn(args)
	g_smallModules:reqEndBeauty()
end
-----------------------------------------
--显示预览框
function BeautyUI_ShowRTT()
	local win = g_WinMgr:getWindow(g_rttBeautyWinName)
	local window = g_WinMgr:getWindow("Root/beauty/background/head")
	window:addChildWindow(win)
	
	EQGlobaFunc.RTTSetActive(g_rttBeautyWinName, true)
end
-----------------------------------------
--点击头发按钮
function Beauty_OnClickHairBtn(args)
	g_BeautyUI:setPosition(CEGUI.UVector2(CEGUI.UDim(oldBeautyUIPos_xScale, newBeautyUIPos_X), 
										CEGUI.UDim(oldBeautyUIPos_yScale, oldBeautyUIPos_yOffset)))
	g_MainUIWindows:addChildWindow(g_BeautyChooseUI)
	m_curSectionType = 2
	m_curAdjustType = 1
	if 1 == m_curMoneyType then
		m_totalPage = g_smallModules:getBeautyPages(m_hairTypes_EQ, 8)
		m_curTypes = m_hairTypes_EQ
	elseif 2 == m_curMoneyType then
		m_totalPage = g_smallModules:getBeautyPages(m_hairTypes_GOLD, 8)
		m_curTypes = m_hairTypes_GOLD
	end
	updateBeautyChooseUI(m_curSectionType, m_curAdjustType, m_curTypes, m_curPage, m_curMoneyType)
	Beauty_DisableBtns()
end
-----------------------------------------
--点击发色向上翻页按钮
function Beauty_HairColorPageUp(args)
	if 1 == m_curMoneyType then
		m_curTypes = m_hairColors_EQ
	elseif 2 == m_curMoneyType then
		m_curTypes = m_hairColors_GOLD
	end
	if 0 == m_curTypes then
		return
	end	
	m_curHairColorValue = m_curHairColorValue - 1
	if m_curHairColorValue < 0 then
		m_curHairColorValue = m_curTypes - 1
	end
	local val = g_smallModules:getBeautySectionVal(2, m_curHairColorValue, m_curMoneyType)
	g_smallModules:beautyRTTChangeLooks(2, val)
	local money = g_smallModules:getBeautySectionMoney(m_curSex, 2, val)
	local id = m_curSex*10000 + 2*1000 + val
	if g_smallModules:getSectionVal(1, 2) == val then
		money = 0
	end
	m_totalCost = m_totalCost - m_curHairColorCost + money
	m_curHairColorCost = money
	EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
	EQGlobaFunc.SetText(m_costWin:getName(), tostring(money))
end
-----------------------------------------
--点击发色向下翻页按钮
function Beauty_HairColorPageDown(args)
	if 1 == m_curMoneyType then
		m_curTypes = m_hairColors_EQ
	elseif 2 == m_curMoneyType then
		m_curTypes = m_hairColors_GOLD
	end
	if 0 == m_curTypes then
		return
	end	
	m_curHairColorValue = m_curHairColorValue + 1
	if m_curHairColorValue > m_curTypes-1 then
		m_curHairColorValue = 0
	end
	local val = g_smallModules:getBeautySectionVal(2, m_curHairColorValue, m_curMoneyType)
	g_smallModules:beautyRTTChangeLooks(2, val)
	local money = g_smallModules:getBeautySectionMoney(m_curSex, 2, val)
	local id = m_curSex*10000 + 2*1000 + val
	if g_smallModules:getSectionVal(1, 2) == val then
		money = 0
	end
	m_totalCost = m_totalCost - m_curHairColorCost + money
	m_curHairColorCost = money
	EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
	EQGlobaFunc.SetText(m_costWin:getName(), tostring(money))
end
-----------------------------------------
--点击脸型按钮
function Beauty_OnClickFaceBtn(args)
	g_BeautyUI:setPosition(CEGUI.UVector2(CEGUI.UDim(oldBeautyUIPos_xScale, newBeautyUIPos_X), 
										CEGUI.UDim(oldBeautyUIPos_yScale, oldBeautyUIPos_yOffset)))
	g_MainUIWindows:addChildWindow(g_BeautyChooseUI)
	m_curSectionType = 1
	m_curAdjustType = 3
	if 1 == m_curMoneyType then
		m_totalPage = g_smallModules:getBeautyPages(m_faceTypes_EQ, 8)
		m_curTypes = m_faceTypes_EQ
	elseif 2 == m_curMoneyType then
		m_totalPage = g_smallModules:getBeautyPages(m_faceTypes_GOLD, 8)
		m_curTypes = m_faceTypes_GOLD
	end	
	updateBeautyChooseUI(m_curSectionType, m_curAdjustType, m_curTypes, m_curPage, m_curMoneyType)
	Beauty_DisableBtns()
end
-----------------------------------------
--点击皮肤向上翻页按钮
function Beauty_SkinPageUp(args)
	if 1 == m_curMoneyType then
		m_curTypes = m_skinColors_EQ
	elseif 2 == m_curMoneyType then
		m_curTypes = m_skinColors_GOLD
	end
	if 0 == m_curTypes then
		return
	end	
	m_curSkinColorValue = m_curSkinColorValue - 1
	if m_curSkinColorValue < 0 then
		m_curSkinColorValue = m_curTypes - 1
	end
	local val = g_smallModules:getBeautySectionVal(4, m_curSkinColorValue, m_curMoneyType)
	g_smallModules:beautyRTTChangeLooks(4, val)
	local money = g_smallModules:getBeautySectionMoney(m_curSex, 4, val)
	local id = m_curSex*10000 + 4*1000 + val
	if g_smallModules:getSectionVal(1, 4) == val then
		money = 0
	end
	m_totalCost = m_totalCost - m_curSkinColorCost + money
	m_curSkinColorCost = money
	EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
	EQGlobaFunc.SetText(m_costWin:getName(), tostring(money))
end
-----------------------------------------
--点击皮肤向下翻页按钮
function Beauty_SkinPageDown(args)
	if 1 == m_curMoneyType then
		m_curTypes = m_skinColors_EQ
	elseif 2 == m_curMoneyType then
		m_curTypes = m_skinColors_GOLD
	end
	if 0 == m_curTypes then
		return
	end
	m_curSkinColorValue = m_curSkinColorValue + 1
	if m_curSkinColorValue > m_curTypes-1 then
		m_curSkinColorValue = 0
	end
	local val = g_smallModules:getBeautySectionVal(4, m_curSkinColorValue, m_curMoneyType)
	g_smallModules:beautyRTTChangeLooks(4, val)
	local money = g_smallModules:getBeautySectionMoney(m_curSex, 4, val)
	local id = m_curSex*10000 + 4*1000 + val
	if g_smallModules:getSectionVal(1, 4) == val then
		money = 0
	end
	m_totalCost = m_totalCost - m_curSkinColorCost + money
	m_curSkinColorCost = money
	EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
	EQGlobaFunc.SetText(m_costWin:getName(), tostring(money))
end
-----------------------------------------
--点击胡子按钮
function Beauty_OnClickBeardBtn(args)
	g_BeautyUI:setPosition(CEGUI.UVector2(CEGUI.UDim(oldBeautyUIPos_xScale, newBeautyUIPos_X), 
										CEGUI.UDim(oldBeautyUIPos_yScale, oldBeautyUIPos_yOffset)))
	g_MainUIWindows:addChildWindow(g_BeautyChooseUI)
	m_curSectionType = 3
	m_curAdjustType = 5
	if 1 == m_curMoneyType then
		m_totalPage = g_smallModules:getBeautyPages(m_beardTypes_EQ, 8)
		m_curTypes = m_beardTypes_EQ
	elseif 2 == m_curMoneyType then
		m_totalPage = g_smallModules:getBeautyPages(m_beardTypes_GOLD, 8)
		m_curTypes = m_beardTypes_GOLD
	end		
	updateBeautyChooseUI(m_curSectionType, m_curAdjustType, m_curTypes, m_curPage, m_curMoneyType)
	Beauty_DisableBtns()
end
-----------------------------------------
--
function BeautyChooseUI_Close()
	g_smallModules:clearBeautyChooseUI() -- 清除
	g_MainUIWindows:removeChildWindow(g_BeautyChooseUI)
	g_BeautyUI:setPosition(CEGUI.UVector2(CEGUI.UDim(oldBeautyUIPos_xScale, oldBeautyUIPos_xOffset), 
										CEGUI.UDim(oldBeautyUIPos_yScale, oldBeautyUIPos_yOffset)))
	Beauty_EnableBtns()
	if nil == tipsAniPareName then
		return
	end
	if nil == g_WinMgr:getWindow("Beauty/SlotTips") then
		return
	end
	g_WinMgr:getWindow(tipsAniPareName):removeChildWindow(g_WinMgr:getWindow("Beauty/SlotTips"))
end
-----------------------------------------
--点击选择界面关闭按钮
function BeautyChooseUI_OnClickCloseBtn(args)
	if 1 == m_curAdjustType then	-- 发型
		m_totalCost = m_totalCost - m_curHairCost + m_oldHairCost
		m_curHairCost = m_oldHairCost
		m_curHairValue = m_oldHairValue
		g_smallModules:beautyRTTChangeLooks(m_curAdjustType, m_curHairValue)
	elseif 3 == m_curAdjustType then	-- 脸型
		m_totalCost = m_totalCost - m_curFaceCost + m_oldFaceCost
		m_curFaceCost = m_oldFaceCost
		m_curFaceValue = m_oldFaceValue
		g_smallModules:beautyRTTChangeLooks(m_curAdjustType, m_curFaceValue)
	elseif 5 == m_curAdjustType then	-- 胡子
		m_totalCost = m_totalCost - m_curBeardCost + m_oldBeardCost
		m_curBeardCost = m_oldBeardCost
		m_curBeardValue = m_oldBeardValue
		g_smallModules:beautyRTTChangeLooks(m_curAdjustType, m_curBeardValue)
	end
	EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
	EQGlobaFunc.SetText(m_costWin:getName(), tostring(0))	
	BeautyChooseUI_Close()
end
-----------------------------------------
--点击选择界面确定按钮
function BeautyChooseUI_OnClickOkBtn(args)
	BeautyChooseUI_Close()
	if 1 == m_curAdjustType then	-- 发型
		m_oldHairCost = m_curHairCost
		m_oldHairValue = m_curHairValue
	elseif 3 == m_curAdjustType then	-- 脸型
		m_oldFaceCost = m_curFaceCost
		m_oldFaceValue = m_curFaceValue
	elseif 5 == m_curAdjustType then	-- 胡子
		m_oldBeardCost = m_curBeardCost
		m_oldBeardValue = m_curBeardValue
	end
end
-----------------------------------------
--点击选择界面向上翻页按钮
function BeautyChooseUI_PageUp(args)
	if 1 == m_curPage then	-- 第一页
		return
	end
	m_curPage = m_curPage - 1
	updateBeautyChooseUI(m_curSectionType, m_curAdjustType, m_curTypes, m_curPage, m_curMoneyType)
end
-----------------------------------------
--点击选择界面向下翻页按钮
function BeautyChooseUI_PageDown(args)
	if m_totalPage == m_curPage then	-- 最后页
		return
	end
	m_curPage = m_curPage + 1
	updateBeautyChooseUI(m_curSectionType, m_curAdjustType, m_curTypes, m_curPage, m_curMoneyType)
end
-----------------------------------------
--更新选择界面内容
function updateBeautyChooseUI(sectionType, adjustType, curTypes, curPage, moneyType)
	g_smallModules:clearBeautyChooseUI() -- 清除
	for i = 1, 8 do	-- 取消鼠标单击事件
		local winname = m_slotBaseName..i
		g_WinMgr:getWindow(winname):removeEvent ("MouseClick")
	end

	local start = (curPage - 1) * 8 + 1
	if curTypes <= start + 7 then
		for i = start, curTypes do
			local flag = i - (curPage - 1) * 8
			local winname = m_slotBaseName..flag
			local val = g_smallModules:getBeautySectionVal(adjustType, i - 1, moneyType)
			g_smallModules:setBeautyChooseUI(winname, adjustType, val)
			g_WinMgr:getWindow(winname):subscribeEvent("MouseClick", "Beauty_ChangeLooks")
		end
	else
		for i = start, start + 7 do
			local flag = i - (curPage - 1) * 8
			local winname = m_slotBaseName..flag
			local val = g_smallModules:getBeautySectionVal(adjustType, i - 1, moneyType)
			g_smallModules:setBeautyChooseUI(winname, adjustType, val)	
			g_WinMgr:getWindow(winname):subscribeEvent("MouseClick", "Beauty_ChangeLooks")			
		end
	end	
end
-----------------------------------------
--提示动画宽、高
function Beauty_TipsGetWH()
	local slotName = "beanty_parts_list/backgroundimg/part1"
	local slotWin = g_WinMgr:getWindow(slotName)
	if nil == slotWin then
		return
	end
	local width = slotWin:getWidth():asAbsolute(g_GuiSystem:getRenderer():getRect():getWidth())
	local height = slotWin:getHeight():asAbsolute(g_GuiSystem:getRenderer():getRect():getHeight())
	return width, height
end
-----------------------------------------
--提示动画初始化
function Beauty_TipsAnimateInit(animate)
	local w, h = Beauty_TipsGetWH()
	animate:initAnimate(40, w, h)
	local imageset = "LightCue"
	for i=1, 20 do
		local image
		if i >= 10 then
			image = imageset..i
		else
			image = imageset.."0"..i
		end
		animate:addAnimateFrame(imageset, image)
	end	
end
-- 该调用放在函数_EquipmentTips_AnimateInit()定义后面
Beauty_TipsAnimateInit(tipsAni)
-----------------------------------------
--点击格子动画提示
function Beauty_SlotTips(slotName)
	local tips = g_WinMgr:getWindow("Beauty/SlotTips")
	if nil == tips then
		tips = g_WinMgr:createWindow("EQHomeLook/AnimateBox", "Beauty/SlotTips")
		local width, height = Beauty_TipsGetWH()		
		tips:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
		tips:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 0), CEGUI.UDim(0, 0)))
		EQGlobaFunc.toAnimateBox(tips:getName()):addAnimate(tipsAni)
		EQGlobaFunc.toAnimateBox(tips:getName()):play(true)	-- 循环播放
	end
	g_WinMgr:getWindow(slotName):addChildWindow(tips)
end
-----------------------------------------
--根据格子获取数值
function Beauty_getValueBySlot(winname, page)
	local id = tonumber(string.sub(winname, -1, -1))
	local index = (page-1)*8+id-1
	local val = g_smallModules:getBeautySectionVal(m_curAdjustType, index, m_curMoneyType)
	return val
end
-----------------------------------------
--点击美容
function Beauty_ChangeLooks(args)
	if 1 == CEGUI.toMouseEventArgs(args).button then	--	鼠标右键
		return
	end
	local slotname = CEGUI.toWindowEventArgs(args).window:getName()
	Beauty_SlotTips(slotname)
	tipsAniPareName = slotname
	local val = Beauty_getValueBySlot(slotname, m_curPage)
	local money = g_smallModules:getBeautySectionMoney(m_curSex, m_curAdjustType, val)
	if 1 == m_curAdjustType then	-- 发型
		local id = m_curSex*10000 + 1*1000 + val
		m_curHairValue = g_smallModules:getSectionVal(2, 1)
		if g_smallModules:getSectionVal(1, 1) == val then
			money = 0
		end
		m_totalCost = m_totalCost - m_curHairCost + money
		m_curHairCost = money
	elseif 3 == m_curAdjustType then	-- 脸型
		local id = m_curSex*10000 + 3*1000 + val
		m_curFaceValue = g_smallModules:getSectionVal(2, 3)
		if g_smallModules:getSectionVal(1, 3) == val then
			money = 0
		end
		m_totalCost = m_totalCost - m_curFaceCost + money
		m_curFaceCost = money
	elseif 5 == m_curAdjustType then	-- 胡子
		local id = m_curSex*10000 + 5*1000 + val
		m_curBeardValue = g_smallModules:getSectionVal(2, 5)
		if g_smallModules:getSectionVal(1, 5) == val then
			money = 0
		end	
		m_totalCost = m_totalCost - m_curBeardCost + money
		m_curBeardCost = money
	end
	EQGlobaFunc.SetText(m_totalCostWin:getName(), tostring(m_totalCost))
	EQGlobaFunc.SetText(m_costWin:getName(), tostring(money))
	g_smallModules:beautyRTTChangeLooks(m_curAdjustType, val)
end
-----------------------------------------
--disable美容界面上所有按钮
function Beauty_DisableBtns()
	g_WinMgr:getWindow("Root/beauty/background/hair"):disable()
	g_WinMgr:getWindow("Root/beauty/background/haircolor_pgup"):disable()
	g_WinMgr:getWindow("Root/beauty/background/haircolor_pgdown"):disable()
	g_WinMgr:getWindow("Root/beauty/background/face"):disable()
	g_WinMgr:getWindow("Root/beauty/background/skin_pgup"):disable()
	g_WinMgr:getWindow("Root/beauty/background/skin_pgdown"):disable()
	g_WinMgr:getWindow("Root/beauty/background/beard"):disable()
	g_WinMgr:getWindow("Root/beauty/background/done"):disable()
	g_WinMgr:getWindow("Root/beauty/background/cancel"):disable()
	g_WinMgr:getWindow("Root/beauty__auto_closebutton__"):disable()
end
-----------------------------------------
--若没有可选项，则disable美容界面上按钮
function Beauty_DisableBtnSelect(moneyType)
	if 1 == moneyType then	-- 游戏币
		if 0 == m_hairTypes_EQ then
			g_WinMgr:getWindow("Root/beauty/background/hair"):disable()
			m_hairBtnVisible = false
		end
		if 0 == m_hairColors_EQ then
			g_WinMgr:getWindow("Root/beauty/background/haircolor_pgup"):disable()
			g_WinMgr:getWindow("Root/beauty/background/haircolor_pgdown"):disable()
			m_hairColorUpBtnVisible, m_hairColorDownBtnVisible = false, false
		end
		if 0 == m_faceTypes_EQ then
			g_WinMgr:getWindow("Root/beauty/background/face"):disable()
			m_faceBtnVisible = false
		end
		if 0 == m_skinColors_EQ then
			g_WinMgr:getWindow("Root/beauty/background/skin_pgup"):disable()
			g_WinMgr:getWindow("Root/beauty/background/skin_pgdown"):disable()
			m_skinUpBtnVisible, m_skinDownBtnVisible = false, false
		end
		if 0 == m_beardTypes_EQ then
			g_WinMgr:getWindow("Root/beauty/background/beard"):disable()
			m_beardBtnVisible = false
		end
	elseif 2 == moneyType then	-- EQ点
		if 0 == m_hairTypes_GOLD then
			g_WinMgr:getWindow("Root/beauty/background/hair"):disable()
			m_hairBtnVisible = false
		end
		if 0 == m_hairColors_GOLD then
			g_WinMgr:getWindow("Root/beauty/background/haircolor_pgup"):disable()
			g_WinMgr:getWindow("Root/beauty/background/haircolor_pgdown"):disable()
			m_hairColorUpBtnVisible, m_hairColorDownBtnVisible = false, false
		end
		if 0 == m_faceTypes_GOLD then
			g_WinMgr:getWindow("Root/beauty/background/face"):disable()
			m_faceBtnVisible = false
		end
		if 0 == m_skinColors_GOLD then
			g_WinMgr:getWindow("Root/beauty/background/skin_pgup"):disable()
			g_WinMgr:getWindow("Root/beauty/background/skin_pgdown"):disable()
			 m_skinUpBtnVisible, m_skinDownBtnVisible = false, false
		end
		if 0 == m_beardTypes_GOLD then
			g_WinMgr:getWindow("Root/beauty/background/beard"):disable()
			m_beardBtnVisible = false
		end
	end
end
-----------------------------------------
--enable美容界面上按钮
function Beauty_EnableBtns()
	if true == m_hairBtnVisible then
		g_WinMgr:getWindow("Root/beauty/background/hair"):enable()
	end
	if true == m_hairColorUpBtnVisible then
		g_WinMgr:getWindow("Root/beauty/background/haircolor_pgup"):enable()
	end
	if true == m_hairColorDownBtnVisible then
		g_WinMgr:getWindow("Root/beauty/background/haircolor_pgdown"):enable()
	end
	if true == m_faceBtnVisible then
		g_WinMgr:getWindow("Root/beauty/background/face"):enable()
	end
	if true == m_skinUpBtnVisible then
		g_WinMgr:getWindow("Root/beauty/background/skin_pgup"):enable()
	end
	if true == m_skinDownBtnVisible then
		g_WinMgr:getWindow("Root/beauty/background/skin_pgdown"):enable()
	end
	if true == g_mainplayer:isMale() then	--男性
		if true == m_beardBtnVisible then
			g_WinMgr:getWindow("Root/beauty/background/beard"):enable()
		end
	end
	g_WinMgr:getWindow("Root/beauty/background/done"):enable()
	g_WinMgr:getWindow("Root/beauty/background/cancel"):enable()
	g_WinMgr:getWindow("Root/beauty__auto_closebutton__"):enable()
end
-----------------------------------------
--enable美容界面上所有按钮
function Beauty_EnableAllBtn()
	g_WinMgr:getWindow("Root/beauty/background/hair"):enable()
	g_WinMgr:getWindow("Root/beauty/background/haircolor_pgup"):enable()
	g_WinMgr:getWindow("Root/beauty/background/haircolor_pgdown"):enable()
	g_WinMgr:getWindow("Root/beauty/background/face"):enable()
	g_WinMgr:getWindow("Root/beauty/background/skin_pgup"):enable()
	g_WinMgr:getWindow("Root/beauty/background/skin_pgdown"):enable()
	g_WinMgr:getWindow("Root/beauty/background/beard"):enable()
	g_WinMgr:getWindow("Root/beauty/background/done"):enable()
	g_WinMgr:getWindow("Root/beauty/background/cancel"):enable()
	g_WinMgr:getWindow("Root/beauty__auto_closebutton__"):enable()
end
-----------------------------------------
--打开美容院，开始互斥其他界面上的按钮(切换场景按钮除外)
function Beauty_StartMutexOtherBtns()
	local childCount = g_MainUIWindows:getChildCount()
	for i=0, childCount-1 do
		local child = g_MainUIWindows:getChildAtIdx(i)
		if nil ~= child then
			if g_BeautyUI:getName() == child:getName() then
			elseif g_BeautyChooseUI:getName() == child:getName() then
			elseif "MainRoot/SImg_VMenubar" == child:getName() then
				childCount = g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):getChildCount()
				for j=0, childCount-1 do
					child = g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):getChildAtIdx(j)
					if nil ~= child then
						if "MainRoot/VMenubar/Btn_Navigate" == child:getName() then
							child:enable()
						else
							child:disable()
							table.insert(m_disabledWindow, child)
						end
					end
				end
			else
				child:disable()
				table.insert(m_disabledWindow, child)
			end
		end
	end
end
-----------------------------------------
--关闭美容院，解除互斥其他界面上的按钮
function Beauty_EndMutexOtherBtns()
	for i, v in pairs(m_disabledWindow) do
		if nil ~= v then
			v:enable()
		end
	end
	m_disabledWindow = {}
end

-----------------------------------------
--  人物旋转处理
-----------------------------------------
local Btn_TurnRight = g_WinMgr:getWindow("Root/beauty/background/head/leftbutton")
local Btn_TurnLeft = g_WinMgr:getWindow("Root/beauty/background/head/rightbutton")
Btn_TurnRight:setAlwaysOnTop(true)
Btn_TurnLeft:setAlwaysOnTop(true)
--连续旋转
Btn_TurnRight:setMouseAutoRepeatEnabled(true)
Btn_TurnRight:setAutoRepeatRate(0.1)

Btn_TurnLeft:setMouseAutoRepeatEnabled(true)
Btn_TurnLeft:setAutoRepeatRate(0.1)

--屏蔽双击按键
Btn_TurnRight:setWantsMultiClickEvents(false)
Btn_TurnLeft:setWantsMultiClickEvents(false)

Btn_TurnRight:subscribeEvent("MouseButtonDown", "Beauty_TurnCounterclockwise")
Btn_TurnLeft:subscribeEvent("MouseButtonDown", "Beauty_TurnClockwise")

function Beauty_TurnCounterclockwise(args)
	g_smallModules:beautyRTTRotate(18, 2)
end

function Beauty_TurnClockwise(args)
	g_smallModules:beautyRTTRotate(-18, 2)
end

