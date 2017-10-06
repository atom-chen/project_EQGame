-----------------------------------------------------------------
--礼物系统
--xiaoMing
--2010-12-02
-----------------------------------------------------------------
local MakeGiftType = {
					[1]	= "DIY",
					[2] = "Mail",
						}

------------------------------------------------------------------
--数据成员

--送礼类型
local mMakeGiftType

--礼盒类型
local mBoxType = {
				["free"] = 1,
				["fine"] = 2,	 --精美
				["funny"] = 3,   --搞怪
				["uniquely"] = 4,--无双	
					}
--礼盒数据
local mBoxData = {
				[mBoxType.free] = {},
				[mBoxType.fine] = {},
				[mBoxType.funny] = {},
				[mBoxType.uniquely] = {},
					}
--礼卡类型
local mCardType = {
				["free"] = 1,
				["fine"] = 2,	 --精美
				["funny"] = 3,   --搞怪
				["uniquely"] = 4,--无双	
					}
--礼卡数据
local mCardData = {
				[mCardType.free] = {},
				[mCardType.fine] = {},
				[mCardType.funny] = {},
				[mCardType.uniquely] = {},
					}
--礼物的类型					
local mGiftType = {
				["DIY"] = 1,
				["Mail"] = 2,
				}
--当前打包礼物的类型，快递OR手工
local mCurPackGiftType
			
local mOffset --离第一个礼物盒或礼卡的偏移
local mBoxSlotSize = 7
local mCardSlotSize = 7
local mChooseBoxId 
local mChooseCardId
local mStorageGlobaIndex
--图文混排窗口(超链接)
local mPutGiftHyperLink
local mMailGiftHyperLink
local mDIYGiftHyperLink
--赠言窗口
local mPutGiftGreeting
local mDIYGiftGreeting
local mMailGiftGreeting
--显示手工制作的礼物（礼盒）界面被使用于点击
--地上的礼物和点击已收礼盒超链接时显示，此时
--由于触发显示的途径不同，造成取消按钮必须响应
--不同事件。故用此标识，为真表示正此时由超链接
--触发.
local mIsShowLogGift = false
--收礼礼物物品模板id
local mPutGiftItemId = 0
local mDIYGiftItemId = 0
local mMailGiftItemId = 0
--收礼礼物物品时效
local mPutGiftItemEffecTime = ""
local mDIYGiftItemEffecTime = ""
local mMailGiftItemEffecTime = ""
--itemhint用到(hezhr)
local m_diyGiftItemId = 0	-- DIY礼物物品的id
local m_diyGiftBoxId = 0	-- DIY礼物盒子id
local m_diyGiftCardId = 0	-- DIY礼物卡id
local m_mailGiftItemId = 0	-- Mail礼物物品的id
local m_mailGiftCardId = 0	-- Mail礼物卡id

local mFListName = "showRecverSearchResult1"
local mFListWindow = nil
local mRecvManEditName = "lwchuangkou3/chuangkou2/mingzishuru"
local mRecvManEditActive = false
local mFListWindowActive = false
local mTxtChangedBySelFriend = false
local mShowFListName = "showFListButton1"
local mShowFListBtn = nil
local mGSDataTimeName = "GiftSystemDataTime"
g_gift_express_type =
{
	gm = 1,
	player = 2
};
g_DoSelectGiftRecver = false
------------------------------------------------------------------
function GiftSys_setWinActive(args)
	local Win = CEGUI.toWindowEventArgs(args).window
	Win:activate()
