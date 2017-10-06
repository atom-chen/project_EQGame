-----------------------------------------
-- 文件说明	：界面管理系统
-- 作者		：xiaoMing
-- 日期		：2010-11-01
-----------------------------------------

--注册列表：窗体名、打开函数、关闭函数
local g_uiEventRegistry = {}

--注册一个窗体对应的打开和关闭事件
function UIM_RegistUIEvent(name, openFun, closeFun)
	g_uiEventRegistry[name] = { open = openFun, close = closeFun }
	g_Loger:logError(name.." open and close function registed.")
end

--打开一个窗体关联的所有窗体
function UIM_OpenRelationUI(name)
	local row = g_logictableMgr:getMutexUIRow(name)
	
--	g_Loger:logMessage("start open "..name.." relation ui.")
	
	if row then
		local arr = {}
		SplitDigit(arr, row.relation)
		local c = table.getn(arr)
		local x
		for x = 1, c do
			local prow = g_logictableMgr:getMutexParentIdxRow(arr[x])
			if prow then
				if g_uiEventRegistry[prow.name] then
					if g_uiEventRegistry[prow.name].open then
						g_uiEventRegistry[prow.name].open()
--						g_Loger:logMessage("  "..prow.name.." opened.")
					else
--						g_Loger:logMessage("  "..prow.name.." open fun not regist.")
					end
				else
					g_Loger:logError("  "..prow.name.." not regist.")
				end
			else
				g_Loger:logError("  "..tostring(arr[x]).." not in parentIdx table.")
			end
		end
	else
		g_Loger:logError("  "..name.." not in openUI table.")
	end
	
--	g_Loger:logMessage("end open "..name.." relation ui.")
end

--关闭一个窗体关联的所有窗体
function UIM_CloseMutexUI(name)
	local row = g_logictableMgr:getMutexUIRow(name)
	
--	g_Loger:logMessage("start close "..name.." mutex ui.")
	
	if row then
		local arr = {}
		SplitDigit(arr, row.mutex)
		local c = table.getn(arr)
		local x
		for x = 1, c do
			local prow = g_logictableMgr:getMutexParentIdxRow(arr[x])
			if prow then
				if g_uiEventRegistry[prow.name] then
					if g_uiEventRegistry[prow.name].close then
						g_uiEventRegistry[prow.name].close()
--						g_Loger:logMessage("  "..prow.name.." closed.")
					else
--						g_Loger:logMessage("  "..prow.name.." close fun not regist.")
					end
				else
					g_Loger:logError("  "..prow.name.." not regist.")
				end
			else
				g_Loger:logError("  "..tostring(arr[x]).." not in parentIdx table.")
			end
		end
	else
		g_Loger:logError("  "..name.." not in openUI table.")
	end
	
--	g_Loger:logMessage("end close "..name.." mutex ui.")
end

--执行关联窗体的打开和关闭
function UIM_DoOpenAndCloseUI(name)
	UIM_OpenRelationUI(name)
	UIM_CloseMutexUI(name)
end

--从父窗体索引表中获取窗口和父窗口
function UIM_GetWindowAndParent(name)
	local w = nil
	local wp = nil
	local prow = g_logictableMgr:getMutexParentIdxRowN(name)
	
	if prow then
		w = g_WinMgr:getWindow(prow.name)
		
		if prow.parent >= 0 then
			local pprow = g_logictableMgr:getMutexParentIdxRow(prow.parent)
			if pprow then
				wp = g_WinMgr:getWindow(pprow.name)
			end
		end
	else
--		g_Loger:logError(name.." not in parentIdx table.")
	end
	
	return wp, w
end

--根据父窗体索引表打开某个窗体
function UIM_OpenWindow(name)
	local wp, w = UIM_GetWindowAndParent(name)
	
	if nil == w then
		return
	end
	
	if wp then
		wp:addChildWindow(w)
	else
		g_GuiSystem:setGUISheet(w)
	end
end

--根据父窗体索引表关闭某个窗体
function UIM_CloseWindow(name)
	local wp, w = UIM_GetWindowAndParent(name)
	
	if nil == w then
		return
	end
	
	if wp then
		wp:removeChildWindow(w)
	else
		g_GuiSystem:setGUISheet(nil)
	end
end

--------------------------------------------
-- 从"n1,n2...nn"字符串中取出数字到数组中 --
--------------------------------------------
function SplitDigit(arr, str)
	local i
	local j = -1
	
	while 1 do
		i, j = string.find(str, "%d+", j + 1)
		
		if nil == i then
			break
		end
		
		table.insert(arr, tonumber(string.sub(str, i, j)))
	end
