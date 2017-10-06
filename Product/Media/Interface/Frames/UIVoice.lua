-----------------------------------------
-- 文件说明	：
-- 作者		：
-- 日期		：
-----------------------------------------

g_special_voice_map = {
	screenshot = 1,
	viewscale = 2,
	paging = 3,
	change_scene = 4,
	up_down_floor = 5,
	new_mail = 6,
	send_mail = 7,
	visitor = 8,
	pay = 9
					}
					
g_paging_uis = {
	"FitmodeMktRoot/LeftGo",
	"FitmodeMktRoot/RightGo",
	"NewFrameWindow1/zuo",
	"NewFrameWindow1/zuo1",
	"ChatExpressionUI/pageup",
	"ChatExpressionUI/pagedown",
	"freindlist/fenyeanniu",
	"freindlist/fenyeanniu1",
	"friend_list_sys/control/Tab 1/pg_up",
	"friend_list_sys/control/Tab 1/pg_down"
				}

--注册ui_sound.xml表里的所有窗体声音事件
function InitAllUIVoiceEvent()
	g_logictableMgr:getUIVoiceRowByIdxStart()
	
	while true do
		local row = g_logictableMgr:getUIVoiceRowByIdxNext()
		
		if nil == row then
			break
		end
		
		RegistUIVoiceEvent(row.name)
	end
end

--注册某个窗体的声音事件
function RegistUIVoiceEvent(winName)
	local win = g_WinMgr:getWindow(winName)
		
	if win then
		win:subscribeEvent("MouseEnter", "OnUIVoiceMouseEnter")
		win:subscribeEvent("MouseClick", "OnUIVoiceMouseClick")
	else
		g_Loger:logError("RegistUIVoiceEvent: window '"..winName.."' not exist.")
	end
end

function OnUIVoiceMouseEnter(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local row = g_logictableMgr:getUIVoiceRow(winName)
	
	if row then
		EQGlobaClass.getVoiceMgr():play(row.hovervid, false)
	else
		g_Loger:logError("OnWindowMouseEnter: window '"..winName.."' not regist ui voice event.")
	end
end

function OnUIVoiceMouseClick(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local row = g_logictableMgr:getUIVoiceRow(winName)
	
	if row then
		EQGlobaClass.getVoiceMgr():play(row.clickvid, false)
	else
		g_Loger:logError("OnUIVoiceMouseClick: window '"..winName.."' not regist ui voice event.")
	end
end

function UIVoice_OnLoad()
	InitAllUIVoiceEvent()
end

UIVoice_OnLoad()

--

function PlaySpecialVoice(idx)
	local row = g_logictableMgr:getSpecialVoiceRow(idx)
	
	if row then
		EQGlobaClass.getVoiceMgr():play(row.voiceid, false)
	else
		g_Loger:logError("PlaySpecialVoice: not find special void idx:"..tostring(idx))
	end
end

function OnZoomView()
	PlaySpecialVoice(g_special_voice_map.viewscale)
end

function OnUpDownFloor()
	PlaySpecialVoice(g_special_voice_map.up_down_floor)
end

function OnRecvNewMail()
	PlaySpecialVoice(g_special_voice_map.new_mail)
end

function OnHouseWelcome()
	PlaySpecialVoice(g_special_voice_map.visitor)
end
