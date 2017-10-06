-----------------------------------------------------------------
--农场系统设置
--xiaoMing
-----------------------------------------------------------------
g_FSettingFlag = false
local g_TextSize = 12

local cbRes = CEGUI.toCombobox(g_WinMgr:getWindow("MSystemSetting/bg/Check01"))
local cbAA = CEGUI.toCombobox(g_WinMgr:getWindow("MSystemSetting/bg/Check02"))
local cbShadow = CEGUI.toCombobox(g_WinMgr:getWindow("MSystemSetting/bg/Check03"))
local sdSysVoice = CEGUI.toSlider(g_WinMgr:getWindow("MSystemSetting/bg/Slider1"))
local sdBgVoice = CEGUI.toSlider(g_WinMgr:getWindow("MSystemSetting/bg/Slider2"))
local idRes,idAA, idShadow
local mVolumeType = { bg = 0, sys = 1 }
local mChanged = false
-----------------------------------------
--
function FSettingUIOnLoad()
	--弹出对话框
	g_GameEventMgr:subscribeEvent("POP_FARM_SETTING", "FPopSSettingPanel")


	g_WinMgr:getWindow(g_farmSetting.settingBtn):subscribeEvent("Clicked", "FReqSetSetting")
	g_WinMgr:getWindow(g_farmSetting.setDefaultBtn):subscribeEvent("Clicked", "FReqSetDefault")
	g_WinMgr:getWindow(g_farmSetting.closeBtn):subscribeEvent("Clicked", "FSettingClose")
	cbRes:subscribeEvent("ListSelectionChanged", "FTextChange")
	
	sdSysVoice:subscribeEvent("ValueChanged", "OnFarmSysVolumeChanged")
	sdBgVoice:subscribeEvent("ValueChanged", "OnFarmBgVolumeChanged")
	
	sdSysVoice:setMaxValue(100)
	sdBgVoice:setMaxValue(100)
	sdSysVoice:setCurrentValue(EQGlobaClass.getVoiceMgr():getVolume(mVolumeType.sys))
	sdBgVoice:setCurrentValue(EQGlobaClass.getVoiceMgr():getVolume(mVolumeType.bg))

	--设置输入框长度等限制
	local win
	win = g_WinMgr:getWindow(g_farmSetting.waterText)
	win:subscribeEvent("TextChanged", "FTextChange")
	CEGUI.toEditbox(win):setMaxTextLength(g_TextSize)

	win = g_WinMgr:getWindow(g_farmSetting.weedText)
	CEGUI.toEditbox(win):setMaxTextLength(g_TextSize)
	win:subscribeEvent("TextChanged", "FTextChange")

	win = g_WinMgr:getWindow(g_farmSetting.wormText)
	CEGUI.toEditbox(win):setMaxTextLength(g_TextSize)
	win:subscribeEvent("TextChanged", "FTextChange")

	win = g_WinMgr:getWindow(g_farmSetting.putGrassText)
	CEGUI.toEditbox(win):setMaxTextLength(g_TextSize)
	win:subscribeEvent("TextChanged", "FTextChange")

	win = g_WinMgr:getWindow(g_farmSetting.putPestText)
	CEGUI.toEditbox(win):setMaxTextLength(g_TextSize)
	win:subscribeEvent("TextChanged", "FTextChange")
	
	--视频选项
	EQGlobaFunc.createComboBoxTextItem(cbRes:getName(), "800 x 600", 0,"")
	EQGlobaFunc.createComboBoxTextItem(cbRes:getName(), "全屏", 1,"")
	cbRes:setReadOnly(true)
	
	EQGlobaFunc.createComboBoxTextItem(cbAA:getName(), "无", 0,"")
	EQGlobaClass.getEQOgreSys():getConfigOption():insertAAOption(cbAA:getName())
	cbAA:setReadOnly(true)
	
	EQGlobaFunc.createComboBoxTextItem(cbShadow:getName(), "开", 0,"")
	EQGlobaFunc.createComboBoxTextItem(cbShadow:getName(), "关", 1,"")
	cbShadow:setReadOnly(true)
	
	local config_option = EQGlobaClass.getEQOgreSys():getConfigOption()
	
	if config_option:isFullScreen() == true then
		cbRes:setItemSelectState(1,true)
	else
		cbRes:setItemSelectState(0,true)
	end
	
	local i
	for i=0,cbAA:getItemCount()-1 do
		local item = cbAA:getListboxItemFromIndex(i)
		if config_option:getAA() == item:getID() then
			cbAA:setItemSelectState(i,true)
		end
	end
	
	if config_option:getShadow() == 0 then
		cbShadow:setItemSelectState(1,true)
	else
		cbShadow:setItemSelectState(0,true)
	end
