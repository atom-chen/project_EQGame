---------
----有关农场提示信息的内容都写这
---------
--
m_ClickLandID = 0
--
local mposx = 0
local mposy = 0
local mindex = 0
local mposx_3d = 0
local mposy_3d = 0
local mposz_3d = 0
local mcanPickLandPosX = 0
local mcanPickLandPosY = 0
local mcanPickLandPosZ = 0
local mcanPickLandPosTable ={}
function FarmTip_setCoord(index,posx,posy,x_3d,y_3d,z_3d)
	--g_Loger:logDebug("posx..." ..posx)
	--g_Loger:logDebug("posy..." ..posy)
	--g_Loger:logDebug("mposx_3d..." ..x_3d)
	--g_Loger:logDebug("mposy_3d..." ..y_3d)
	--g_Loger:logDebug("mposz_3d..." ..z_3d)
	mindex = index
	mposx = posx
	mposy = posy
	mposx_3d = x_3d
	mposy_3d = x_3y
	mposz_3d = z_3d
end

--显示状态UI
function FarmTip_showCropStaUI()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local winW = g_FarmCropSta:getWidth():asAbsolute(screenW)
	local winH = g_FarmCropSta:getHeight():asAbsolute(screenH)
	local mousPos = {x, y}
	mousPos = g_MouseCur:getPosition()

	local posx,posy
	--鼠标跟随
	posx = mousPos.x - winW/2
	posy = mousPos.y - winH - 50
	--地块跟随
	--posx = mposx - winW/2
	--posy = mposy - winH-50
	if posx < 0 then
		posx =  0
	end
	if (posx + winW) > screenW then
		posx = screenW - winW
	end
	if posy < 0 then
		posy = 0
	end
	--if (posy + winH) > screenH then
	--	posy = screenH - winH
	--end
	g_FarmCropSta:setMousePassThroughEnabled(true)
	g_FarmCropSta:setPosition(CEGUI.UVector2(CEGUI.UDim(0,posx), CEGUI.UDim(0,posy)))
	g_FarmUI:addChildWindow(g_FarmCropSta)
	g_FarmCropSta:setZOrderingEnabled(true)
	g_FarmCropSta:moveToBack()
end

--
function FarmTip_removeCropSta()
	local ischild = g_FarmUI:isChild(g_FarmCropSta)
	if true == ischild then
		g_FarmCropSta:moveToFront()
		g_FarmUI:removeChildWindow(g_FarmCropSta)
	end
end

