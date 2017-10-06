-----------------------------------------
-- 文件说明：播放动画处理
-----------------------------------------
-----------------------------------------
--随机播放动作的频率，值为0时，每隔设定的时间都会随机播放，值越高，越不频繁
local random_frequency = 3
--
local randIdleActionTable = {"stand2","stand3","stand4"}
--站立待机随机播放的动作表
--local randStandAnimaSize = 1
local randPlayStandAnimaTable = {"standby1","standby2","standby3"}--,"stand2","stand3","stand4"
--local randPlayStandAnimaTable = {"Lookaround","Laugh","Helpless","Shake","Cry"}
--
--local randSitAnimaSize = 1
local randSitAnimaTable = {"sit_lookaround"}
-----------------------------------------
g_ActionUIFlag = false
--注册界面按钮事件函数
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Action"):subscribeEvent("Clicked", "Action_Onclicked")
g_WinMgr:getWindow("ActionWnd/act1"):subscribeEvent("Clicked", "Action_SitDown")
g_WinMgr:getWindow("ActionWnd/act2"):subscribeEvent("Clicked", "Action_LayDown")
g_WinMgr:getWindow("ActionWnd/act3"):subscribeEvent("Clicked", "Action_Sigh")
g_WinMgr:getWindow("ActionWnd/act4"):subscribeEvent("Clicked", "Action_Hail")
g_WinMgr:getWindow("ActionWnd/act5"):subscribeEvent("Clicked", "Action_Cry")
g_WinMgr:getWindow("ActionWnd/act6"):subscribeEvent("Clicked", "Action_Bow")
g_WinMgr:getWindow("ActionWnd/act7"):subscribeEvent("Clicked", "Action_Yawn")
g_WinMgr:getWindow("ActionWnd/act8"):subscribeEvent("Clicked", "Action_Clap")
g_WinMgr:getWindow("ActionWnd/act9"):subscribeEvent("Clicked", "Action_Smile")
g_WinMgr:getWindow("ActionWnd/act10"):subscribeEvent("Clicked", "Action_Angry")
g_WinMgr:getWindow("ActionWnd/act11"):subscribeEvent("Clicked", "Action_Shake")
g_WinMgr:getWindow("ActionWnd/act12"):subscribeEvent("Clicked", "Action_Helpless")
g_WinMgr:getWindow("ActionWnd/act13"):subscribeEvent("Clicked", "Action_Jump")
g_WinMgr:getWindow("ActionWnd/act14"):subscribeEvent("Clicked", "Action_Wave")
g_WinMgr:getWindow("ActionWnd/act15"):subscribeEvent("Clicked", "Action_Dance")
--注册动作添加完成的事件
g_GameEventMgr:subscribeEvent("ADD_ALLACTION_OK","Action_notifyAdd_ok")

--屏蔽窗口鼠标穿透
MainUI_ShieldClick(g_ActionUI)
----------------------------------------------
--打开界面
function Action_openActionUI()
	--g_MainUIWindows:addChildWindow(g_ActionUI)
	UIM_OpenWindow(g_ActionUINames.root)
	g_ActionUIFlag = true
	PlayerGuide_do_NewerTeachTaskScript(15009) --新手教学任务
end
----------------------------------------------
--关闭界面
function Action_closeActionUI()
	--g_MainUIWindows:removeChildWindow(g_ActionUI)
	UIM_CloseWindow(g_ActionUINames.root)
	g_ActionUIFlag = false
end
----------------------------------------------
--动作按钮函数
function Action_Onclicked()
	if g_ActionUIFlag == false then
		Action_openActionUI()
		UIM_DoOpenAndCloseUI(g_ActionUINames.root)
	else
		Action_closeActionUI()
	end
end
---------------------------------------------
--接收动作添加完成的事件处理函数
function Action_notifyAdd_ok()
	EQGlobaFunc.check_Animation_List()
end
---------------------------------------------
--清除非循环列表和循环列表中的所有动作
function Action_clearAllAnimaList()
	EQGlobaFunc.clear_allAnimaList()
end
---------------------------------------------
--设置循环动作的循环次数
function Action_setAnimaLoopNum(loop_num)
	EQGlobaFunc.setAnimaLoopNum(loop_num)
end
---------------------------------------------
--获取人物当前的动作状态
function Action_getCurentAnimat()
	return EQGlobaFunc.getCurrentAnimation()
