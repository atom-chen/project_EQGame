-----------------------------------------
-- 文件说明	：玩家向导脚本
-- 作者		：何展然
-- 修改		：
-- 日期		：11/4/6
-----------------------------------------
-- 脚本指令:
-- complete_task(id)			-- 完成任务		(id:任务id)
-- is_cur_complete(id)			-- 是否为当前完成的任务	(id:任务id, true:成功, false:失败)
-- is_exist_task(id)			-- 是否存在任务	(id:任务id, true:存在, false:不存在)
-- locate_task(id)				-- 定位任务		(id:任务id, true:成功, false:失败)
-- show_picture(id)				-- 秀图			(id:图片id)
-- open_ciweiui(id)				-- 打开刺猬界面	(id:文字id)
-- next_step(event, func)		-- 指定下一步	(event:步骤名, func:步骤入口)
-- next_step_ex(event, func)	-- 指定下一步(扩展)(event:步骤名, func:步骤入口)
-- do_task_script(event)		-- 执行某个脚本	(event:脚本名)
-- is_male_player()				-- 是否男性玩家	(true:男, false:女)
-- is_at_home()					-- 是否在家		(true:在家, false:不在家)
-- 备注: 
-- 1.同种类型的操作(如:显示几张图片)可以放在同一步中,不同类型(如:显示图片和显示刺猬界面)则必须各自为一步
-- 2.脚本入口里有且只有一个步骤入口,每个步骤结束后,若有下一步,则必须指定下一个步骤
--		a.若上一步的操作不是complete_task(),则使用next_step()
--		b.若上一步的操作是complete_task(),则使用next_step_ex()
-- 3.若中途转去执行另一个脚本do_task_script,则不必指定下一个步骤(next_step)
-- 4.调用delete_task需要指定下一步,exist_task和locate_task则不必,可以直接调用下一步

--填入openUI.xml表里的ID号
local relatedUITable = {50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67}
-----
function PlayerGuide_enabledUI(flag)
	for i,v in ipairs(relatedUITable) do
		local row = g_logictableMgr:getMutexParentIdxRow(v)
		if row ~= nil then
			local win_name = row.name
			local win = g_WinMgr:getWindow(win_name)
			if win ~= nil then
				win:setEnabled(flag)
			else
				
			end
		end
	end
end
-------------------------------------------------------------
-- 脚本1(进入游戏)
-------------------------------------------------------------
function script1()	-- 脚本入口
	g_Loger:logMessage("--- script1() ---")
	if true == is_exist_task(1102) then			-- 检查玩家是不是要开始做新手
		if true == is_exist_task(1100) then		-- 检查玩家完成新手没有（男）
			script1_step7()
		end	
		if true == is_exist_task(1101) then		-- 检查玩家完成新手没有（女）
			script1_step7()
		end
	end
end
function script1_step7()
	open_ciweiui(101)
	next_step("script1_step1", script1_step1)
end
function script1_step1()
	g_Loger:logError("script1_step1")
	open_ciweiui(102)
	next_step("script1_step2", script1_step2)
end
function script1_step2()
	g_Loger:logError("script1_step2")
	show_picture(1)
	next_step("script1_step3", script1_step3)
end
function script1_step3()
	g_Loger:logError("script1_step3")
	open_ciweiui(103)
	pre_step("script1_step2",script1_step2)
	next_step("script1_step5", script1_step5)
end
function script1_step4()
	g_Loger:logError("script1_step4")
	open_ciweiui(3)
	next_step("script1_step5", script1_step5)
end
function script1_step5()
	g_Loger:logError("script1_step5")
	open_ciweiui(104)
	complete_task(1102)
	next_step("script1_step6", script1_step6)
end
function script1_step6()
	show_picture(4)
end


-------------------------------------------------------------
-- 脚本2(任务面板)
-------------------------------------------------------------
function script2()	-- 脚本入口
	g_Loger:logMessage("--- script2() ---")
	if true == is_exist_task(1103) then		-- 检查玩家有没有任务面板教学任务
		script2_step1()
	end
end
function script2_step1()
	complete_task(1103)	
	open_ciweiui(105)
	next_step("script2_step4", script2_step4)
end
function script2_step4()
	show_picture(5)
	next_step("script2_step5", script2_step5)
end
function script2_step5()
	open_ciweiui(106)
	next_step("script2_step6", script2_step6)
end
function script2_step6()
	open_ciweiui(107)
	next_step("script2_step7", script2_step7)
end
function script2_step7()
	show_picture(10)
	next_step("script2_step8", script2_step8)
end
function script2_step8()
end


