-----------------------------------------
-- 文件说明	：EQ农场防沉迷
-- 作者		：何展然
-- 修改		：
-- 日期		：11/6/17
-----------------------------------------

-- 累计上线时间(s)
local m_time = -1
local m_hour = 60*60	-- 1小时(3600s)
-- 每1000毫秒触发一次
local m_wallowTimer = createTimer(1000, 0, 0)
-- m_temp无实际用处
local m_temp, m_msgBoxName = {}, ""
-- 头像hint
local m_hintVec = EQGlobaClass.createDynamicCtrlVector()
local m_hint = nil
local m_onlineTime = nil
local m_tip = nil
-- 头像闪烁
local m_twinkleState = 0	-- 0.不闪烁;1.慢闪烁;2.快闪烁
local m_twinkleFlag = 1		-- 1.第一帧;2.第二帧;3.第三帧...
local m_twinkleStart = 0
-- 事件注册
g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):subscribeEvent("MouseEnter", "WallowTime_enterTouxiang")
g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):subscribeEvent("MouseLeave", "WallowTime_leaveTouxiang")
-- 提示内容
local m_content = 
{
	"您已累计在线1小时，请合理安排游戏时间。",	-- (1)
	"您已累计在线2小时，请合理安排游戏时间。",	-- (2)
	"您已累计在线3小时，您已进入疲劳游戏时间，游戏收益变为正常的50%，请您下线休息。",	-- (3)
	"您已累计在线3.5小时，您已进入疲劳游戏时间，游戏收益变成正常的50%，请您下线休息。",	-- (4)
	"您已累计在线4小时，您已进入疲劳游戏时间，游戏收益变成正常的50%，请您下线休息。",	-- (5)
	"您已累计在线4.5小时，您已进入疲劳游戏时间，游戏收益变成正常的50%，请您下线休息。",	-- (6)
	"您当前已经进入疲劳游戏时间，游戏收益变成正常的50%，请您合理安排游戏时间。",	-- (7)
	"您已累计在线5小时，您已进入不健康游戏时间，游戏收益变为0，请下线休息。",	-- (8)
	"抵制不良游戏，拒绝盗版游戏。注意自我保护，谨防受骗上当。适度游戏益脑，沉迷游戏伤身。合理安排时间，享受健康生活。",	-- (9)
}