end
---------------------------------------------
--向动作列表中添加动作
function Action_addAction(noloop_table,loop_table)
	local noloop_count = table.getn(noloop_table)
	local loop_count = table.getn(loop_table)
	Action_clearAllAnimaList()
	if noloop_count ~= 0 then
		for i,v in ipairs(noloop_table)  do
			EQGlobaFunc.AddAnimation(v.name,v.action,v.loop)
		end
	end
	--
	if loop_count ~= 0 then
		for i,v in ipairs(loop_table) do
			EQGlobaFunc.AddAnimation_loop(v.name,v.action,v.loop)
		end
	end
	g_GameEventMgr:fireEvent("ADD_ALLACTION_OK")
end
------------------------------------------------
--
function Action_randPlaySitAnima()
	local table_size = table.getn(randSitAnimaTable)
	if table_size == 0 then
		--g_Loger:logError("randSitAnimaTable is empty!")
		return
	end
	--频率
	local frequency = 1
	if random_frequency > 0 then
		math.randomseed(os.time())
		math.random(0, random_frequency)
		frequency = math.random(0, random_frequency)
	end
	--表索引
	math.randomseed(os.time())
	math.random(1, table_size)
	local idx = math.random(1, table_size)
	--g_Loger:logMessage(tostring(frequency))
	if frequency == 1 then
		local noloop_t = {}
		local loop_t = {}
		table.insert(noloop_t, {name = randSitAnimaTable[idx],action = "sit_1",loop = false})
		Action_addAction(noloop_t,loop_t)
	else
	
	end
end
------------------------------------------------
--站着的随机动作
function Action_randPlayStandAnima()
	local table_size = table.getn(randPlayStandAnimaTable)
	if table_size == 0 then
		--g_Loger:logError("randPlayStandAnimaTable is empty!")
		return
	end
	--频率
	local frequency = 1
	if random_frequency > 0 then
		math.randomseed(os.time())
		math.random(0, random_frequency)
		frequency = math.random(0, random_frequency)
	end
	math.randomseed(os.time())
	math.random(1, table_size)
	math.random(1, table_size)
	local idx = math.random(1, table_size)
	--g_Loger:logMessage(tostring(frequency))
	--g_Loger:logMessage("idx***" ..tostring(idx))
	if frequency == 1 then
		local noloop_t = {}
		local loop_t = {}
		local player_state = g_mainplayer:getState()
		table.insert(noloop_t, {name = randPlayStandAnimaTable[idx],action = "Stand",loop = false})
		Action_addAction(noloop_t,loop_t)
	else
	
	end
end
--
function Action_randSelectIdleAction()
	local table_size = table.getn(randIdleActionTable)
	if table_size == 0 then
		return ""
	end
	math.randomseed(os.time())
	math.random(1, table_size)
	math.random(1, table_size)
	local idx = math.random(1, table_size)
	local action_name = randIdleActionTable[idx]
	return action_name