--
function FarmTip_getCropStaParam(CropCtrl)
	--当前状态数值处在的状态
	local cropCurStaStr = nil
	--下一状态值
	local nextSta = nil
	--下一状态值处在的状态
	local nextStaStr = nil
	--到下一状态剩余的时间（格式是xx天xx时。。。）
	local nextremainTime = nil
	--当前状态以后的状态到成熟剩余的时间
	local elseTotalTime = nil
	--当前状态到成熟剩余的时间
	local totalRemainTime = nil
	--作物从播种到成熟所需的总时间
	local totalRemainTime1 = nil
	--

	--获取作物物品表ID
	local cropTpltID = CropCtrl:getCropTpltID()
	if cropTpltID == nil then
		--g_Loger:logError(tostring(cropTpltID) .."is not exist!" )
		return
	end
	--获取作物物品表信息
	local cropTpltRow =  g_itemtableMgr:getItemRow(cropTpltID)
	
		local cropName = cropTpltRow.name
		local cropImgset = cropTpltRow.imageset
		local cropIcn = cropTpltRow.icon

	--获取作物种子表信息
	local cropSeedRow = g_FarmTableMgr:getSeedRow(cropTpltRow.subID)
	--获取作物阶段名字表
	local cropStaNameRow = g_FarmTableMgr:getCropStaNameRow(cropTpltRow.subID)
	
	--剩余产量
	local cropCurRemain = CropCtrl:getRemain()
		--g_Loger:logDebug("cropCurRemain..." ..cropCurRemain)
	--总共产量
	local cropOutput = CropCtrl:getOutput()
		--g_Loger:logDebug("cropOutput..." ..cropOutput)
	local cropPicked = CropCtrl:getPicked()
		--g_Loger:logDebug("cropPicked..." ..cropPicked)
		
	--作物当前状态值
	local cropCurSta = CropCtrl:getCurStage()
		--g_Loger:logDebug("cropCurSta**************..." ..cropCurSta)
	
	if cropCurSta == 0 then
		cropCurStaStr ="种子"
		nextSta = cropCurSta+ 1
		elseTotalTime = cropSeedRow.sprout_time - CropCtrl:getRemainTime()
	elseif cropCurSta == 1 then
		cropCurStaStr = FarmTip_curSta(cropCurSta,cropSeedRow,cropStaNameRow)
		nextSta = cropCurSta+ 1
		elseTotalTime = cropSeedRow.sprout_time - CropCtrl:getRemainTime()
	elseif cropCurSta == 2 then
			cropCurStaStr = FarmTip_curSta(cropCurSta,cropSeedRow,cropStaNameRow)
		nextSta = cropCurSta+ 1
		elseTotalTime = cropSeedRow.s_leaf_time - CropCtrl:getRemainTime()
	elseif cropCurSta == 3 then
		cropCurStaStr = FarmTip_curSta(cropCurSta,cropSeedRow,cropStaNameRow)
		nextSta = cropCurSta+ 1
		elseTotalTime = cropSeedRow.b_leaf_time - CropCtrl:getRemainTime()

	elseif cropCurSta == 4 then
		cropCurStaStr = FarmTip_curSta(cropCurSta,cropSeedRow,cropStaNameRow)
		nextSta = cropCurSta+ 1
		elseTotalTime = cropSeedRow.bloom_time - CropCtrl:getRemainTime()

	elseif cropCurSta == 5 then
		cropCurStaStr = FarmTip_curSta(cropCurSta,cropSeedRow,cropStaNameRow)
		nextSta = cropCurSta+ 1
		elseTotalTime = cropSeedRow.fruit_time - CropCtrl:getRemainTime()
	elseif cropCurSta == 6 then
		FarmTip_showGainSta(cropImgset,cropIcn,cropName,cropCurRemain,cropOutput,cropPicked)
		return
		--cropCurStaStr =cropStaNameRow.pick_name--"采摘"
		--nextSta = cropCurSta+ 1
		--elseTotalTime = 0
	elseif cropCurSta == 7 then
		FarmTip_showDieSta(cropImgset,cropIcn,cropName)
		return
		--cropCurStaStr ="枯死"
		--nextSta = cropCurSta+ 1
		--elseTotalTime = 0
	end
	totalRemainTime =  elseTotalTime
	totalRemainTime1 = cropSeedRow.fruit_time
	if nextSta == 1 then
		nextStaStr ="种子"
	elseif nextSta == 2 then
		--if cropSeedRow.s_leaf_time == cropSeedRow.sprout_time then
		--	nextSta = nextSta + 1
		--else
			nextStaStr =cropStaNameRow.s_leaf_name--"幼芽"
		--end
		
	elseif nextSta == 3 then
		--if cropSeedRow.b_leaf_time == cropSeedRow.s_leaf_time then
		--	nextSta = nextSta + 1
		--else
			nextStaStr =cropStaNameRow.b_leaf_name--"小叶"
		--end
		
	elseif nextSta == 4 then
		--if cropSeedRow.bloom_time == cropSeedRow.b_leaf_time then
		--	nextSta = nextSta + 1
		--else
			nextStaStr =cropStaNameRow.bloom_name--"大叶"
		--end
		
	elseif nextSta == 5 then
		--if cropSeedRow.fruit_time == cropSeedRow.bloom_time then
		--	nextSta = nextSta + 1
		--else
			nextStaStr =cropStaNameRow.fruit_name--"开花"
		--	break
		--end
		
	elseif nextSta == 6 then
		--if cropSeedRow.pick_time == cropSeedRow.fruit_time then
		--	nextSta = nextSta + 1
		--else
			nextStaStr =cropStaNameRow.pick_name--"成熟"
		--end
		
	elseif nextSta == 7 then
		nextStaStr ="枯死"
	end
	
	--当前健康值
	local cropCurHealth = CropCtrl:getHealth()
		--g_Loger:logDebug("cropCurHealth..." ..cropCurHealth)
		
	local cropCurHealthStr = nil
	if cropCurHealth>=81 and cropCurHealth <=100 then
		cropCurHealthStr = "优秀"
	elseif cropCurHealth >=61 and cropCurHealth <81 then
		cropCurHealthStr = "良好"
	else
		cropCurHealthStr = "差"
	end
	
	--获得从到下阶段的剩余总时间
	local cropRemainTime = CropCtrl:getRemainStime()

	--local remainYear = cropRemainTime.year
		--g_Loger:logDebug("remainYear..." ..remainYear)

	--local remainMonth = cropRemainTime.month
		--g_Loger:logDebug("remainMonth..." ..remainMonth)
		
	--剩余天数
	local remainDay = cropRemainTime.day
		--g_Loger:logDebug("remainDay..." ..remainDay)
	
	--剩余小时
	local remainhour = cropRemainTime.hour
		--g_Loger:logDebug("remainhour..." ..remainhour)
	
	--剩余分钟
	local remainMinute = cropRemainTime.minute
		--g_Loger:logDebug("remainMinute..." ..remainMinute)

	--剩余秒数
	local remainSec = cropRemainTime.second
		--g_Loger:logDebug("remainSec..." ..remainSec)
	
	--local namGB = EQGlobaFunc.UTF8ToGB2312Bylua(cropName)
	--	g_Loger:logDebug("namGB..." ..namGB)
	
	--统计到下一阶段的剩余时间
	if remainDay ~= 0 then
		nextremainTime = cropCurStaStr .."(" ..remainDay .."天" ..remainhour .."小时" ..nextStaStr ..")"--..remainMinute .."分" ..remainSec .."秒" ..nextStaStr ..")"
	elseif remainhour ~= 0 then
		nextremainTime = cropCurStaStr .."(" ..remainhour .."小时" ..remainMinute .."分钟" ..nextStaStr ..")"--..remainSec .."秒" ..nextStaStr ..")"
	elseif remainMinute ~= 0 then
		nextremainTime = cropCurStaStr .."(" ..remainMinute .."分钟" ..remainSec .."秒" ..nextStaStr ..")"
	else
		nextremainTime = cropCurStaStr .."(" ..remainSec .."秒" ..nextStaStr ..")"
	end

	FarmTip_showGrowSta(cropImgset,cropIcn,cropName,cropCurHealthStr,cropCurHealth,nextremainTime,totalRemainTime,totalRemainTime1)
