--大表情
--
g_BigExpUIOpened = false
local mBigExpTable = {}
local mShowBigExpTable = {}
local mPage_MaxSize = 23
local mCurPage = 1
local mTotalPage = 1
local counter = 0 

local mPreviewBigExpWin = g_WinMgr:createWindow("EQHomeLook/AnimateBox", "BigExpPreviewWin")
local m_animate = EQGlobaClass.createAnimate()
local m_Headup_BigExpAnimate = EQGlobaClass.createAnimate()
--
Common_injectEmptyEvt("BigExpressionUI")
MainUI_ShieldClick(g_BigExpressionUI)

-------------------------------
--
g_WinMgr:getWindow("MainRoot/SImg_HMenubar1/BigExpBtn")
		:subscribeEvent("Clicked", "BigExpression_clickBigExpBtn")
g_WinMgr:getWindow("BigExpressionUI/pageup")
		:subscribeEvent("Clicked", "BigExpression_prePage")
g_WinMgr:getWindow("BigExpressionUI/pagedown")
		:subscribeEvent("Clicked", "BigExpression_nextPage")



--
function BigExpression_loadBigExpressionResource()
	local size = g_logictableMgr:getBigExpressionTableCount()
	for i=1,size do
		local exp_row = g_logictableMgr:getBigExpressionRow(i)
		if exp_row ~= nil then
			local firstf 	= BigExpression_analyseImgStr(exp_row.firstf)
			local secondf 	= BigExpression_analyseImgStr(exp_row.secondf)
			local thirdf 	= BigExpression_analyseImgStr(exp_row.thirdf)
			local forthf 	= BigExpression_analyseImgStr(exp_row.forthf)
			local fifthf 	= BigExpression_analyseImgStr(exp_row.fifthf)
			local sixthf 	= BigExpression_analyseImgStr(exp_row.sixthf)
			local seventhf 	= BigExpression_analyseImgStr(exp_row.seventhf)
			local eightf 	= BigExpression_analyseImgStr(exp_row.eightf)
			local ninthf 	= BigExpression_analyseImgStr(exp_row.ninthf)
			local tenthf 	= BigExpression_analyseImgStr(exp_row.tenthf)
			local temp_t 	= {	id=exp_row.id,playtime=exp_row.playtime,
								firstf=firstf,secondf=secondf,thirdf=thirdf,
								forthf=forthf,fifthf=fifthf,sixthf=sixthf,
								seventhf=seventhf,eightf=eightf,ninthf=ninthf,
								tenthf=tenthf}
			table.insert(mBigExpTable,temp_t)
		end
	end
	for i,v in ipairs(mBigExpTable) do
		--g_Loger:logMessage("***********" .." "..v.id.." "..v.playtime.." "..v.firstf.imgset.."*"..v.firstf.img)
	end
	mTotalPage = getTotalPage(mBigExpTable,mPage_MaxSize)
	BigExpression_updatePageData(mCurPage)
end
---------------------------------
--
function BigExpression_clickBigExpBtn()
	if table.getn(mBigExpTable) == 0 then
		BigExpression_loadBigExpressionResource()
	end
	if not g_BigExpUIOpened then
		BigExpression_openBigExpUI()
		g_BigExpUIOpened = true
	else
		BigExpression_closeBigExp()
	end
end
----------------------------------
--
function BigExpression_openBigExpUI()
	g_MainUIWindows:addChildWindow(g_BigExpressionUI)
	BigExpression_updatePageData(mCurPage)
end
-----------------------------------
--
function BigExpression_closeBigExp()
	mCurPage = 1
	g_MainUIWindows:removeChildWindow(g_BigExpressionUI)
	g_BigExpUIOpened = false
end
-----------------------------------
--
function BigExpression_prePage(args)
	if mCurPage > 1 then
		mCurPage = mCurPage - 1
	end
	BigExpression_updatePageData(mCurPage)
end
-----------------------------------
--
function BigExpression_nextPage(args)
	if mCurPage < mTotalPage then
		mCurPage = mCurPage + 1
	end
	BigExpression_updatePageData(mCurPage)
end

-----------------------------------
--
function BigExpression_updatePageData(page)
	mShowBigExpTable = getData(mBigExpTable,page,mPage_MaxSize)
	for i,v in ipairs(mShowBigExpTable) do
		--g_Loger:logMessage("***********" .." "..v.id.." "..v.playtime.." "..v.firstf.imgset.."*"..v.firstf.img)
	end
	BigExpression_showBigExp()