end
------------------------------------------------
--
--
--人物状态
g_GOS_NONE 			= 0
g_GOS_IDLE 			= 1		--站立的空闲状态
--g_GOS_STAND_IDLE 	= 2		
g_GOS_SIT_IDLE 		= 3		--坐着的空闲状态
g_GOS_LIE_IDLE 		= 4		--躺着的空闲状态
g_GOS_WALK 			= 5		--正在走路
--g_GOS_RUN 		= 6
g_GOS_ANIMATION 	= 7		--正在做动作
--
--
------------------------------------------------
------------------------------------------------
--坐下按钮动作
function Action_SitDown()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState() --获得人物状态
	
	if player_state == g_GOS_LIE_IDLE then --如果人物是躺着的
		table.insert(noloop_t, {name = "lie_sit",action = "sit_1",loop = false})
		
	elseif player_state == g_GOS_SIT_IDLE then --坐的时候屏蔽该按钮
	
	elseif player_state == g_GOS_ANIMATION   --lie_sit的时候屏蔽该按钮
	and Action_getCurentAnimat() == "lie_sit" then
	
	elseif player_state == g_GOS_ANIMATION   --Sit的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Sit" then
	
	elseif player_state == g_GOS_ANIMATION   --sit_lookaround的时候屏蔽该按钮
	and Action_getCurentAnimat() == "sit_lookaround" then
	
	elseif player_state == g_GOS_ANIMATION   --sit_lie的时候屏蔽该按钮
	and Action_getCurentAnimat() == "sit_lie" then
	
	elseif player_state == g_GOS_ANIMATION   --Lie的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Lie" then
	
	else
		table.insert(noloop_t, {name = "Sit",action = "sit_1",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
------------------------------------------------
--躺下按钮动作
function Action_LayDown()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	
	if player_state == g_GOS_SIT_IDLE then	--如果人物是坐着的，插入sit_lie
		table.insert(noloop_t, {name = "sit_lie",action = "lie_1",loop = false})
		
	elseif player_state == g_GOS_ANIMATION  --如果人物是在做动作，
	and Action_getCurentAnimat() == "sit_lookaround" then --并且是在做“坐着东张西望”的动作时，插入sit_lie
		table.insert(noloop_t, {name = "sit_lie",action = "lie_1",loop = false})
		
	elseif player_state == g_GOS_LIE_IDLE then   --躺的时候屏蔽该按钮
		
	elseif player_state == g_GOS_ANIMATION   --lie_sit的时候屏蔽该按钮
	and Action_getCurentAnimat() == "lie_sit" then
	
	elseif player_state == g_GOS_ANIMATION   --Sit的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Sit" then
	
	elseif player_state == g_GOS_ANIMATION   --sit_lookaround的时候屏蔽该按钮
	and Action_getCurentAnimat() == "sit_lookaround" then
	
	elseif player_state == g_GOS_ANIMATION   --sit_lie的时候屏蔽该按钮
	and Action_getCurentAnimat() == "sit_lie" then
	
	elseif player_state == g_GOS_ANIMATION   --Lie的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Lie" then
	
	else
		table.insert(noloop_t, {name = "Lie",action = "lie_1",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
------------------------------------------------
--大笑按钮动作
function Action_Sigh()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --笑的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Laugh" then			
	else
		table.insert(noloop_t, {name = "Laugh",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
------------------------------------------------
--欢呼按钮动作
function Action_Hail()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --欢呼的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Hail" then			
	else
		table.insert(noloop_t, {name = "Hail",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
-------------------------------------------------
--哭泣按钮动作
function Action_Cry()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --哭泣的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Cry" then			
	else
		table.insert(noloop_t, {name = "Cry",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
--------------------------------------------------
--鞠躬按钮动作
function Action_Bow()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --鞠躬的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Bow" then			
	else
		table.insert(noloop_t, {name = "Bow",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
--------------------------------------------------
--打哈欠按钮动作
function Action_Yawn()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --哈欠的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Yawn" then			
	else
		table.insert(noloop_t, {name ="Yawn",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
---------------------------------------------------
--鼓掌按钮动作
function Action_Clap()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --鼓掌的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Clap" then			
	else
		table.insert(noloop_t, {name = "Clap",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
---------------------------------------------------
--偷笑按钮动作
function Action_Smile()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --偷笑的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Smile" then			
	else
		table.insert(noloop_t, {name = "Smile",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
---------------------------------------------------
--发怒按钮动作
function Action_Angry()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --发怒的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Angry" then			
	else
		table.insert(noloop_t, {name = "Angry",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
---------------------------------------------------
--摇头按钮动作
function Action_Shake()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --摇头的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Shake" then			
	else
		table.insert(noloop_t, {name = "Shake",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
---------------------------------------------------
--无奈按钮动作
function Action_Helpless()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --无奈的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Helpless" then			
	else
		table.insert(noloop_t, {name = "Helpless",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
---------------------------------------------------
--蹦跳按钮动作
function Action_Jump()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --蹦跳的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Jump" then			
	else
		table.insert(noloop_t, {name = "Jump",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
end
---------------------------------------------------
--挥手按钮动作
function Action_Wave()
	local noloop_t = {}
	local loop_t = {}
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --挥手的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Wave" then			
	else
		table.insert(noloop_t, {name = "Wave",action = "Stand",loop = false})
	end
	Action_addAction(noloop_t,loop_t)
	
	---原先的方式
	--EQGlobaFunc.PlayAnimation("Wave", "")

end
---------------------------------------------
--跳舞按钮动作
function Action_Dance()
	local noloop_t = {}		-------------------	非循环动作列表
	local loop_t = {}		-------------------	循环动作列表
	local player_state = g_mainplayer:getState()
	if player_state == g_GOS_ANIMATION  --跳舞的时候屏蔽该按钮
	and Action_getCurentAnimat() == "Dance" then			
	else
		table.insert(noloop_t, {name = "Dance",action = "Stand",loop = true})
	end
	Action_addAction(noloop_t,loop_t)
end