end

--显示正在生长的状态
function FarmTip_showGrowSta(cropImgset,cropIcn,cropName,cropCurHealthStr,cropCurHealth,nextremainTime,totalRemainTime,totalRemainTime1)
	--g_Loger:logDebug("growing..." )
	FarmTip_showWin()
	g_WinMgr:getWindow("Root/farmpacebar/statebar"):setVisible(true)
	--图片框
	local iconWin = g_WinMgr:getWindow("Root/farmpacebar/image")
	iconWin:setProperty("Image", "set:" ..cropImgset .." image:" ..cropIcn)
	iconWin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,5), 
										CEGUI.UDim(0, 4)))
	local nameWin, stateWin, phaseWin = getFarmPacebarCtrl()
	--
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setVisible(true)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText(cropName, "#0000FF", false)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setPosition(44,19)
	--
	EQGlobaFunc.toMultiLineBox(stateWin:getName()):setVisible(true)
	EQGlobaFunc.toMultiLineBox(stateWin:getName()):setSpecSide("#FFFFFF", 1)
	EQGlobaFunc.toMultiLineBox(stateWin:getName()):addElementText("状态(" ..cropCurHealthStr ..")" ..cropCurHealth .."|100", "#000000", false)
	EQGlobaFunc.toMultiLineBox(stateWin:getName()):setPosition(5,43)
	--
	g_WinMgr:getWindow("Root/farmpacebar/statebar"):setVisible(true)
	CEGUI.toProgressBar(g_WinMgr:getWindow("Root/farmpacebar/statebar")):setProgress(cropCurHealth/100)
	g_WinMgr:getWindow("Root/farmpacebar/statebar"):setPosition(CEGUI.UVector2(CEGUI.UDim(0,5), 
																CEGUI.UDim(0, 58)))
	--
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setVisible(true)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setSpecSide("#FFFFFF", 1)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):addElementText(nextremainTime, "#000000", false)
	local width = EQGlobaFunc.toMultiLineBox(phaseWin:getName()):getFitWidth("simsun-10", nextremainTime)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setWidth(width)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setPosition(5,70)
	--
	g_WinMgr:getWindow("Root/farmpacebar/growbar"):setVisible(true)
	CEGUI.toProgressBar(g_WinMgr:getWindow("Root/farmpacebar/growbar")):setProgress(totalRemainTime/totalRemainTime1)
	g_WinMgr:getWindow("Root/farmpacebar/growbar"):setPosition(CEGUI.UVector2(CEGUI.UDim(0,5), 
																CEGUI.UDim(0, 85)))