-----------------------------------------
-- 初始头像hint
function WallowTime_initHint()
	m_hint = g_WinMgr:getWindow("WallowTime/hint")
	if nil == m_hint then
		m_hint = g_WinMgr:createWindow("EQHomeLook/ItemhintFrame", "WallowTime/hint")
		m_hint:setWidth(CEGUI.UDim(0, 250))	-- 控件18个汉字宽度
		m_hintVec:initialise("WallowTime/hint")
	end
	m_onlineTime = g_WinMgr:getWindow("WallowTime/hint/time")
	if nil == m_onlineTime then
		m_onlineTime = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "WallowTime/hint/time")
		EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):setCols("#FFFFFF")
		EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):setSide("#045098", 1)
		EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):setWidth(240)	-- 18个汉字宽度
	end
	m_tip = g_WinMgr:getWindow("WallowTime/hint/tip")
	if nil == m_tip then
		m_tip = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "WallowTime/hint/tip")
		EQGlobaFunc.toMultiLineBox(m_tip:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(m_tip:getName()):setCols("#05E5FF")
		EQGlobaFunc.toMultiLineBox(m_tip:getName()):setSide("#043644", 1)
		EQGlobaFunc.toMultiLineBox(m_tip:getName()):setWidth(240)	-- 18个汉字宽度
	end
end
WallowTime_initHint()
-----------------------------------------
-- 显示头像hint
function WallowTime_showHint(x, y)
	EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(m_tip:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):addElementText(WallowTime_formatTime(m_time), false)
	local height = EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(m_onlineTime:getName()):setHeight(height)
	EQGlobaFunc.toMultiLineBox(m_tip:getName()):addElementText(WallowTime_getTip(m_time), false)
	height = EQGlobaFunc.toMultiLineBox(m_tip:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(m_tip:getName()):setHeight(height)	
	m_hintVec:addControl(m_onlineTime:getName())
	m_hintVec:addControl(m_tip:getName())
	m_hintVec:controlTypeset()
	m_hint:setPosition(CEGUI.UVector2(CEGUI.UDim(0, x), CEGUI.UDim(0, y)))
	g_FarmUI:addChildWindow(m_hint)
end
-----------------------------------------
-- 隐藏头像hint
function WallowTime_hideHint()
	g_FarmUI:removeChildWindow(m_hint)
	m_hintVec:removeAllControl()
end
-----------------------------------------
-- 事件触发函数,放在在addTimerListener前
function WallowTime_event(timer, param1, param2)
	m_time = m_time + 1
	-- g_Loger:logMessage("--- now online time: "..m_time.."(s) ---")
	WallowTime_setTwinkle(m_time)
	WallowTime_checkTime(m_time)
end
-- 添加计时器
addTimerListener(m_wallowTimer, TE_TIMER, WallowTime_event)
-----------------------------------------
-- 开始沉迷计时器
function WallowTime_handle(seconds)
	if seconds >= 0 then	-- 玩家需要防沉迷
		m_time = seconds
		-- g_Loger:logMessage("--- last online time: "..m_time.."(s) ---")
		WallowTime_setTwinkle(m_time)
		if WallowTime_checkTime(m_time) then
		else
			if m_time>3*m_hour and m_time<5*m_hour then	-- 在3-5个小时间
				m_temp, m_msgBoxName = MsgBox_createMsgBoxExA(g_FarmUI, 200, "消息提示", m_content[7], {}, "MsgBoxEx_close", "")
			end
			if m_time > 5*m_hour then	-- 大于5小时
				m_temp, m_msgBoxName = MsgBox_createMsgBoxExA(g_FarmUI, 200, "消息提示", m_content[8], {}, "MsgBoxEx_close", "")		
			end
		end
		startTimer(m_wallowTimer)
	end
end
-----------------------------------------
-- 时间检查
function WallowTime_checkTime(seconds)
	local tip = nil
	if m_hour == seconds then	-- 1个小时
		tip = m_content[1]
	end
	if 2*m_hour == seconds then	-- 2个小时
		tip = m_content[2]
	end
	if 3*m_hour == seconds then	-- 3个小时
		tip = m_content[3]
	end
	if 3.5*m_hour == seconds then -- 3.5个小时
		tip = m_content[4]
	end
	if 4*m_hour == seconds then	--4个小时
		tip = m_content[5]
	end
	if 4.5*m_hour == seconds then	-- 4.5个小时
		tip = m_content[6]
	end
	if 5*m_hour == seconds then	-- 5个小时
		tip = m_content[8]
	end
	if seconds>5*m_hour and 0==seconds%15 then	-- 5个小时后,每隔15分钟
		tip = m_content[8]
	end
	if nil ~= tip then
		if nil ~= g_WinMgr:getWindow(m_msgBoxName) then
			Common_setModalState(m_msgBoxName, false)
			g_WinMgr:destroyWindow(m_msgBoxName)
		end
		m_temp, m_msgBoxName = MsgBox_createMsgBoxExA(g_FarmUI, 200, "消息提示", tip, {}, "MsgBoxEx_close", "")
		return true
	end
	return false
end
-----------------------------------------
-- 时间格式化
function WallowTime_formatTime(seconds)
	local hour = math.floor(seconds/m_hour)
	local minu = math.floor((seconds%m_hour)/60)
	return "累计在线时间："..hour.."小时"..minu.."分钟"
end
-----------------------------------------
-- 根据时间获取提示内容
function WallowTime_getTip(seconds)
	local tip = m_content[9]
	if seconds>=3*m_hour and seconds<3.5*m_hour then	-- >=3小时,<3.5小时
		tip = m_content[3]
	end
	if seconds>=3.5*m_hour and seconds<4*m_hour then	-- >=3.5小时,<4小时
		tip = m_content[4]
	end
	if seconds>=4*m_hour and seconds<4.5*m_hour then	-- >=4小时,<4.5小时
		tip = m_content[5]
	end
	if seconds>=4.5*m_hour and seconds<5*m_hour then	-- >=4.5小时,<5小时
		tip = m_content[6]
	end
	if seconds >= 5*m_hour then	-- >=5小时
		tip = m_content[8]
	end
	return tip
end
-----------------------------------------
-- 鼠标移进头像
function WallowTime_enterTouxiang(args)
	if -1 == m_time then
		return
	end
	local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
	WallowTime_showHint(rect.right, rect.bottom)
end
-----------------------------------------
-- 鼠标移出头像
function WallowTime_leaveTouxiang(args)
	if -1 == m_time then
		return
	end
	WallowTime_hideHint()
end
-----------------------------------------
-- 设置头像闪烁
function WallowTime_setTwinkle(seconds)
	if seconds>=3*m_hour and seconds<5*m_hour then	-- >=3小时,<5小时
		m_twinkleState = 1	-- 慢闪烁
		m_twinkleStart = EQGlobaFunc.GetTickCountByLua()
	end
	if seconds >= 5*m_hour then	-- >=5小时
		m_twinkleState = 2	-- 快闪烁
		m_twinkleStart = EQGlobaFunc.GetTickCountByLua()
	end
end
-----------------------------------------
-- 改变头像图标
function WallowTime_changeTouxiang(dif)
	if (EQGlobaFunc.GetTickCountByLua()-m_twinkleStart) >= dif then
		m_twinkleStart = EQGlobaFunc.GetTickCountByLua()
		if 1 == m_twinkleFlag then
			m_twinkleFlag = m_twinkleFlag + 1
			g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):setProperty("ImageColours", "tl:FFF1D2D2 tr:FFF1D2D2 bl:FFF1D2D2 br:FFF1D2D2")
		elseif 2 == m_twinkleFlag then
			m_twinkleFlag = m_twinkleFlag + 1
			g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):setProperty("ImageColours", "tl:FFF1AFAF tr:FFF1AFAF bl:FFF1AFAF br:FFF1AFAF")
		elseif 3 == m_twinkleFlag then
			m_twinkleFlag = m_twinkleFlag + 1
			g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):setProperty("ImageColours", "tl:FFF19292 tr:FFF19292 bl:FFF19292 br:FFF19292")
		elseif 4 == m_twinkleFlag then
			m_twinkleFlag = m_twinkleFlag + 1
			g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):setProperty("ImageColours", "tl:FFF26A6A tr:FFF26A6A bl:FFF26A6A br:FFF26A6A")
		elseif 5 == m_twinkleFlag then
			m_twinkleFlag = m_twinkleFlag + 1
			g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):setProperty("ImageColours", "tl:FFF19292 tr:FFF19292 bl:FFF19292 br:FFF19292")
		elseif 6 == m_twinkleFlag then
			m_twinkleFlag = 1
			g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):setProperty("ImageColours", "tl:FFF1AFAF tr:FFF1AFAF bl:FFF1AFAF br:FFF1AFAF")
		end
	end
end
-----------------------------------------
-- 头像闪烁
function WallowTime_twinkle()
	if 1 == m_twinkleState then	-- 慢闪烁
		WallowTime_changeTouxiang(200)
	end
	if 2 == m_twinkleState then	-- 快闪烁
		WallowTime_changeTouxiang(20)
	end
end
-----------------------------------------
