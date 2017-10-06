function Garbage_Collect()
	local time = EQGlobaFunc.GetTickCountByLua()
	local size = collectgarbage("count")
	--collectgarbage("collect")
	--g_Loger:logMessage("collect garbage time(" .. (EQGlobaFunc.GetTickCountByLua()-time) .. ") size(" .. (size-collectgarbage("count")) .. ")")
	g_Loger:logMessage("collect garbage : " .. collectgarbage("count"))
end

--每分钟执行一次垃圾回收
--local gGarbageTimer = createTimer(1000, 0, 0)
--addTimerListener(gGarbageTimer, TE_TIMER, Garbage_Collect)
--startTimer(gGarbageTimer)




--collectgarbage (opt [, arg])
--功能：是垃圾收集器的通用接口，用于操作垃圾收集器
--opt：操作方法标志
--"Stop": 停止垃圾收集器
--"Restart": 重启垃圾收集器
--"Collect": 执行一次全垃圾收集循环
--"Count": 返回当前Lua中使用的内存量(以KB为单位)
--"Step": 单步执行一个垃圾收集. 步长 "Size" 由参数arg指定　(大型的值需要多步才能完成)，如果要准确指定步长，需要多次实验以达最优效果。如果步长完成一次收集循环，将返回True
--"Setpause": 设置 arg/100 的值作为暂定收集的时长
--"Setstepmul": 设置 arg/100 的值，作为步长的增幅(即新步长=旧步长*arg/100)


--garbage-collector pause 控制了收集器在开始一个新的收集周期之前要等待多久。随着数字的增大就导致收集器工作工作的不那么主动。
--小于 1 的值意味着收集器在新的周期开始时不再等待。当值为 2 的时候意味着在总使用内存数量达到原来的两倍时再开启新的周期。


--这里设置成100，大概每10秒回收一次，每次耗时100多ms
collectgarbage("setpause",100)