end

--------------------------------------
-- 注册所有界面窗口的打开和关闭函数 --
--------------------------------------
function UIM_RegistAllUIEvent()
	UIM_RegistUIEvent(g_GuideUINames.root, Goto_OpenDialog, Goto_CloseDialog)
	UIM_RegistUIEvent(g_ActionUINames.root, Action_openActionUI, Action_closeActionUI)
	UIM_RegistUIEvent(g_PlayerAttrUINames.root, MainUI_OpenPlayerAttr, MainUI_ClosePlayerAttr)
	UIM_RegistUIEvent(g_storageUIName.root, ReqOpenStorageUI, RemoveStorageUI)
	UIM_RegistUIEvent(g_PackUINames.root, ReqOpenPackUI, StorageUI_OnCloseStorage)
	UIM_RegistUIEvent(g_PlayerPropertyUINames.root, ReqOpenPlayerPropertyUI, PlayerPropertyUI_OnClosePlayerProperty)
	UIM_RegistUIEvent(g_houseInfo.root, ReqOpenHouseInfoUI, CloseHouseInfoUI)
	UIM_RegistUIEvent(g_MarketUINames.root, ReqOpenShopCenter, ShopCenter_Close)
	UIM_RegistUIEvent(g_OfflineMsgUIName.root, FLofflineMsgMgr_DoopenUI, FLofflineMsgMgr_DocloseUI)
	UIM_RegistUIEvent(g_FriendListUINames.root, OpenFriendListUI, MainUI_closeFriendlist)
	UIM_RegistUIEvent(g_SysConfigUINames.root, OpenConfigOptionUI, ConfigOption_Close)
	UIM_RegistUIEvent(g_mail.mailBox, OpenMailBox, CloseMailBox)
	UIM_RegistUIEvent(g_FunnyUINames.root, OpenFunnyUI, CloseFunnyUI)
	UIM_RegistUIEvent(g_SmallMapUINames.root, OpenSmallMapUI, CloseSmallMapUI)
	UIM_RegistUIEvent(g_BroadUINames.root, OpenBroadCastUI, CloseBroadCastUI)
	UIM_RegistUIEvent(g_GiftSelUINames.root, ReqOpenGiveGiftUI, ReqEndGiftService)
	UIM_RegistUIEvent(g_AddBookUINames.root, OpenAddressBook, CloseAddressBook)
	UIM_RegistUIEvent(g_ChatExpUINames.root, OpenChatExpressionUI, CloseChatExpressionUI)
	UIM_RegistUIEvent(g_TaskUINames.root, ReqOpenTaskUI, Task_Close)
	UIM_RegistUIEvent(g_HouseSalesUINames.root, ReqOpenHouseSalesUI, OnCloseHouseSalesUI)
	UIM_RegistUIEvent(g_NpcDlgUINames.root, nil, closeNPCDlg)
	UIM_RegistUIEvent(g_NPCMarketUINames.root, NpcShop_Open, NpcShop_Close)
	UIM_RegistUIEvent(g_ItemInteractUIName.root, IIT_reqOpenIITUI, IIT_closeIITUI)
	UIM_RegistUIEvent(g_CameraUINames.root, OpenCameraUI, CloseCameraUI)
	UIM_RegistUIEvent("Root/drifting", DBS_openDBSUI, DBS_closeDBSUI)
	UIM_RegistUIEvent("DefaultWindow", DBS_openDropBottleStartUI, DBS_closeDropBottleStartUI)
	UIM_RegistUIEvent("ordinary", DBS__openNormalBottleEditUI, DBS_closeNormalBottleEditUI)
	UIM_RegistUIEvent("contacts/n/", DBS__openContactBottleEditUI, DBS_closeContactBottleEditUI)
	UIM_RegistUIEvent("NewFrameWindow1", DBS__openMyBottleRecordsUI, DBS_closeMyBottleRecordsUI)
	UIM_RegistUIEvent("countkan", DBS_openBottleDetailInfoUI, DBS_closeBottleDetailInfoUI)
	UIM_RegistUIEvent(g_housesweepUINames.root, OnOpenBackGround, OnCloseBackGround)
	UIM_RegistUIEvent(g_works.bgWindow, OpenWorkUI, CloseWorkUI)
end

function UIM_OnLoad()
	UIM_RegistAllUIEvent()
end

UIM_OnLoad()