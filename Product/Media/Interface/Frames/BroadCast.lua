-----------------------------------------
-- 文件说明	：客户端公告系统
-- 作者		：何展然
-- 修改		：
-- 日期		：10/11/3
-----------------------------------------

-- 自动播放公告配置，可更改
local m_autoShowTxt = "BroadCast/Auto/TxtName"
local m_maxCount = 3	-- 每页的条数
local m_curPage = 1	-- 当前页
local m_totalPage = 1	-- 总页数
local m_reqAuto = false	-- 请求自动播放公告
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
-- 玩家行为公告配置，可更改
local m_behaviorSpeaker = "BroadCast/BE/SpeakerName"	-- 玩家行为公告喇叭按钮名称
local m_behaviorSpeakerTxt = "BroadCast/BE/TxtName"	-- 玩家行为公告喇叭面板名称
local m_behaviorFrame = "BroadCast/BE/PareName"	-- 玩家行为公告父窗口名称
local m_behaviorContent = "BroadCast/BE/ChildName"	-- 玩家行为公告子窗口名称
local m_behaviorSpeakerWidth, m_behaviorSpeakerHeight = 25, 25	-- 玩家行为公告喇叭按钮宽度、高度
local m_behaviorWidth = 288	-- 玩家行为公告宽度(最多可显示16个汉字，不包含"消息：")
local m_behaviorHeight = 17	-- 玩家行为公告初始高度
local m_behaviorContentSide = true
local m_behaviorContentText = ""
-- 优先级别
local m_curPriorityAT = 0	-- 当前播放的定点公告的优先级(1,2,3,4,5从高到低)
-- 当前接收的定点公告信息
local m_curInfoAT = {id=0, content="", playTimes=0}
-- 各类型公告表
local m_autoShowTable = {}	-- 系统自动显示播放的公告
local m_atTimeTable = {{}, {}, {}, {}, {}}	-- 5中优先级别的定点公告
local m_behaviorTable = {}	-- 玩家行为引起的服务器公告
-- 公告动画
local m_animateWin = nil	-- 公告动画窗口
local m_animate = EQGlobaClass.createAnimate()	-- 公告动画
--事件注册
g_WinMgr:getWindow("broadcast__auto_closebutton__"):subscribeEvent("Clicked", "BroadCast_AutoClose")
g_WinMgr:getWindow("broadcast/fan1"):subscribeEvent("Clicked", "BroadCast_AutoPageUp")
g_WinMgr:getWindow("broadcast/fanye2"):subscribeEvent("Clicked", "BroadCast_AutoPageDown")
g_WinMgr:getWindow("broadcast2__auto_closebutton__"):subscribeEvent("Clicked", "BroadCast_SpeakerClose")
g_WinMgr:getWindow("broadcast2/queding"):subscribeEvent("Clicked", "BroadCast_SpeakerClose")
--屏蔽鼠标穿透
MainUI_ShieldClick(g_BroadCastUI)
MainUI_ShieldClick(g_BroadCastSpeakerUI)
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
	g_MainUIWindows:addChildWindow(m_animateWin)	
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
--创建自动显示公告控件
function BroadCast_CreateAutoCtrl()
	local txtWin = g_WinMgr:getWindow(m_autoShowTxt)
	if nil == txtWin then
		txtWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", m_autoShowTxt)
		EQGlobaFunc.toMultiLineBox(m_autoShowTxt):setFont("simsun-10")	-- 设置字体
		EQGlobaFunc.toMultiLineBox(m_autoShowTxt):setCols("#EBEBEB")	-- 设置颜色		
		EQGlobaFunc.toMultiLineBox(m_autoShowTxt):setSize(275, 135)	-- 设置大小
		EQGlobaFunc.toMultiLineBox(m_autoShowTxt):setScrollbarVisible("SP_OLD", true, 1)	-- 设置滚动条
		txtWin:setAlwaysOnTop(true)
		txtWin:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 15), CEGUI.UDim(0, 32)))	-- 设置位置
		txtWin:subscribeEvent("HyperLink_MouseEnter", "BroadCast_OpenHyperLink")
		txtWin:subscribeEvent("HyperLink_MouseLeave", "BroadCast_CloseHyperLink")			
		g_WinMgr:getWindow("broadcast"):addChildWindow(txtWin)
	end
	return txtWin