end
------------------------------------------------------------------
function OnLoadGiftSystem()
	g_chooseMakeGiftUI:subscribeEvent("MouseClick", "GiftSys_setWinActive")
	g_PutGiftUI:subscribeEvent("MouseClick", "GiftSys_setWinActive")
	g_OpenGiftUI:subscribeEvent("MouseClick", "GiftSys_setWinActive")
	g_LogGiftUI:subscribeEvent("MouseClick", "GiftSys_setWinActive")
	g_WinMgr:getWindow(g_giftUIName_choose.diyBtn):subscribeEvent("MouseClick", "MakeDIYGiftUI")
	g_WinMgr:getWindow(g_giftUIName_choose.mailBtn):subscribeEvent("MouseClick", "OpenMakeMailGiftUI")
	g_WinMgr:getWindow(g_giftUIName_choose.cancleBtn):subscribeEvent("Clicked", "ReqEndGiftService")
		
	g_WinMgr:getWindow(g_giftUIName_diy.closeBtn):subscribeEvent("Clicked", "ReqEndGiftService")
	g_WinMgr:getWindow(g_giftUIName_diy.cancleBtn):subscribeEvent("Clicked", "ReqEndGiftService")
	g_WinMgr:getWindow(g_giftUIName_mail.closeBtn):subscribeEvent("Clicked", "ReqEndGiftService")
	g_WinMgr:getWindow(g_giftUIName_mail.cancleBtn):subscribeEvent("Clicked", "ReqEndGiftService")	
	--注册打开选择礼盒界面（模态）
	g_WinMgr:getWindow(g_giftUIName_diy.boxSlot):subscribeEvent("MouseClick", "OpenGiftBoxUI")
	g_WinMgr:getWindow(g_giftUIName_diy.boxSlot):subscribeEvent("MouseEnter", "GiftSystem_MouseEnterBoxSlot")
	g_WinMgr:getWindow(g_giftUIName_diy.boxSlot):subscribeEvent("MouseLeave", "GiftSystem_DestroyItemhint")
	--选取物品
	g_WinMgr:getWindow(g_giftUIName_diy.itemSlot):subscribeEvent("MouseClick", "OpenChooseItemUI")
	g_WinMgr:getWindow(g_giftUIName_mail.itemSlot):subscribeEvent("MouseClick", "OpenChooseItemUI")
	g_WinMgr:getWindow(g_giftUIName_diy.itemSlot):subscribeEvent("MouseEnter", "GiftSystem_MouseEnterItemSlot")
	g_WinMgr:getWindow(g_giftUIName_mail.itemSlot):subscribeEvent("MouseEnter", "GiftSystem_MouseEnterItemSlot")
	g_WinMgr:getWindow(g_giftUIName_diy.itemSlot):subscribeEvent("MouseLeave", "GiftSystem_DestroyItemhint")
	g_WinMgr:getWindow(g_giftUIName_mail.itemSlot):subscribeEvent("MouseLeave", "GiftSystem_DestroyItemhint")	
	--选择礼物卡
	g_WinMgr:getWindow(g_giftUIName_diy.cardSlot):subscribeEvent("MouseClick", "OpenGiftCardUI")
	g_WinMgr:getWindow(g_giftUIName_diy.cardSlot):subscribeEvent("MouseEnter", "GiftSystem_MouseEnterCardSlot")
	g_WinMgr:getWindow(g_giftUIName_diy.cardSlot):subscribeEvent("MouseLeave", "GiftSystem_DestroyItemhint")	
	g_WinMgr:getWindow(g_giftUIName_mail.cardSlot):subscribeEvent("MouseClick", "OpenGiftCardUI")
	g_WinMgr:getWindow(g_giftUIName_mail.cardSlot):subscribeEvent("MouseEnter", "GiftSystem_MouseEnterCardSlot")
	g_WinMgr:getWindow(g_giftUIName_mail.cardSlot):subscribeEvent("MouseLeave", "GiftSystem_DestroyItemhint")		
	
	g_WinMgr:getWindow(g_giftBoxUIName.sureBtn):subscribeEvent("Clicked", "SureBox")
	g_WinMgr:getWindow(g_giftCardUIName.sureBtn):subscribeEvent("Clicked", "SureCard")
	
	g_WinMgr:getWindow(g_giftBoxUIName.closeBtn):subscribeEvent("Clicked", "CloseGiftBoxUI")
	g_WinMgr:getWindow(g_giftCardUIName.closeBtn):subscribeEvent("Clicked", "CloseGiftCardUI")
	
	--连续点击
	g_WinMgr:getWindow(g_giftBoxUIName.preBtn):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow(g_giftBoxUIName.preBtn):setAutoRepeatRate(0.1)

	g_WinMgr:getWindow(g_giftBoxUIName.nextBtn):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow(g_giftBoxUIName.nextBtn):setAutoRepeatRate(0.1)
	
	g_WinMgr:getWindow(g_giftCardUIName.preBtn):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow(g_giftCardUIName.preBtn):setAutoRepeatRate(0.1)

	g_WinMgr:getWindow(g_giftCardUIName.nextBtn):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow(g_giftCardUIName.nextBtn):setAutoRepeatRate(0.1)
	
	--屏蔽双击按键
	g_WinMgr:getWindow(g_giftBoxUIName.preBtn):setWantsMultiClickEvents(false)
	g_WinMgr:getWindow(g_giftBoxUIName.nextBtn):setWantsMultiClickEvents(false)
	
	g_WinMgr:getWindow(g_giftBoxUIName.preBtn):subscribeEvent("MouseButtonDown", "PreBox")
	g_WinMgr:getWindow(g_giftBoxUIName.nextBtn):subscribeEvent("MouseButtonDown", "NextBox")
	
	g_WinMgr:getWindow(g_giftCardUIName.preBtn):setWantsMultiClickEvents(false)
	g_WinMgr:getWindow(g_giftCardUIName.nextBtn):setWantsMultiClickEvents(false)
	
	g_WinMgr:getWindow(g_giftCardUIName.preBtn):subscribeEvent("MouseButtonDown", "PreGiftCard")
	g_WinMgr:getWindow(g_giftCardUIName.nextBtn):subscribeEvent("MouseButtonDown", "NextGiftCard")
	
	g_WinMgr:getWindow(g_giftUIName_diy.sureBtn):subscribeEvent("Clicked", "ReqPackGift")
	g_WinMgr:getWindow(g_giftUIName_mail.sureBtn):subscribeEvent("Clicked", "ReqMailGift")
	
	g_WinMgr:getWindow(g_putGiftUIName.putBtn):subscribeEvent("Clicked", "PutGift")
	g_WinMgr:getWindow(g_putGiftUIName.takeBackBtn):subscribeEvent("Clicked", "CanclePackGift")
	g_WinMgr:getWindow(g_putGiftUIName.cancleBtn):subscribeEvent("Clicked", "ReqEndGiftService")
	
	g_WinMgr:getWindow(g_MailGiftUIName.cancleBtn):subscribeEvent("Clicked", "CloseMailGiftUI")
	g_WinMgr:getWindow(g_MailGiftUIName.pickUpBtn):subscribeEvent("Clicked", "ReqStartReciveExpress")
	--回复快递礼物事件
	g_WinMgr:getWindow(g_MailGiftUIName.reply):subscribeEvent("Clicked", "ReplyMailGift")
	
	
	for i = 1, 4 do
		g_WinMgr:getWindow(g_giftBoxUIName["tab"..i]):subscribeEvent("Clicked", "ChooseBoxType")
	end
	
	for i = 1, 4 do
		g_WinMgr:getWindow(g_giftCardUIName["tab"..i]):subscribeEvent("Clicked", "ChooseCardType")
	end
	
	g_WinMgr:getWindow(g_DIYGiftUIName.pickUpBtn):subscribeEvent("Clicked", "ReqPickUpGift")
	g_WinMgr:getWindow(g_DIYGiftUIName.cancleBtn):subscribeEvent("Clicked", "OnCancleDIYGiftUI")
	
	--礼物选择物品仓库切换页面事件
	for i = 1, 4 do
		g_WinMgr:getWindow(g_GiftStorageUI["page"..i]):subscribeEvent("MouseClick", "ChangeGiftItemPage")
	end
	--选择礼物物品仓库格子鼠标点击/移进/移出事件
	for i = 1, C_STORAGE_GIRD_NUM do
		g_WinMgr:getWindow(g_GiftStorageUI.slotBaseName..i):subscribeEvent("MouseClick", "ChooseGiftItem")
		g_WinMgr:getWindow(g_GiftStorageUI.slotBaseName..i):removeEvent("MouseEnter")
		g_WinMgr:getWindow(g_GiftStorageUI.slotBaseName..i):removeEvent("MouseLeave")
		g_WinMgr:getWindow(g_GiftStorageUI.slotBaseName..i):subscribeEvent("MouseEnter", "SendGiftItem_MouseEnter")
		g_WinMgr:getWindow(g_GiftStorageUI.slotBaseName..i):subscribeEvent("MouseLeave", "GiftItem_MouseLeave")		
	end
	
	g_WinMgr:getWindow(g_GiftStorageUI.closeBtn):subscribeEvent("Clicked", "CloseChooseItemUI")
	
	--礼盒Slot点击事件
	for i = 1, mBoxSlotSize do
		g_WinMgr:getWindow(g_giftBoxUIName.slotBase..i):subscribeEvent("MouseClick", "ChooseGiftBox")
	end
	
	--礼卡Slot点击事件
	for i = 1, mCardSlotSize do
		g_WinMgr:getWindow(g_giftCardUIName.slotBase..i):subscribeEvent("MouseClick", "ChooseGiftCard")
	end
	
	--收礼界面礼物鼠标移进/移出事件
	g_WinMgr:getWindow(g_putGiftUIName.itemSlot):subscribeEvent("MouseEnter", "RecvGiftItem_MouseEnter")
	g_WinMgr:getWindow(g_putGiftUIName.itemSlot):subscribeEvent("MouseLeave", "GiftItem_MouseLeave")
	g_WinMgr:getWindow(g_DIYGiftUIName.itemSlot):subscribeEvent("MouseEnter", "RecvGiftItem_MouseEnter")
	g_WinMgr:getWindow(g_DIYGiftUIName.itemSlot):subscribeEvent("MouseLeave", "GiftItem_MouseLeave")
	g_WinMgr:getWindow(g_MailGiftUIName.itemSlot):subscribeEvent("MouseEnter", "RecvGiftItem_MouseEnter")
	g_WinMgr:getWindow(g_MailGiftUIName.itemSlot):subscribeEvent("MouseLeave", "GiftItem_MouseLeave")	
	
	--日期（临时替代）
	--CEGUI.toEditbox(g_WinMgr:getWindow(g_giftUIName_mail.year)):setValidationString("^$|^[0-9]*[1-9][0-9]*$")
	--CEGUI.toEditbox(g_WinMgr:getWindow(g_giftUIName_mail.month)):setValidationString("^$|^(0?[1-9]|1[0-2])$")
	--CEGUI.toEditbox(g_WinMgr:getWindow(g_giftUIName_mail.day)):setValidationString("^$|^((0?[1-9])|((1|2)[0-9])|30|31)$")
	--
	--图文混排 超链接	
	mPutGiftHyperLink = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mPutGiftHyperLink")
	mPutGiftHyperLink:setSize(CEGUI.UVector2(CEGUI.UDim(0, 330), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_putGiftUIName.hyperLinker):addChildWindow(mPutGiftHyperLink)
	EQGlobaFunc.toMultiLineBox(mPutGiftHyperLink:getName()):setFont("simsun-10")
	
	mMailGiftHyperLink = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mMailGiftHyperLink")
	mMailGiftHyperLink:setSize(CEGUI.UVector2(CEGUI.UDim(0, 330), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_MailGiftUIName.hyperLinker):addChildWindow(mMailGiftHyperLink)
	EQGlobaFunc.toMultiLineBox(mMailGiftHyperLink:getName()):setFont("simsun-10")
	
	mDIYGiftHyperLink = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mDIYGiftHyperLink")
	mDIYGiftHyperLink:setSize(CEGUI.UVector2(CEGUI.UDim(0, 330), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_DIYGiftUIName.hyperLinker):addChildWindow(mDIYGiftHyperLink)
	EQGlobaFunc.toMultiLineBox(mDIYGiftHyperLink:getName()):setFont("simsun-10")
	
	mDIYGiftHyperLink:subscribeEvent("HyperLink_MouseClick", "ShowGiftCard")
	mMailGiftHyperLink:subscribeEvent("HyperLink_MouseClick", "ShowGiftCard")
	mPutGiftHyperLink:subscribeEvent("HyperLink_MouseClick", "ShowGiftCard")
	
	mPutGiftGreeting = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mPutGiftGreeting")
	mPutGiftGreeting:setSize(CEGUI.UVector2(CEGUI.UDim(0, 355), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_putGiftUIName.greetings):addChildWindow(mPutGiftGreeting)
	mDIYGiftGreeting = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mDIYGiftGreeting")
	mDIYGiftGreeting:setSize(CEGUI.UVector2(CEGUI.UDim(0, 355), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_DIYGiftUIName.greetings):addChildWindow(mDIYGiftGreeting)
	mMailGiftGreeting = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mMailGiftGreeting")
	mMailGiftGreeting:setSize(CEGUI.UVector2(CEGUI.UDim(0, 355), CEGUI.UDim(0, 160)))
	g_WinMgr:getWindow(g_MailGiftUIName.greetings):addChildWindow(mMailGiftGreeting)
	
	
	--设置输入框输入长度限制
	CEGUI.toMultiLineEditbox(g_WinMgr:getWindow(g_giftUIName_diy.greetings)):setMaxTextLength(40)
	CEGUI.toMultiLineEditbox(g_WinMgr:getWindow(g_giftUIName_mail.greetings)):setMaxTextLength(40)
	
	CEGUI.toEditbox(g_WinMgr:getWindow(g_giftUIName_mail.reciverAcc)):setMaxTextLength(50)
	CEGUI.toEditbox(g_WinMgr:getWindow(g_giftUIName_diy.giftName)):setMaxTextLength(8)
	
	
	MainUI_ShieldClick(g_chooseMakeGiftUI)
	MainUI_ShieldClick(g_DIYGiftUI)
	MainUI_ShieldClick(g_GiftBoxUI)
	MainUI_ShieldClick(g_LogGiftUI)
	MainUI_ShieldClick(g_GiftItemUI)
	MainUI_ShieldClick(g_GiftCardUI)
	MainUI_ShieldClick(g_PutGiftUI)
	MainUI_ShieldClick(g_MailGiftUI)
	MainUI_ShieldClick(g_OpenGiftUI)
	
	--创建收件人列表框
	mFListWindow = g_WinMgr:createWindow("NewEQHomeLook/NewComboDropList",mFListName)
	mFListWindow:setVisible(false)
	mFListWindow:setPosition(CEGUI.UVector2(CEGUI.UDim(0,137), CEGUI.UDim(0,40)))
	mFListWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,196), CEGUI.UDim(0,140)))
	mFListWindow:setProperty("AlwaysOnTop", "true")
	g_WinMgr:getWindow("lwchuangkou3/chuangkou2"):addChildWindow(mFListWindow)
	g_WinMgr:getWindow(mRecvManEditName):subscribeEvent("TextChanged", "OnInputReceiveManChar1")
	g_WinMgr:getWindow(mRecvManEditName):subscribeEvent("Activated", "OnInputReceiveManActivated1")
	g_WinMgr:getWindow(mRecvManEditName):subscribeEvent("Deactivated", "OnInputReceiveManDeactivated1")
	mFListWindow:subscribeEvent("Activated", "OnFLWindowActivated1")
	mFListWindow:subscribeEvent("Deactivated", "OnFLWindowDeactivated1")
	mFListWindow:subscribeEvent("MouseClick", "OnFLWindowSelected1")
	CEGUI.toComboDropList(mFListWindow):setAutoArmEnabled(true)
	--创建临时弹出好友列表按钮
	mShowFListBtn = g_WinMgr:getWindow("lwchuangkou3/chuangkou2/tianjiahaoyou")--g_WinMgr:createWindow("NewEQHomeLook/NewStaticImage3",mShowFListName)
	--mShowFListBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0,311), CEGUI.UDim(0,26)))
	--mShowFListBtn:setSize(CEGUI.UVector2(CEGUI.UDim(0,22), CEGUI.UDim(0,22)))
	--mShowFListBtn:setProperty("AlwaysOnTop", "true")
	--g_WinMgr:getWindow("lwchuangkou3/chuangkou2"):addChildWindow(mShowFListBtn)
	mShowFListBtn:subscribeEvent("MouseClick", "OnSelectFriendFromList1")
	
	CreateDataTime(mGSDataTimeName, g_giftUIName_mail.year, g_giftUIName_mail.month, g_giftUIName_mail.day)