-------------------------------------------------------------
-- 脚本3(到达指定地图)
-------------------------------------------------------------
--function script3()	-- 脚本入口
--	g_Loger:logMessage("--- script3() ---")
--	if true == is_exist_task(1004) then		-- 检查玩家有没有雷达教学任务
--		script3_step1()
--	end
--end
--function script3_step1()
--	script3_step2()
--end
--function script3_step2()
--	open_ciweiui(7)
--	next_step("script3_step3", script3_step3)
--end
--function script3_step3()
--	show_picture(11)
--	next_step("script3_step4", script3_step4)	
--end
--function script3_step4()
--	complete_task(1004)	
--end



-------------------------------------------------------------
-- 脚本4(点击小地图按钮)
-------------------------------------------------------------
--function script4()	-- 脚本入口
--	-- g_Loger:logMessage("--- script4() ---")
--	if true == is_exist_task(1005) then		-- 检查玩家有没有雷达教学任务
--		script4_step2()
--	end
--end
--function script4_step2()
--	complete_task(1005)
--	open_ciweiui(40)	
--end

-------------------------------------------------------------
-- 脚本5(完成npc对话任务)
-------------------------------------------------------------
function script5()	-- 脚本入口
--	 g_Loger:logMessage("--- script5() ---")
	if true == is_cur_complete(1100) then
		script5_step1()
	end
	if true == is_cur_complete(1101) then               ---------------完成服装任务，跳转
		script5_step1()
	end
	if true == is_cur_complete(1107) then              -----------------完成家具任务，跳转
		script5_step10()
	end
end
------------与服装店NPC对话-----------
function script5_step1()
	open_ciweiui(110)
	next_step("script5_step2", script5_step2)
end
function script5_step2()
	show_picture(13)
	next_step("script5_step3", script5_step3)
end
function script5_step3()
end
------------与家具店NPC对话-----------
function script5_step10()
	open_ciweiui(114)
	next_step("script5_step11", script5_step11)
end
function script5_step11()
	open_ciweiui(115)
	next_step("script5_step12", script5_step12)
end
function script5_step12()
	show_picture(23)
end


--function script5_step3()
--	open_ciweiui(8)
--	next_step("script5_step4", script5_step4)
--end
--function script5_step4()
--	show_picture(13)
--	next_step("script5_step5", script5_step5)	
--end
--function script5_step5()
--	if true == is_exist_task(1001) then		-- 检查玩家有没有初始化开始任务
--			complete_task(1001)	
--			next_step_ex("script5_step6", script5_step6)
--	else
--			next_step("script5_step6", script5_step6)
--	end
--end
--function script5_step6()
--	if true == is_exist_task(1002) then		-- 检查玩家有没有初始化开始任务
--			complete_task(1002)	
--			next_step_ex("script5_step7", script5_step7)
--	else
--			next_step("script5_step7", script5_step7)
--	end
--end
--function script5_step7()
--	if true == is_exist_task(1003) then		-- 检查玩家有没有标记玩家打开任务面板
--			complete_task(1003)	
--			next_step_ex("script5_step8", script5_step8)
--	else
--			next_step("script5_step8", script5_step8)
--	end
--end
--function script5_step8()
--	if true == is_exist_task(1004) then		-- 检查玩家有没有标记玩家打开任务面板
--			complete_task(1004)	
--			next_step("script5_step9", script5_step9)
--	else
--			next_step("script5_step9", script5_step9)
--	end
--end
--function script5_step9()
--	if true == is_exist_task(1005) then		-- 检查玩家有没有标记玩家打开任务面板
--			complete_task(1005)	
--			next_step_ex("script5_step10", script5_step10)
--	else
--			next_step("script5_step10", script5_step10)
--	end
--end
--function script5_step10()
--			complete_task(1006)	
--end


-------------------------------------------------------------
-- 脚本6(打开背包)
-------------------------------------------------------------
function script6()	-- 脚本入口
	-- g_Loger:logMessage("--- script6() ---")
	if true == is_exist_task(1106) then		-- 检查玩家有没有换装教学任务
		script6_step1()
	end
--	if true == is_exist_task(1013) then		-- 检查玩家有没有要玩家回家摆家具任务
--		script6_step5()
--	end
end

function script6_step1()
	open_ciweiui(111)
	next_step("script6_step3", script6_step3)
end
function script6_step3()
	show_picture(6)
	next_step("script6_step4", script6_step4)
end
function script6_step4()
	open_ciweiui(112)
	pre_step("script6_step3",script6_step3)
	next_step("script6_step5", script6_step5)
