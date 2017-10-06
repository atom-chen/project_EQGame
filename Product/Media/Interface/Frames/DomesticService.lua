-----------------------------------------------------------------
--家政系统
--xiaoMing
--2010-07-26
-----------------------------------------------------------------
g_DomesticService = false

local mDSPack = {}
local mDSCurPage = 1
local mDSPageSize = 7
local mReqServiceId = 0

--电话本界面
g_AddressBook = false
local mReadAddressFlag = false
local mTipFlag = false
local mTextWnd
-----------------------------------------------------------------
local function _Init()
	mDSCurPage = 1
	mReqServiceId = 0
end
-----------------------------------------------------------------
--更新显示家政类型
local function _Update(page , tableData)
	data = getData(tableData, page, mDSPageSize)
	local baseStr = g_addressBook.slotName
	local itemRow, name
	for i, v in pairs(data) do
		itemRow = g_logictableMgr:getDoserviceRow(v)
		if nil ~= itemRow then
			name = itemRow.name
			FSetText(baseStr..i, name)
		end
	end
	--FUpdatePageInfo(, page)
end
-----------------------------------------------------------------
function DomesticServiceOnLoad()
	MainUI_ShieldClick(g_BackGroundUI)
	g_WinMgr:getWindow(g_HomeService.image):subscribeEvent("MouseClick", "DSCheckMoneyRate")
	g_WinMgr:getWindow(g_HomeService.textBack):subscribeEvent("MouseClick", "DSCheckOut")
	g_GameEventMgr:subscribeEvent("POP_HOME_SERVICE", "PopDomesticService")
	g_WinMgr:getWindow(g_addressBook.closeBtn):subscribeEvent("Clicked", "CloseAddressBook")
	g_WinMgr:getWindow(g_addressBook.leaveBtn):subscribeEvent("Clicked", "CloseAddressBook")

	g_WinMgr:getWindow(g_HomeService.tipCloseBtn):subscribeEvent("Clicked", "EndHouseService")
	g_WinMgr:getWindow(g_HomeService.tipCancleBtn):subscribeEvent("Clicked", "EndHouseService")
	g_WinMgr:getWindow(g_HomeService.tipSureBtn):subscribeEvent("Clicked", "ReqHouseService")

	g_WinMgr:getWindow(g_HomeService.imageFrame):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow(g_HomeService.textFrame ):setProperty("MousePassThroughEnabled", "true")

	for i = 1, mDSPageSize do
		g_WinMgr:getWindow(g_addressBook.slotName .. i):subscribeEvent("MouseClick", "DSOnClicked")
	end	
	
	mTextWnd = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "HomeServiceTextWnd")
	mTextWnd:setSize(CEGUI.UVector2(CEGUI.UDim(0, 280), CEGUI.UDim(0,210)))
	EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):setFont("simsun-12")
	EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):setCols("#39C3F9")
	g_WinMgr:getWindow(g_HomeService.text):addChildWindow(mTextWnd)
end
-----------------------------------------------------------------
--点击家政图片
function DSOnClicked(args)
	local clickName = CEGUI.toWindowEventArgs(args).window:getName()

	local index = GetClickWindowIndex(clickName, g_addressBook.slotName, 1, mDSPageSize)

	if 0 ~= index then
		index = FShopGetTableIndex(index, mDSCurPage, mDSPageSize)
	else
		return
	end

	if index > table.getn(mDSPack) then
		return
	else
		mReqServiceId = mDSPack[index]
		g_smallModules:reqStartDomestic()
	end
end
-----------------------------------------------------------------
--选择家政服务界面
function PopAddressBook()
	if false == g_AddressBook then
		OpenAddressBook()
		
		UIM_DoOpenAndCloseUI(g_AddBookUINames.root)
	else
		CloseAddressBook()
	end
end
-----------------------------------------------------------------
function lua_addServices(id, i, num)
	table.insert(mDSPack, id)
	if i == num then
		mReadAddressFlag = true
	end
end
-----------------------------------------------------------------
function OpenAddressBookUI()
	UIM_OpenWindow(g_AddBookUINames.root)
	g_AddressBook = true
end

function CloseAddressBookUI()
	UIM_CloseWindow(g_AddBookUINames.root)
	g_AddressBook = false
end

function OpenAddressBook()
	_Init()
	if false == mReadAddressFlag then
		mDSPack = {}
		g_smallModules:saveDoServiceData()
	end
	_Update(mDSCurPage, mDSPack)
		
	OpenAddressBookUI()
end

function CloseAddressBook()
	CloseAddressBookUI()

	if true == mTipFlag then
		g_MainUIWindows:removeChildWindow(g_houseServiceTip)
		Common_setModalState(g_houseServiceTip:getName(), false)
		mTipFlag = false
	end
end
-----------------------------------------------------------------
function NextPageService()
end
-----------------------------------------------------------------
function PrePageService()
	if 1 >= mDSCurPage then
		return
	end

	mDSCurPage = mDSCurPage + 1