end
------------------------------------
--
function BigExpression_showBigExp()
	local slotWin_name_base = "BigExpressionUI/expression"
	local BigExpWin_name = "bigExp_icon"
	BigExpression_clearAllSlotIcon()
	for i,v in ipairs(mShowBigExpTable) do
		g_WinMgr:getWindow(slotWin_name_base..tostring(i)):removeEvent("MouseClick")
		g_WinMgr:getWindow(slotWin_name_base..tostring(i)):removeEvent("MouseEnter")
		g_WinMgr:getWindow(slotWin_name_base..tostring(i)):removeEvent("MouseLeave")
		g_WinMgr:getWindow(slotWin_name_base..tostring(i))
				:subscribeEvent("MouseClick", "BigExpression_MouseClick")
		g_WinMgr:getWindow(slotWin_name_base..tostring(i))
				:subscribeEvent("MouseEnter", "BigExpression_MouseEnter")
		g_WinMgr:getWindow(slotWin_name_base..tostring(i))
				:subscribeEvent("MouseLeave", "BigExpression_MouseLeave")
		g_WinMgr:getWindow(slotWin_name_base..tostring(i)):setID(v.id)
		StorageUI_DynamicLoadItem(	slotWin_name_base..tostring(i),
									BigExpWin_name,
									i, 
									v.firstf.imgset, 
									v.firstf.img, 
									"False")
	end
end
------------------------------------
--
function BigExpression_MouseClick(args)
	local click_win = CEGUI.toWindowEventArgs(args).window
	local id = click_win:getID()
	--g_Loger:logError("*********-------"..tostring(id))
	if id == 0 then
		return
	end
	local bigExpInfo = mBigExpTable[id]
	if bigExpInfo ~= nil then
		--g_Loger:logError(bigExpInfo.firstf.img)
		--g_Loger:logError(bigExpInfo.secondf.img)
	end
	g_mainplayer:showHeadupBigExpression(id)
	g_mainplayer:reqPlayBigExpression(id)
	BigExpression_closeBigExp()
end
------------------------------------
--
function BigExpression_MouseEnter(args)
	local movein_win = CEGUI.toWindowEventArgs(args).window
	local id = movein_win:getID()
	g_Loger:logError("*********-------"..tostring(id))
	if id == 0 then
		return
	end
	local bigExpInfo = mBigExpTable[id]
	if bigExpInfo == nil then
		return
	end
	--g_Loger:logError("*********-------"..tostring(id))
	g_Loger:logError(bigExpInfo.firstf.imgset)
	g_Loger:logError(bigExpInfo.secondf.img)
	if g_ImagesetMgr:getImageset(bigExpInfo.firstf.imgset) == nil then
		return
	end
	local imgWidth = g_ImagesetMgr:getImageset(bigExpInfo.firstf.imgset):getImage(bigExpInfo.firstf.img):getWidth()
	local imgHeight = g_ImagesetMgr:getImageset(bigExpInfo.firstf.imgset):getImage(bigExpInfo.firstf.img):getHeight()
	m_animate:initAnimate(bigExpInfo.playtime, imgWidth, imgHeight)
	m_animate:addAnimateFrame(bigExpInfo.firstf.imgset, bigExpInfo.firstf.img)
	if bigExpInfo.secondf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.secondf.imgset, bigExpInfo.secondf.img)
	end
	if bigExpInfo.thirdf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.thirdf.imgset, bigExpInfo.thirdf.img)
	end
	if bigExpInfo.forthf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.forthf.imgset, bigExpInfo.forthf.img)
	end
	if bigExpInfo.fifthf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.fifthf.imgset, bigExpInfo.fifthf.img)
	end
	if bigExpInfo.sixthf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.sixthf.imgset, bigExpInfo.sixthf.img)
	end
	if bigExpInfo.seventhf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.seventhf.imgset, bigExpInfo.seventhf.img)
	end
	if bigExpInfo.eightf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.eightf.imgset, bigExpInfo.eightf.img)
	end
	if bigExpInfo.ninthf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.ninthf.imgset, bigExpInfo.ninthf.img)
	end
	if bigExpInfo.tenthf.imgset ~= "0" then
		m_animate:addAnimateFrame(bigExpInfo.tenthf.imgset, bigExpInfo.tenthf.img)
	end
	mPreviewBigExpWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, imgWidth), CEGUI.UDim(0, imgHeight)))
	EQGlobaFunc.toAnimateBox(mPreviewBigExpWin:getName()):addAnimate(m_animate)
	
	mPreviewBigExpWin:setXPosition(g_BigExpressionUI:getXPosition() + g_BigExpressionUI:getWidth())
	mPreviewBigExpWin:setYPosition(g_BigExpressionUI:getYPosition())
	g_MainUIWindows:addChildWindow(mPreviewBigExpWin)
	EQGlobaFunc.toAnimateBox(mPreviewBigExpWin:getName()):play(true)
end
------------------------------------
--
function BigExpression_MouseLeave(args)
	g_MainUIWindows:removeChildWindow(mPreviewBigExpWin)