end

function OnSelectFriendFromList1(args)
	if false == g_DoSelectGiftRecver then
		g_DoSelectGiftRecver = true
		DoOpenFriendListUI()
	else
		DoCloseFriendListUI()
		g_DoSelectGiftRecver = false
	end
end

--在好友列表中选择一个好友
function Mail_OnSelectFriendChanged1(name)
	if false == g_DoSelectGiftRecver then
		return
	end
	mTxtChangedBySelFriend = true
	EQGlobaFunc.SetText(mRecvManEditName, name)
end

function OnInputReceiveManActivated1(args)
	mRecvManEditActive = true
end

function OnInputReceiveManDeactivated1(args)
	mRecvManEditActive = false
	HideFLWindow1()
end

function OnFLWindowActivated1(args)
	mFListWindowActive = true
end

function OnFLWindowDeactivated1(args)
	mFListWindowActive = false
	HideFLWindow1()
end

--收件人字符串更改
function OnInputReceiveManChar1(args)
	--跳过非手动更改
	if mTxtChangedBySelFriend then
		mTxtChangedBySelFriend = false
		return
	end
	
	local txt = g_WinMgr:getWindow(mRecvManEditName):getText()
	
	CEGUI.toListbox(mFListWindow):resetList()
	
	if "" == txt then
		mFListWindow:setVisible(false)
		return
	end
	
	mFListWindow:setVisible(true)
	
	FillFList1(txt)
end

--在收件人搜索列表中选择一个收件人
function OnFLWindowSelected1(args)
	local lbox = CEGUI.toListbox(mFListWindow)
	local item = lbox:getFirstSelectedItem()
	if item then
		EQGlobaFunc.SetText(mRecvManEditName, item:getText())
	end
	mFListWindow:setVisible(false)
end

function HideFLWindow1()
	if mRecvManEditActive or mFListWindowActive then
		return
	end
	
	mFListWindow:setVisible(false)
end

--根据关键字填充收件人列表
function FillFList1(key)
	g_FriendSys:searchEx(key)
	
	local i
	local cnt = g_FriendSys:getSearchExCnt()
	
	if 0 == cnt then
		mFListWindow:setVisible(false)
		return
	end
	
	for i = 1, cnt do
		EQGlobaFunc.createListboxTextItem(mFListWindow:getName(), g_FriendSys:getSearchExResult(i - 1), i)
	end
end
-----------------------------------------------------------------
------------------------      私有     --------------------------
-----------------------------------------------------------------
local function _Init()
	mStorageGlobaIndex = -1
    mChooseBoxId = 0
    mChooseCardId = 0
end
-----------------------------------------------------------------
--移除格子中的图片
local function _CleanSlotImage(slotName)
	local slotWin = g_WinMgr:getWindow(slotName)
	
	if slotWin:getChildCount() > 0 then
		local TempSource = slotWin:getChildAtIdx(0)
		
		if nil ~= TempSource then
			slotWin:removeChildWindow(TempSource)
		end
	end
end
-----------------------------------------------------------------
--设置格子中的图片
local function _SetSlotImage(slotName, imageSetName, imageName, overlay,itemEnabled)
	StorageUI_DynamicLoadItem(slotName,
							"newImg",
							0,
							imageSetName,
							imageName, "False", overlay,itemEnabled)