end
function script6_step5()
end
--function script6_step5()
--	if true == is_at_home() then		-- 检查玩家有没有在家
--		script6_step8()								-- 玩家在家，收走1013给让他回家的1014--同时继续
--	else
--		script6_step6()								-- 玩家不在家，收走1013给让他回家的1014
--	end
--end
--function script6_step6()
--	open_ciweiui(22)
--	next_step("script6_step7", script6_step7)
--end
--function script6_step7()
--	complete_task(1013)	
--end
--function script6_step8()
--	open_ciweiui(24)
--	next_step("script6_step9", script6_step9)	
--end
--function script6_step9()
--	complete_task(1013)	
--end
-------------------------------------------------------------
-- 脚本7(换装)
-------------------------------------------------------------
function script7()	-- 脚本入口
	-- g_Loger:logMessage("--- script7() ---")
	if true == is_exist_task(1106) then		-- 检查玩家有没有换装教学任务
			script7_step20()
	end	
end
function script7_step20()
	open_ciweiui(113)
	complete_task(1106)	
	next_step("script7_step21", script7_step21)
end
function script7_step21()
end



-------------------------------------------------------------
-- 脚本8(超链接按钮使用帮助,不属于新手任务)
-------------------------------------------------------------
function script8()
	-- g_Loger:logMessage("--- script8() ---")
	if true == is_exist_task(10000) then
		local data = {}
		data["row"] = 2
		data["col"] = 3
		data["set"] = {}
		data["img"] = {}
		for i=1, 6 do
			table.insert(data["set"], "linkhelp_0"..tostring(i))
			table.insert(data["img"], "NewImage")		
		end
		PlayerGuide_ShowPictureByData(data)
		complete_task(10000)
	end
end



-- 新手任务二期内容
-------------------------------------------------------------
-- 脚本9(玩家进入自己房屋时触发)
-------------------------------------------------------------
local is_excute_script9_step1 = true
local is_excute_script9_step10 = true
local is_excute_script9_step20 = true
local is_excute_script9_step30 = true
local is_excute_script5_step1 = true
local is_excute_script9_step40 = true
local is_excute_script27_step1 = true
local is_excute_script9_step50 = true
local is_excute_script30_step2 = true
local is_excute_script9_step60 = true
local is_excute_script9_step70 = true
--
function script9()
	g_Loger:logMessage("--- script9()------------------------------")
	if true == is_exist_task(1108) then		-- 检查玩家有没有家装的教学任务
		if is_excute_script9_step1 then
			script9_step1()
			is_excute_script9_step1 = false
		end
	end	
	if true == is_exist_task(1103) then		-- 检查玩家有没查看任务的教学任务
		if is_excute_script9_step10 then
			script9_step10()
			is_excute_script9_step10 = false
		end
	end	
	if true == is_exist_task(1104) then		-- 检查玩家有没有前往服装店教学任务
		if is_excute_script9_step20 then
			script9_step20()
			is_excute_script9_step20 = false
		end
	end	
	if true == is_exist_task(1105) then		-- 检查玩家有没有和洛克谈谈教学任务
		if is_excute_script9_step30 then
			script9_step30()
			is_excute_script9_step30 = false
		end
	end	
	if true == is_exist_task(1106) then		-- 检查玩家有没有穿衣服教学任务
		if is_excute_script5_step1 then
			script5_step1()
			is_excute_script5_step1 = false
		end
	end	
	if true == is_exist_task(1107) then		-- 检查玩家有没有和家具店的麦克斯韦谈谈教学任务
		if is_excute_script9_step40 then
			script9_step40()
			is_excute_script9_step40 = false
		end
	end	
	if true == is_exist_task(1109) then		-- 检查玩家有没有开灯教学任务
		if is_excute_script27_step1 then
			script27_step1()
			is_excute_script27_step1 = false
		end
	end	
	if true == is_exist_task(1110) then		-- 检查玩家有没有打工教学任务
		if is_excute_script9_step50 then
			script9_step50()
			is_excute_script9_step50 = false
		end
	end	
	if true == is_exist_task(1111) then		-- 检查玩家有没有访问gm房间教学任务
		if is_excute_script30_step2 then
			script30_step2()
			is_excute_script30_step2 = false
		end
	end	
	if true == is_exist_task(1112) then		-- 检查玩家有没有帮好友整理房间教学任务
		if is_excute_script9_step60 then
			script9_step60()
			is_excute_script9_step60 = false
		end
	end	
	if true == is_exist_task(1113) then		-- 检查玩家有没有最后介绍教学任务
		if is_excute_script9_step70 then
			script9_step70()
			is_excute_script9_step70 = false
		end
	end	