end
------------------------------------
--
function BigExpression_clearAllSlotIcon()
	local slotWin_name_base = "BigExpressionUI/expression"
	local BigExpWin_name = "bigExp_icon"
	for i=1,mPage_MaxSize do
		g_WinMgr:getWindow(slotWin_name_base..tostring(i)):setID(0)
		g_WinMgr:getWindow(slotWin_name_base..tostring(i)):removeChildWindow(slotWin_name_base..tostring(i).."/DragContainer" ..i)
	end
end

--
--
function BigExpression_CreateBigExpressionWin(winname,id)
	local win = g_WinMgr:getWindow(winname)
	local bigExpInfo = mBigExpTable[id]
	if bigExpInfo == nil then
		return winname
	end
	if g_ImagesetMgr:getImageset(bigExpInfo.firstf.imgset) == nil then
		return winname
	end
	local imgWidth = g_ImagesetMgr:getImageset(bigExpInfo.firstf.imgset):getImage(bigExpInfo.firstf.img):getWidth()
	local imgHeight = g_ImagesetMgr:getImageset(bigExpInfo.firstf.imgset):getImage(bigExpInfo.firstf.img):getHeight()
	m_Headup_BigExpAnimate:initAnimate(bigExpInfo.playtime, imgWidth, imgHeight)
	m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.firstf.imgset, bigExpInfo.firstf.img)
	if bigExpInfo.secondf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.secondf.imgset, bigExpInfo.secondf.img)
	end
	if bigExpInfo.thirdf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.thirdf.imgset, bigExpInfo.thirdf.img)
	end
	if bigExpInfo.forthf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.forthf.imgset, bigExpInfo.forthf.img)
	end
	if bigExpInfo.fifthf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.fifthf.imgset, bigExpInfo.fifthf.img)
	end
	if bigExpInfo.sixthf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.sixthf.imgset, bigExpInfo.sixthf.img)
	end
	if bigExpInfo.seventhf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.seventhf.imgset, bigExpInfo.seventhf.img)
	end
	if bigExpInfo.eightf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.eightf.imgset, bigExpInfo.eightf.img)
	end
	if bigExpInfo.ninthf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.ninthf.imgset, bigExpInfo.ninthf.img)
	end
	if bigExpInfo.tenthf.imgset ~= "0" then
		m_Headup_BigExpAnimate:addAnimateFrame(bigExpInfo.tenthf.imgset, bigExpInfo.tenthf.img)
	end
	
	if win == nil then
		winname = "DynamicUI_BigExp_"..counter
		counter = counter + 1
		
		win = g_WinMgr:createWindow("EQHomeLook/AnimateBox", winname)
		win:setSize(CEGUI.UVector2(CEGUI.UDim(0, imgWidth), CEGUI.UDim(0, imgHeight)))
		EQGlobaFunc.toAnimateBox(win:getName()):addAnimate(m_Headup_BigExpAnimate)
		
		g_MainUIWindows:addChildWindow(win)
		EQGlobaFunc.toAnimateBox(win:getName()):play(true)
	else
		win:setSize(CEGUI.UVector2(CEGUI.UDim(0, imgWidth), CEGUI.UDim(0, imgHeight)))
		EQGlobaFunc.toAnimateBox(win:getName()):addAnimate(m_Headup_BigExpAnimate)
		g_MainUIWindows:addChildWindow(win)
		EQGlobaFunc.toAnimateBox(win:getName()):play(true)
		win:setAlpha(1)
	end
	
	win:setAlwaysOnTop(true)
	return winname
end
--
function BigExpression_SetBigExpressionWinPos(winname,x,y,chatpaopaoVisible)
	g_Loger:logError("----chatpaopaoVisible---"..tostring(chatpaopaoVisible))
	if winname == "" then
		return
	end
	local win = g_WinMgr:getWindow(winname)
	if win == nil then
		return
	end
	local dx = 0	
	local dy = 0	
	if chatpaopaoVisible then
		dx = CEGUI.UDim(0,x+73)
		dy = CEGUI.UDim(0,y-5) - win:getHeight()
	else
		dx = CEGUI.UDim(0,x) - CEGUI.UDim(0,win:getWidth():asAbsolute(800)/2)
		dy = CEGUI.UDim(0,y-25) - win:getHeight()
	end
	win:setPosition(CEGUI.UVector2(dx, dy))
end
------------------------------------
--
function BigExpression_analyseImgStr(imgStr)
	local t = Split(imgStr," ")
	local ret = {}
	if imgStr ~= "0" then
		ret.imgset = t[1]
		ret.img = t[2]
	else
		ret.imgset = "0"
		ret.img = "0"
	end
		return ret
end