end

function OnFarmSysVolumeChanged(args)
	EQGlobaClass.getVoiceMgr():setVolume(mVolumeType.sys, sdSysVoice:getCurrentValue())
	mChanged = true
end

function OnFarmBgVolumeChanged(args)
	EQGlobaClass.getVoiceMgr():setVolume(mVolumeType.bg, sdBgVoice:getCurrentValue())
	mChanged = true
end

-----------------------------------------
--请求农场设置
function FReqSetting()
	if false == g_FSettingFlag then
		g_FarmMgr:reqSetting()
	else
		FSettingClose()
	end
end
-----------------------------------------
--请求设置农场内容
function FReqSetSetting()
	g_FarmMgr:reqSetSetting()
	
	ManorConfig_OK()
end
-----------------------------------------
--恢复默认值
function FReqSetDefault()
	g_FarmMgr:reqSettingDefault()
	
	cbRes:setItemSelectState(0,true)
	cbAA:setItemSelectState(0,true)
	cbShadow:setItemSelectState(1,true)
	sdSysVoice:setCurrentValue(80)
	sdBgVoice:setCurrentValue(80)
	mChanged = true
	ManorConfig_OK()
end
-----------------------------------------
function FSettingClose()
	--modify by hjx 有时无法输入问题
	--g_FarmUI:removeChildWindow(g_FarmSystemSetUI)		
	g_FarmSystemSetUI:setVisible(false)	
	g_FSettingFlag = false
	mChanged = false
end
-----------------------------------------
function FPopSSettingPanel()
	if false == g_FSettingFlag then
		FsysSet_closeOtherUI()
		--modify by hjx 有时无法输入问题
		if g_FarmUI:isChild(g_FarmSystemSetUI) then
			g_FarmSystemSetUI:setVisible(true)	
		else
			g_FarmUI:addChildWindow(g_FarmSystemSetUI)
			g_FarmSystemSetUI:setVisible(true)	
		end
				
		Common_injectEmptyEvt("MSystemSetting")
		g_FSettingFlag = true
		g_FarmMgr:setSettingFlag(false)
		SetWindowCenter(g_FarmSystemSetUI)
		
		idRes = cbRes:getSelectedItem():getID()
		idAA = cbAA:getSelectedItem():getID()
		idShadow = cbShadow:getSelectedItem():getID()
	end
end
-----------------------------------------
--文本改变
function FTextChange()
	--
	g_FarmMgr:setSettingFlag(true)
end

--
function FsysSet_closeOtherUI()
	if true== g_FStorageFlag then
		FRemoveStoragePanel()
	end
	if true == g_FShopFlag then
		CloseFarmShopUI()
	end
	if true == g_FarmDecoFlag then
		Dec_closeDecUI()
	end
	if true == g_FPlayerInfoFlag then
		FClosePlayerInfoUI()
	end
	if true == mOpenFlag then
		CloseUpgradeLandUI()
	end
end

-----------------------------------------
--确定
-----------------------------------------
function ManorConfig_OK()
	local config_option = EQGlobaClass.getEQOgreSys():getConfigOption()
	local needRestart = false
	
	local id = cbRes:getSelectedItem():getID()
	if id ~= idRes then
		if id == 0 then
			config_option:changeResolution(800, 600, true)
		elseif id == 1 then
			config_option:setFullScreen()
		end
		mChanged = true
		idRes = id
	end
	
	id = cbAA:getSelectedItem():getID()
	if id ~= idAA then
		config_option:changeAntiAliasing(id)
		mChanged = true
		idAA = id
		needRestart = true
	end
	
	id = cbShadow:getSelectedItem():getID()
	if id ~= idShadow then
		if id == 0 then
			config_option:changeShadow(34)
		elseif id == 1 then
			config_option:changeShadow(0)
		end
		mChanged = true
		idShadow = id
	end
	
	if mChanged == true then
		config_option:save()
	end
	
	if needRestart == true then
		MsgBox_createMsgBox(g_FarmUI,0,"","部分设置需要重启才能生效.", "MsgBox_cancelFunc", "")
	end
end
