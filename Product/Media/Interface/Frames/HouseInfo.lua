-----------------------------------------------------------------
--房屋信息
--xiaoMing
--2010-08-05
-----------------------------------------------------------------
-----------------------------------------------------------------
g_houseInfoFlag = false
--暂时用于存放访问记录 等待多行文本支持
local mMsgBuffer = ""
--房屋访问权限
local mRadioChangeFlag = false
local mWelWordsChangeFlag = false
local mHouseType = {"套房", "别墅"}
local mVisitedLogWnd

--访问记录类型
local mHVTType = { visit = 1, leave = 2, pick_garbage = 3}

-- 礼物记录数据
-- 礼物图文混排框
local mSendLogWin
local mRecLogWin

local mCurLogPage = 1
local mCurType

local mReplyAcc--回复快递型礼物帐号

local mTotalPage = {} --每种类型记录的总页数
local mGiftLogTypeTips = {
						[EQGlobaClass.GiftLogManager.DIY_RECIVE_NO_VIEW] = "显示未查收的礼盒",
						[EQGlobaClass.GiftLogManager.DIY_SEND] = "显示你送出的礼盒记录",
						[EQGlobaClass.GiftLogManager.MAIL_SEND] = "显示你送出的快递礼物记录",
						[EQGlobaClass.GiftLogManager.MAIL_RECIVE_NO_VIEW] = "显示未查收的快递礼物",
						[EQGlobaClass.GiftLogManager.DIY_RECIVE_VIEW] = "显示已查收的礼盒",
						[EQGlobaClass.GiftLogManager.MAIL_RECIVE_VIEW] = "显示已查收的快递礼物",
							}

local mHouseUseState = { yes = 1, no = 0 }
local mHouseDataList = {}
local mHouseChangeIdx = 0
local mHouseChangeTypeDef = { yes = 1, no = 2 }
local mHouseChangeType = 0
local m_checkBoxNameTable = {}	-- 消息框中复选框表(add by hezhr)

local mHouseSelSize = 2
local mHouseSelState = { none = 0, used = 1, unused = 2 }
local mHouseSelList = {}
local mHouseSelItemName = ""
local mUnViewMailGiftIds = {}
local mMailGiftIconName = "DFwnd_MainRoot/MailGiftIcon"
local g_OpenMailGiftId = nil
g_ToHouseFromDef = { friendList = 1, circleMenu = 2 }
g_ToHouseFromType = 1
-----------------------------------------------------------------
local function _InitComboBox()
	EQGlobaFunc.createComboBoxTextItem(g_houseInfo.sendLogComboBox, "送出的礼盒", EQGlobaClass.GiftLogManager.DIY_SEND,"")
	EQGlobaFunc.createComboBoxTextItem(g_houseInfo.sendLogComboBox, "送出的快递礼物", EQGlobaClass.GiftLogManager.MAIL_SEND,"")
		
	EQGlobaFunc.createComboBoxTextItem(g_houseInfo.recLogComboBox, "未收的礼盒", EQGlobaClass.GiftLogManager.DIY_RECIVE_NO_VIEW,"")
	EQGlobaFunc.createComboBoxTextItem(g_houseInfo.recLogComboBox, "未收的快递礼物 ", EQGlobaClass.GiftLogManager.MAIL_RECIVE_NO_VIEW,"")
	EQGlobaFunc.createComboBoxTextItem(g_houseInfo.recLogComboBox, "已收的礼盒", EQGlobaClass.GiftLogManager.DIY_RECIVE_VIEW,"")
	EQGlobaFunc.createComboBoxTextItem(g_houseInfo.recLogComboBox, "已收的快递礼物", EQGlobaClass.GiftLogManager.MAIL_RECIVE_VIEW,"")
	
	g_WinMgr:getWindow(g_houseInfo.sendLogComboBox):subscribeEvent("ListSelectionAccepted", "SelectSendLogType")
	g_WinMgr:getWindow(g_houseInfo.recLogComboBox):subscribeEvent("ListSelectionAccepted", "SelectRecLogType")
	
	g_WinMgr:getWindow(g_houseInfo.prePageSendLog):subscribeEvent("Clicked", "PrePageGiftLog")
	g_WinMgr:getWindow(g_houseInfo.nextPageSendLog):subscribeEvent("Clicked", "NextPageGiftLog")
	g_WinMgr:getWindow(g_houseInfo.prePageRecLog):subscribeEvent("Clicked", "PrePageGiftLog")
	g_WinMgr:getWindow(g_houseInfo.nextPageRecLog):subscribeEvent("Clicked", "NextPageGiftLog")
	
	g_WinMgr:getWindow(mMailGiftIconName):subscribeEvent("MouseClick", "OnOpenUnViewGift")
	