end

--显示成熟的状态
function FarmTip_showGainSta(cropImgset,cropIcn,cropName,cropCurRemain,cropOutput,cropPicked)
	local gainStaStr
	if cropPicked == 1 then --已采摘过
		gainStaStr = "产" ..cropOutput .."剩" ..cropCurRemain .. "——已采摘"
	else
		gainStaStr = "产" ..cropOutput .."剩" ..cropCurRemain
	end
	local nameWin, stateWin, phaseWin = getFarmPacebarCtrl()
	EQGlobaFunc.toMultiLineBox(stateWin:getName()):setVisible(false)
	g_WinMgr:getWindow("Root/farmpacebar/statebar"):setVisible(false)
	
	--图片框
	g_WinMgr:getWindow("Root/farmpacebar/image"):setProperty("Image", "set:" ..cropImgset .." image:" ..cropIcn)
	g_WinMgr:getWindow("Root/farmpacebar/image"):setPosition(CEGUI.UVector2(CEGUI.UDim(0,5), 
															CEGUI.UDim(0, 35)))
	--名字条
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setVisible(true)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText(cropName, "#0000FF", false)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setPosition(44,50)
	--产剩
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setVisible(true)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setSpecSide("#FFFFFF", 1)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):addElementText(gainStaStr, "#000000", false)
	local width = EQGlobaFunc.toMultiLineBox(phaseWin:getName()):getFitWidth("simsun-10", gainStaStr)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setWidth(width)	
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setPosition(5,70)
	--
	g_WinMgr:getWindow("Root/farmpacebar/growbar"):setVisible(true)
	CEGUI.toProgressBar(g_WinMgr:getWindow("Root/farmpacebar/growbar")):setProgress(1/1)
	g_WinMgr:getWindow("Root/farmpacebar/growbar"):setPosition(CEGUI.UVector2(CEGUI.UDim(0,5), 
																CEGUI.UDim(0, 85)))
end

--显示枯死的状态
function FarmTip_showDieSta(cropImgset,cropIcn,cropName)
	--g_Loger:logDebug("have died..." )
	local nameWin, stateWin, phaseWin = getFarmPacebarCtrl()
	EQGlobaFunc.toMultiLineBox(stateWin:getName()):setVisible(false)
	g_WinMgr:getWindow("Root/farmpacebar/statebar"):setVisible(false)
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setVisible(false)
	g_WinMgr:getWindow("Root/farmpacebar/growbar"):setVisible(false)
	
	--
	g_WinMgr:getWindow("Root/farmpacebar/image"):setProperty("Image", "set:" ..cropImgset .." image:" ..cropIcn)
	g_WinMgr:getWindow("Root/farmpacebar/image"):setPosition(CEGUI.UVector2(CEGUI.UDim(0,5), 
															CEGUI.UDim(0, 35)))
	--
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText(cropName .."(已枯萎）", "#0000FF", false)
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setPosition(44,50)
	--g_WinMgr:getWindow("Root/farmpacebar/growbar"):setVisible(false)
	--EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setVisible(false)
	--EQGlobaFunc.toMultiLineBox(stateWin:getName()):setVisible(false)
	--g_WinMgr:getWindow("Root/farmpacebar/statebar"):setVisible(false)
--
end
--显示被隐藏的窗口
function FarmTip_showWin()
	g_WinMgr:getWindow("Root/farmpacebar/growbar"):setVisible(true)
	local nameWin, stateWin, phaseWin = getFarmPacebarCtrl()
	EQGlobaFunc.toMultiLineBox(phaseWin:getName()):setVisible(true)
end