--	if true == is_exist_task(1014) then		-- 检查玩家有没有回家开始家装的任务
--		script9_step5()
--		g_Loger:logMessage("--- 2222222 ---")
--	end	
end
----------------------------------------------有家装任务时，回到家里
function script9_step1()
	open_ciweiui(116)
	next_step("script9_step2", script9_step2)
end
function script9_step2()
	show_picture(7)	
	next_step("script9_step3", script9_step3)
end
function script9_step3()
	open_ciweiui(117)
	pre_step("script9_step2",script9_step2)
	next_step("script9_step4", script9_step4)
end
function script9_step4()
end
---------------------------------------还没按任务按钮时退出，重新进入房间
function script9_step10()
	open_ciweiui(104)
	next_step("script1_step6", script1_step6)
end
---------------------------------------还没按路标时退出
function script9_step20()
	open_ciweiui(105)
	next_step("script2_step4", script2_step4)
end
--------------------------------------------还没和洛克对话退出
function script9_step30()
	open_ciweiui(109)
	next_step("script20_step2", script20_step2)
end
----------------------------------------还没和麦克斯韦对话退出
function script9_step40()
	open_ciweiui(113)
	next_step("script7_step21", script7_step21)
end
------------------------------------------刚接到打工任务退出
function script9_step50()
	open_ciweiui(123)
	next_step("script29_step2", script29_step2)
end
-----------------------------------------刚到gm房间就退出
function script9_step60()
	open_ciweiui(131)
	open_ciweiui(132)
	next_step("script33_step3", script33_step3)
end
----------------------------------------最后教学没完就退出
function script9_step70()
	open_ciweiui(134)
	next_step("script31_step3", script31_step3)
end

------------------------邮件的教学----------------------------------------
---function script9_step2()
---	open_ciweiui(13)
---	next_step("script9_step3", script9_step3)
---end
---function script9_step3()
---	show_picture(14)	
---	next_step("script9_step4", script9_step4)
---end
---function script9_step4()
---	complete_task(1009)	
---end
------------------------家装的教学----------------------------------------
--function script9_step5()
--	open_ciweiui(23)
--	next_step("script9_step6", script9_step6)
--end
--function script9_step6()
--	show_picture(7)	
--	next_step("script9_step7", script9_step7)
--end
--function script9_step7()
--	complete_task(1014)	
--end
------------------------礼物的教学----------------------------------------
--function script9_step10()
--	open_ciweiui(39)
--	next_step("script9_step11", script9_step11)
--end
--function script9_step11()
--	show_picture(16)	
--	next_step("script9_step12", script9_step12)
--end
--function script9_step12()
--	complete_task(1009)	
--end
-------------------------------------------------------------
-- 脚本10(打开家装系统时触发)
-------------------------------------------------------------
function script10()
	 g_Loger:logMessage("--- script10() ---")
	if true == is_exist_task(1108) then		-- 检查玩家有没有进入家装的任务
		script10_step1()
	end	
end
function script10_step1()
	 g_Loger:logMessage("--- stept1 ---")
	open_ciweiui(118)
	next_step("script10_step2", script10_step2)
end
function script10_step2()
	show_picture(22)
	next_step("script10_step3", script10_step3)
end
function script10_step3()
	open_ciweiui(139)
	next_step("script10_step4", script10_step4)
end
function script10_step4()
	open_ciweiui(119)
	pre_step("script10_step2",script10_step2)
	next_step("script10_step5", script10_step5)
end
function script10_step5()
	show_picture(24)
end
--function script10_step1()
--	open_ciweiui(25)
--	next_step("script10_step2", script10_step2)
--end
--function script10_step2()
--	show_picture(17)	
--	next_step("script10_step3", script10_step3)
--end
--function script10_step3()
--	open_ciweiui(26)
--	next_step("script10_step4", script10_step4)
--end
--function script10_step4()
--	show_picture(8)	
--	next_step("script10_step5", script10_step5)
--end
--function script10_step5()
--	open_ciweiui(27)
--	open_ciweiui(13)
--	next_step("script10_step6", script10_step6)
--end
--function script10_step6()
--	show_picture(19)	
--	next_step("script10_step7", script10_step7)
--end
--function script10_step7()
--	complete_task(1015)	
--end
-------------------------------------------------------------
-- 脚本11(打开打工界面时触发)
-------------------------------------------------------------
function script11()
	-- g_Loger:logMessage("--- script11() ---")
	if true == is_exist_task(1110) then		-- 检查玩家有没有打工教学任务
		script11_step1()
	end
end
function script11_step1()
	open_ciweiui(125)
	next_step("script11_step2", script11_step2)
end
function script11_step2()
	open_ciweiui(126)
	next_step("script11_step3", script11_step3)