end
-----------------------------------------------------------------
local function _InitGiftLog()
	CEGUI.toCombobox(g_WinMgr:getWindow(g_houseInfo.sendLogComboBox)):getEditbox():setText("")
	CEGUI.toCombobox(g_WinMgr:getWindow(g_houseInfo.recLogComboBox)):getEditbox():setText("")
	
	CleanRecGiftLogUI()
	CleanSendGiftLogUI()
	
	EQGlobaFunc.SetText(g_houseInfo.sendLogTip, "")
	EQGlobaFunc.SetText(g_houseInfo.recLogTip, "")
	
	mCurLogPage = 1
	mCurType = EQGlobaClass.GiftLogManager.NIL
	
	mTotalPage = {}
	
	g_smallModules:initGiftLog()
	
	local win = g_WinMgr:getWindow(g_houseInfo.recLogComboBox)
	CEGUI.toCombobox(win):setItemSelectState(0,true)
	SelectRecLogType()
	
	local win = g_WinMgr:getWindow(g_houseInfo.sendLogComboBox)
	CEGUI.toCombobox(win):setItemSelectState(0,true)
	SelectSendLogType()
end
-----------------------------------------------------------------
function HouseInfoOnload()
	g_WinMgr:getWindow(g_houseInfo.closeBtn):subscribeEvent("Clicked", "CloseHouseInfoUI")
	g_WinMgr:getWindow(g_houseInfo.sureBtn):subscribeEvent("Clicked", "OnSureButton")
	g_WinMgr:getWindow(g_houseInfo.cancleBtn):subscribeEvent("Clicked", "CloseHouseInfoUI")
	g_WinMgr:getWindow(g_houseInfo.clear):subscribeEvent("MouseClick", "ReqClearHouseLog")

	g_WinMgr:getWindow(g_houseInfo.useFur):subscribeEvent("CheckStateChanged", "OnStateChanged")
	g_WinMgr:getWindow(g_houseInfo.accessY):subscribeEvent("SelectStateChanged", "OnStateChanged")


	g_WinMgr:getWindow(g_houseInfo.welcomeTxt):subscribeEvent("TextChanged", "OnWelWordsChanged")
	CEGUI.toMultiLineEditbox(g_WinMgr:getWindow(g_houseInfo.welcomeTxt)):setMaxTextLength(64)
	CEGUI.toMultiLineEditbox(g_WinMgr:getWindow(g_houseInfo.welcomeTxt)):setProperty("SelectionBrushImage","set:TaharezLook image:MultiLineEditboxSelectionBrush")
	local window--给RaidoButton分组
	window = g_WinMgr:getWindow(g_houseInfo.accessY)
	CEGUI.toRadioButton(window):setGroupID(1)
	window = g_WinMgr:getWindow(g_houseInfo.accessN)
	CEGUI.toRadioButton(window):setGroupID(1)
	MainUI_ShieldClick(g_houseInfoUI)
	
	--图文混排
	mVisitedLogWnd = g_WinMgr:createWindow("NewEQHomeLook/NewMultiLineBox", "visitedLogWnd")
	mVisitedLogWnd:setSize(CEGUI.UVector2(CEGUI.UDim(0, 340), CEGUI.UDim(0, 200)))
	g_WinMgr:getWindow(g_houseInfo.log):addChildWindow(mVisitedLogWnd)
	EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):setScrollbarVisible("SP_OLD", true, 0)
	
	mSendLogWin = g_WinMgr:createWindow("NewEQHomeLook/NewMultiLineBox", "mSendLogWin")
	mSendLogWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, 330), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_houseInfo.sendGiftLog):addChildWindow(mSendLogWin)
	EQGlobaFunc.toMultiLineBox(mSendLogWin:getName()):setFont("simsun-10")
	
	mRecLogWin = g_WinMgr:createWindow("NewEQHomeLook/NewMultiLineBox", "mRecLogWin")
	mRecLogWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, 330), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_houseInfo.recGiftLog):addChildWindow(mRecLogWin)
	EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):setFont("simsun-10")
	mRecLogWin:subscribeEvent("HyperLink_MouseClick", "DoShowLogGift")
	
	g_WinMgr:getWindow(g_houseInfo.clearSendLog):subscribeEvent("Clicked", "ReqCleanGiftLog")
	g_WinMgr:getWindow(g_houseInfo.clearRecLog):subscribeEvent("Clicked", "ReqCleanGiftLog")
	
	_InitComboBox()
end
-----------------------------------------------------------------

-----------------------------------------------------------------
--请求重置欢迎语
local function _ReqSetWelcomeWord()
	if true == mWelWordsChangeFlag then
		local welcomeWord = g_WinMgr:getWindow(g_houseInfo.welcomeTxt):getText()
		g_smallModules:reqSetHouseWelWords(welcomeWord)
	else
		CloseHouseInfoUI()
	end
end
--------------------------------------------------------------
--请求设置房屋权限
local function _ReqSetPerMission()
	if true == mRadioChangeFlag then
		local accessPermission, furPermission
		local window = g_WinMgr:getWindow(g_houseInfo.useFur)
		local useFurniture = CEGUI.toCheckbox(window):isSelected()

		window = g_WinMgr:getWindow(g_houseInfo.accessY)
		local access = CEGUI.toRadioButton(window):isSelected()


		if false == access then
			accessPermission = 2
		else
			accessPermission = 1
		end

		if false == useFurniture then
			furPermission = 2
		else
			furPermission = 1
		end
		g_smallModules:reqSetHousePermission(furPermission, accessPermission)
	else
		CloseHouseInfoUI()
	end
end
-----------------------------------------------------------------
local function _ClearAccessRecords()
	EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):clearUp()
