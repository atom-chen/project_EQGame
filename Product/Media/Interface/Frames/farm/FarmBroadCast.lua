-----------------------------------------
-- 文件说明	：农场公告系统
-- 作者		：何展然
-- 修改		：
-- 日期		：11/1/12
-----------------------------------------

-- AtTime公告配置，可更改
local m_atTimeFrame = "BroadCast/AT/Frame"		-- AtTime公告边框控件名称
local m_atTimeContent = "BroadCast/AT/Content"	-- AtTime公告内容控件名称
local m_atTimeWidth = 247	-- AtTime公告宽度(最多可显示16个汉字)
local m_atTimeHeight = 30	-- AtTime公告高度
-- GM公告配置，可更改
local m_GMFrame = "BroadCast/GM/Frame"		-- GM公告边框控件名称
local m_GMContent = "BroadCast/GM/Content"	-- GM公告内容控件名称
local m_GMWidth = 247	-- GM公告宽度(最多可显示16个汉字)
local m_GMHeight = 30	-- GM公告高度
-- AtTime和GM共用配置
local m_delayTime = 30000	-- 每条公告延迟时间(毫秒)
local m_spaceStep = 1	-- 滚动的距离步进(像素)
local m_timeStep = 30	-- 滚动的时间步进(毫秒)
-- 农场公告配置，可更改
local m_farmTxt = "BroadCast/Farm/TxtName"
local m_maxCount = 3	-- 每页的条数
local m_curPage = 1	-- 当前页
local m_totalPage = 1	-- 总页数
local m_reqFarm = false	-- 请求农场公告
-- 优先级别
local m_curPriorityAT = 0	-- 当前播放的定点公告的优先级(1,2,3,4,5从高到低)
-- 当前接收的定点公告信息
local m_curInfoAT = {id=0, content="", playTimes=0}
-- 5种优先级别的定点公告表
local m_atTimeTable = {{}, {}, {}, {}, {}}
local m_farmTable = {}
-- 公告动画
local m_animateWin = nil	-- 公告动画窗口
local m_animate = EQGlobaClass.createAnimate()	-- 公告动画
--事件注册
g_WinMgr:getWindow("broadcast__auto_closebutton__"):subscribeEvent("Clicked", "BroadCast_FarmClose")
g_WinMgr:getWindow("broadcast/fan1"):subscribeEvent("Clicked", "BroadCast_FarmPageUp")
g_WinMgr:getWindow("broadcast/fanye2"):subscribeEvent("Clicked", "BroadCast_FarmPageDown")
--屏蔽鼠标穿透
MainUI_ShieldClick(g_BroadCastUI)
--
local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
-----------------------------------------
--公告设置坐标
function BroadCast_SetPostion()
	local xPos = (g_GuiSystem:getRenderer():getRect():getWidth()-247)/2	-- 公告在界面水平居中
	local yPos = 50	-- 公告在界面垂直位置
	
	g_WinMgr:getWindow("BroadCast/AT/Frame"):setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))
	g_WinMgr:getWindow("BroadCast/GM/Frame"):setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))
	g_WinMgr:getWindow("BroadCast/animate"):setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos-32), CEGUI.UDim(0, yPos-1)))
end
-----------------------------------------
--公告动画初始化
function BroadCast_AnimateInit(animate)
	animate:initAnimate(40, 32, 32)
	for i = 1, 18 do
		local image = tostring(i)
		if i < 10 then
			image = "0"..i
		end
		animate:addAnimateFrame("broadcast01", image)
	end
	m_animateWin = g_WinMgr:getWindow("BroadCast/animate")
	if nil == m_animateWin then
		m_animateWin = g_WinMgr:createWindow("EQHomeLook/AnimateBox", "BroadCast/animate")		
		m_animateWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))
		m_animateWin:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 100), CEGUI.UDim(0, 100)))
		EQGlobaFunc.toAnimateBox(m_animateWin:getName()):addAnimate(animate)
	end
	m_animateWin:setVisible(false)
	g_FarmUI:addChildWindow(m_animateWin)	
