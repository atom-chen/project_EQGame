-----------------------------------------------------------------
--打工系统
--xiaoMing
--2010-08-16
-----------------------------------------------------------------
local mWorksPack = {}
local mWorkPageSize = 6
local mCurPage = 1
local mChooseWorkUI = false
local mWorkFlag = false
local mSaveWorkTable = false
local mStep = 1
local mWorkRow
local mTextWnd
local mSelWorkIdx = nil
local mHealthStateDesc = {"状态-绿色","状态-绿色、白色","状态-绿色、白色、黄色","状态-绿色、白色、黄色、紫色","无状态需求"}
--流程 点击打工-》弹出提示 -》播放打工动画-》结算-》触发特殊事件-》播放固定图片-》播放特殊事件图片-》结算
-----------------------------------------------------------------
local function _ClearWorks()
	for i = 1, mWorkPageSize do
		SetWindowImage(g_works.slotName..i, "", "")
	end
end
-----------------------------------------------------------------
local function _UpdateWorkPage(page, pack)
	_ClearWorks()
	local data = getData(pack, page, mWorkPageSize )
	local itemRow

	for i, v in pairs(data) do
		itemRow = g_logictableMgr:getWorkRow(v)
		if nil ~= itemRow then
			SetWindowImage(g_works.slotName..i, itemRow.typeImageSet, itemRow.typeImage)
		end
	end
	FUpdatePageInfo(g_works.pageText,page)
end
-----------------------------------------------------------------
local function _ShowImage(flag)
	g_WinMgr:getWindow(g_works.textBack):setVisible(not flag)
	g_WinMgr:getWindow(g_works.textWindow):setVisible(not flag)
	g_WinMgr:getWindow(g_works.imageWindow):setVisible(flag)
	g_WinMgr:getWindow(g_works.imageFrame):setVisible(flag)
	g_WinMgr:getWindow(g_works.textFrame):setVisible(not flag)
end
-----------------------------------------------------------------
local function _DoStep(step)
	local str = g_mainplayer:getName().."获得"
	if 2 == step then
		--打工结算
		_ShowImage(false)	
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):clearUp()	
		
		str = "	获得：金币"..g_smallModules.workMoney.."\n\n"
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", str, "#39C3F9", true)
		--属性值改变
		if 0 ~= g_smallModules.changeActive then
		
			if 0 > g_smallModules.changeActive then
				str =  "消耗："..-g_smallModules.changeActive.."活跃值 "
			else
				str =  "增加："..g_smallModules.changeActive.."活跃值 "
			end
			
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", str, "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "", "#39C3F9", true)
		end

		if 0 ~= g_smallModules.changeClean then
		
			if 0 > g_smallModules.changeClean then
				str =  "消耗："..-g_smallModules.changeClean.."人物清洁度 "
			else
				str =  "增加："..g_smallModules.changeClean.."人物清洁度 "
			end
			
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", str, "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "", "#39C3F9", true)
		end

		if 0 ~= g_smallModules.changeHealth then
		
			if 0 > g_smallModules.changeHealth then
				str =  "消耗："..-g_smallModules.changeHealth.."健康值 "
			else
				str =  "增加："..g_smallModules.changeHealth.."健康值 "
			end
			
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", str, "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "", "#39C3F9", true)
		end

		if 0 ~= g_smallModules.changeCharm then
		
			if 0 > g_smallModules.changeCharm then
				str =  "消耗："..-g_smallModules.changeCharm.."魅力值  "
			else
				str =  "增加："..g_smallModules.changeCharm.."魅力值  "
			end
			
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", str, "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "", "#39C3F9", true)
		end
		PlayFadeAnimation(g_works.textWindow, 2000, 0, 1, 0)
	elseif 3 == step then
		--特殊事件固定图片
		_ShowImage(true)
		SetWindowImage(g_works.imageWindow, "find", "find")
		PlayFadeAnimation(g_works.imageWindow, 2000, 0, 1, 0)
	elseif 4 == step then
		--特殊事件图片
		local row = g_logictableMgr:getWorkEventRow(g_smallModules.workEventID)
		local str = GetImageName(row.image)
		g_WinMgr:getWindow(g_works.imageWindow):setProperty("Image", str)
		PlayFadeAnimation(g_works.imageWindow, 2000, 0, 1, 0)
	elseif 5 == step then
		--特殊事件结算
		_ShowImage(false)
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):clearUp()	
		
		if 0 ~= g_smallModules.eventMoney then	
			str = "获得：金币"..g_smallModules.eventMoney
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "", "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "", "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", str, "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "", "#39C3F9", true)	
		end

		local itemRow = g_itemtableMgr:getItemRow(g_smallModules.eventItem)

		if nil ~= itemRow then
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", "获得：", "#39C3F9", true)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementImage(itemRow.imageset, itemRow.icon, 32, 32, false)
			EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("simsun-12", itemRow.name, "#39C3F9", false)
		end
		PlayFadeAnimation(g_works.textWindow, 2000, 0, 1, 0)
	else
		--请求结束打工
		ReqEndWork(g_smallModules.workId)
	end