end
-----------------------------------------------------------------
function PopDomesticService()
	CloseAddressBook()
	if false == g_DomesticService then
		g_WinMgr:getWindow(g_HomeService.textBack):setVisible(false)
		g_WinMgr:getWindow(g_HomeService.textFrame):setVisible(false)
		g_WinMgr:getWindow(g_HomeService.image):setVisible(true)
		g_WinMgr:getWindow(g_HomeService.imageFrame):setVisible(true)

		g_DomesticService = true
		--有个边框的宽度
		ChangeImageFormat(802, 602, g_HomeService.image)
		--设置图片
		local serviceId = g_smallModules:getServiceId()
		--读表
		local row = g_logictableMgr:getDoserviceRow(serviceId)
		local imageName = GetImageName(row.image)

		g_WinMgr:getWindow(g_HomeService.image):setProperty("Image", imageName)

		PlayFadeAnimation(g_HomeService.image, 2000, 0, 1, 0)

		--AddFullScreenUI(g_BackGroundUI)
		OnOpenBackGround()
		UIM_DoOpenAndCloseUI(g_housesweepUINames.root)
	else
		CloseDomesticService()
	end
end
-----------------------------------------------------------------
function CloseDomesticService()
	--RemoveFullScreenUI(g_BackGroundUI)
	OnCloseBackGround()
	g_DomesticService = false
end

function OnOpenBackGround(args)
	UIM_OpenWindow(g_housesweepUINames.root)
	Common_setModalState(g_housesweepUINames.root, true)
end

function OnCloseBackGround(args)
	UIM_CloseWindow(g_housesweepUINames.root)
	Common_setModalState(g_housesweepUINames.root, false)
end
-----------------------------------------------------------------
--
function getDomesticServiceFlag()
	return (g_AddressBook or g_DomesticService)
end
-----------------------------------------------------------------
function DSCheckMoneyRate()

	if true == EQGlobaFunc.uiAnimationIsPlaying(g_HomeService.image) then
		return
	end

	local moneyRate = g_smallModules:getMoneyRate()
	if 100 ~= moneyRate then
		moneyRate = moneyRate * 0.1
		g_WinMgr:getWindow(g_HomeService.textBack):setVisible(true)
		g_WinMgr:getWindow(g_HomeService.textFrame):setVisible(true)
		g_WinMgr:getWindow(g_HomeService.image):setVisible(false)
		g_WinMgr:getWindow(g_HomeService.imageFrame):setVisible(false)
		
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):clearUp()
		local str = "  家政公司特别优惠！"
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText(str, true)
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("", true)
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText("", true)	
		str = g_mainplayer:getName()..",恭喜你获得我们公司特别"..moneyRate.."折优惠,只收取您"..g_smallModules.mCostMoney.."枚金币,希望下次您还能选择我们公司的服务"	
		EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):addElementText(str, true)

		PlayFadeAnimation(g_HomeService.text, 2000, 0, 1, 0)
	else
		DSCheckOut()
	end
end
-----------------------------------------------------------------
--家政結算
function DSCheckOut()
	if true == EQGlobaFunc.uiAnimationIsPlaying(g_HomeService.text) then
		return
	end

	CloseDomesticService()
	g_smallModules:changeValue()
	g_smallModules:reqEndService(g_smallModules:getServiceId())
end
-----------------------------------------------------------------
function ReqDoService(id)
	if false == g_DomesticService then
		g_smallModules:reqDomesticService(id)
	end
end
-----------------------------------------------------------------
function AddFullScreenUI(ui)
	g_MainUIWindows:addChildWindow(ui)
	g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):setProperty("AlwaysOnTop", "true")
	g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):setProperty("AlwaysOnTop", "true")
end
-----------------------------------------------------------------
function RemoveFullScreenUI(ui)
	g_MainUIWindows:removeChildWindow(ui)
	g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):setProperty("AlwaysOnTop", "false")
	g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):setProperty("AlwaysOnTop", "false")
end
-----------------------------------------------------------------
--图片是否因分辨率的变化而变化
function ChangeImageFormat(standardW, standardH, winName)
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()

	if screenW > standardW  then
		g_WinMgr:getWindow(winName):setProperty("HorzFormatting", "CentreAligned")
	else
		g_WinMgr:getWindow(winName):setProperty("HorzFormatting", "Stretched")
	end

	if screenH > standardH then
		g_WinMgr:getWindow(winName):setProperty("VertFormatting", "CentreAligned")
	else
		g_WinMgr:getWindow(winName):setProperty("VertFormatting", "Stretched")
	end
end
-----------------------------------------------------------------
function SetWindowImage(winName, imageSet, image)
	local str = "set:"..imageSet.." image:"..image
	local window = g_WinMgr:getWindow(winName):setProperty("Image", str)
end
-----------------------------------------------------------------
--添加家政服务提示
function AddHouseServiceTip()
	if false == mTipFlag then
		CloseAddressBook()
		g_MainUIWindows:addChildWindow(g_houseServiceTip)
		Common_setModalState(g_houseServiceTip:getName(), true)
		mTipFlag = true
	end
	
	local level = g_sceneMgr:getHouse():getLevel()
	local row = g_logictableMgr:getServicePriceRow(level) 
	
	EQGlobaFunc.toMultiLineBox(mTextWnd:getName()):clearUp()
	local str = "\n　　家政公司帮您打扫，增加1点清洁度的费用为"..row.price.."金币，确认呼叫家政公司吗？"
	EQGlobaFunc.SetText(g_HomeService.tipText, str)
end
-----------------------------------------------------------------
function EndHouseService()
	g_MainUIWindows:removeChildWindow(g_houseServiceTip)
	Common_setModalState(g_houseServiceTip:getName(), false)
	mTipFlag = false
	g_smallModules:reqEndService(g_smallModules:getServiceId())
end
-----------------------------------------------------------------
function ReqHouseService()
	CloseAddressBook()
	ReqDoService(mReqServiceId)
end
-----------------------------------------------------------------