end
-- 该调用放在函数BroadCast_AnimateInit()定义后面
BroadCast_AnimateInit(m_animate)
-----------------------------------------
--设置动画是否显示
function BroadCast_ATGM_PlayState(flag)
	m_animateWin:setVisible(flag)
	if true == flag then
		EQGlobaFunc.toAnimateBox(m_animateWin:getName()):play(true)
	else
		EQGlobaFunc.toAnimateBox(m_animateWin:getName()):stop()
	end
end
-----------------------------------------
--创建AtTime公告控件
function createATCtrl()
	local xPos = (g_GuiSystem:getRenderer():getRect():getWidth()-m_GMWidth)/2	-- 公告在界面水平居中
	local yPos = 50	-- 公告在界面垂直位置
	
	local frame = g_WinMgr:getWindow(m_atTimeFrame)
	if nil == frame then
		frame = g_WinMgr:createWindow("EQHomeLook/NoticeFrame", m_atTimeFrame)
		frame:setSize(CEGUI.UVector2(CEGUI.UDim(0, m_atTimeWidth), CEGUI.UDim(0, m_atTimeHeight)))
		frame:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))
		MainUI_ShieldClick(frame)
		frame:setVisible(false)
		g_FarmUI:addChildWindow(frame)
	end
	
	local content = g_WinMgr:getWindow(m_atTimeContent)
	if nil == content then
		content = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", m_atTimeContent)
		EQGlobaFunc.toMultiLineBox(m_atTimeContent):setFont("simsun-11")	-- 设置字体
		EQGlobaFunc.toMultiLineBox(m_atTimeContent):setCols("#FA5134")	-- 设置颜色
		EQGlobaFunc.toMultiLineBox(m_atTimeContent):setSide("#FFFFFF", 2)	-- 设置描边
		EQGlobaFunc.toMultiLineBox(m_atTimeContent):setAlign("AT_CENTER_VER")	-- 设置垂直居中
		EQGlobaFunc.toMultiLineBox(m_atTimeContent):setSize(m_atTimeWidth, m_atTimeHeight)	-- 设置初始大小
		EQGlobaFunc.toMultiLineBox(m_atTimeContent):setPosition(0, 0)
		frame:addChildWindow(content)
	end
	
	return frame, content
end
-----------------------------------------
--单条AtTime公告开始播放时刻
function AT_playBegin()
	g_WinMgr:getWindow(m_atTimeFrame):setVisible(true)
end
-----------------------------------------
--AtTime公告重置函数
function AT_resetFunc(text)
	g_WinMgr:getWindow(m_atTimeFrame):setVisible(false)
	EQGlobaFunc.toMultiLineBox(m_atTimeContent):clearUp()
	EQGlobaFunc.toMultiLineBox(m_atTimeContent):setPosition(0, 0)
	EQGlobaFunc.toMultiLineBox(m_atTimeContent):addElementText(text, false)	-- 最多可容纳53、42个汉字
	local width = EQGlobaFunc.toMultiLineBox(m_atTimeContent):getFitWidth("simsun-11", text)
	EQGlobaFunc.toMultiLineBox(m_atTimeContent):setMaxSize(width, m_atTimeHeight)
	EQGlobaFunc.toMultiLineBox(m_atTimeContent):setWidth(width)
end
-----------------------------------------
--AtTime单条公告结束播放时刻
function AT_playEnd()
	g_WinMgr:getWindow(m_atTimeFrame):setVisible(false)
end
-----------------------------------------
--初始AtTime公告系统
function BoardCast_AT_Init()
	createATCtrl()
	EQGlobaClass.getBroadCastSystem():setPlayParamsAT("RIGHT_LEFT", m_delayTime, m_delayTime, m_spaceStep, m_timeStep)
	EQGlobaClass.getBroadCastSystem():setCalledFuncNameAT("AT_playBegin", "AT_resetFunc", "AT_playEnd")	