end
-----------------------------------------------------------------
function OnLoadWorksUI()
	g_WinMgr:getWindow(g_works.prePage):subscribeEvent("MouseClick", "OnPrePageWork")
	g_WinMgr:getWindow(g_works.nextPage):subscribeEvent("MouseClick", "OnNextPageWork")
	g_WinMgr:getWindow(g_works.closeBtn):subscribeEvent("Clicked", "EndWork")
	g_WinMgr:getWindow(g_workConfirmUINames.okBtn):subscribeEvent("Clicked", "OnWorkConfirmOK")
	g_WinMgr:getWindow(g_workConfirmUINames.cancelBtn):subscribeEvent("Clicked", "OnWorkConfirmCancel")
	g_WinMgr:getWindow(g_works.imageWindow):subscribeEvent("MouseClick", "ClickWorkImage")
	g_WinMgr:getWindow(g_works.textBack):subscribeEvent("MouseClick", "ClickWorkText")
	g_GameEventMgr:subscribeEvent("POP_WORK_UI", "PopWordUI")

	for i = 1, mWorkPageSize do
		g_WinMgr:getWindow(g_works.slotName..i):subscribeEvent("MouseClick", "ChooseWork")
	end

	g_WinMgr:getWindow(g_works.imageFrame):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow(g_works.textFrame ):setProperty("MousePassThroughEnabled", "true")
	
	mTextWnd = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mTextWnd")
	mTextWnd:setSize(CEGUI.UVector2(CEGUI.UDim(0, 210), CEGUI.UDim(0,180)))
	g_WinMgr:getWindow(g_works.textWindow):addChildWindow(mTextWnd)	

	MainUI_ShieldClick(g_chooseWorksUI)
	MainUI_ShieldClick(g_workUI)
	MainUI_ShieldClick(g_workConfirmUI)
	
	g_WinMgr:getWindow("dagongtishi__auto_closebutton__"):subscribeEvent("MouseClick", "OnWorkConfirmCancel")
	
end
-----------------------------------------------------------------
function ReqStartWord()
	g_smallModules:reqStartWork()
end
-----------------------------------------------------------------
function ReqWork(workId)
	g_smallModules:reqWork(workId)
end
-----------------------------------------------------------------
function ReqEndWork(workId)
	g_smallModules:reqEndWork(workId)
end
-----------------------------------------------------------------
function SaveWorksTable(id, index, count)
	table.insert(mWorksPack, id)
	if index +1 == count then
		_UpdateWorkPage(mCurPage, mWorksPack)
	end