end
function script11_step3()
end



----------------------------------------------------------
---打工完毕---------------------目前没有打工完毕的脚本入口，暂时直接跳过
--function script11_step3()
--	complete_task(1110)	
--	next_step("script11_step4", script11_step4)
--end
--function script11_step4()
--	open_ciweiui(56)
--	open_ciweiui(57)
--	open_ciweiui(58)
--	open_ciweiui(59)
--	open_ciweiui(60)
--	next_step("script11_step5", script11_step5)
--end
--function script11_step5()
--	show_picture(14)
--	next_step("script11_step6", script11_step6)
--end
--function script11_step6()
--	open_ciweiui(61)
--	pre_step("script11_step5",script11_step5)
--	next_step("script11_step7", script11_step7)
--end
--function script11_step7()
--end

-------------------------------------------------------------
-- 脚本12(打开娱乐界面时触发)
-------------------------------------------------------------
function script12()
	-- g_Loger:logMessage("--- script12() ---")
	if true == is_exist_task(1110) then		-- 检查玩家有没有打工教学任务
		script12_step1()
	end	
--	if true == is_exist_task(1113) then		-- 检查玩家有没有打工教学任务
--		script12_step10()
--	end	
--	if true == is_exist_task(1018) then		-- 检查玩家有没有进入娱乐去农场的任务
--		script12_step5()
--	end	
end
function script12_step1()
	open_ciweiui(62)
	next_step("script12_step2", script12_step2)
end
function script12_step2()
end
function script12_step10()
	open_ciweiui(138)
	next_step("script12_step11", script12_step11)
end
function script12_step11()
	open_ciweiui(136)
	next_step("script12_step12", script12_step12)
end
function script12_step12()
	show_picture(14)	
	next_step("script12_step13", script12_step13)
end
function script12_step13()
	open_ciweiui(137)	
	complete_task(1113)	
	next_step("script12_step14", script12_step14)
end
function script12_step14()
end

--function script12_step5()
--	open_ciweiui(35)	
--	next_step("script12_step6", script12_step6)
--end
--function script12_step6()
--	show_picture(20)	
--	next_step("script12_step7", script12_step7)
--end
--function script12_step7()
--	open_ciweiui(36)	
--	open_ciweiui(37)
--	open_ciweiui(38)
--	next_step("script12_step8", script12_step8)
--end
--function script12_step8()
--	complete_task(1018)	
--end
-------------------------------------------------------------
-- 脚本13(打开邮箱界面时触发)
-------------------------------------------------------------
--function script13()
---	-- g_Loger:logMessage("--- script13() ---")
--	if true == is_exist_task(1020) then		-- 检查玩家有没有指示邮件位置的教学任务
--		script13_step1()
--	end
--end
--function script13_step1()
--	open_ciweiui(14)
--	open_ciweiui(15)
--	next_step("script13_step4", script13_step4)
--end
--function script13_step4()
--	open_ciweiui(28)
--	open_ciweiui(29)
--	next_step("script13_step5", script13_step5)
--end
--function script13_step5()
--	show_picture(18)	  -- 指示娱乐位置
--	next_step("script13_step6", script13_step6)
--end
--function script13_step6()
--	complete_task(1020)	
--end
-------------------------------------------------------------
-- 脚本14(点击礼物按钮时触发)
-------------------------------------------------------------
--function script14()
--	-- g_Loger:logMessage("--- script14() ---")
--	if true == is_exist_task(1011) then		-- 检查玩家有没有发送礼物的教学任务
--		script14_step1()
--	end	
--end
--function script14_step1()
--	open_ciweiui(18)
--	open_ciweiui(19)
--	open_ciweiui(20)
--	open_ciweiui(21)
--	next_step("script14_step2", script14_step2)
--end
--function script14_step2()
--	show_picture(13)	
--	next_step("script14_step3", script14_step3)	
--end
--function script14_step3()
--	complete_task(1011)	
--end
-------------------------------------------------------------
-- 脚本15(打开快递礼物时触发)
-------------------------------------------------------------
--function script15()
--	-- g_Loger:logMessage("--- script15() ---")
--	if true == is_exist_task(1010) then		-- 检查玩家有没有发送礼物的教学任务
--		script15_step4()
--	end	
--	if true == is_exist_task(1009) then		-- 检查玩家有没有发送礼物的教学任务
--		script15_step1()
--	end	
--end
--function script15_step1()
--	complete_task(1009)
--	next_step_ex("script15_step4", script15_step4)
--end
--function script15_step4()
--	open_ciweiui(16)
--	open_ciweiui(41)
--	next_step("script15_step5", script15_step5)
--end
--function script15_step5()
--	complete_task(1010)	
--end



