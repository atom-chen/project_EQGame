function Garbage_Collect()
	local time = EQGlobaFunc.GetTickCountByLua()
	local size = collectgarbage("count")
	--collectgarbage("collect")
	--g_Loger:logMessage("collect garbage time(" .. (EQGlobaFunc.GetTickCountByLua()-time) .. ") size(" .. (size-collectgarbage("count")) .. ")")
	g_Loger:logMessage("collect garbage : " .. collectgarbage("count"))
end

--ÿ����ִ��һ����������
--local gGarbageTimer = createTimer(1000, 0, 0)
--addTimerListener(gGarbageTimer, TE_TIMER, Garbage_Collect)
--startTimer(gGarbageTimer)




--collectgarbage (opt [, arg])
--���ܣ��������ռ�����ͨ�ýӿڣ����ڲ��������ռ���
--opt������������־
--"Stop": ֹͣ�����ռ���
--"Restart": ���������ռ���
--"Collect": ִ��һ��ȫ�����ռ�ѭ��
--"Count": ���ص�ǰLua��ʹ�õ��ڴ���(��KBΪ��λ)
--"Step": ����ִ��һ�������ռ�. ���� "Size" �ɲ���argָ����(���͵�ֵ��Ҫ�ಽ�������)�����Ҫ׼ȷָ����������Ҫ���ʵ���Դ�����Ч��������������һ���ռ�ѭ����������True
--"Setpause": ���� arg/100 ��ֵ��Ϊ�ݶ��ռ���ʱ��
--"Setstepmul": ���� arg/100 ��ֵ����Ϊ����������(���²���=�ɲ���*arg/100)


--garbage-collector pause �������ռ����ڿ�ʼһ���µ��ռ�����֮ǰҪ�ȴ���á��������ֵ�����͵����ռ������������Ĳ���ô������
--С�� 1 ��ֵ��ζ���ռ������µ����ڿ�ʼʱ���ٵȴ�����ֵΪ 2 ��ʱ����ζ������ʹ���ڴ������ﵽԭ��������ʱ�ٿ����µ����ڡ�


--�������ó�100�����ÿ10�����һ�Σ�ÿ�κ�ʱ100��ms
collectgarbage("setpause",100)