end
-----------------------------------------------------------------
function OnNextPageWork()
	if getTotalPage(mWorksPack, mWorkPageSize ) <= mCurPage then
		return
	end

	mCurPage = mCurPage + 1
	_UpdateWorkPage(mCurPage, mWorksPack)
end
-----------------------------------------------------------------
function OnPrePageWork()
	if 1 >= mCurPage then
		return
	end

	mCurPage = mCurPage - 1
	_UpdateWorkPage(mCurPage, mWorksPack)
end
-----------------------------------------------------------------
local firstOpenWorkChooseUI = true
function PopWorkChooseUI()
	g_MainUIWindows:addChildWindow(g_chooseWorksUI)
	mChooseWorkUI = true
	if false == mSaveWorkTable then
		g_smallModules:saveWorks()
		mSaveWorkTable = true
	end
	if firstOpenWorkChooseUI then
		do_task_script("script11")	-- (打开打工界面)执行新手任务脚本11(2011/6/15 add by hezhr)
		firstOpenWorkChooseUI = false
	end
end
-----------------------------------------------------------------
function CloseWorkChooseUI()
	g_MainUIWindows:removeChildWindow(g_chooseWorksUI)
	mChooseWorkUI = false
end
-----------------------------------------------------------------
function EndWork()
	CloseWorkChooseUI()
	ReqEndWork(0)
end
-------------------------------------------------------------
--
function PWork_getchooseWorkUIOpenedFlag()
	return mChooseWorkUI
end
-----------------------------------------------------------------
function PopWordUI()
	if false == mWorkFlag then
		g_MainUIWindows:removeChildWindow(g_chooseWorksUI)
		mChooseWorkUI = false
		--AddFullScreenUI(g_workUI, "common1 ", "Window13", g_works.bgWindow)
		OpenWorkUI()
		UIM_DoOpenAndCloseUI(g_works.bgWindow)
		mWorkFlag = true
		_ShowImage(true)
		--读表
		mWorkRow = g_logictableMgr:getWorkRow(g_smallModules.workId)
		local str = GetImageName(mWorkRow.workImage)
		g_WinMgr:getWindow(g_works.imageWindow):setProperty("Image", str)
		--播放淡入动画
		PlayFadeAnimation(g_works.imageWindow, 2000, 0, 1, 0)
		--有个边框的宽度
		ChangeImageFormat(802, 602, g_works.bgWindow)
		mStep = 1
	end
end
function OpenWorkUI()
	UIM_OpenWindow(g_works.bgWindow)
	Common_setModalState(g_works.bgWindow, true)
end
-----------------------------------------------------------------
function CloseWorkUI()
	--RemoveFullScreenUI(g_workUI)
	UIM_CloseWindow(g_works.bgWindow)
	Common_setModalState(g_works.bgWindow, false)
	mWorkFlag = false
	CloseWorkChooseUI()
	do_task_script("script30")
end
-------------------------------------------------------------
--
function PWork_getUIOpenedFlag()
	return mWorkFlag
end
-----------------------------------------------------------------
function ChooseWork(args)
	local windowName = CEGUI.toWindowEventArgs(args).window:getName()
	local index = GetClickWindowIndex(windowName, g_works.slotName, 1, mWorkPageSize)

	if nil ~= index then
		index = FShopGetTableIndex(index, mCurPage, mWorkPageSize)

		if index <= table.getn(mWorksPack) then
			mSelWorkIdx = index
			
			OpenWorkConfirmUI(mSelWorkIdx)
		end
	end
end
-----------------------------------------------------------------
function ConcatWorkConfirmStringArray(arr)
	local i,n,p,s
	
	s = ""
	p = 1
	n = table.getn(arr)
	for i = 1, n do
		if arr[i][1] > 0 or arr[i][1] == -1 then
		
			if arr[i][1] == -1 then
				s = s .. arr[i][2]
			else
				s = s .. arr[i][1] .. arr[i][2]
			end
			
			if (p % 2) == 0 then
				s = s .. "\n"
			elseif i ~= n then
				s = s .. "、"
			end
			
			p = p + 1
		end
	end
	
	return s