-------------------------------------------------------------
-- 脚本16(点击快递礼物接收按钮时触发)
-------------------------------------------------------------
--function script16()
--	-- g_Loger:logMessage("--- script16() ---")
--	if true == is_exist_task(1022) then		-- 检查玩家有没有要玩家回家摆家具任务
--		script16_step1()
--	end
--end
--function script16_step1()
--	open_ciweiui(42)
--	open_ciweiui(17)
--	next_step("script16_step2", script16_step2)
--end
--function script16_step2()
--	show_picture(21)	-- 这个要换
--	next_step("script16_step3", script16_step3)	
--end
--function script16_step3()
--	complete_task(1022)	
--end
-------------------------------------------
--摆放完家具触发
function script17()
	g_Loger:logMessage("--- script17() ---")
	if true == is_exist_task(1108) then		-- 检查玩家有没有要摆家具任务
		script17_step1()
	end
end
function script17_step1()
	complete_task(1108)	
	open_ciweiui(120)
	next_step("script17_step4", script17_step4)
end
function script17_step4()
show_picture(25)
end
-------------------------------------------
----幽静山谷
function script18()
	g_Loger:logMessage("--- script18() ---")
end
-------------------------------------------
--乐园大道
function script19()
	g_Loger:logMessage("--- script19() ---")
end
-------------------------------------------
--服装店
function script20()
	g_Loger:logMessage("--- script20() ---")
	if true == is_exist_task(1104) then		-- 检查玩家有没有路标教学任务
		script20_step1()
	end
end
function script20_step1()
	open_ciweiui(109)
	complete_task(1104)
	next_step("script20_step2", script20_step2)
end
function script20_step2()
end
-------------------------------------------
--家具店
function script21()
	g_Loger:logMessage("--- script21() ---")
end-------------------------------------------
--美容院
function script22()
	g_Loger:logMessage("--- script22() ---")
end-------------------------------------------
--道具店
function script23()
	g_Loger:logMessage("--- script23() ---")
end-------------------------------------------
--医院
function script24()
	g_Loger:logMessage("--- script24() ---")
end-------------------------------------------
--售楼部
function script25()
	g_Loger:logMessage("--- script25() ---")
end
---------------------------------------------
--点击路标
function script26()
	g_Loger:logMessage("--- script26() ---")
	if true == is_exist_task(1104) then		-- 检查玩家有没有路标教学任务
		script26_step1()
	end
end
function script26_step1()
	open_ciweiui(108)
	next_step("script26_step3", script26_step3)
end
function script26_step2()
end
---------------------------------------------
--退出家装
function script27()
	g_Loger:logMessage("--- script27() ---")
	if true == is_exist_task(1109) then		-- 检查玩家有没有使用家具任务
		script27_step1()
	end
end
function script27_step1()
	open_ciweiui(121)
	next_step("script27_step2", script27_step2)
end
function script27_step2()
	show_picture(8)
	next_step("script27_step3", script27_step3)
end
function script27_step3()
	open_ciweiui(122)
	pre_step("script27_step2",script27_step2)
	next_step("script27_step4", script27_step4)
end
function script27_step4()
end


-------------------------------------------------------------
---开灯操作后------------------目前没有开灯的脚本入口，直接跳过来
--function script27_step4()
--	complete_task(1109)	
--	open_ciweiui(52)
--	next_step("script27_step5", script27_step5)
--end
--function script27_step5()
--	open_ciweiui(53)
--	next_step("script27_step6", script27_step6)
--end
--function script27_step6()
--end

---------------------------------------------
--摆放完灯具
function script28()
	g_Loger:logMessage("--- script28() ---")
	if true == is_exist_task(1108) then		-- 检查玩家有没有要摆家具任务
		script17_step1()
	end
end
---------------------------------------------
--开灯后
function script29()
	g_Loger:logMessage("--- script29() ---")
	if true == is_exist_task(1109) then		-- 检查玩家有没有开灯任务
		script29_step1()
	end
end
function script29_step1()
	complete_task(1109)	
	open_ciweiui(123)
	next_step("script29_step2", script29_step2)
end
function script29_step2()
	open_ciweiui(124)
	next_step("script29_step3", script29_step3)
end
function script29_step3()
	show_picture(16)
end
---------------------------------------------
--打工结束
function script30()
	g_Loger:logMessage("--- script30() ---")
	if true == is_exist_task(1110) then		-- 检查玩家有没有打工任务
		script30_step1()
	end
end
function script30_step1()
	complete_task(1110)	
	next_step("script30_step2", script30_step2)