end
-----------------------------------------
--此调用放在函数BoardCast_AT_Init()定义后面
BoardCast_AT_Init()
-----------------------------------------
--创建GM公告控件
function createGMCtrl()
	local xPos = (g_GuiSystem:getRenderer():getRect():getWidth()-m_GMWidth)/2	-- 公告在界面水平居中
	local yPos = 50	-- 公告在界面垂直位置
	
	local frame = g_WinMgr:getWindow(m_GMFrame)
	if nil == frame then
		frame = g_WinMgr:createWindow("EQHomeLook/NoticeFrame", m_GMFrame)
		frame:setSize(CEGUI.UVector2(CEGUI.UDim(0, m_GMWidth), CEGUI.UDim(0, m_GMHeight)))
		frame:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))
		MainUI_ShieldClick(frame)
		frame:setVisible(false)
		g_FarmUI:addChildWindow(frame)
	end
	
	local content = g_WinMgr:getWindow(m_GMContent)
	if nil == content then
		content = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", m_GMContent)
		EQGlobaFunc.toMultiLineBox(m_GMContent):setFont("simsun-11")	-- 设置字体
		EQGlobaFunc.toMultiLineBox(m_GMContent):setCols("#FA5134")	-- 设置颜色
		EQGlobaFunc.toMultiLineBox(m_GMContent):setSide("#FFFFFF", 2)	-- 设置描边
		EQGlobaFunc.toMultiLineBox(m_GMContent):setAlign("AT_CENTER_VER")	-- 设置垂直居中
		EQGlobaFunc.toMultiLineBox(m_GMContent):setSize(m_GMWidth, m_GMHeight)	-- 设置初始大小
		EQGlobaFunc.toMultiLineBox(m_GMContent):setPosition(0, 0)
		frame:addChildWindow(content)
	end
	
	return frame, content
end
-----------------------------------------
--单条GM公告开始播放时刻
function GM_playBegin()
	g_WinMgr:getWindow(m_GMFrame):setVisible(true)
end
-----------------------------------------
--GM公告重置函数
function GM_resetFunc(text)
	g_WinMgr:getWindow(m_GMFrame):setVisible(false)
	EQGlobaFunc.toMultiLineBox(m_GMContent):clearUp()
	EQGlobaFunc.toMultiLineBox(m_GMContent):setPosition(0, 0)
	EQGlobaFunc.toMultiLineBox(m_GMContent):addElementText(text, false)
	local width = EQGlobaFunc.toMultiLineBox(m_GMContent):getFitWidth("simsun-11", text)
	EQGlobaFunc.toMultiLineBox(m_GMContent):setMaxSize(width, m_GMHeight)
	EQGlobaFunc.toMultiLineBox(m_GMContent):setWidth(width)
end
-----------------------------------------
--GM单条公告结束播放时刻
function GM_playEnd()
	g_WinMgr:getWindow(m_GMFrame):setVisible(false)
end
-----------------------------------------
--初始GM公告系统
function BoardCast_GM_Init()
	createGMCtrl()
	EQGlobaClass.getBroadCastSystem():setPlayParamsGM("RIGHT_LEFT", m_delayTime, m_delayTime, m_spaceStep, m_timeStep)
	EQGlobaClass.getBroadCastSystem():setCalledFuncNameGM("GM_playBegin", "GM_resetFunc", "GM_playEnd")	