end

function OpenWorkConfirmUI(workId)
	if workId ~= nil then
		local wrow = g_logictableMgr:getWorkRow(workId)
		local prow = g_logictableMgr:getPlayerPropertyRow(wrow.propertyid)
		local arr_need = {{wrow.clean, "点清洁度"},{wrow.health, "点健康值"},{wrow.active, "点活跃值"},{wrow.charm, "点魅力值"},{-1, mHealthStateDesc[wrow.healthState]}}
		local arr_expend = {{math.abs(prow.clean), "点清洁度"},{math.abs(prow.health), "点健康值"},{math.abs(prow.active), "点活跃值"},{math.abs(prow.charm), "点魅力值"}}
		
		local sneed = ConcatWorkConfirmStringArray(arr_need)
		local sexpend = ConcatWorkConfirmStringArray(arr_expend)
		local sgain = tostring(wrow.money).."枚金币"
		
		EQGlobaFunc.SetText(g_workConfirmUINames.labelNeed, sneed)
		EQGlobaFunc.SetText(g_workConfirmUINames.labelExpend, sexpend)
		EQGlobaFunc.SetText(g_workConfirmUINames.labelGain, sgain)
	end
	
	g_MainUIWindows:addChildWindow(g_workConfirmUI)
	Common_setModalState(g_workConfirmUI:getName(), true)
end
-----------------------------------------------------------------
function CloseWorkConfirmUI()
	g_MainUIWindows:removeChildWindow(g_workConfirmUI)
	Common_setModalState(g_workConfirmUI:getName(), false)
end
-----------------------------------------------------------------
function OnWorkConfirmOK(args)
	CloseWorkConfirmUI()
	
	CloseWorkChooseUI()
	--请求打工
	ReqWork(mWorksPack[mSelWorkIdx])
end
-----------------------------------------------------------------
function OnWorkConfirmCancel(args)
	CloseWorkConfirmUI()
end
-----------------------------------------------------------------
function PlayFadeAnimation(winName, playTime, startAlpha, endAlpha, delayTime)
	g_WinMgr:getWindow(winName):setAlpha(0)
	g_uiAnimationInfo.type = EQGlobaClass.UIAnimationInfo.EUIA_FADE
	g_uiAnimationInfo.playTime = playTime
	g_uiAnimationInfo.startAlpha = startAlpha
	g_uiAnimationInfo.endAlpha = endAlpha
	g_uiAnimationInfo.delayTime = delayTime
	EQGlobaFunc.playUIAnimation(winName, g_uiAnimationInfo)
end
-----------------------------------------------------------------
function ClickWorkImage()
	if true == EQGlobaFunc.uiAnimationIsPlaying(g_works.imageWindow) then
		return
	end

	mStep = mStep + 1
	_DoStep(mStep)
end
-----------------------------------------------------------------
function ClickWorkText()
	if true == EQGlobaFunc.uiAnimationIsPlaying(g_works.textWindow) then
		return
	end

	if 2 == mStep and 0 == g_smallModules.workEventID then
		ReqEndWork(g_smallModules.workId)
		return
	end

	mStep = mStep + 1
	_DoStep(mStep)
end
-----------------------------------------------------------------
function GetImageName(str)
	--设置图片 从N张图片中随机选取一张播放
	local strSet = {}
	local imageName

	strSet = Split(str, ",")
	local size = table.getn(strSet)

	if 0 == size then
		g_Loger:logError("work table workImage row is NULL")
	end

	math.randomseed(os.time())

	size = size * 100 - 1

	--过滤第一次随机结果，因为种子差异不大，第一次随机的结果一样
	--for i = 1, 5 do
		math.random(1, size)
	--end

	local index = math.random(1, size)

	return strSet[math.ceil(index/100)]
end