end
function script30_step2()
	open_ciweiui(127)
	next_step("script30_step3", script30_step3)
end
function script30_step3()
	open_ciweiui(128)
	next_step("script30_step4", script30_step4)
end
function script30_step4()
	open_ciweiui(129)
	next_step("script30_step5", script30_step5)
end
function script30_step5()
	show_picture(26)
	next_step("script30_step6", script30_step6)
end
function script30_step6()
	open_ciweiui(130)
	pre_step("script30_step5",script30_step5)
	next_step("script30_step7", script30_step7)
end
function script30_step7()
end
---------------------------------------------
--捡到垃圾
function script31()
	g_Loger:logMessage("--- script31() ---")
	if true == is_exist_task(1112) then		-- 检查玩家有没有打扫任务
		script31_step2()
	end
end
function script31_step2()
	complete_task(1112)	
	open_ciweiui(134)
	next_step("script31_step3", script31_step3)
end
function script31_step3()
	open_ciweiui(135)
	next_step("script31_step11", script31_step11)
end
function script31_step11()
	open_ciweiui(136)
	next_step("script31_step12", script31_step12)
end
function script31_step12()
	show_picture(14)	
	next_step("script31_step13", script31_step13)
end
function script31_step13()
	open_ciweiui(137)	
	complete_task(1113)	
	next_step("script31_step14", script31_step14)
end
function script32_step14()
end
---------------------------------------------
--捡到宝箱
function script32()
	g_Loger:logMessage("--- script32() ---")
	if true == is_exist_task(1112) then		-- 检查玩家有没有打扫任务
		script32_step2()
	end
end
function script32_step2()
	complete_task(1112)	
	open_ciweiui(134)
	next_step("script32_step3", script32_step3)
end
function script32_step3()
	open_ciweiui(135)
	next_step("script32_step11", script32_step11)
end
function script32_step11()
	open_ciweiui(136)
	next_step("script32_step12", script32_step12)
end
function script32_step12()
	show_picture(14)	
	next_step("script32_step13", script32_step13)
end
function script32_step13()
	open_ciweiui(137)	
	complete_task(1113)	
	next_step("script32_step14", script32_step14)
end
function script32_step14()
end
---------------------------------------------
--访问GM房间
function script33()
	g_Loger:logMessage("--- script33() ---")
	if true == is_exist_task(1111) then		-- 检查玩家有没有拜访好友任务
		script33_step2()
	end
end
--function script33_step1()
--	complete_task(1111)	
--	g_Loger:logMessage("--- script33() ---step1")
--	next_step("script33_step2", script33_step2)
--end
function script33_step2()
	complete_task(1111)	
	g_Loger:logMessage("--- script33() ---step2")
	open_ciweiui(131)
	open_ciweiui(132)
	next_step("script33_step3", script33_step3)
end
function script33_step3()
	show_picture(27)
	next_step("script33_step4", script33_step4)
end
function script33_step4()
	open_ciweiui(133)
	pre_step("script33_step3",script33_step3)
	next_step("script33_step5", script33_step5)
end
function script33_step5()
end





---------------------------------------------
--第一次看到垃圾、宝箱
function script15000()
	g_Loger:logMessage("--- script15000() ---")
	if true == is_exist_task(15000) then	
		script15000_step1()
	end
end
function script15000_step1()
	open_ciweiui(300)
	complete_task(15000)	
end

---------------------------------------------
--第一次进入商城（仅限点击界面上按钮进入）
function script15001()
	g_Loger:logMessage("--- script15001() ---")
	if true == is_exist_task(15001) then	
		script15001_step1()
	end
end
function script15001_step1()
	open_ciweiui(301)
	complete_task(15001)	
end
---------------------------------------------
--第一次在家中收到礼物
function script15002()
	if true == is_exist_task(15002) then	
		script15002_step1()
	end
end
function script15002_step1()
	open_ciweiui(302)
	complete_task(15002)	
end
---------------------------------------------
--第一次收到快递礼物
function script15003()
	if true == is_exist_task(15003) then	
		script15003_step1()
	end
end
function script15003_step1()
	open_ciweiui(303)
	complete_task(15003)	
end
---------------------------------------------
--第一次打开礼物界面
function script15004()
	if true == is_exist_task(15004) then	
		script15004_step1()
	end
end
function script15004_step1()
	open_ciweiui(304)
	complete_task(15004)	
end
---------------------------------------------
--第一次使用漂流水池
function script15005()
	if true == is_exist_task(15005) then	
		script15005_step1()
	end
end
function script15005_step1()
	open_ciweiui(305)
	complete_task(15005)	