end
-----------------------------------------
--此调用放在函数BoardCast_GM_Init()定义后面
BoardCast_GM_Init()
BroadCast_SetPostion()
-----------------------------------------
--创建农场公告控件
function BroadCast_CreateFarmCtrl()
	local txtWin = g_WinMgr:getWindow(m_farmTxt)
	if nil == txtWin then
		txtWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", m_farmTxt)
		EQGlobaFunc.toMultiLineBox(m_farmTxt):setFont("simsun-10")	-- 设置字体
		EQGlobaFunc.toMultiLineBox(m_farmTxt):setCols("#EBEBEB")	-- 设置颜色
		EQGlobaFunc.toMultiLineBox(m_farmTxt):setSize(275, 135)	-- 设置大小
		EQGlobaFunc.toMultiLineBox(m_farmTxt):setScrollbarVisible("SP_OLD", true, 1)	-- 设置滚动条
		txtWin:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 15), CEGUI.UDim(0, 32)))	-- 设置位置
		txtWin:subscribeEvent("HyperLink_MouseEnter", "BroadCast_OpenHyperLink")
		txtWin:subscribeEvent("HyperLink_MouseLeave", "BroadCast_CloseHyperLink")			
		g_WinMgr:getWindow("broadcast"):addChildWindow(txtWin)
	end
	return txtWin
end
-----------------------------------------
--此调用放在函数BroadCast_CreateFarmCtrl定义后面
BroadCast_CreateFarmCtrl()
-----------------------------------------
--显示农场公告界面
function BroadCast_addFarmUI()
	if false == m_reqFarm then
		EQGlobaClass.getBroadCastSystem():reqBroadCast(5)
		m_reqFarm = true
	end
	m_curPage = 1
	EQGlobaFunc.SetText("broadcast/yeshu1", tostring(m_curPage))
	if m_totalPage >= 10 then
		EQGlobaFunc.SetText("broadcast/yeshu2", "/"..tostring(m_totalPage))
	else
		EQGlobaFunc.SetText("broadcast/yeshu2", "/ "..tostring(m_totalPage))
	end
	BroadCast_FarmUpdateMsg(m_farmTable, m_curPage, m_maxCount)
	g_FarmUI:addChildWindow(g_BroadCastUI)
end
-----------------------------------------
--关闭农场公告界面
function BroadCast_FarmClose(args)
	g_FarmUI:removeChildWindow(g_BroadCastUI)
end
-----------------------------------------
--对公告内容进行排序
function BroadCast_TableSort(a, b)
	-- 截取日期字符串
	local aparts, ay, am, ad, ah, an, as = cutString(a.startTime, ",", 7)
	local bparts, by, bm, bd, bh, bn, bs = cutString(b.startTime, ",", 7)
	if nil==aparts or nil==bparts then
		return true
	end
	ay, am, ad = tonumber(aparts[1]), tonumber(aparts[2]), tonumber(aparts[3])
	ah, an, as = tonumber(aparts[4]), tonumber(aparts[5]), tonumber(aparts[6])
	by, bm, bd = tonumber(bparts[1]), tonumber(bparts[2]), tonumber(bparts[3])
	bh, bn, bs = tonumber(bparts[4]), tonumber(bparts[5]), tonumber(bparts[6])
	-- 比较日期
	local tm1 = tonumber(string.format("%04d%02d%02d%02d%02d%02d", ay, am, ad, ah, an, as))
	local tm2 = tonumber(string.format("%04d%02d%02d%02d%02d%02d", by, bm, bd, bh, bn, bs))
	return tm1 > tm2
end
-----------------------------------------
--翻页更新消息
function BroadCast_FarmUpdateMsg(data, curPage, maxCount)
	EQGlobaFunc.SetText("broadcast/yeshu1", tostring(curPage))
	EQGlobaFunc.toMultiLineBox(m_farmTxt):setScrollbarPosition(0)
	EQGlobaFunc.toMultiLineBox(m_farmTxt):clearUp()
	local page = getData(data, curPage, maxCount)
	for i=1, table.getn(page) do
		local parts, y, m, d = cutString(page[i].startTime, ",", 7)	-- 截取日期字符串
		if nil ~= parts then
			y, m, d = tonumber(parts[1]), tonumber(parts[2]), tonumber(parts[3])
			local text = y.."年"..m.."月"..d.."日 "..page[i].content
			EQGlobaFunc.toMultiLineBox(m_farmTxt):addElementText("", true)
			g_mlbParse:getData(text, m_farmTxt)		
		end	
	end
