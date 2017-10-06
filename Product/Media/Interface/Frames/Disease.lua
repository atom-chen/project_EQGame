------------------------------------------
-- 文件说明	疾病模块
-- 作者		：xiaoMing
-- 日期		：10/07/22
-----------------------------------------
--	dt_normal = 1,
--	dt_white = 2,
--	dt_yellow = 3,
--	dt_purple = 4,
--	dt_red = 5
-----------------------------------------
local mImgCmbCtrl = EQGlobaClass.createImageCombineCtrl()

g_WinMgr:getWindow(g_diseaseSpecialEventUINames.image):subscribeEvent("MouseClick", "OnDiseaseCloseWindow")

MainUI_ShieldClick(g_DiseaseSpecialEventUI)

--暂时只在Player个人信息面板中显示
function DiseaseRegEvent()
	g_GameEventMgr:subscribeEvent("UPDATE_DISEASE", "UpdateDiseaseMsg")
end
-----------------------------------------
function UpdateDiseaseState(state, type)
	local str = "疾病"
	if 1 == state then
		str = "健康" 
	end
	
	local slotWin = g_WinMgr:getWindow(g_playerInfoName[type].diseaseWindow)
	--目前用1 2 3 4 5表示 。合理的做法是将c++的枚举透露给LUA
	SetDiseaseStateImage(g_playerInfoName[type].diseaseWindow, state)

	--设置文本
	FSetText(g_playerInfoName[type].diseaseTxt, str)
end
-----------------------------------------
function SetDiseaseStateImage(winName, healthState)
	local slotWin = g_WinMgr:getWindow(winName)
	
	if nil == slotWin then
		return
	end

	local icomName = ""
	
	if 1 == healthState then
		str = "健康"
		icomName = "set:Button5 image:NewImage341"
	elseif 2 == healthState then
		icomName = "set:Button5 image:NewImage342"
	elseif 3 == healthState then
		icomName = "set:Button5 image:NewImage343"
	elseif 4 == healthState then
		icomName = "set:Button5 image:NewImage344"
	else
		icomName = "set:Button5 image:NewImage345"
	end
	
	--更新图片
	slotWin:setProperty("Image",icomName)
end
-----------------------------------------
function UpdateDiseaseMsg()	
	local healtyState = g_mainplayer:getHealtyState()
	
	SetDiseaseStateImage("DFwnd_MainRoot/tip/jibingzhuangtai", healtyState)
	--动态信息 暂时不用 没需求了
	--if 1 ~= state then
	--	AddDynamicMsg("disease", "common1", "zhentong")
	--else
	--	RemoveDynamicMsg("disease")
	--end
	
	MainUI_updateUserInfo()
	Goto_LoadUserData()
end

function OnDiseaseSpecialEvent(eid)
	local row = g_logictableMgr:getDiseaseSpecialEventRow(eid)
	
	if nil == row then
		return
	end
	
	local i
	local imgs = Split(row.images, ",")
	local n = table.getn(imgs)
	
	mImgCmbCtrl:clear()
	mImgCmbCtrl:init(n/2, n/2, g_diseaseSpecialEventUINames.image)
	for i = 1, n do
		local img = Split(imgs[i], " ")
		local st = Split(img[1], ":")
		local it = Split(img[2], ":")
		
		--g_Loger:logError(imgs[i])
		--g_Loger:logError("  "..img[1])
		--g_Loger:logError("  "..img[2])
		
		mImgCmbCtrl:addImage(st[2], it[2])
	end
	
	g_MainUIWindows:addChildWindow(g_DiseaseSpecialEventUI)
	PlayFadeAnimation(g_diseaseSpecialEventUINames.root, 2000, 0, 1, 0)
	
	--g_Loger:logError("id:"..tostring(row.id))
	--g_Loger:logError("pid:"..tostring(row.pid))
	--g_Loger:logError("images:"..tostring(row.images))
end

function OnDiseaseCloseWindow(args)
	g_MainUIWindows:removeChildWindow(g_DiseaseSpecialEventUI)
end