end
-----------------------------------------------------------------
--请求房屋基本信息
local function _ReqHouseBaseInfo()
	_ClearAccessRecords()
	g_smallModules:reqHouseBaseInfo()
end
-----------------------------------------------------------------
function ReqClearHouseLog()
	--C++代码分离中，暂时于此清空访问记录
	_ClearAccessRecords()
	g_smallModules:reqClearHouseLog()
end
-----------------------------------------------------------------
function PopHouseInfoUI()
	if false == g_houseInfoFlag then
		ReqOpenHouseInfoUI()
		
		UIM_DoOpenAndCloseUI(g_houseInfo.root)
	else
		CloseHouseInfoUI()
	end
end
-----------------------------------------------------------------
function ReqOpenHouseInfoUI()
	_ReqHouseBaseInfo()
	ReqHouseList()
end
function OpenHouseInfoUI()
	UIM_OpenWindow(g_houseInfo.root)
	g_houseInfoFlag = true
end
function CloseHouseInfoUI()
	UIM_CloseWindow(g_houseInfo.root)
	g_houseInfoFlag = false
end
-----------------------------------------------------------------
function SaveHouseLog(time, acc, type, itemId)
	time = tostring(time)
	EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", time, "#5C96FF", true)
	EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", acc.." ", "#59d346", false)
	
	if mHVTType.visit == type then
		EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", "访问了你的房间", "#00FFFC", false)
	elseif mHVTType.leave == type then
		EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", "离开了你的房间", "#5C96FF", false)
	elseif mHVTType.pick_garbage == type then
		EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", "帮你清理了垃圾", "#5CA7FF", false)
		local itemRow = g_itemtableMgr:getItemRow(itemId)
		if nil ~= itemRow then
			EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", ",并获得了", "#5CA7FF", false)
			EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", itemRow.name, "#FF0000", false)
			EQGlobaFunc.toMultiLineBox(mVisitedLogWnd:getName()):addElementText("simsun-10", "物品", "#5CA7FF", false)
		end
	end
end
-----------------------------------------------------------------
function SetHouseBaseInfo(accessPermission, furPermission, level, maxPlayer, type, price, clean)
	if false == g_houseInfoFlag then
		mMsgBuffer = ""
		FSetText(g_houseInfo.log, mMsgBuffer)
		CEGUI.toTabControl(g_WinMgr:getWindow(g_houseInfo.tabName)):setSelectedTab(0)
	
		OpenHouseInfoUI()
		
		_InitGiftLog()
		InitHouseListUI()
	end

	if 1 == accessPermission then
		CEGUI.toRadioButton(g_WinMgr:getWindow(g_houseInfo.accessN)):setSelected(false)
		CEGUI.toRadioButton(g_WinMgr:getWindow(g_houseInfo.accessY)):setSelected(true)
	else
		CEGUI.toRadioButton(g_WinMgr:getWindow(g_houseInfo.accessY)):setSelected(false)
		CEGUI.toRadioButton(g_WinMgr:getWindow(g_houseInfo.accessN)):setSelected(true)
	end

	if 1 == furPermission then
		CEGUI.toCheckbox(g_WinMgr:getWindow(g_houseInfo.useFur)):setSelected(true)
	else
		CEGUI.toCheckbox(g_WinMgr:getWindow(g_houseInfo.useFur)):setSelected(false)
	end

	mRadioChangeFlag = false
	mWelWordsChangeFlag = false

	--等级 名称 类型 价格 人数上限 清洁度
	g_WinMgr:getWindow(g_houseInfo.level):setText(level)
	g_WinMgr:getWindow(g_houseInfo.contain):setText(maxPlayer)
	g_WinMgr:getWindow(g_houseInfo.price):setText(price)
	FSetText(g_houseInfo.type, mHouseType[type])
	HouseInfo_setClean(clean)
end
-----------------------------------------------------------------
function HouseInfo_setClean(clean)
	g_WinMgr:getWindow(g_houseInfo.clean):setText(clean.."/100")
	if clean < 30 then
		PlayerGuide_do_NewerTeachTaskScript(15013) --新手教学任务(房屋清洁度低到30以下)
	end
end
-----------------------------------------------------------------
function OnSureButton()
--0 为基本信息页  1为权限设置 2为访问日志
	local index = GetCurTabIndex(g_houseInfo.tabName)
	if 0 == index then
		_ReqSetWelcomeWord()
	elseif 1 == index then
		_ReqSetPerMission()
	else
		CloseHouseInfoUI()
	end
end
-----------------------------------------------------------------
function OnStateChanged()
	mRadioChangeFlag = true
end
-----------------------------------------------------------------
function OnWelWordsChanged()
	mWelWordsChangeFlag = true
end
-----------------------------------------------------------------

-----------------------------------------------------------------
----------------------------礼物记录显示-------------------------
-----------------------------------------------------------------

-----------------------------------------------------------------
function _ReqLog(pageNum)
	--清空记录
	if EQGlobaClass.GiftLogManager.MAIL_SEND == mCurType or EQGlobaClass.GiftLogManager.DIY_SEND == mCurType then
		CleanSendGiftLogUI()
	else
		CleanRecGiftLogUI()
	end
	
	g_smallModules:reqGiftLog(mCurType, pageNum)
