-----------------------------------------------------------------
--动态消息
--xiaoMing
--2010-09-27
--在界面的左上方显示，如疾病，新闻爆料、报纸等等 目前只临时显示疾病
--只能从表头压入消息
--消息的移除使用table.remove 由于表的长度很小 所以对性能不会造成影响
-----------------------------------------------------------------
local mMsgTable = {}
local mSlotCount = 5
local mSlotName = "DFwnd_MainRoot/tip/tip"

-----------------------------------------------------------------
function AddDynamicMsg(name, imageSet, image, functionName )
	if true == IsExist(name) then
		return
	end
	
	table.insert(mMsgTable, 1, {name = name, imageSet = imageSet, image = image, functionName = functionName})
	UpdataDynamicMsg()
end
-----------------------------------------------------------------
function IsExist(name)
	for i, v in pairs(mMsgTable) do
		if name == v.name then
			return true
		end
	end
	return false
end
-----------------------------------------------------------------
function RemoveDynamicMsg(name)
	for i = 1, table.getn(mMsgTable) do
		if mMsgTable[i].name == name then
			table.remove(mMsgTable, i)
			break
		end
	end
	
	UpdataDynamicMsg()

end
-----------------------------------------------------------------
function UpdataDynamicMsg()
	ClearDynamicMsgWnd()
	local slotName, slotWnd, imageStr
	
	for i = 1, math.min(table.getn(mMsgTable), mSlotCount) do
		slotName = mSlotName..i
		
		if nil ~= mMsgTable[i].functionName then
			slotWnd = g_WinMgr:getWindow(slotName)
			slotWnd:subscribeEvent("MouseClick", mMsgTable[i].functionName)
		end
		SetWindowImage(slotName, mMsgTable[i].imageSet, mMsgTable[i].image)
	end
end
-----------------------------------------------------------------
function ClearDynamicMsgWnd()
	for i = 1, mSlotCount do
		local slotWnd = g_WinMgr:getWindow(mSlotName..i)
		if true == slotWnd:isPropertyPresent ("Image") then
			slotWnd:setProperty("Image", "")
			--slotWnd:removeProperty("Image")
		end
		slotWnd:removeEvent("MouseClick")
	end
end