end
-----------------------------------------------------------------
--清除手工制作礼物的数据（包括界面上的数据）
local function _CleanDIYGiftData()
	--清空数据
	g_giftSystem:cleanCurMakingGiftInfo()
	--清空界面
	EQGlobaFunc.SetText(g_giftUIName_diy.giftName, "礼物")
	EQGlobaFunc.SetText(g_giftUIName_diy.greetings, "给你一个惊喜!")
	
	_CleanSlotImage(g_giftUIName_diy.itemSlot)
	_CleanSlotImage(g_giftUIName_diy.boxSlot)
	_CleanSlotImage(g_giftUIName_diy.cardSlot)
	
	m_diyGiftItemId = 0
	
	CEGUI.toCheckbox(g_WinMgr:getWindow(g_giftUIName_diy.anonymousBtn)):setSelected(false)
end
-----------------------------------------------------------------
--清除快递礼物的数据（包括界面上的数据）
local function _CleanMailGiftData()
	--清空数据
	g_giftSystem:cleanCurMakingGiftInfo()
	--清空界面
	g_WinMgr:getWindow(g_giftUIName_mail.reciverAcc):setText("")
	EQGlobaFunc.SetText(g_giftUIName_mail.greetings, "给你一个惊喜!")
	
	_CleanSlotImage(g_giftUIName_mail.itemSlot)
	_CleanSlotImage(g_giftUIName_mail.cardSlot)
	
	m_mailGiftItemId = 0
end
-----------------------------------------------------------------
local function _ReqLoadGiftBoxData()
	g_giftSystem:loadGiftBoxData()
end
-----------------------------------------------------------------
local function _ReqLoadGiftCardData()
	g_giftSystem:loadGiftCardData()
end
-----------------------------------------------------------------
local function _InitBoxData()
	mOffset = 0
	mChooseBoxId = 0
end
-----------------------------------------------------------------
local function _InitCardData()
	mOffset = 0
	mChooseCardId = 0
end
-----------------------------------------------------------------
--获得当前应显示的数据
--showCount一次最多显示的个数
local function _GetData(startIndex, showCount, pack)
	endIndex = math.min(startIndex + showCount, table.getn(pack))
	j = 1
	
	t = {}
	
	for i = startIndex + 1, endIndex do
		t[j] = pack[i]
		j = j + 1
	end
	
	return t
end
-----------------------------------------------------------------
--清空选择礼盒的界面
local function _CleanBoxListUI()
	for i = 1, mBoxSlotSize do
		_CleanSlotImage(g_giftBoxUIName.slotBase..i)
	end
	
	_CleanSlotImage(g_giftBoxUIName.showSlot)
	
	EQGlobaFunc.SetText(g_giftBoxUIName.name, "")
	EQGlobaFunc.SetText(g_giftBoxUIName.desc, "")
	EQGlobaFunc.SetText(g_giftBoxUIName.price, "")
	
end
-----------------------------------------------------------------
--清空选择礼卡的界面
local function _CleanCardListUI()
	for i = 1, mCardSlotSize do
		_CleanSlotImage(g_giftCardUIName.slotBase..i)
	end
	
	_CleanSlotImage(g_giftCardUIName.showSlot)
	
	EQGlobaFunc.SetText(g_giftCardUIName.name, "")
	EQGlobaFunc.SetText(g_giftCardUIName.desc, "")
	EQGlobaFunc.SetText(g_giftCardUIName.price, "")
	
end
-----------------------------------------------------------------
--获得当前所显示Tab的礼盒类型
local function _GetCurBoxType()
	local tabIndex = GetCurTabIndex(g_giftBoxUIName.tabName)
	return tabIndex + 1
end
-----------------------------------------------------------------
--获得当前所显示Tab的礼卡类型
local function _GetCurCardType()
	local tabIndex = GetCurTabIndex(g_giftCardUIName.tabName)
	return tabIndex + 1
end
-----------------------------------------------------------------
local function _SetCard(cardId)
	mChooseCardId = cardId
	local cardInfo = g_logictableMgr:getGiftCardRow(cardId)
	
	if nil ~= cardInfo then
		_SetSlotImage(g_giftCardUIName.showSlot, cardInfo.imageset, cardInfo.image)
		
		EQGlobaFunc.SetText(g_giftCardUIName.name, cardInfo.name)
		EQGlobaFunc.SetText(g_giftCardUIName.desc, cardInfo.desc)
		
		if 0 ~= cardInfo.eqGold then
			EQGlobaFunc.SetText(g_giftCardUIName.price, tostring(cardInfo.eqGold).."EQ点")
		elseif 0 ~= cardInfo.gameGold then
			EQGlobaFunc.SetText(g_giftCardUIName.price, tostring(cardInfo.gameGold).."金币")
		else
			EQGlobaFunc.SetText(g_giftCardUIName.price, "免费")
		end
	end
end
-----------------------------------------------------------------
local function _SetBox(boxId)
	if 0 == boxId then
		--g_Loger:logError("boxId == 0 in GiftSystem.lua _SetBox Function")
	end
	
	mChooseBoxId = boxId

	local boxInfo = g_logictableMgr:getGiftBoxRow(boxId)
	
	if nil ~= boxInfo then
		_SetSlotImage(g_giftBoxUIName.showSlot, boxInfo.imageset, boxInfo.image)
		
		EQGlobaFunc.SetText(g_giftBoxUIName.name, boxInfo.name)
		EQGlobaFunc.SetText(g_giftBoxUIName.desc, boxInfo.desc)
		
		if 0 ~= boxInfo.eqGold then
			EQGlobaFunc.SetText(g_giftBoxUIName.price, tostring(boxInfo.eqGold).."EQ点")
		elseif 0 ~= boxInfo.gameGold then
			EQGlobaFunc.SetText(g_giftBoxUIName.price, tostring(boxInfo.gameGold).."金币")
		else
			EQGlobaFunc.SetText(g_giftBoxUIName.price, "免费")
		end
	end
end
-----------------------------------------------------------------
--设置选中的礼盒
local function _SetChooseBox(type)
	local type = _GetCurBoxType()
	local chooseBoxId = mBoxData[type][mOffset + 1]
	
	if nil ~= chooseBoxId then
		_SetBox(chooseBoxId)
	end
end
-----------------------------------------------------------------
--设置选中的礼卡
local function _SetChooseCard(type)
	local type = _GetCurCardType()
	local chooseCardId = mCardData[type][mOffset + 1]
	
	if nil ~= chooseCardId then
		_SetCard(chooseCardId)
	end
end
-----------------------------------------------------------------
--更新显示礼物盒选择界面
local function _UpdateBoxListUI(boxType)
	--清空界面
	_CleanBoxListUI()
	--获得数据
	data = {}
	data = _GetData(mOffset, mBoxSlotSize, mBoxData[boxType])
	
	if 0 == table.getn(data) then
		return
	end
	
	--显示
	for i, v in pairs(data) do
		local boxInfo = g_logictableMgr:getGiftBoxRow(v)
		_SetSlotImage(g_giftBoxUIName.slotBase..i, boxInfo.imageset, boxInfo.image)
	end
	
	_SetChooseBox(boxType)
end
-----------------------------------------------------------------
--更新显示礼物卡选择界面
local function _UpdateCardListUI(cardType)
	--清空界面
	_CleanCardListUI()
	--获得数据
	data = {}
	data = _GetData(mOffset, mCardSlotSize, mCardData[cardType])
	
	if 0 == table.getn(data) then
		return
	end
	
	--显示
	for i, v in pairs(data) do
		local cardInfo = g_logictableMgr:getGiftCardRow(v)
		_SetSlotImage(g_giftCardUIName.slotBase..i, cardInfo.imageset, cardInfo.image)
	end
	
	_SetChooseCard(cardInfo)
