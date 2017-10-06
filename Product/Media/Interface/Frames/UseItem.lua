-----------------------------------------
-- 文件说明	：物品使用
-- 作者		：lf
-- 修改		：
-- 日期		：11/04/20
-----------------------------------------

-----------------------------------------
--入口
-----------------------------------------
function UseItem(itemid)
	local itemrow = g_itemtableMgr:getItemRow(itemid)
	if itemrow ~= nil then
		if itemrow.useID ~= 0 then
			g_Loger:logDebug("UseItem("..itemrow.useID..")")
			EQGlobaFunc.callFunction("UseItem_"..itemrow.useID)
		end
	else
		g_Loger:logDebug("item dont exist "..itemid)
	end
end

-----------------------------------------
--接口说明
-----------------------------------------
-- #T                                 主角目标
-- openUI(win)                        打开UI
-- print(msg)                         打印信息
-- playAnimation(ani, target)         主角播放动画
--

function UseItem_1()
	g_itemMgr:openUI("Root/store")
	g_itemMgr:playAnimation("Cry", "#T")
	g_itemMgr:print("使用物品")
end