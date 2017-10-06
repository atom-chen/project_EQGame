-- 定时器
--[[
useage:
function test2(timer, param1, param2)
end

function test2_complete(timer, param1, param2)
end

timer = createTimer(1100, 3, 0);
addTimerListener(timer, TE_TIMER, test2)
addTimerListener(timer, TE_COMPLETE, test2_complete)
startTimer(timer)
--]]

-- 定时器事件
TE_TIMER = 0	   -- 定时器触发事件
TE_COMPLETE = 1    -- 定时器结束事件
TE_MAX_COUNT = 2   -- 定时器事件的最大数量

local timers = {}

-----------------------------------------------------------------------------------
local function findTimer(flag)
    return timers[flag]
end

local function addTimer(flag, timer)
    timers[flag] = timer
end

--------------------------------------------------------------------------------------------------------
-- 使用指定的 delay 和 repeatCount 状态构造新的 Timer 对象。
-- delay — 计时器事件间的延迟（以毫秒为单位）。 
-- repeatCount (default = 0) — 指定重复次数。如果为 0，则计时器重复无限次数。
--                                  如果不为 0，则将运行计时器，运行次数为指定的次数，然后停止。
function createTimer(delay, repeatCount, param)
	timer = {flag = EQGlobaFunc.createLuaTimer(delay, repeatCount), 
            handlers = {},
            param = param}
	addTimer(timer.flag, timer)
	return timer
end

-- 开始定时器
function startTimer(timer)
    EQGlobaFunc.startLuaTimer(timer.flag)
end

-- 结束定时器
function stopTimer(timer)
    EQGlobaFunc.stopLuaTimer(timer.flag)
end

-- 判断定时器是否还在运行
function isTimerRunning(timer)
    return EQGlobaFunc.isLuaTimerRunning(timer.flag)
end

-- 返回定时器被触发的次数
function getTimerCurrentCount(timer)
    return EQGlobaFunc.getLuaTimerCurrentCount(timer.flag)
end

-- 注册定时器响应事件
function addTimerListener(timer, event, fun)
    timer.handlers[event] = fun
end

-- 移除定时器响应事件 
function removeTimerListener(timer, event)
    timer.handlers[event] = nil
end

function timerEvent(event, flag)
    timer = findTimer(flag)
    if timer.handlers[event] ~= nil then
        timer.handlers[event](timer.param)
    end
end