end
-----------------------------------------------------------------
--初始化选择礼盒界面
local function _InitBoxListUI()
	_InitBoxData()

	CEGUI.toTabControl(g_WinMgr:getWindow(g_giftBoxUIName.tabName)):setSelectedTabAtIndex(0)
	
	_UpdateBoxListUI(mBoxType.free)
end
-----------------------------------------------------------------
--初始化选择礼卡界面
local function _InitCardListUI()
	_InitCardData()

	CEGUI.toTabControl(g_WinMgr:getWindow(g_giftCardUIName.tabName)):setSelectedTabAtIndex(0)
	
	_UpdateCardListUI(mCardType.free)
end
-----------------------------------------------------------------
local function _IsInHouse()
	if EQGlobaClass.EQGameSceneMgr.SCENE_HOUSE == g_SceneMgr:getSceneType() then
		return true
	else
		return false
	end
end
-----------------------------------------------------------------
--查看礼卡flash的超链接
local function _SetCardHyperLink(winName, text, cardId)
	if nil == cardId or 0 == cardId then
		return
	end
	local urlAddress = g_logictableMgr:getGiftCardRow(cardId).urlAddress
	EQGlobaFunc.toMultiLineBox(winName):addElementHyperLink(text, urlAddress, "#FFFFFF", false)
end
-----------------------------------------------------------------
--关闭所有礼物相关界面
function CloseAllGiftUI()
	CloseGiveGiftUI()
	CloseMakeDIYGiftUI()
	CloseGiftBoxUI()
	ClosePutGiftUI()
	CloseDIYGiftUI()
	CloseChooseItemUI()
	CloseMakeMailGiftUI()
	CloseMailGiftUI()
	--CloseLogGiftUI()
end
-----------------------------------------------------------------
------------------------      送礼     --------------------------
-----------------------------------------------------------------
-----------------------------------------------------------------
local mIsOpenGiveGiftUI = false
local mIsOpenPutGiftUI	= false
--打开选择送礼类型界面
function SwitchGiveGiftUI()
	if false == mIsOpenGiveGiftUI then
		ReqOpenGiveGiftUI()
		
		UIM_DoOpenAndCloseUI(g_GiftSelUINames.root)
	else
		ReqEndGiftService()
	end
end
-----------------------------------------------------------------
function ReqOpenGiveGiftUI()
	ReqStartGiftService(EQGlobaClass.GiftSystem.GS_PACK)
end
--打开送礼界面（第一个界面）
local firstOpenGiftUI = true
function OpenGiveGiftUI()
	_Init()
	
	UIM_OpenWindow(g_GiftSelUINames.root)
	PlayerGuide_do_NewerTeachTaskScript(15004) --新手教学任务
	mIsOpenGiveGiftUI = true	
	if firstOpenGiftUI then
		do_task_script("script14")	-- (点击礼物制作按钮时触发)执行新手任务脚本14(2011/6/15 add by hezhr)
		firstOpenGiftUI = false
	end
end
-----------------------------------------------------------------
function CloseGiveGiftUI()
	UIM_CloseWindow(g_GiftSelUINames.root)
	mIsOpenGiveGiftUI = false
end
-----------------------------------------------------------
--手工制作礼物（礼盒）界面
function MakeDIYGiftUI()
	if not mLiWuAnJianVisible then
		--关闭上一界面
		CloseGiveGiftUI()
		--清空数据
		_CleanDIYGiftData()
		--请求加载数据
		_ReqLoadGiftBoxData()
		_ReqLoadGiftCardData()
		
		g_MainUIWindows:addChildWindow(g_DIYGiftUI)
		g_DIYGiftUI:setVisible(true)
		
		mCurPackGiftType = mGiftType.DIY
	else
		local sysMsgRow = g_itemtableMgr:getSysMsgRow(137)
		MsgBox_createMsgBox(g_MainUIWindows,65535, "消息提示",sysMsgRow.content, "MsgBox_cancelFunc", "")
	end
end
-----------------------------------------------------------------
function CloseMakeDIYGiftUI()
	g_DIYGiftUI:setVisible(false)
	--g_MainUIWindows:removeChildWindow(g_DIYGiftUI)