end
---------------------------------------------
--第一次打开仓库
function script15006()
	if true == is_exist_task(15006) then	
		script15006_step1()
	end
end
function script15006_step1()
	open_ciweiui(306)
	complete_task(15006)	
end
---------------------------------------------
--第一次打开邮件
function script15007()
	if true == is_exist_task(15007) then	
		script15007_step1()
	end
end
function script15007_step1()
	complete_task(15007)	
end
---------------------------------------------
--第一次成功购买房屋之后
function script15008()
	if true == is_exist_task(15008) then	
		script15008_step1()
	end
end
function script15008_step1()
	open_ciweiui(308)
	complete_task(15008)	
end
---------------------------------------------
--第一次点击动作按钮
function script15009()
	if true == is_exist_task(15009) then	
		script15009_step1()
	end
end
function script15009_step1()
	open_ciweiui(309)
	complete_task(15009)	
end
---------------------------------------------
--清洁度低到30以下
function script15010()
	if true == is_exist_task(15010) then	
		script15010_step1()
	end
end
function script15010_step1()
	open_ciweiui(310)
	complete_task(15010)	
end
---------------------------------------------
--健康度低到30以下
function script15011()
	if true == is_exist_task(15011) then	
		script15011_step1()
	end
end
function script15011_step1()
	open_ciweiui(311)
	complete_task(15011)	
end
---------------------------------------------
--第一次生病
function script15012()
	if true == is_exist_task(15012) then	
		script15012_step1()
	end
end
function script15012_step1()
	open_ciweiui(312)
	complete_task(15012)	
end
---------------------------------------------
--房屋清洁度低到30以下
function script15013()
	if true == is_exist_task(15013) then	
		script15013_step1()
	end
end
function script15013_step1()
	open_ciweiui(313)
	complete_task(15013)	
end
---------------------------------------------
--玩家在新手完成后，替换完服装或摆放完家具，出现提示玩家使用照相功能（新手是否完成由策划判断）
function script15014()
	if true == is_exist_task(15014) then	
		script15014_step1()
	end
end
function script15014_step1()
	open_ciweiui(314)
	complete_task(15014)	
end
---------------------------------------------
--教玩家去访问好友
function script17500()
	if true == is_exist_task(17500) then	
		script17500_step1()
	end
end
function script17500_step1()
	open_ciweiui(500)
	complete_task(17500)	
end
---------------------------------------------
--教玩家去赚钱
function script17501()
	if true == is_exist_task(17501) then	
		script17501_step1()
	end
end
function script17501_step1()
	open_ciweiui(501)
	complete_task(17501)	
end
---------------------------------------------
--教玩家提高个人清洁度
function script17502()
	if true == is_exist_task(17502) then	
		script17502_step1()
	end
end
function script17502_step1()
	complete_task(17502)	
end
---------------------------------------------
--教玩家替换发型
function script17503()
	if true == is_exist_task(17503) then	
		script17503_step1()
	end
end
function script17503_step1()
	open_ciweiui(503)
	complete_task(17503)	
end
---------------------------------------------
--教玩家扔漂流瓶
function script17504()
	if true == is_exist_task(17504) then	
		script17504_step1()
	end
end
function script17504_step1()
	open_ciweiui(504)
	complete_task(17504)	
end
---------------------------------------------
--教玩家去使用农场
function script17505()
	if true == is_exist_task(17505) then	
		script17505_step1()
	end
end
function script17505_step1()
	open_ciweiui(505)
	complete_task(17505)	
end
---------------------------------------------
--教玩家去将农场和游戏中的物品互通
function script17506()
	if true == is_exist_task(17506) then	
		script17506_step1()
	end
end
function script17506_step1()
	open_ciweiui(506)
	complete_task(17506)	
end
---------------------------------------------
--教玩家去使用交易
function script17507()
	if true == is_exist_task(17507) then	
		script17507_step1()
	end
end
function script17507_step1()
	open_ciweiui(507)
	complete_task(17507)	
end
---------------------------------------------
--教玩家去使用商店
function script17508()
	if true == is_exist_task(17508) then	
		script17508_step1()
	end
end
function script17508_step1()
	open_ciweiui(508)
	complete_task(17508)	
end
---------------------------------------------
--教玩家去购买服装
function script17509()
	if true == is_exist_task(17509) then	
		script17509_step1()
	end
end
function script17509_step1()
	open_ciweiui(509)
	complete_task(17509)	
end
---------------------------------------------
--教玩家去购买家具
function script17510()
	if true == is_exist_task(17510) then	
		script17510_step1()
	end
end
function script17510_step1()
	open_ciweiui(510)
	complete_task(17510)	
end