end		
-----------------------------------------------------------------	 
--设置送出礼物记录
function SetSendGiftLog(reciverName, time, index, count)	
	if EQGlobaClass.GiftLogManager.DIY_SEND ==  mCurType then
	--手工
		EQGlobaFunc.toMultiLineBox(mSendLogWin:getName()):addElementText(time.." 你给", "#00FFFC", true)
		EQGlobaFunc.toMultiLineBox(mSendLogWin:getName()):addElementText(reciverName, "#ff0000", false)
		EQGlobaFunc.toMultiLineBox(mSendLogWin:getName()):addElementText("送了一份礼物！", "#00FFFC", false)
	else
	--快递
		EQGlobaFunc.toMultiLineBox(mSendLogWin:getName()):addElementText(time.." 成功发送了一份礼物给", "#00FFFC", true)
		EQGlobaFunc.toMultiLineBox(mSendLogWin:getName()):addElementText(reciverName, "#ff0000", false)
	end
	
	if count == index then
		--g_Loger:logMessage("setTotalPageText")
		EQGlobaFunc.SetText(g_houseInfo.sendLogPageText, mCurLogPage.."/"..mTotalPage[mCurType])
	end
end
-----------------------------------------------------------------
--设置收到礼物日志
function SetReciveGiftLog(reciverName, time, index, count, idString)

	if EQGlobaClass.GiftLogManager.DIY_RECIVE_VIEW == mCurType then
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(time.." ", "#00FFFC", true)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(reciverName, "#ff0000", false)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText("送给你的礼物（已查收）！", "#00FFFC", false)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementHyperLink("点击查看礼物", idString, "#00FFFC", false)
		
	elseif EQGlobaClass.GiftLogManager.DIY_RECIVE_NO_VIEW == mCurType then
	
		if "" ~= reciverName then
			EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(time.." ", "#00FFFC", true)
			EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(reciverName, "#ff0000", false)
			EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText("送给你一份礼物，快回家查看吧！", "#00FFFC", false)
		else
			EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(time.." 你收到了一份神秘礼物", "#00FFFC", true)
		end
		
	elseif EQGlobaClass.GiftLogManager.MAIL_RECIVE_VIEW == mCurType then
	
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(time.." ", "#00FFFC", true)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(reciverName, "#ff0000", false)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText("送给你的礼物（已查收）", "#00FFFC", false)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementHyperLink("点击查看礼物", idString, "#00FFFC", false)
	
	elseif EQGlobaClass.GiftLogManager.MAIL_RECIVE_NO_VIEW == mCurType then
	
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(time.." 收到了来自", "#00FFFC", true)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText(reciverName, "#ff0000", false)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementText("的一份礼物，", "#00FFFC", false)
		EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):addElementHyperLink("点击查看", idString, "#00FFFC", false)
		
	end
	
	if count == index then
		EQGlobaFunc.SetText(g_houseInfo.recLogPageText, mCurLogPage.."/"..mTotalPage[mCurType])
	end
	
end

function OnStartAddUnViewGift()
	mUnViewMailGiftIds = {}
end

function OnAddUnViewGift(gid)
	table.insert(mUnViewMailGiftIds, gid)
	--g_Loger:logError("Inset gift id:"..gid)
end

function OnEndAddUnViewGift()
	if table.getn(mUnViewMailGiftIds) == 0 then
		g_WinMgr:getWindow(mMailGiftIconName):setVisible(false)
	else
		g_WinMgr:getWindow(mMailGiftIconName):setVisible(true)
	end
	
	--g_Loger:logError("Gift count:"..tostring(table.getn(mUnViewMailGiftIds)))
end

function SetUnViewMailGiftUIVisible(visible)
	if false == visible then
		g_WinMgr:getWindow(mMailGiftIconName):setVisible(visible)
	else
		if table.getn(mUnViewMailGiftIds) > 0 then
			g_WinMgr:getWindow(mMailGiftIconName):setVisible(visible)
		end
	end
end

function OnDelUnViewGift()
	if nil == g_OpenMailGiftId then
		return
	end
	
	local i
	for i = 1, table.getn(mUnViewMailGiftIds) do
		if mUnViewMailGiftIds[i] == g_OpenMailGiftId then
			table.remove(mUnViewMailGiftIds, i)
			g_smallModules:delLogGiftByIcon(g_OpenMailGiftId)
			OnEndAddUnViewGift()
			break
		end
	end
	
	g_OpenMailGiftId = nil
end

function OnOpenUnViewGift(args)
	if 0 == table.getn(mUnViewMailGiftIds) then
		--g_Loger:logError("Un view mail gift is empty!")
		return
	end
	
	local ids = mUnViewMailGiftIds[1]
	g_OpenMailGiftId = ids;
	mCurType = EQGlobaClass.GiftLogManager.MAIL_RECIVE_NO_VIEW
	g_smallModules:showLogGiftByIcon(ids)
end
-----------------------------------------------------------------
function SetGiftLogTotalPage(type, totalPage)
	if 0 > totalPage then
		--g_Loger:logError("totalPage is no valid in SetGiftLogTotalPage, HouseInfo.lua")
	end
	
	if 0 == totalPage then
		totalPage = 1
	end
	
	mTotalPage[mCurType] = totalPage