end
-----------------------------------------
--向上翻页
function BroadCast_FarmPageUp(args)
	if 1 == m_curPage then	-- 第一页
		return
	end
	m_curPage = m_curPage - 1
	BroadCast_FarmUpdateMsg(m_farmTable, m_curPage, m_maxCount)
end
-----------------------------------------
--向下翻页
function BroadCast_FarmPageDown(args)
	if m_totalPage == m_curPage then	-- 最后页
		return
	end
	m_curPage = m_curPage + 1
	BroadCast_FarmUpdateMsg(m_farmTable, m_curPage, m_maxCount)	
end
-----------------------------------------
--处理公告消息
function BroadCast_HandleMsg(id, type, content, gmPlayTimes, priority, showSeconds, startTime)
	local msg = {}
	msg["id"] = id
	msg["type"] = type
	msg["content"] = content
	msg["gmPlayTimes"] = gmPlayTimes
	msg["priority"] = priority
	msg["showSeconds"] = showSeconds
	msg["startTime"] = startTime

	if 2 == type then	-- 定点播放公告
		BroadCast_AtTime(msg)
	elseif 4 == type then	-- GM公告
		BroadCast_GM(msg)
	elseif 5 == type then	-- 农场公告类型
		table.insert(m_farmTable, 1, msg)	-- 在前面插入消息
		table.sort(m_farmTable, BroadCast_TableSort)
		m_totalPage = getTotalPage(m_farmTable, m_maxCount)	-- 计算总页数
		if m_totalPage >= 10 then
			EQGlobaFunc.SetText("broadcast/yeshu2", "/"..tostring(m_totalPage))
		else
			EQGlobaFunc.SetText("broadcast/yeshu2", "/ "..tostring(m_totalPage))
		end
		BroadCast_FarmUpdateMsg(m_farmTable, m_curPage, m_maxCount)
	else	-- type=1,3(自动播放公告和GM公告这里不处理，在BroadCast.lua里面处理)
		return
	end
end
-----------------------------------------
--系统定点发送的公告
function BroadCast_AtTime(msg)
	m_curInfoAT.id = msg.id
	m_curInfoAT.content = msg.content
	m_curInfoAT.playTimes = msg.gmPlayTimes
	if 1 == msg.priority then	-- 优先级最高
		table.insert(m_atTimeTable[1], m_curInfoAT)
	elseif 2 == msg.priority then
		table.insert(m_atTimeTable[2], m_curInfoAT)
	elseif 3 == msg.priority then
		table.insert(m_atTimeTable[3], m_curInfoAT)
	elseif 4 == msg.priority then
		table.insert(m_atTimeTable[4], m_curInfoAT)
	elseif 5 == msg.priority then
		table.insert(m_atTimeTable[5], m_curInfoAT)
	end
	BroadCast_AtTime_PlayHighPriority(msg.priority)
