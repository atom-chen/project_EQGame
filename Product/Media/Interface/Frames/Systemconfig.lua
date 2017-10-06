-----------------------------------------
-- 文件说明	：游戏配置
-- 作者		：lf
-- 修改		：
-- 日期		：10/12/27
-----------------------------------------
local cbRes = CEGUI.toCombobox(g_WinMgr:getWindow("Systemconfig/Systemconfig/windows2/Tab1/box1"))
local cbAA = CEGUI.toCombobox(g_WinMgr:getWindow("Systemconfig/Systemconfig/windows2/Tab1/box2"))
local cbShadow = CEGUI.toCombobox(g_WinMgr:getWindow("Systemconfig/Systemconfig/windows2/Tab1/box3"))
local cbParticle = CEGUI.toCombobox(g_WinMgr:getWindow("Systemconfig/Systemconfig/windows2/Tab1/box4"))
local sdSysVoice = CEGUI.toSlider(g_WinMgr:getWindow("Systemconfig/Systemconfig/windows2/Tab 3/Slider1"))
local sdBgVoice = CEGUI.toSlider(g_WinMgr:getWindow("Systemconfig/Systemconfig/windows2/Tab 3/Slider2"))
local idRes,idAA,idShadow,idParticle
local mVolumeType = { bg = 0, sys = 1 }
local mChanged = false;

-----------------------------------------
--脚本加载后执行
-----------------------------------------
function ConfigOption_Onload()
	MainUI_ShieldClick(g_ConfigOption)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_System"):subscribeEvent("Clicked", "ConfigOption_Open")
	
	g_WinMgr:getWindow("Systemconfig/Systemconfig__auto_closebutton__"):subscribeEvent("Clicked", "ConfigOption_Close")
	--g_WinMgr:getWindow("Systemconfig/Systemconfig/windows1/display"):subscribeEvent("MouseClick", "ConfigOption_Display")
	--g_WinMgr:getWindow("Systemconfig/Systemconfig/windows1/effect"):subscribeEvent("MouseClick", "ConfigOption_Effect")
	--g_WinMgr:getWindow("Systemconfig/Systemconfig/windows1/sound"):subscribeEvent("MouseClick", "ConfigOption_Sound")
	--g_WinMgr:getWindow("Systemconfig/Systemconfig/windows1/gameset"):subscribeEvent("MouseClick", "ConfigOption_Set")
	g_WinMgr:getWindow("Systemconfig/Systemconfig/windows1/exit"):subscribeEvent("MouseClick", "ConfigOption_Exit")
	g_WinMgr:getWindow("Systemconfig/Systemconfig/windows1/yes"):subscribeEvent("MouseClick", "ConfigOption_OK")
	g_WinMgr:getWindow("Systemconfig/Systemconfig/windows1/no"):subscribeEvent("MouseClick", "ConfigOption_Close")
	
	sdSysVoice:subscribeEvent("ValueChanged", "OnSysVolumeChanged")
	sdBgVoice:subscribeEvent("ValueChanged", "OnBgVolumeChanged")
	
	sdSysVoice:setMaxValue(100)
	sdBgVoice:setMaxValue(100)
	sdSysVoice:setCurrentValue(EQGlobaClass.getVoiceMgr():getVolume(mVolumeType.sys))
	sdBgVoice:setCurrentValue(EQGlobaClass.getVoiceMgr():getVolume(mVolumeType.bg))
	
	EQGlobaFunc.createComboBoxTextItem(cbRes:getName(), "800 x 600", 0,"")
	EQGlobaFunc.createComboBoxTextItem(cbRes:getName(), "全屏", 1,"")
	cbRes:setReadOnly(true)
	
	EQGlobaFunc.createComboBoxTextItem(cbAA:getName(), "无", 0,"")
	EQGlobaClass.getEQOgreSys():getConfigOption():insertAAOption(cbAA:getName())
	cbAA:setReadOnly(true)
	
	EQGlobaFunc.createComboBoxTextItem(cbShadow:getName(), "开", 0,"")
	EQGlobaFunc.createComboBoxTextItem(cbShadow:getName(), "关", 1,"")
	cbShadow:setReadOnly(true)
	
	EQGlobaFunc.createComboBoxTextItem(cbParticle:getName(), "低", 0,"")
	EQGlobaFunc.createComboBoxTextItem(cbParticle:getName(), "中", 1,"")
	EQGlobaFunc.createComboBoxTextItem(cbParticle:getName(), "高", 2,"")
	cbParticle:setReadOnly(true)
	
	ConfigOption_RefreshUI()
end

function ConfigOption_RefreshUI()
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
	
	if config_option:getParticleQuality() == 1 then
		cbParticle:setItemSelectState(0,true)
	elseif config_option:getParticleQuality() == 5 then
		cbParticle:setItemSelectState(1,true)
	elseif config_option:getParticleQuality() == 10 then
		cbParticle:setItemSelectState(2,true)
	end
end

function OnSysVolumeChanged(args)
	EQGlobaClass.getVoiceMgr():setVolume(mVolumeType.sys, sdSysVoice:getCurrentValue())
	mChanged = true
end

function OnBgVolumeChanged(args)
	EQGlobaClass.getVoiceMgr():setVolume(mVolumeType.bg, sdBgVoice:getCurrentValue())
	mChanged = true;
end

-----------------------------------------
--打开
-----------------------------------------
function ConfigOption_Open()
	if g_MainUIWindows:isChild(g_ConfigOption) == true then
		ConfigOption_Close()
	else
		OpenConfigOptionUI()
		UIM_DoOpenAndCloseUI(g_SysConfigUINames.root)
		
		ConfigOption_RefreshUI()
	end
end

function OpenConfigOptionUI()
	UIM_OpenWindow(g_SysConfigUINames.root)
	
	idRes = cbRes:getSelectedItem():getID()
	idAA = cbAA:getSelectedItem():getID()
	idShadow = cbShadow:getSelectedItem():getID()
	idParticle = cbParticle:getSelectedItem():getID()
end

function CloseConfigOptionUI()
	UIM_CloseWindow(g_SysConfigUINames.root)
end

-----------------------------------------
--关闭
-----------------------------------------
function ConfigOption_Close()
	CloseConfigOptionUI()
	mChanged = false;
end

-----------------------------------------
--退出
-----------------------------------------
function ConfigOption_Exit()
	ConfigOption_Close()
	OnExit()
end

-----------------------------------------
--确定
-----------------------------------------
function ConfigOption_OK()
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
	end
	
	id = cbAA:getSelectedItem():getID()
	if id ~= idAA then
		config_option:changeAntiAliasing(id)
		mChanged = true
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
	end
	
	id = cbParticle:getSelectedItem():getID()
	if id ~= idParticle then
		if id == 0 then
			config_option:changeParticleQuality(1)
		elseif id == 1 then
			config_option:changeParticleQuality(5)
		elseif id == 2 then
			config_option:changeParticleQuality(10)
		end
		mChanged = true
	end
	
	if mChanged == true then
		config_option:save()
	end
	
	ConfigOption_Close()
	
	if needRestart == true then
		MsgBox_createMsgBox(g_MainUIWindows,0,"","部分设置需要重启才能生效.", "MsgBox_cancelFunc", "")
	end
end