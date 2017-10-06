-----------------------------------------
-- 文件说明	：访问好友鲜花鸡蛋
-- 作者		：lf
-- 修改		：
-- 日期		：11/09/21
-----------------------------------------

-----------------------------------------
--脚本加载后执行
-----------------------------------------
function Fne_Onload()
	g_WinMgr:getWindow("fne/Btn_exit"):subscribeEvent("Clicked", "Fne_Close")
	g_WinMgr:getWindow("fne/Btn_flower"):subscribeEvent("Clicked", "Fne_Flower")
	g_WinMgr:getWindow("fne/Btn_egg"):subscribeEvent("Clicked", "Fne_Egg")
end

-----------------------------------------
--打开
-----------------------------------------
function Fne_Open(flower, egg)
	Fne_setFlower(flower)
	Fne_setEgg(egg)
	g_MainUIWindows:addChildWindow(g_Fne)
end

-----------------------------------------
--退出
-----------------------------------------
function Fne_Close()
	g_MainUIWindows:removeChildWindow(g_Fne)	
end

-----------------------------------------
--鲜花数
-----------------------------------------
function Fne_setFlower(flower)
	g_WinMgr:getWindow("fne/txt2"):setText(flower)
end

-----------------------------------------
--鸡蛋数
-----------------------------------------
function Fne_setEgg(egg)
	g_WinMgr:getWindow("fne/txt3"):setText(egg)
end

-----------------------------------------
--送鲜花
-----------------------------------------
function Fne_Flower()
	g_msgmgr:send( EQGlobaClass.req_send_flower() )
	Fne_Close()
end

-----------------------------------------
--扔鸡蛋
-----------------------------------------
function Fne_Egg()
	g_msgmgr:send( EQGlobaClass.req_send_egg() )
	Fne_Close()
end