end
-----------------------------------------
--播放高优先级
function BroadCast_AtTime_PlayHighPriority(priority)
	local playState = EQGlobaClass.getBroadCastSystem():playStateAT()
	if 0 == playState then	-- 没有消息
		BroadCast_AT_SaveNewMsg()
		BroadCast_AT_Play()
	elseif 1 == playState then	-- 有消息，播放状态
		if priority == m_curPriorityAT then	-- 接收优先级=当前优先级
			BroadCast_AT_SaveNewMsg()
		elseif priority < m_curPriorityAT then	-- 接收优先级>当前优先级
			BroadCast_AT_ClearMsg()
			EQGlobaClass.getBroadCastSystem():stopAT()	-- 停止播放当前较低优先级
		else	-- 接收优先级<当前优先级
			return
		end
	elseif 2 == playState then	-- 有消息，停止状态(被GM停止)
		if priority == m_curPriorityAT then	-- 接收优先级=当前优先级
			BroadCast_AT_SaveNewMsg()
		elseif priority < m_curPriorityAT then	-- 接收优先级>当前优先级
			BroadCast_AT_ClearMsg()
			EQGlobaClass.getBroadCastSystem():saveOldMsgAT()
			BroadCast_AT_SaveNewMsg()
		else	-- 接收优先级<当前优先级
			return
		end
	elseif 3 == playState then -- 有消息，播放状态，处于延时期间
		if priority == m_curPriorityAT then	-- 接收优先级=当前优先级
			BroadCast_AT_SaveNewMsg()
		elseif priority < m_curPriorityAT then	-- 接收优先级>当前优先级
			BroadCast_AT_ClearMsg()
			EQGlobaClass.getBroadCastSystem():saveOldMsgAT()
			BroadCast_AT_SaveNewMsg()
			BroadCast_AT_Play()
		else	-- 接收优先级<当前优先级
			return
		end		
	end
	m_curPriorityAT = priority	-- 重设当前优先级
end
-----------------------------------------
--清除当前优先级消息
function BroadCast_AT_ClearMsg()
	if 1 == m_curPriorityAT then
		m_atTimeTable[1] = {}
	elseif 2 == m_curPriorityAT then
		m_atTimeTable[2] = {}
	elseif 3 == m_curPriorityAT then
		m_atTimeTable[3] = {}
	elseif 4 == m_curPriorityAT then
		m_atTimeTable[4] = {}
	elseif 5 == m_curPriorityAT then
		m_atTimeTable[5] = {}
	end
end
-----------------------------------------
--保存被停止播放的公告消息
function BroadCast_AT_SaveOldMsg(id, content, playTimes)
	--停止播放，保存当前播放的公告的信息（公告id，播放次数），以待下次再次播放
	local info = {id=0, content="", playTimes=0}
	info.id = id
	info.content = content
	info.playTimes = playTimes
	if 1 == m_curPriorityAT then
		table.insert(m_atTimeTable[1], info)
	elseif 2 == m_curPriorityAT then
		table.insert(m_atTimeTable[2], info)
	elseif 3 == m_curPriorityAT then
		table.insert(m_atTimeTable[3], info)
	elseif 4 == m_curPriorityAT then
		table.insert(m_atTimeTable[4], info)
	elseif 5 == m_curPriorityAT then
		table.insert(m_atTimeTable[5], info)
	end
end
-----------------------------------------
--定点公告播放停止后存入新优先级消息
function BroadCast_AT_SaveNewMsg()
	EQGlobaClass.getBroadCastSystem():saveMsgAT(m_curInfoAT.id, m_curInfoAT.content, m_curInfoAT.playTimes)
end
-----------------------------------------
--播放定点公告
function BroadCast_AT_Play()
	EQGlobaClass.getBroadCastSystem():playAT(m_atTimeFrame, m_atTimeContent)
end
-----------------------------------------
--更新定点公告消息
function BroadCast_AT_UpdateMsg(broadCastTable)
	for i=1, table.getn(broadCastTable) do
		local id = broadCastTable[i].id
		local content = broadCastTable[i].content
		local playTimes = broadCastTable[i].playTimes
		EQGlobaClass.getBroadCastSystem():saveMsgAT(id, content, playTimes)
	end
	broadCastTable = {}
	BroadCast_AT_Play()
end
-----------------------------------------
--定点公告播放结束
function BroadCast_AT_PlayEnd()
	BroadCast_AT_ClearMsg()
	if 0 ~= table.getn(m_atTimeTable[1]) then
		BroadCast_AT_UpdateMsg(m_atTimeTable[1])
		m_curPriorityAT = 1
	elseif 0 ~= table.getn(m_atTimeTable[2]) then
		BroadCast_AT_UpdateMsg(m_atTimeTable[2])
		m_curPriorityAT = 2
	elseif 0 ~= table.getn(m_atTimeTable[3]) then
		BroadCast_AT_UpdateMsg(m_atTimeTable[3])
		m_curPriorityAT = 3
	elseif 0 ~= table.getn(m_atTimeTable[4]) then
		BroadCast_AT_UpdateMsg(m_atTimeTable[4])
		m_curPriorityAT = 4
	elseif 0 ~= table.getn(m_atTimeTable[5]) then
		BroadCast_AT_UpdateMsg(m_atTimeTable[5])
		m_curPriorityAT = 5
	end