end
-----------------------------------------
--此调用放在函数BroadCast_CreateAutoCtrl定义后面
BroadCast_CreateAutoCtrl()
-----------------------------------------
--显示自动播放公告
function BroadCast_OpenAutoUI(args)
	if true == g_MainUIWindows:isChild(g_BroadCastUI) then
		CloseBroadCastUI()
	else
		OpenBroadCastUI()
		
		UIM_DoOpenAndCloseUI(g_BroadUINames.root)
	end
end
-----------------------------------------
function OpenBroadCastUI()
	if false == m_reqAuto then
		EQGlobaClass.getBroadCastSystem():reqBroadCast(1)
		m_reqAuto = true
	end
	m_curPage = 1
	EQGlobaFunc.SetText("broadcast/yeshu1", tostring(m_curPage))
	if m_totalPage >= 10 then
		EQGlobaFunc.SetText("broadcast/yeshu2", "/"..tostring(m_totalPage))
	else
		EQGlobaFunc.SetText("broadcast/yeshu2", "/ "..tostring(m_totalPage))
	end
	BroadCast_AutoUpdateMsg(m_autoShowTable, m_curPage, m_maxCount)
	
	UIM_OpenWindow(g_BroadUINames.root)
end

function CloseBroadCastUI()
	UIM_CloseWindow(g_BroadUINames.root)
end
--关闭自动显示公告界面
function BroadCast_AutoClose(args)
	CloseBroadCastUI()
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
function BroadCast_AutoUpdateMsg(data, curPage, maxCount)
	EQGlobaFunc.SetText("broadcast/yeshu1", tostring(curPage))
	EQGlobaFunc.toMultiLineBox(m_autoShowTxt):setScrollbarPosition(0)
	EQGlobaFunc.toMultiLineBox(m_autoShowTxt):clearUp()
	local page = getData(data, curPage, maxCount)
	for i=1, table.getn(page) do
		local parts, y, m, d = cutString(page[i].startTime, ",", 7)	-- 截取日期字符串
		if nil ~= parts then
			y, m, d = tonumber(parts[1]), tonumber(parts[2]), tonumber(parts[3])
			local text = y.."年"..m.."月"..d.."日 "..page[i].content
			EQGlobaFunc.toMultiLineBox(m_autoShowTxt):addElementText("", true)
			g_mlbParse:getData(text, m_autoShowTxt)		
		end
	end
end
-----------------------------------------
--向上翻页
function BroadCast_AutoPageUp(args)
	if 1 == m_curPage then	-- 第一页
		return
	end
	m_curPage = m_curPage - 1
	BroadCast_AutoUpdateMsg(m_autoShowTable, m_curPage, m_maxCount)
end
-----------------------------------------
--向下翻页
function BroadCast_AutoPageDown(args)
	if m_totalPage == m_curPage then	-- 最后页
		return
	end
	m_curPage = m_curPage + 1
	BroadCast_AutoUpdateMsg(m_autoShowTable, m_curPage, m_maxCount)
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
		g_MainUIWindows:addChildWindow(frame)
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
	EQGlobaFunc.toMultiLineBox(m_atTimeContent):addElementText(text, false)
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
		g_MainUIWindows:addChildWindow(frame)
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
--创建玩家行为公告控件
function createBECtrl()
	local speaker = g_WinMgr:getWindow(m_behaviorSpeaker)
	if nil == speaker then
		speaker = g_WinMgr:createWindow("EQHomeLook/ImageButton", m_behaviorSpeaker)
		speaker:setSize(CEGUI.UVector2(CEGUI.UDim(0, m_behaviorSpeakerWidth), CEGUI.UDim(0, m_behaviorSpeakerHeight)))
		speaker:setProperty("NormalImage", "set:Button6 image:NewImage143")
		speaker:setProperty("HoverImage", "set:Button6 image:NewImage144")
		speaker:setProperty("PushedImage", "set:Button6 image:NewImage145")
		speaker:subscribeEvent("Clicked", "BroadCast_SpeakerBtnClick")
		g_MainUIWindows:addChildWindow(speaker)
	end
	
	local speakerTxt = g_WinMgr:getWindow(m_behaviorSpeakerTxt)
	if nil == speakerTxt then
		speakerTxt = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", m_behaviorSpeakerTxt)
		EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):setFont("simsun-10")	-- 设置字体
		EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):setCols("#EBEBEB")	-- 设置颜色
		EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):setSize(185, 115)	-- 设置大小
		EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):setScrollbarVisible("SP_OLD", true, 1)	-- 设置滚动条
		speakerTxt:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 15), CEGUI.UDim(0, 35)))	-- 设置位置
		speakerTxt:subscribeEvent("HyperLink_MouseEnter", "BroadCast_OpenHyperLink")
		speakerTxt:subscribeEvent("HyperLink_MouseLeave", "BroadCast_CloseHyperLink")		
		g_WinMgr:getWindow("broadcast2"):addChildWindow(speakerTxt)
	end

	local pare = g_WinMgr:getWindow(m_behaviorFrame)
	if nil == pare then
		pare = g_WinMgr:createWindow("EQHomeLook/StaticImage", m_behaviorFrame)
		pare:setSize(CEGUI.UVector2(CEGUI.UDim(0, m_behaviorWidth), CEGUI.UDim(0, m_behaviorHeight)))
		MainUI_ShieldClick(pare)
		pare:setVisible(false)
		pare:setID(1)
		g_MainUIWindows:addChildWindow(pare)
	end
	
	local child = g_WinMgr:getWindow(m_behaviorContent)
	if nil == child then
		child = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", m_behaviorContent)
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setFont("simsun-10")	-- 设置字体
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setCols("#FA5134")	-- 设置颜色
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setSide("#FFF603", 1)	-- 设置描边
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setAlign("AT_CENTER_VER")	-- 设置垂直居中
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setSize(m_behaviorWidth, m_behaviorHeight)	-- 设置初始大小
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setPosition(0, 0)
		pare:addChildWindow(child)
	end

	return speaker, pare, child