end
-----------------------------------------------------------------
function NextPageGiftLog()
	if nil == mTotalPage[mCurType] then
		--g_Loger:logError("mTotalPage[mCurType] is nil in Houseinfo.lua mCurType = "..mCurType)
		return
	end
	
	if mCurLogPage >= mTotalPage[mCurType] then
		return
	end
	
	mCurLogPage = mCurLogPage + 1
	_ReqLog(mCurLogPage)
end
-----------------------------------------------------------------
function PrePageGiftLog()
	if 1 >= mCurLogPage then
		return
	end
	
	mCurLogPage = mCurLogPage - 1
	_ReqLog(mCurLogPage)
end
-----------------------------------------------------------------
function SelectSendLogType()
	local win = g_WinMgr:getWindow(g_houseInfo.sendLogComboBox)
	local selectedItemId = CEGUI.toCombobox(win):getSelectedItem():getID()
	
	mCurType = selectedItemId
	mCurLogPage = 1
	
	EQGlobaFunc.SetText(g_houseInfo.sendLogTip, mGiftLogTypeTips[mCurType])

	_ReqLog(mCurLogPage)
end
-----------------------------------------------------------------
function SelectRecLogType()
	local win = g_WinMgr:getWindow(g_houseInfo.recLogComboBox)
	local selectedItemId = CEGUI.toCombobox(win):getSelectedItem():getID()
	
	mCurType = selectedItemId
	mCurLogPage = 1
	
	EQGlobaFunc.SetText(g_houseInfo.recLogTip, mGiftLogTypeTips[mCurType])
	
	_ReqLog(mCurLogPage)
