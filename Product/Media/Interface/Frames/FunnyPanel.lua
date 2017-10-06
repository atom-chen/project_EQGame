-----------------------------------------------------------------
--娱乐面板
--提供娱乐模块入口 如 农场 打工
--xiaoMing
--2010-08-18
-----------------------------------------------------------------
--数据目前写死
g_funnyType = {
				[1] = {image = "yule_1" ,imageSet = "Occupationicon", pushImage = "", hoverImage = "", funcName = "", name = "打工"},
				[2] = {image = "yule_2" ,imageSet = "Occupationicon", pushImage = "", hoverImage = "", funcName = "", name = "农场"},
				[3] = {image = "NewImage400" ,imageSet = "Button5", pushImage = "", hoverImage = "", funcName = "", name = "漂流瓶"}
				}
g_funnyBtnCount = 10

g_funnyFlag = false

local mCurPage = 1

-----------------------------------------------------------------
function RegisterFunnyType(type, functionName)
	g_funnyType[type].funcName = functionName
end
-----------------------------------------------------------------
function UpdateFunnyUI(page, pack)
	ClearFunnyUI()
	--显示的按钮
	local data = getData(pack, page , g_funnyBtnCount)
	local btnName
	for i, v in pairs(data) do
		btnName = g_funnyPanel.btnBaseName..i
		--g_WinMgr:getWindow(btnName):setProperty("HoverImage", v.hoverImage)
		--g_WinMgr:getWindow(btnName):setProperty("NormalImage", v.normalImage)
		--g_WinMgr:getWindow(btnName):setProperty("PushedImage", v.pushImage)
		SetWindowImage(btnName, v.imageSet, v.image)

		if "" ~= v.funcName then
			g_WinMgr:getWindow(btnName):subscribeEvent("MouseClick", v.funcName)
		end
		g_WinMgr:getWindow(btnName):subscribeEvent("MouseClick", "CloseFunnyUI")
		FSetText(g_funnyPanel.textBaseName..i, v.name)
	end

	FUpdatePageInfo(g_funnyPanel.pageText,page)
end
-----------------------------------------------------------------
function ClearFunnyUI()
	local btnName
	for i = 1, g_funnyBtnCount do
		btnName = g_funnyPanel.btnBaseName..i
		--g_WinMgr:getWindow(btnName):setProperty("HoverImage", "")
		--g_WinMgr:getWindow(btnName):setProperty("NormalImage", "")
		--g_WinMgr:getWindow(btnName):setProperty("PushedImage", "")
		--g_WinMgr:getWindow(btnName):setProperty("Image", "set:windows1 image:game_1")
		g_WinMgr:getWindow(btnName):removeEvent("MouseClick")
		g_WinMgr:getWindow(g_funnyPanel.textBaseName..i):setText("")
	end
end
-----------------------------------------------------------------
function OnNextPageFunny()
	if getTotalPage(g_funnyType, g_funnyBtnCount) <= mCurPage then
		return
	end

	mCurPage = mCurPage + 1

	UpdateFunnyUI(mCurPage, g_funnyType)
end
-----------------------------------------------------------------
function OnPrePageFunny()
	if 1 >= mCurPage then
		return
	end

	mCurPage = mCurPage - 1

	UpdateFunnyUI(mCurPage, g_funnyType)
end
-----------------------------------------------------------------
local firstOpenFunnyUI = true
function PopFunnyUI()
	if false == g_funnyFlag then
		OpenFunnyUI()
		UIM_DoOpenAndCloseUI(g_FunnyUINames.root)
		if firstOpenFunnyUI then
			do_task_script("script12")	-- (打开娱乐界面)执行新手任务脚本12(2011/6/15 add by hezhr)
			firstOpenFunnyUI = false
		end
	else
		CloseFunnyUI()
	end
end
-----------------------------------------------------------------
function OpenFunnyUI()
	InitFunnyUI()
	UpdateFunnyUI(mCurPage, g_funnyType)
	Common_injectEmptyEvt("zhuchuangkou")
	
	UIM_OpenWindow(g_FunnyUINames.root)
	g_funnyFlag = true
end

function CloseFunnyUI()
	UIM_CloseWindow(g_FunnyUINames.root)
	g_funnyFlag = false
end
-----------------------------------------------------------------
function OnLoadFunnyUI()
	g_WinMgr:getWindow(g_funnyPanel.closeBtn):subscribeEvent("Clicked", "CloseFunnyUI")
	g_WinMgr:getWindow(g_funnyPanel.prePage):subscribeEvent("MouseClick", "OnPrePageFunny")
	g_WinMgr:getWindow(g_funnyPanel.nextPage):subscribeEvent("MouseClick", "OnNextPageFunny")

	MainUI_ShieldClick(g_funnyUI)
end
-----------------------------------------------------------------
function InitFunnyUI()
	--应改成ID由程序内部控制，其他数据由外部填写
	mCurPage = 1
	RegisterFunnyType(1, "ReqStartWord")
	RegisterFunnyType(2, "reqEnterFarm")
	RegisterFunnyType(3, "gotoWishPool")
end
-----------------------------------------------------------------
function GetClickFunny(args)
	local clickWinName = CEGUI.toWindow(args):getName()
	local index

	for i = 1, g_funnyBtnCount do
		if nil ~= string.find(clickWinName, g_funnyPanel.btnBaseName..i) then
			index = i
			break
		end
	end

	if nil ~= index then
		index = FShopGetTableIndex(index, curPage, g_funnyType)

		if index > table.getn(g_funnyType) then
			return
		end
	end

end
-----------------------------------------------------------------
function funnySubscribeEvent(winName, funName)
	g_WinMgr:getWindow(btnName):subscribeEvent("MouseClick", v.funcName)
end
-----------------------------------------------------------------
function gotoWishPool()
	g_sceneMgr:reqTeleport(16,0)
end