end
-----------------------------------------
--设置玩家行为公告的坐标
function BE_setPosition(speaker, bePare)
	local chatBoxW = g_WinMgr:getWindow("MainRoot/MTxt_ShowChatbox"):getWidth():asAbsolute(screenW)
	local xScale = EQGlobaFunc.getPositionVal("MainRoot/MTxt_ShowChatbox", 0, 0)
	local yScale = EQGlobaFunc.getPositionVal("MainRoot/MTxt_ShowChatbox", 1, 0)
	local xOffset, yOffset = 0, 0
	if nil ~= speaker then
		xOffset = EQGlobaFunc.getPositionVal("MainRoot/MTxt_ShowChatbox", 0, 1)
		yOffset = EQGlobaFunc.getPositionVal("MainRoot/MTxt_ShowChatbox", 1, 1) - m_behaviorSpeakerHeight	
		speaker:setPosition(CEGUI.UVector2(CEGUI.UDim(xScale, xOffset), CEGUI.UDim(yScale, yOffset)))	
	end
	EQGlobaFunc.toMultiLineBox(m_behaviorContent):setWidth(chatBoxW-m_behaviorSpeakerWidth)
	local height = EQGlobaFunc.toMultiLineBox(m_behaviorContent):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(m_behaviorContent):setHeight(height)
	g_WinMgr:getWindow(m_behaviorFrame):setSize(CEGUI.UVector2(CEGUI.UDim(0, chatBoxW-m_behaviorSpeakerWidth), CEGUI.UDim(0, height)))
	xOffset = EQGlobaFunc.getPositionVal("MainRoot/MTxt_ShowChatbox", 0, 1) + m_behaviorSpeakerWidth
	yOffset = EQGlobaFunc.getPositionVal("MainRoot/MTxt_ShowChatbox", 1, 1) - height
	bePare:setPosition(CEGUI.UVector2(CEGUI.UDim(xScale, xOffset), CEGUI.UDim(yScale, yOffset)))
end
-----------------------------------------
--单条玩家行为公告开始播放时刻
function BE_playBegin()
	g_WinMgr:getWindow(m_behaviorFrame):setVisible(true)
end
-----------------------------------------
--玩家行为公告重置函数
function BE_resetFunc(text)
	g_WinMgr:getWindow(m_behaviorFrame):setVisible(false)
	EQGlobaFunc.toMultiLineBox(m_behaviorContent):clearUp()
	EQGlobaFunc.toMultiLineBox(m_behaviorContent):addElementText(text, false)
	BE_setPosition(nil, g_WinMgr:getWindow(m_behaviorFrame))
end
-----------------------------------------
--玩家行为单条公告结束播放时刻
function BE_playEnd()
	g_WinMgr:getWindow(m_behaviorFrame):setVisible(false)
end
function BroadCast_BE_ChangeColor()
	if true == m_behaviorContentSide then
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):clearUp()
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setSide("#FFFFFF", 1)	-- 设置描边
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):addElementText(m_behaviorContentText, false)
		m_behaviorContentSide = false
	else
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):clearUp()
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):setSide("#FFF603", 1)	-- 设置描边
		EQGlobaFunc.toMultiLineBox(m_behaviorContent):addElementText(m_behaviorContentText, false)
		m_behaviorContentSide = true
	end
