-- ��ʱ��
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

-- ��ʱ���¼�
TE_TIMER = 0	   -- ��ʱ�������¼�
TE_COMPLETE = 1    -- ��ʱ�������¼�
TE_MAX_COUNT = 2   -- ��ʱ���¼����������

local timers = {}

-----------------------------------------------------------------------------------
local function findTimer(flag)
    return timers[flag]
end

local function addTimer(flag, timer)
    timers[flag] = timer
end

--------------------------------------------------------------------------------------------------------
-- ʹ��ָ���� delay �� repeatCount ״̬�����µ� Timer ����
-- delay �� ��ʱ���¼�����ӳ٣��Ժ���Ϊ��λ���� 
-- repeatCount (default = 0) �� ָ���ظ����������Ϊ 0�����ʱ���ظ����޴�����
--                                  �����Ϊ 0�������м�ʱ�������д���Ϊָ���Ĵ�����Ȼ��ֹͣ��
function createTimer(delay, repeatCount, param)
	timer = {flag = EQGlobaFunc.createLuaTimer(delay, repeatCount), 
            handlers = {},
            param = param}
	addTimer(timer.flag, timer)
	return timer
end

-- ��ʼ��ʱ��
function startTimer(timer)
    EQGlobaFunc.startLuaTimer(timer.flag)
end

-- ������ʱ��
function stopTimer(timer)
    EQGlobaFunc.stopLuaTimer(timer.flag)
end

-- �ж϶�ʱ���Ƿ�������
function isTimerRunning(timer)
    return EQGlobaFunc.isLuaTimerRunning(timer.flag)
end

-- ���ض�ʱ���������Ĵ���
function getTimerCurrentCount(timer)
    return EQGlobaFunc.getLuaTimerCurrentCount(timer.flag)
end

-- ע�ᶨʱ����Ӧ�¼�
function addTimerListener(timer, event, fun)
    timer.handlers[event] = fun
end

-- �Ƴ���ʱ����Ӧ�¼� 
function removeTimerListener(timer, event)
    timer.handlers[event] = nil
end

function timerEvent(event, flag)
    timer = findTimer(flag)
    if timer.handlers[event] ~= nil then
        timer.handlers[event](timer.param)
    end
end