end
-----------------------------------------------------------------
function DoShowLogGift(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local id = EQGlobaFunc.toMultiLineBox(win:getName()):getHyperLinkId()
	g_OpenMailGiftId = id;
	g_smallModules:showLogGift(id)
end
-----------------------------------------------------------------
function ShowLogGift(greetings, giverName, itemTemplateId, effecTime, overLay, cardId, giverAcc, expressType)
	--该函数调用的界面都在GiftSystem中加载显示
	if EQGlobaClass.GiftLogManager.DIY_RECIVE_VIEW == mCurType then
		SetDIYGiftUIFlag(true)
		SetDIYGiftUI(giverName, greetings, itemTemplateId, effecTime, overLay, cardId, false)
		
		--g_Loger:logError("DIY_RECIVE_VIEW")
	elseif EQGlobaClass.GiftLogManager.DIY_RECIVE_NO_VIEW == mCurType then
		SetDIYGiftUIFlag(true)
		SetDIYGiftUI(giverName, greetings, itemTemplateId, effecTime, overLay, cardId, true)
		
		--g_Loger:logError("DIY_RECIVE_NO_VIEW")
	elseif EQGlobaClass.GiftLogManager.MAIL_RECIVE_VIEW == mCurType then
		SetMailGiftUI(greetings, giverName, itemTemplateId, effecTime, overLay, cardId, giverAcc, expressType, false)
		--设置可回复的帐号
		mReplyAcc = giverAcc
		
		--g_Loger:logError("MAIL_RECIVE_VIEW")
	elseif EQGlobaClass.GiftLogManager.MAIL_RECIVE_NO_VIEW == mCurType then
		SetMailGiftUI(greetings, giverName, itemTemplateId, effecTime, overLay, cardId, giverAcc, expressType, true)		
		--设置可回复的帐号
		mReplyAcc = giverAcc
		
		--g_Loger:logError("MAIL_RECIVE_NO_VIEW")
	end
end
-----------------------------------------------------------------
function GetReplyMailGiftAcc()
	return mReplyAcc
end
-----------------------------------------------------------------
function GetGiftLogCurPage()
	return mCurLogPage
end
-----------------------------------------------------------------
function ReqCleanGiftLog()

	if EQGlobaClass.GiftLogManager.NIL == mCurType	then
		return
	end

	if EQGlobaClass.GiftLogManager.DIY_SEND == mCurType or EQGlobaClass.GiftLogManager.MAIL_SEND == mCurType then
		CleanSendGiftLogUI()
	else
		CleanRecGiftLogUI()
	end
	
	g_smallModules:reqCleanGiftLog(mCurType)
end
-----------------------------------------------------------------
--清空收礼记录
function CleanRecGiftLogUI()
	EQGlobaFunc.toMultiLineBox(mRecLogWin:getName()):clearUp()
	EQGlobaFunc.SetText(g_houseInfo.recLogPageText, "")
end
-----------------------------------------------------------------
--清空送礼记录
function CleanSendGiftLogUI()
	EQGlobaFunc.toMultiLineBox(mSendLogWin:getName()):clearUp()
	EQGlobaFunc.SetText(g_houseInfo.sendLogPageText, "")
end

-------------------------------------------------------------------------------
--房屋管理相关操作-------------------------------------------------------------
-------------------------------------------------------------------------------
--请求房屋列表
function ReqHouseList()
	g_smallModules:reqHouseList()
end

--收到房屋列表数据通知
function OnUpdateHouseList()
	InitHouseListUI()
	RefreshHouseListUI()
end

--点击按钮切换房屋
function OnClickToChangeHouse(args)
	local wname = CEGUI.toWindowEventArgs(args).window:getName()
	
	--g_Loger:logMessage("name:"..wname)
	
	if nil == mHouseDataList[wname] then
		return
	end
	
	--g_Loger:logMessage("index:"..tostring(mHouseDataList[wname].idx))
	--g_Loger:logMessage("state:"..tostring(mHouseDataList[wname].state))
	
	if mHouseUseState.yes == mHouseDataList[wname].state then
		return
	end
	
	--保存要切换的房屋索引
	mHouseChangeIdx = mHouseDataList[wname].idx - 1
	-- (modify by hezhr)
	local ckblist = {{text = "沿用房屋权限设置（包括欢迎语）", state = true}}
	--默认为yes
	mHouseChangeType = mHouseChangeTypeDef.yes
	m_checkBoxNameTable = MsgBox_createMsgBoxExA(g_MainUIWindows,300,"","切换房屋后房间内的数据将被封存起来，是否继续？", ckblist, "ReqChangeHouse", "MsgBox_cancelFunc")
end

--初使化房屋界面内容
function InitHouseListUI()
	local i, j
	for i = 1, 2 do
		for j = 1, 7 do
			local txtName = "housedata/tab1/Tab 7/house"..i.."/txt"..j
			local lbName = "housedata/tab1/Tab 7/house"..i.."/lb"..j
			EQGlobaFunc.SetText(txtName, "")
			
			g_WinMgr:getWindow(lbName):setVisible(true)
			g_WinMgr:getWindow(txtName):setVisible(true)
		end
		
		local btnUseName = "housedata/tab1/Tab 7/house"..i.."/lb1"
		g_WinMgr:getWindow(btnUseName):removeAllEvents()
		g_WinMgr:getWindow(btnUseName):subscribeEvent("Clicked", "OnClickToChangeHouse")
		g_WinMgr:getWindow(btnUseName):enable()
		mHouseDataList[btnUseName] = nil
	end
end

--刷新房屋界面内容
function RefreshHouseListUI()
	local i, j
	local size = g_smallModules:getHouseListSize()
	
	for i = 1, size do
		local btnUseName = "housedata/tab1/Tab 7/house"..i.."/lb1"
		local lbUseName = "housedata/tab1/Tab 7/house"..i.."/txt1"
		
		local item = g_smallModules:getHouseListData(i - 1)
		if 0 == item.empty then
			mHouseDataList[btnUseName] = { idx = i, state = mHouseUseState.no }
			if 0 ~= item.used then
				EQGlobaFunc.SetText(lbUseName, "使用中")
				g_WinMgr:getWindow(lbUseName):setProperty("TextColours", "tl:FF0FE10D tr:FF0FE10D bl:FF0FE10D br:FF0FE10D")
				mHouseDataList[btnUseName].state = mHouseUseState.yes
				g_WinMgr:getWindow(btnUseName):disable()
			else
				EQGlobaFunc.SetText(lbUseName, "库存中")
				g_WinMgr:getWindow(lbUseName):setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000")
			end
			
			EQGlobaFunc.SetText("housedata/tab1/Tab 7/house"..i.."/txt2", item.name)
			EQGlobaFunc.SetText("housedata/tab1/Tab 7/house"..i.."/txt3", tostring(item.level))
			EQGlobaFunc.SetText("housedata/tab1/Tab 7/house"..i.."/txt4", tostring(item.price))
			EQGlobaFunc.SetText("housedata/tab1/Tab 7/house"..i.."/txt5", tostring(item.max_player))
			EQGlobaFunc.SetText("housedata/tab1/Tab 7/house"..i.."/txt6", mHouseType[item.type])
			EQGlobaFunc.SetText("housedata/tab1/Tab 7/house"..i.."/txt7", item.buy_time)
		else
			EQGlobaFunc.SetText("housedata/tab1/Tab 7/house"..i, "房屋购买权")
			
			for j = 1, 7 do
				local txtName = "housedata/tab1/Tab 7/house"..i.."/txt"..j
				local lbName = "housedata/tab1/Tab 7/house"..i.."/lb"..j
				
				g_WinMgr:getWindow(lbName):setVisible(false)
				g_WinMgr:getWindow(txtName):setVisible(false)
			end
		end
	end
end

--请求更换房屋
function ReqChangeHouse()
	-- (add by hezhr)
	if CEGUI.toCheckbox(g_WinMgr:getWindow(m_checkBoxNameTable[1])):isSelected() then
		mHouseChangeType = mHouseChangeTypeDef.yes
	else
		mHouseChangeType = mHouseChangeTypeDef.no
	end
	g_smallModules:reqChangeHouse(mHouseChangeIdx, mHouseChangeType)
	_ReqHouseBaseInfo()
	ReqHouseList()
end

--收到更改房屋通知
function OnChangeHouse()
	InitHouseListUI()
	RefreshHouseListUI()
end

-------------------------------------------------------------------------------
--进入好友房间相关操作-------------------------------------------------------------
-------------------------------------------------------------------------------
g_WinMgr:getWindow("gotohouse/queding"):subscribeEvent("Clicked", "OnGotoFriendHouse")
g_WinMgr:getWindow("gotohouse/yulan"):subscribeEvent("Clicked", "OnPreviewFriendHouse")
g_WinMgr:getWindow("gotohouse/quxiao"):subscribeEvent("Clicked", "OnCancelToFriendHouse")
g_WinMgr:getWindow("gotohouse/fanye1"):subscribeEvent("Clicked", "OnBack2House")
g_WinMgr:getWindow("gotohouse/fanye11"):subscribeEvent("Clicked", "OnNext2House")
g_WinMgr:getWindow("gotohouse/house1"):subscribeEvent("MouseClick", "OnSelFriendHouseClick")
g_WinMgr:getWindow("gotohouse/house1"):subscribeEvent("MouseDoubleClick", "OnSelFriendHouseDbClick")
g_WinMgr:getWindow("gotohouse/house2"):subscribeEvent("MouseClick", "OnSelFriendHouseClick")
g_WinMgr:getWindow("gotohouse/house2"):subscribeEvent("MouseDoubleClick", "OnSelFriendHouseDbClick")

MainUI_ShieldClick(g_goToFriendHouseUI)

function OnGotoFriendHouse(args)
	ToFriendHouse(mHouseSelState.used)
end

function OnPreviewFriendHouse(args)
	ToFriendHouse(mHouseSelState.unused)
end

function OnCancelToFriendHouse(args)
	CloseSelHouseListUI()
end

function OnBack2House(args)
end

function OnNext2House(args)
end

function OnSelFriendHouseClick(args)
	mHouseSelItemName = CEGUI.toWindowEventArgs(args).window:getName()
	
	ResetActiveSelItem()
	CEGUI.toWindowEventArgs(args).window:setProperty("BackgroundEnabled", "true")
	
	ResetToBtnState()
end

function OnSelFriendHouseDbClick(args)
	ToFriendHouse(nil)
	g_eqoisMgr:setSkip(true)
end

function ToFriendHouse(expectState)
	if nil == mHouseSelList[mHouseSelItemName] then
		return
	end
	
	local reqType = nil
	
	if nil == expectState then
		reqType = mHouseSelList[mHouseSelItemName].state
	else
		if mHouseSelState.used == mHouseSelList[mHouseSelItemName].state and mHouseSelState.used == expectState then
			reqType = mHouseSelState.used
		elseif mHouseSelState.unused == mHouseSelList[mHouseSelItemName].state and mHouseSelState.unused == expectState then
			reqType = mHouseSelState.unused
		end
	end
	
	if mHouseSelState.used == reqType then
		if g_ToHouseFromType == g_ToHouseFromDef.friendList then
			g_FriendSys:reqInviteSelFriendHouse()
		elseif g_ToHouseFromType == g_ToHouseFromDef.circleMenu then
			g_FriendSys:reqEnterFriendHouseByLua()
		end
		CloseSelHouseListUI()
	elseif mHouseSelState.unused == reqType then
		if g_ToHouseFromType == g_ToHouseFromDef.friendList then
			g_FriendSys:reqPreviewFHousedataByFriendList(mHouseSelList[mHouseSelItemName].idx)
			CloseFriendListUI()
		elseif g_ToHouseFromType == g_ToHouseFromDef.circleMenu then
			g_FriendSys:reqPreviewFHousedataByCircleMenu(mHouseSelList[mHouseSelItemName].idx)
		end
		CloseSelHouseListUI()
	end
end

function ResetActiveSelItem()
	local i
	
	for i = 1, mHouseSelSize do
		local lbHousePanel = "gotohouse/house"..i
		
		g_WinMgr:getWindow(lbHousePanel):setProperty("BackgroundEnabled", "false")
	end
end

function ResetToBtnState()
	local toBtn = g_WinMgr:getWindow("gotohouse/queding")
	local pvBtn = g_WinMgr:getWindow("gotohouse/yulan")
	
	toBtn:disable()
	pvBtn:disable()
	
	if nil == mHouseSelList[mHouseSelItemName] then
		return
	end
	
	if mHouseSelState.used == mHouseSelList[mHouseSelItemName].state then
		toBtn:enable()
	elseif mHouseSelState.unused == mHouseSelList[mHouseSelItemName].state then
		pvBtn:enable()
	end
end

function InitSelHouseListUI()
	local i
	
	for i = 1, mHouseSelSize do
		local lbHouseName = "gotohouse/house"..i.."/mingcheng"
		local lbHouseState = "gotohouse/house"..i.."/zhuangtai"
		local lbHousePanel = "gotohouse/house"..i
		
		EQGlobaFunc.SetText(lbHouseName, "")
		EQGlobaFunc.SetText(lbHouseState, "")
		
		g_WinMgr:getWindow(lbHousePanel):setProperty("BackgroundEnabled", "false")
		g_WinMgr:getWindow(lbHouseName):setProperty("MousePassThroughEnabled", "True")
		g_WinMgr:getWindow(lbHouseState):setProperty("MousePassThroughEnabled", "True")
		
		mHouseSelList[lbHousePanel] = nil
		mHouseSelItemName = ""
	end
	
	g_WinMgr:getWindow("gotohouse/queding"):enable()
	g_WinMgr:getWindow("gotohouse/yulan"):disable()
end

function UpdateSelHouseListUI(startIdx)
	local i
	local idx
	local size = g_smallModules:getFHouseListSize()
	
	for i = 1, mHouseSelSize do
		local lbHouseName = "gotohouse/house"..i.."/mingcheng"
		local lbHouseState = "gotohouse/house"..i.."/zhuangtai"
		local lbHousePanel = "gotohouse/house"..i
		
		idx = i - 1 + startIdx
		
		if idx >= size then
			break
		end
		
		local item = g_smallModules:getFHouseListData(idx)
		if 0 == item.empty then
			mHouseSelList[lbHousePanel] = { idx = idx, state = mHouseSelState.none }
			if 0 ~= item.used then
				EQGlobaFunc.SetText(lbHouseState, "使用中")
				g_WinMgr:getWindow(lbHouseState):setProperty("TextColours", "tl:FF00FFFC tr:FF00FFFC bl:FF00FFFC br:FF00FFFC")
				g_WinMgr:getWindow(lbHouseName):setProperty("TextColours", "tl:FF00FFFC tr:FF00FFFC bl:FF00FFFC br:FF00FFFC")
				g_WinMgr:getWindow(lbHousePanel):setProperty("BackgroundEnabled", "true")
				mHouseSelList[lbHousePanel].state = mHouseSelState.used
				mHouseSelItemName = lbHousePanel
			else
				EQGlobaFunc.SetText(lbHouseState, "库存中")
				g_WinMgr:getWindow(lbHouseState):setProperty("TextColours", "tl:FFB50B20 tr:FFB50B20 bl:FFB50B20 br:FFB50B20")
				g_WinMgr:getWindow(lbHouseName):setProperty("TextColours", "tl:FFB50B20 tr:FFB50B20 bl:FFB50B20 br:FFB50B20")
				mHouseSelList[lbHousePanel].state = mHouseSelState.unused
			end
			EQGlobaFunc.SetText(lbHouseName, item.name)
		end
	end
end

function RefreshSelHouseListUI(startIdx)
	InitSelHouseListUI()
	UpdateSelHouseListUI(startIdx)
end

function OpenSelHouseListUI()
	g_MainUIWindows:addChildWindow(g_goToFriendHouseUI)
	Common_setModalState(g_goToFriendHouseUI:getName(), true)
end

function OpenAndRefreshSelHouseListUI(startIdx)
	RefreshSelHouseListUI(startIdx)
	OpenSelHouseListUI()
end

function CloseSelHouseListUI()
	g_MainUIWindows:removeChildWindow(g_goToFriendHouseUI)
	Common_setModalState(g_goToFriendHouseUI:getName(), false)
end

function OnUpdateFriendHouseList(args)
	if g_smallModules:getFHouseSize() > 1 then
		OpenAndRefreshSelHouseListUI(0)
	else
		if g_ToHouseFromType == g_ToHouseFromDef.friendList then
			g_FriendSys:reqInviteSelFriendHouse()
		elseif g_ToHouseFromType == g_ToHouseFromDef.circleMenu then
			g_FriendSys:reqEnterFriendHouseByLua()
		end
	end
end

function OnClickToFriendHouseFromFriendList(args)
	if DBS_getDropingFlag() or DBS_getPickingFlag() then
		DBS_cantChangeSceneTip()
	else
		g_FriendSys:reqHouseListFromFriendList()
		g_ToHouseFromType = g_ToHouseFromDef.friendList
	end
end
----------------------------------------------------
--
MainUI_ShieldClick(g_magicBoxPickUI)
g_WinMgr:getWindow("NewStaticImage2/queding"):subscribeEvent("Clicked", "MsgBox_cancelFunc")
MainUI_ShieldClick(g_garbagePickUI)
g_WinMgr:getWindow("NewStaticImage2/queding1"):subscribeEvent("Clicked", "MsgBox_cancelFunc")
function HandlePickMagicBox(type, itemId, money)
	if 0 ~= money then
		EQGlobaClass.getDynamicUIMgr():createNumberPaoPao(g_mainplayer, 17, money)
		g_mainplayer:addParticle(1018)
	end
	
	local itemRow = g_itemtableMgr:getItemRow(itemId)
	if nil ~= itemRow then
		local w = g_magicBoxPickUI
		local iw = g_WinMgr:getWindow("NewStaticImage2/ICON")
		
		if 2 == type then
			w = g_garbagePickUI
			iw = g_WinMgr:getWindow("NewStaticImage2/ICON1")
		end
		iw:setProperty("Image", "set:" ..itemRow.imageset .." image:" ..itemRow.icon)
		g_MainUIWindows:addChildWindow(w)
		Common_setModalState(w:getName(), true)
		
		--播放抓取音效
		EQGlobaClass.getVoiceMgr():play(itemRow.uvoiceid, false)
		--EQGlobaClass.getVoiceMgr():play(1, false)
	end
	if 2 == type then
		do_task_script("script31")
	else
		do_task_script("script32")
	end
	--g_Loger:logError("type:"..tostring(type).." itemId:"..tostring(itemId).." money:"..tostring(money))
end