end
-----------------------------------------
--初始玩家行为公告系统
function BoardCast_BE_Init()
	local speaker, pare = createBECtrl()
	BE_setPosition(speaker, pare)
	EQGlobaClass.getBroadCastSystem():setCalledFuncNameBE("BE_playBegin", "BE_resetFunc", "BE_playEnd")
	EQGlobaClass.getBroadCastSystem():setPlayParamsBE("NONE", 3000, 3000, 0, 30000)	-- 每条间隔3秒，播放30秒
end
-----------------------------------------
--此调用放在函数BoardCast_BE_Init()定义后面
BoardCast_BE_Init()
-----------------------------------------
--点击玩家行为公告小喇叭按钮
function BroadCast_SpeakerBtnClick(args)
	if true == g_MainUIWindows:isChild(g_BroadCastSpeakerUI) then
		g_MainUIWindows:removeChildWindow(g_BroadCastSpeakerUI)
	else
		g_MainUIWindows:addChildWindow(g_BroadCastSpeakerUI)
	end
end
-----------------------------------------
--关闭玩家行为小喇叭公告面板
function BroadCast_SpeakerClose(args)
	g_MainUIWindows:removeChildWindow(g_BroadCastSpeakerUI)
	local size = EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):getScrollbarDocumentSize()
	EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):setScrollbarPosition(size)
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

	if 1 == type then	-- 自动播放公告
		if m_reqAuto then
			table.insert(m_autoShowTable, 1, msg)	-- 在前面插入消息
			table.sort(m_autoShowTable, BroadCast_TableSort)
			m_totalPage = getTotalPage(m_autoShowTable, m_maxCount)	-- 计算总页数
			if m_totalPage >= 10 then
				EQGlobaFunc.SetText("broadcast/yeshu2", "/"..tostring(m_totalPage))
			else
				EQGlobaFunc.SetText("broadcast/yeshu2", "/ "..tostring(m_totalPage))
			end
			BroadCast_AutoUpdateMsg(m_autoShowTable, m_curPage, m_maxCount)
		end
	elseif 2 == type then	-- 定点播放公告
		BroadCast_AtTime(msg)
		parseChatShowBox("公告："..msg.content, 2)
	elseif 3 == type then	-- 玩家行为公告
		table.insert(m_behaviorTable, msg)
		BroadCast_Behavior(msg)
	elseif 4 == type then	-- GM公告
		BroadCast_GM(msg)
		parseChatShowBox("公告："..msg.content, 2)
	else	-- 5,农场公告(这里不处理，放在FarmBroadCast.lua里面处理)
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
	elseif 1 == playState then	-- 有消息，播放状态，处于滚动期间
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
	local info = {}
	info["id"] = id
	info["content"] = content
	info["playTimes"] = playTimes
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
--玩家行为引起的公告
function BroadCast_Behavior(msg)
	EQGlobaClass.getBroadCastSystem():stopBE()
	m_behaviorContentText = "消息："..msg.content
	EQGlobaClass.getBroadCastSystem():saveMsgBE(msg.id, m_behaviorContentText, 3)
	BroadCast_BE_Play()
	if table.getn(m_behaviorTable) > 20 then	-- 控制公告条数不超过20条
		table.remove(m_behaviorTable, 1)
	end
	EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):clearUp()
	for i=1, table.getn(m_behaviorTable) do
		local text = m_behaviorTable[i].content
		EQGlobaFunc.toMultiLineBox(m_behaviorSpeakerTxt):addElementText("", true)
		g_mlbParse:getData(text, m_behaviorSpeakerTxt)
	end
end
-----------------------------------------
--播放玩家行为引起的公告
function BroadCast_BE_Play()
	EQGlobaClass.getBroadCastSystem():playBE(m_behaviorFrame, m_behaviorContent)
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
		for i=1, table.getn(m_autoShowTable) do
			if id == m_autoShowTable[i].id then
				table.remove(m_autoShowTable, i)
				m_totalPage = getTotalPage(m_autoShowTable, m_maxCount)	-- 计算总页数
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
				BroadCast_AutoUpdateMsg(m_autoShowTable, m_curPage, m_maxCount)
				return
			end
		end
	elseif 2 == type then	-- 定点播放公告
		if EQGlobaClass.getBroadCastSystem():isPlayingMsgAT(id) then
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