end
-----------------------------------------
--GM或其他官方管理的公告
function BroadCast_GM(msg)
	EQGlobaClass.getBroadCastSystem():saveMsgGM(msg.id, msg.content, msg.gmPlayTimes)
	EQGlobaClass.getBroadCastSystem():playGM(m_GMFrame, m_GMContent)
end
-----------------------------------------
--删除指定的公告
function BroadCast_DeleteMsg(type, id)
	if 1 == type then	-- 自动播放公告
		for i=1, table.getn(m_farmTable) do
			if id == m_farmTable[i].id then
				table.remove(m_farmTable, i)
				m_totalPage = getTotalPage(m_farmTable, m_maxCount)	-- 计算总页数
				if m_totalPage == 0 then
					m_totalPage = 1
				end			
				if m_totalPage >= 10 then
					EQGlobaFunc.SetText("broadcast/yeshu2", "/"..tostring(m_totalPage))
				else
					EQGlobaFunc.SetText("broadcast/yeshu2", "/ "..tostring(m_totalPage))
				end
				if m_curPage > m_totalPage then
					m_curPage = m_totalPage
				end
				BroadCast_FarmUpdateMsg(m_farmTable, m_curPage, m_maxCount)
				return
			end
		end
	elseif 2 == type then	-- 定点播放公告
		if EQGlobaClass.getBroadCastSystem():isPlayingMsgAT() then
			EQGlobaClass.getBroadCastSystem():stopAT()	
		end
		EQGlobaClass.getBroadCastSystem():deleteMsgAT(id)
		for i=1, 5 do
			for j=1, table.getn(m_atTimeTable[i]) do
				if id == m_atTimeTable[i].id then
					table.remove(m_atTimeTable[i], j)
					return
				end
			end		
		end		
	elseif 4 == type then	-- GM公告
		if EQGlobaClass.getBroadCastSystem():isPlayingMsgGM(id) then
			EQGlobaClass.getBroadCastSystem():stopGM()
		end
		EQGlobaClass.getBroadCastSystem():deleteMsgGM(id)		
	end
end
-----------------------------------------
--打开公告上的超链接
function BroadCast_OpenHyperLink(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local id = tonumber(EQGlobaFunc.toMultiLineBox(win:getName()):getHyperLinkId())
	local rectStr = EQGlobaFunc.toMultiLineBox(win:getName()):getTriggerRect()
	local rect = StringToRect(rectStr)
	local parentPos = {x, y}
	parentPos = win:getParent():getPosition()
	parentXPos = parentPos.x:asAbsolute(g_GuiSystem:getRenderer():getRect():getWidth())
	parentYPos = parentPos.y:asAbsolute(g_GuiSystem:getRenderer():getRect():getHeight())
	rect.left = rect.left + parentXPos
	rect.top = rect.top + parentYPos
	rect.right = rect.right + parentXPos
	rect.bottom = rect.bottom + parentYPos
	--时效相关
	-- local effectTime = ""
	-- local itemrow = g_itemtableMgr:getItemRow(id)
	-- if 1 == itemrow.effectTimeID then
		-- effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
	-- elseif 2 == itemrow.effectTimeID then
		-- effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
	-- end
	-- createItemhint_shop(id, rect, g_MainUIWindows, effectTime, 0)
end
-----------------------------------------
--关闭公告上的超链接
function BroadCast_CloseHyperLink(args)
	destroyItemhint(g_MainUIWindows)
end