end
-----------------------------------------------------------------
--清空礼物面板
local function _CleanDIYGiftUI()
	EQGlobaFunc.SetText(g_DIYGiftUIName.senderName, "")
	_CleanSlotImage(g_DIYGiftUIName.itemSlot)
	mDIYGiftItemId = 0
	mDIYGiftItemEffecTime = ""
	EQGlobaFunc.toMultiLineBox(mDIYGiftGreeting:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(mDIYGiftHyperLink:getName()):clearUp()
end
-----------------------------------------------------------------
--设置当前查看礼物（手工制作的礼盒）的界面
function SetDIYGiftUI(giverName, greetings, itemTemplateId, effecTime, overLay, cardId, canPick)
	_CleanDIYGiftUI()
	
	OpenDIYGiftUI()

	if("" ~= giverName) then
		EQGlobaFunc.SetText(g_DIYGiftUIName.senderName, giverName)
	else
		EQGlobaFunc.SetText(g_DIYGiftUIName.senderName, "匿名礼物")
	end
	
	EQGlobaFunc.toMultiLineBox(mDIYGiftGreeting:getName()):addElementText("simsun-10", greetings, "#ffffff", false)
	
	if 0 ~= itemTemplateId then
		local itemRow = g_itemtableMgr:getItemRow(itemTemplateId)
		
		if nil ~= itemRow then
			mDIYGiftItemId = itemTemplateId
			mDIYGiftItemEffecTime = effecTime
			_SetSlotImage(g_DIYGiftUIName.itemSlot, itemRow.imageset, itemRow.icon, overLay)
		end
	end
	
	_SetCardHyperLink(mDIYGiftHyperLink:getName(), "点击查看对方精心挑选的贺卡", cardId)
	g_WinMgr:getWindow(g_DIYGiftUIName.pickUpBtn):setEnabled(canPick)
end
-----------------------------------------------------------------
--清理放置礼物界面
local function _CleanPutGiftUI()
	EQGlobaFunc.SetText(g_putGiftUIName.senderName, "")
	_CleanSlotImage(g_putGiftUIName.itemSlot)
	mPutGiftItemId = 0
	mPutGiftItemEffecTime = ""
	EQGlobaFunc.toMultiLineBox(mPutGiftHyperLink:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(mPutGiftGreeting:getName()):clearUp()
end
-----------------------------------------------------------------
--设置放置礼物界面内容
function SetPutGiftUI(giverName, greetings, itemTemplate, effecTime, overLay, cardId)
	_CleanPutGiftUI()

	if "" ~= giverName then
		EQGlobaFunc.SetText(g_putGiftUIName.senderName, giverName)
	else
		EQGlobaFunc.SetText(g_putGiftUIName.senderName, "匿名礼物")
	end
	
	EQGlobaFunc.toMultiLineBox(mPutGiftGreeting:getName()):addElementText("simsun-10", greetings, "#ffffff", false)
	
	if 0 ~= itemTemplate then
		local itemRow = g_itemtableMgr:getItemRow(itemTemplate)
		
		if nil ~= itemRow then
			mPutGiftItemId = itemTemplate
			mPutGiftItemEffecTime = effecTime
			_SetSlotImage(g_putGiftUIName.itemSlot, itemRow.imageset, itemRow.icon)
		end
	end
	--超链接
	_SetCardHyperLink(mPutGiftHyperLink:getName(), "点击查看对方精心挑选的贺卡", cardId)
end
-----------------------------------------------------------------
--通知已经有打包好的物品
mLiWuAnJianVisible = false
function NotifyHavePacked()
	mLiWuAnJianVisible = true
	g_WinMgr:getWindow("DFwnd_MainRoot/liwuanjian"):setVisible(true)
end
-----------------------------------------------------------------
--通知没有打包好的物品
function NotifyWithoutPacked()
	mLiWuAnJianVisible = false
	g_WinMgr:getWindow("DFwnd_MainRoot/liwuanjian"):setVisible(false)
end
-----------------------------------------------------------------
function SwitchPutGiftUI()
	--没在房子中 无法打开界面
	if false == _IsInHouse() then
		MsgBox_createMsgBox(g_MainUIWindows,10505,"","礼物不能放置在这里.", "MsgBox_cancelFunc", "")
		return
	end
	
	if false == mIsOpenPutGiftUI then
		ReqStartGiftService(EQGlobaClass.GiftSystem.GS_PUT)
	else
		ReqEndGiftService()
	end
end
-----------------------------------------------------------------
function OpenPutGiftUI()
	g_MainUIWindows:addChildWindow(g_PutGiftUI)
	mIsOpenPutGiftUI = true
end
-----------------------------------------------------------------
function ClosePutGiftUI()
	g_MainUIWindows:removeChildWindow(g_PutGiftUI)
	mIsOpenPutGiftUI = false
end
-----------------------------------------------------------------
--------------------------* 礼卡相关 *---------------------------
-----------------------------------------------------------------
function LoadGiftCardData(giftCardInfo, index, count)
	--收到第一条数据，初始化
	if 0 == index then
		_InitCardData()
	end
	--保存数据
	table.insert(mCardData[giftCardInfo.type], giftCardInfo.id)
	
end
-----------------------------------------------------------------
function OpenGiftCardUI()
	_InitCardListUI()

	g_MainUIWindows:addChildWindow(g_GiftCardUI)
	Common_setModalState(g_GiftCardUI:getName(), true)
	
end
-----------------------------------------------------------------
function CloseGiftCardUI()
	g_MainUIWindows:removeChildWindow(g_GiftCardUI)
	Common_setModalState(g_GiftCardUI:getName(), false)
end
-----------------------------------------------------------------
function ChooseCardType()
	local type = _GetCurCardType()
	mOffset = 0
	
	_UpdateCardListUI(type)
end
-----------------------------------------------------------------
function ChooseGiftCard(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index = GetClickWindowIndex(winName, g_giftCardUIName.slotBase, 1, mCardSlotSize)
	
	local curType = _GetCurCardType()
	
	if nil ~= index then
		--判断是否有数据
		if mOffset + index > table.getn(mCardData[curType]) then
			mChooseCardId = 0
		else
			--设置当前选中的礼物盒
			_SetCard(mCardData[curType][mOffset + index])
		end
	end
end
-----------------------------------------------------------------
--确定选择礼物卡
function SureCard()
	CloseGiftCardUI()
	local cardInfo = g_logictableMgr:getGiftCardRow(mChooseCardId)
	if cardInfo ~= nil then
		if mGiftType.DIY == mCurPackGiftType then
			_SetSlotImage(g_giftUIName_diy.cardSlot, cardInfo.imageset, cardInfo.image)
		else
			_SetSlotImage(g_giftUIName_mail.cardSlot, cardInfo.imageset, cardInfo.image)
		end
	end
end
-----------------------------------------------------------------
function NextGiftCard()
	local type = _GetCurCardType()
	
	if mOffset + mCardSlotSize < table.getn(mCardData[type]) then
		mOffset = mOffset + 1
		_UpdateCardListUI(type)
	end
end
-----------------------------------------------------------------
function PreGiftCard()
	local type = _GetCurCardType()
	
	if 0 < mOffset then
		mOffset = mOffset - 1
		_UpdateCardListUI(type)
	end
end
-----------------------------------------------------------------
--------------------------* 礼盒相关 *---------------------------
-----------------------------------------------------------------
--礼盒 数据只加载一次。即在游戏开始的时候，在C++中控制
function LoadGiftBoxData(giftBoxInfo, index, count)	
	--收到第一条数据，初始化
	if 0 == index then
		_InitBoxData()
	end
	--保存数据
	table.insert(mBoxData[giftBoxInfo.type], giftBoxInfo.id)
end
-----------------------------------------------------------------
function OpenGiftBoxUI()
	_InitBoxListUI()

	g_MainUIWindows:addChildWindow(g_GiftBoxUI)
	Common_setModalState(g_GiftBoxUI:getName(), true)
end
-----------------------------------------------------------------
function CloseGiftBoxUI()
	g_MainUIWindows:removeChildWindow(g_GiftBoxUI)
	Common_setModalState(g_GiftBoxUI:getName(), false)
end
-----------------------------------------------------------------
--选择礼盒类型
function ChooseBoxType()
	local type = _GetCurBoxType()
	mOffset = 0
	
	_UpdateBoxListUI(type)
end
-----------------------------------------------------------------
function ChooseGiftBox(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index = GetClickWindowIndex(winName, g_giftBoxUIName.slotBase, 1, mBoxSlotSize)
	
	local curType = _GetCurBoxType()
	
	if nil ~= index then
		--判断是否有数据
		if mOffset + index > table.getn(mBoxData[curType]) then
			mChooseBoxId = 0
		else
			--设置当前选中的礼物盒
			_SetBox(mBoxData[curType][mOffset + index])
		end
	end
end
-----------------------------------------------------------------
--确定选择礼物盒
function SureBox()
	CloseGiftBoxUI()
	
	local boxInfo = g_logictableMgr:getGiftBoxRow(mChooseBoxId)
	if boxInfo ~= nil then
		_SetSlotImage(g_giftUIName_diy.boxSlot, boxInfo.imageset, boxInfo.image)
	end
end
-----------------------------------------------------------------
function NextBox()
	local type = _GetCurBoxType()
	
	if mOffset + mBoxSlotSize < table.getn(mBoxData[type]) then
		mOffset = mOffset + 1
		_UpdateBoxListUI(type)
	end
end
-----------------------------------------------------------------
function PreBox()
	local type = _GetCurBoxType()
	
	if 0 < mOffset then
		mOffset = mOffset - 1
		_UpdateBoxListUI(type)
	end
end
-----------------------------------------------------------------
--请求开始礼物服务
function ReqStartGiftService(serviceType)
	g_giftSystem:reqStartGiftService(serviceType)
end
-----------------------------------------------------------------
--请求结束礼物服务
function ReqEndGiftService()
	g_giftSystem:reqEndGiftService()
	m_diyGiftItemId, m_mailGiftItemId = 0, 0
end

-----------------------------------------------------------------
------------------------*礼物物品相关*---------------------------
-----------------------------------------------------------------
local mCurStoragePage = 1
-----------------------------------------------------------------
--获得仓库的全局索引
local function _GetStorgeDataIndex(curIndex, curPageIndex, slotSize)
	return curIndex + (curPageIndex - 1) * slotSize
end
-----------------------------------------------------------------
local function _GetClickPageIndex(winName)
	for i = 1, 4  do
		if g_GiftStorageUI["page"..i] == winName then
			return i
		end
	end
end
-----------------------------------------------------------------
local function _UpdatePageState()
	local pagenum = g_mainplayer:getPackPageCount(g_PT_WAREHOUSE)
	
	for pageNum = 1, pagenum do
		if pageNum == mCurStoragePage then
			g_WinMgr:getWindow(g_GiftStorageUI["page"..pageNum]):setProperty("NormalImage",g_StoragePageActive)
			g_WinMgr:getWindow(g_GiftStorageUI["page"..pageNum]):setProperty("HoverImage",g_StoragePageActive)
			g_WinMgr:getWindow(g_GiftStorageUI["page"..pageNum]):setProperty("PushedImage",g_StoragePageActive)
		else
			g_WinMgr:getWindow(g_GiftStorageUI["page"..pageNum]):setProperty("NormalImage",g_StoragePageOpenNormal)
			g_WinMgr:getWindow(g_GiftStorageUI["page"..pageNum]):setProperty("HoverImage",g_StoragePageOpenHover)
		end	
	end
end
-----------------------------------------------------------------
function OpenChooseItemUI()
	if g_StorageUIFlag then
		StorageUI_OnCloseStorage()
	end
	g_MainUIWindows:addChildWindow(g_GiftItemUI)
	Common_setModalState(g_GiftItemUI:getName(), true)
end
-----------------------------------------------------------------
function CloseChooseItemUI()
	g_MainUIWindows:removeChildWindow(g_GiftItemUI)
	Common_setModalState(g_GiftItemUI:getName(), false)
end
-----------------------------------------------------------------
function ChangeGiftItemPage(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	
	local pageIndex = _GetClickPageIndex(winName)
	local pageCount = g_mainplayer:getPackPageCount(g_PT_WAREHOUSE)
		
	if nil ~= pageIndex and pageIndex <= pageCount then
		mCurStoragePage = pageIndex
		UpdateGiftStorage()
	end
	
end
-----------------------------------------------------------------
function ChooseGiftItem(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	--将鼠标状态设置为选取状态
	local slotIndex = GetClickWindowIndex(winName, g_GiftStorageUI.slotBaseName, 1, C_STORAGE_GIRD_NUM)
	
	if nil == slotIndex then
		return
	end
	
	local storageGlobaIndex = _GetStorgeDataIndex(slotIndex-1, mCurStoragePage, C_STORAGE_GIRD_NUM)
	local itemId = g_mainplayer:getItemIDByGlobaIdx(storageGlobaIndex, EQGlobaClass.ItemPack.PT_WAREHOUSE)
	
	if 0 < itemId then
		local itemRow =  g_itemtableMgr:getItemRow(itemId)
		local itemGrid = g_mainplayer:getItemGridByGlobaIdx(storageGlobaIndex, EQGlobaClass.ItemPack.PT_WAREHOUSE)
		
		local overlay = itemGrid.mOverlay
		if itemRow.overlap == 1 then
			overlay = -1
		end
		
		local canSel = true
		if itemRow.bind == 1 then
			canSel = false
		end
		
		if nil ~= itemRow then
			if canSel then
				if mGiftType.DIY == mCurPackGiftType then
					_SetSlotImage(g_giftUIName_diy.itemSlot, itemRow.imageset, itemRow.icon, overlay)
					m_diyGiftItemId = itemId
				else
					_SetSlotImage(g_giftUIName_mail.itemSlot, itemRow.imageset, itemRow.icon, overlay)
					m_mailGiftItemId = itemId
				end
				mStorageGlobaIndex = storageGlobaIndex --g_giftSystem:setGiftItem(storageGlobaIndex)
				CloseChooseItemUI()
			else
				MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","该物品无法被当作礼物送出。", "MsgBox_cancelFunc", "")
			end
		end	
	end
end
-----------------------------------------------------------------
--显示的数据为玩家仓库中的数据
function UpdateGiftStorage()
	--更新切换页面按钮的状态
	_UpdatePageState()
	
	for slotIndex = 1, C_STORAGE_GIRD_NUM do
		local storageGlobaIndex = _GetStorgeDataIndex(slotIndex-1, mCurStoragePage, C_STORAGE_GIRD_NUM)
		local itemId = g_mainplayer:getItemIDByGlobaIdx(storageGlobaIndex, EQGlobaClass.ItemPack.PT_WAREHOUSE)
		
		if 0 < itemId then
			local itemGrid = g_mainplayer:getItemGridByGlobaIdx(storageGlobaIndex, EQGlobaClass.ItemPack.PT_WAREHOUSE)
			local itemRow =  g_itemtableMgr:getItemRow(itemId)
			
			if nil ~= itemRow and nil ~= itemGrid then
				
				local overlay = itemGrid.mOverlay
				if itemRow.overlap == 1 then
					overlay = -1
				end
				local itemEnabled = "True"
				if itemRow.bind == 1 then
					itemEnabled = "False"
				end
				--ICON
				_SetSlotImage(g_GiftStorageUI.slotBaseName..(slotIndex), itemRow.imageset, itemRow.icon, overlay,itemEnabled)
				--LOCK
			end
			
		else
			_CleanSlotImage(g_GiftStorageUI.slotBaseName..(slotIndex))
		end
		
	end
	
end
-----------------------------------------------------------------
-----------------------------------------------------------------
--请求打包礼物
function ReqPackGift()
	if 0 == mChooseBoxId then
		MsgBox_createMsgBox(g_MainUIWindows,10501,"","请选择礼物盒.", "MsgBox_cancelFunc", "")
		return
	end
	
	--直接使用透露给lua的g_giftSystem.mCurMakingGiftInfo会挂 -.-	
	local giftName = g_WinMgr:getWindow(g_giftUIName_diy.giftName):getText()	
	local greetings = g_WinMgr:getWindow(g_giftUIName_diy.greetings):getText()

	--是否匿名
	local window = g_WinMgr:getWindow(g_giftUIName_diy.anonymousBtn)
	local giverName = ""
	if false == CEGUI.toCheckbox(window):isSelected() then
		giverName = g_mainplayer:getName()
	end

	g_giftSystem:setDIYGiftInfo(giftName, mChooseCardId, mChooseBoxId, mStorageGlobaIndex, greetings, giverName)
	g_giftSystem:reqPackGift()
end
-----------------------------------------------------------------
function OnCancleDIYGiftUI()
	if false == mIsShowLogGift then
		ReqEndGiftService()
	else
		mIsShowLogGift = false
		CloseDIYGiftUI()
	end
end
-----------------------------------------------------------------
function CloseDIYGiftUI()
	g_MainUIWindows:removeChildWindow(g_OpenGiftUI)
end
-----------------------------------------------------------------
function OpenDIYGiftUI()
	g_MainUIWindows:addChildWindow(g_OpenGiftUI)
end
-----------------------------------------------------------------
--放置礼物
function PutGift()
	g_giftSystem:reqPutGift()
end
-----------------------------------------------------------------
function CanclePackGift()
	g_giftSystem:reqCanclePackGift()
end
-----------------------------------------------------------------
function ReqPickUpGift()
	g_giftSystem:reqPickUpGift()
end
-----------------------------------------------------------------
function GiftSystem_EndPickUpGift()
end


-----------------------------------------------------------------
--打开制作快递型礼物界面
function OpenMakeMailGiftUI()
	--关闭上一界面
	CloseGiveGiftUI()
	--清空数据
	_CleanMailGiftData()
	--请求加载数据
	_ReqLoadGiftCardData()
	
	g_MainUIWindows:addChildWindow(g_MailGiftUI)
	g_MailGiftUI:setVisible(true)
	
	mCurPackGiftType = mGiftType.Mail
end
-----------------------------------------------------------------
function CloseMakeMailGiftUI()
	g_MailGiftUI:setVisible(false)
	--g_MainUIWindows:removeChildWindow(g_MailGiftUI)
end
-----------------------------------------------------------------
--请求快递送礼
function ReqMailGift()

	local reciverAcc = g_WinMgr:getWindow(g_giftUIName_mail.reciverAcc):getText()	
	local greetings = g_WinMgr:getWindow(g_giftUIName_mail.greetings):getText()
	
	if "" == reciverAcc then
		MsgBox_createMsgBox(g_MainUIWindows,10502,"","请输入玩家帐号.", "MsgBox_cancelFunc", "")
		return
	end
	
	if -1 == mStorageGlobaIndex and 0 == mChooseCardId then
		MsgBox_createMsgBox(g_MainUIWindows,10503,"","礼物卡和物品至少选择一个.", "MsgBox_cancelFunc", "")
		return
	end
	
	local year = GetDataTimeYear(mGSDataTimeName) --tonumber(g_WinMgr:getWindow(g_giftUIName_mail.year):getText())
	local month = GetDataTimeMonth(mGSDataTimeName) --tonumber(g_WinMgr:getWindow(g_giftUIName_mail.month):getText())
	local day = GetDataTimeDay(mGSDataTimeName) --tonumber(g_WinMgr:getWindow(g_giftUIName_mail.day):getText())
	
	if nil == year then
		year = 0
	end
	
	if nil == month then
		month = 0
	end
	
	if nil == day then
		day = 0
	end
	
	reciverAcc = GetAccFromCStr(reciverAcc)
	
	--g_Loger:logError("acc: "..reciverAcc)

	g_giftSystem:setMailGiftInfo(tostring(reciverAcc), tostring(greetings), mChooseCardId, mStorageGlobaIndex, year, month, day)
	g_giftSystem:reqExpressGift()
end
-----------------------------------------------------------------
----------------------  记录中礼物的显示 ------------------------
-----------------------------------------------------------------
-----------------------------------------------------------------
local firstOpenMailGiftUI = true
function OpenMailGiftUI()
	g_MainUIWindows:addChildWindow(g_LogGiftUI)
	if firstOpenMailGiftUI then
		do_task_script("script15")	-- (打开快递礼物时触发)执行新手任务脚本15(2011/6/15 add by hezhr)
		firstOpenMailGiftUI = false
	end
end
-----------------------------------------------------------------
function CloseMailGiftUI()
	g_MainUIWindows:removeChildWindow(g_LogGiftUI)
end
-----------------------------------------------------------------
local function _CleanMailGiftUI()
	EQGlobaFunc.SetText(g_MailGiftUIName.senderName, "")
	_CleanSlotImage(g_MailGiftUIName.itemSlot)
	mMailGiftItemId = 0
	mMailGiftItemEffecTime = ""
	--清空超链接
	EQGlobaFunc.toMultiLineBox(mMailGiftHyperLink:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(mMailGiftGreeting:getName()):clearUp()
end
-----------------------------------------------------------------
function SetMailGiftUI(greetings, giverName, itemTemplateId, effecTime, overLay, cardId, giverAcc, expressType, canPick)
	_CleanMailGiftUI()
	
	OpenMailGiftUI()
	
	EQGlobaFunc.SetText(g_MailGiftUIName.senderName, giverName)
	EQGlobaFunc.toMultiLineBox(mMailGiftGreeting:getName()):addElementText("simsun-10", greetings, "#ffffff", false)
	
	if 0 ~= itemTemplateId then
		local itemRow = g_itemtableMgr:getItemRow(itemTemplateId)
		
		if nil ~= itemRow then
			mMailGiftItemId = itemTemplateId
			mMailGiftItemEffecTime = effecTime
			_SetSlotImage(g_MailGiftUIName.itemSlot, itemRow.imageset, itemRow.icon, overLay)
		end
	end
	
	g_WinMgr:getWindow(g_MailGiftUIName.pickUpBtn):setEnabled(canPick)
	if g_gift_express_type.gm == expressType then
		g_WinMgr:getWindow(g_MailGiftUIName.reply):disable()
	else
		g_WinMgr:getWindow(g_MailGiftUIName.reply):enable()
	end
	--	
	_SetCardHyperLink(mMailGiftHyperLink:getName(), "点击查看对方精心挑选的贺卡", cardId)
end
-----------------------------------------------------------------
function ReqStartReciveExpress(args)
	g_WinMgr:getWindow(g_MailGiftUIName.pickUpBtn):setEnabled(false)
	ReqStartGiftService(EQGlobaClass.GiftSystem.GS_GET_EXPRESS_GIFT)
	do_task_script("script16")	-- (点击快递礼物接收按钮时触发)执行新手任务脚本16(2011/6/15 add by hezhr)
end
-----------------------------------------------------------------
--回复快递礼物
function ReplyMailGift()
	ReqStartGiftService(EQGlobaClass.GiftSystem.GS_REPLY_MAIL_GIFT)
end
-----------------------------------------------------------------
function DoReplyMailGift()
	_Init()
	
	CloseAllGiftUI()
	OpenMakeMailGiftUI()
	
	local acc = GetReplyMailGiftAcc()
	EQGlobaFunc.SetText(g_giftUIName_mail.reciverAcc, acc)
end
-----------------------------------------------------------------
function ShowGiftCard(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local urlStr = EQGlobaFunc.toMultiLineBox(win:getName()):getHyperLinkId()
	
	g_giftSystem:playGiftCardFlash(urlStr)
end
-----------------------------------------------------------------
function SetDIYGiftUIFlag(flag)
	mIsShowLogGift = flag
end
-----------------------------------------------------------------
function NotifyCannotPutGift()
	MsgBox_createMsgBox(g_MainUIWindows,10504,"","该格子已有礼物，无法放置更多的礼物.", "MsgBox_cancelFunc", "")
end
-----------------------------------------------------------------
-- 显示发送礼物物品itemhint
function SendGiftItem_MouseEnter(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	--将鼠标状态设置为选取状态
	local slotIndex = GetClickWindowIndex(winName, g_GiftStorageUI.slotBaseName, 1, C_STORAGE_GIRD_NUM)
	
	if nil == slotIndex then
		return
	end
	
	local storageGlobaIndex = _GetStorgeDataIndex(slotIndex-1, mCurStoragePage, C_STORAGE_GIRD_NUM)
	local itemId = g_mainplayer:getItemIDByGlobaIdx(storageGlobaIndex, EQGlobaClass.ItemPack.PT_WAREHOUSE)
	if 0 < itemId then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_mainplayer:getItemDelTimeByGlobalIdx(storageGlobaIndex, g_PT_WAREHOUSE)
		createItemhint_shop(itemId, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
-----------------------------------------------------------------
-- 显示接收礼物物品itemhint
function RecvGiftItem_MouseEnter(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local itemId = 0
	local itemEffecTime = ""
	if g_putGiftUIName.itemSlot == winName then
		itemId = mPutGiftItemId
		itemEffecTime = mPutGiftItemEffecTime
	elseif g_DIYGiftUIName.itemSlot == winName then
		itemId = mDIYGiftItemId
		itemEffecTime = mDIYGiftItemEffecTime
	elseif g_MailGiftUIName.itemSlot == winName then
		itemId = mMailGiftItemId
		itemEffecTime = mMailGiftItemEffecTime
	end

	if 0 ~= itemId then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		createItemhint_shop(itemId, rect, g_MainUIWindows, itemEffecTime, 1, nil)
	end	
end
-----------------------------------------------------------------
-- 隐藏礼物物品itemhint
function GiftItem_MouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
-----------------------------------------------------------------
-- 显示礼物格子上物品itemhint
function GiftSystem_MouseEnterItemSlot(args)
	local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
	if 0 ~= m_diyGiftItemId then
		local effectTime = g_mainplayer:getItemDelTime(m_diyGiftItemId, g_PT_WAREHOUSE)
		createItemhint_shop(m_diyGiftItemId, rect, g_MainUIWindows, effectTime, 1, nil)
		return
	end
	if 0 ~= m_mailGiftItemId then
		local effectTime = g_mainplayer:getItemDelTime(m_mailGiftItemId, g_PT_WAREHOUSE)	
		createItemhint_shop(m_mailGiftItemId, rect, g_MainUIWindows, effectTime, 1, nil)
		return
	end
end
-----------------------------------------------------------------
-- 显示礼物格子上礼盒itemhint
function GiftSystem_MouseEnterBoxSlot(args)
	local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
	if 0 ~= mChooseBoxId then
		createItemhint_gift(mChooseBoxId, rect, g_MainUIWindows, 0)
	end
end
-----------------------------------------------------------------
-- 显示礼物格子上礼卡itemhint
function GiftSystem_MouseEnterCardSlot(args)
	local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
	if 0 ~= mChooseCardId then
		createItemhint_gift(mChooseCardId, rect, g_MainUIWindows, 1)
	end
end
-----------------------------------------------------------------
-- 销毁礼物系统上的itemhint
function GiftSystem_DestroyItemhint(args)
	destroyItemhint(g_MainUIWindows)
end