--
function FarmTip_popMsgPaoPao(content)
	--if mindex ~= nil and mposx_3d ~= nil and mposy_3d ~= nil and mposz_3d ~= nil then
		EQGlobaFunc.showMsgPaoPao(mindex,mposx_3d,20,mposz_3d,content,"","")
	--end
end
--确定铲除作物
function FarmTip_confirmUpRoot(args)
	if mindex ~= nil then
		--g_Loger:logDebug("mindex******" ..mindex)
		g_req_hoeing.plot_index = mindex + 1
	end
	g_msgmgr:send(g_req_hoeing)
end
--弹出是否铲除的对话框
function FarmTip_popUpRootUI()
	local sysMsgRow = g_itemtableMgr:getSysMsgRow(60)
	MsgBox_createMsgBox(g_FarmUI,65535, "铲除作物",sysMsgRow.content, "FarmTip_confirmUpRoot", "MsgBox_cancelFunc")
end
--弹出对空地操作的消息提示
function FarmTip_popEmptyLandUI()
	local sysMsgRow = g_itemtableMgr:getSysMsgRow(49)
	FarmTip_popMsgPaoPao(sysMsgRow.content)
end
-- 获取农场进度条相关控件：name，money，title控件(hezhr)
function getFarmPacebarCtrl()
	local farmpacebar = g_WinMgr:getWindow("Root/farmpacebar")
	local name = g_WinMgr:getWindow("Root/farmpacebar/name")
	local state = g_WinMgr:getWindow("Root/farmpacebar/state")
	local phase = g_WinMgr:getWindow("Root/farmpacebar/phase")
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local width = farmpacebar:getWidth():asAbsolute(screenW)

	if nil == name then
		name = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Root/farmpacebar/name")
		EQGlobaFunc.toMultiLineBox(name:getName()):setSize(width-44, 15)
		EQGlobaFunc.toMultiLineBox(name:getName()):setFont("simsun-10")
		farmpacebar:addChildWindow(name)
	end
	if nil == state then
		state = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Root/farmpacebar/state")
		EQGlobaFunc.toMultiLineBox(state:getName()):setSize(width-5, 15)
		EQGlobaFunc.toMultiLineBox(state:getName()):setFont("simsun-10")
		farmpacebar:addChildWindow(state)
	end
	if nil == phase then
		phase = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Root/farmpacebar/phase")
		EQGlobaFunc.toMultiLineBox(phase:getName()):setSize(240, 83)
		EQGlobaFunc.toMultiLineBox(phase:getName()):setFont("simsun-10")
		farmpacebar:addChildWindow(phase)
	end
	EQGlobaFunc.toMultiLineBox(name:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(state:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(phase:getName()):clearUp()
	return name, state, phase
end
--
function lua_updateFarmInfo(dexp,x3d,y3d,z3d,farmidx,exp,lv)
	local farmLevelRow = g_FarmTableMgr:getFarmLevelRow(lv)
	local nextExp = farmLevelRow.next_exp

	--if g_objmgr:isMainPlayer(acc) then
		--g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp/number"):setText(exp .."/" ..nextExp)
		--CEGUI.toProgressBar(g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp")):setProgress (exp/nextExp)
		if g_FarmMgr:getAction() ~= farmCurStaEnum.CS_FARM_ONEKEYHARVEST then
			FarmTip_showDexp(dexp,mposx_3d,mposy_3d,mposz_3d,farmidx)
		else
			FarmTip_showDexp(dexp,x3d,y3d,z3d,farmidx)
			--(dexp,mcanPickLandPosX,mcanPickLandPosY,mcanPickLandPosZ)
		end
	--else
		--g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp/number"):setText(exp .."/" ..nextExp)
		--CEGUI.toProgressBar(g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp")):setProgress (exp/nextExp)
	--end
end
--
function lua_updateFarmInfo1(acc,exp,lv,dexp,x3d,y3d,z3d,farmidx)
	local farmLevelRow = g_FarmTableMgr:getFarmLevelRow(lv)
	local nextExp = farmLevelRow.next_exp

	if g_objmgr:isMainPlayer(acc) then
		g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp/number"):setText(exp .."/" ..nextExp)
		--local nameWin, moneyWin, _ = getPlayerInfoCtrl()
		--EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
		--local len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", tostring(acc))
		--EQGlobaFunc.toMultiLineBox(nameWin:getName()):setWidth(len)
		--EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText(tostring(acc), "#000000", false)
		--EQGlobaFunc.toMultiLineBox(moneyWin:getName()):setSpecSide("#FFFFFF", 1)
		--EQGlobaFunc.toMultiLineBox(moneyWin:getName()):addElementText("金币：" ..tostring(g_FarmMgr:getFarmMoney()), "#000000", false)	
		if g_FarmMgr:getAction() ~= farmCurStaEnum.CS_FARM_ONEKEYHARVEST then
			FarmTip_showDexp(dexp,mposx_3d,mposy_3d,mposz_3d,farmidx)
		else
			FarmTip_showDexp(dexp,x3d,y3d,z3d,farmidx)
			--(dexp,mcanPickLandPosX,mcanPickLandPosY,mcanPickLandPosZ)
		end
	else
		g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp/number1"):setText(exp .."/" ..nextExp)
	end
end
--显示增加经验的UI动画
function FarmTip_showDexp(exp,x_3d,y_3d,z_3d,farmidx)
	--g_Loger:logDebug("********dexp*********" ..exp)
	--g_Loger:logDebug("********x_3d*********" ..x_3d)
	--g_Loger:logDebug("********y_3d*********" ..y_3d)
	--g_Loger:logDebug("********z_3d*********" ..z_3d)
	--
	EQGlobaFunc.showDexpDynamicUI(farmidx,exp,x_3d,20,z_3d)
end
--判断当前阶段名
function FarmTip_curSta(cropCurSta,cropSeedRow,cropStaNameRow)
	local cropCurStaStr1
	if cropCurSta == 1 then--幼芽
		cropCurStaStr1 = cropStaNameRow.sprout_name--"种子"
	end
	if cropCurSta == 2 then--小叶子
		cropCurStaStr1 =cropStaNameRow.s_leaf_name--"幼芽"
	end
	if cropCurSta == 3 then--大叶子
		if cropSeedRow.s_leaf_time == cropSeedRow.sprout_time then --没有小叶子
			cropCurStaStr1 =cropStaNameRow.s_leaf_name--"幼芽"
		else
			cropCurStaStr1 =cropStaNameRow.b_leaf_name--"小叶子"
		end
	end
	if cropCurSta == 4 then--开花
		if cropSeedRow.b_leaf_time == cropSeedRow.s_leaf_time then --没有大叶子
			if cropSeedRow.s_leaf_time == cropSeedRow.sprout_time then --没有大叶子也没小叶子
				cropCurStaStr1 =cropStaNameRow.s_leaf_name--"幼芽"
			else
				cropCurStaStr1 =cropStaNameRow.b_leaf_name--"小叶子"
			end
		else
			cropCurStaStr1 = cropStaNameRow.bloom_name--"大叶子"
		end
	end
	if cropCurSta == 5 then--成熟
		if cropSeedRow.bloom_time == cropSeedRow.b_leaf_time then --没有开花阶段
			if cropSeedRow.b_leaf_time == cropSeedRow.s_leaf_time then --没有开花也没有大叶子
				if cropSeedRow.s_leaf_time == cropSeedRow.sprout_time then --没有开花也没有大叶子也没小叶子
					cropCurStaStr1 =cropStaNameRow.s_leaf_name--"幼芽"
				else
					cropCurStaStr1 =cropStaNameRow.b_leaf_name--"小叶子"
				end
			else
				cropCurStaStr1 = cropStaNameRow.bloom_name--"大叶子"
			end
		else
			cropCurStaStr1 = cropStaNameRow.fruit_name--"开花"
		end
	end
	return cropCurStaStr1
end
--
function FarmTip_setCanPickLandPos(x,y,z)
	mcanPickLandPosX,mcanPickLandPosY,mcanPickLandPosZ = x,y,z
end
--
function FarmTip_reqPlot()
	g_msgmgr:send(g_req_assart_plot)
end
--------------------------------------------------
--
function FarmTip_updateExpInfo(lv,exp, nextExp)
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/LevelImage/Level"):setText(lv)
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp/number"):setText(exp .."/" ..nextExp)
	CEGUI.toProgressBar(g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp")):setProgress (exp/nextExp)
end
--------------------------------------------------
function FarmTip_set_ClickLand_Id(index)
	m_ClickLandID = index
end