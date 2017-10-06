-----------------------------------------
-- 文件说明	：简单日期控件
-- 作者		：xj
-- 日期		：2011/4/28
-----------------------------------------

--日期控件对象列表
local mDataTimeCtrlList = {}
--开始年份
local mDataTimeYearStart = 1949
--结束年份
local mDataTimeYearEnd = 2049

--------------------------------------------
--创建日期控件对象
--name:对象名称
--syear:年组合框控件名称
--smonth:月组合框控件名称
--sday:日组合框控件名称
--------------------------------------------
function CreateDataTime(name, syear, smonth, sday)
	if nil ~= mDataTimeCtrlList[name] then
		return false
	end
	
	--创建对象
	mDataTimeCtrlList[name] = { ywn = syear, mwn = smonth, dwn = sday, year = mDataTimeYearStart, month = 1, day = 1 }
	
	local cmbYear = CEGUI.toCombobox(g_WinMgr:getWindow(syear))
	local cmbMonth = CEGUI.toCombobox(g_WinMgr:getWindow(smonth))
	local cmbDay = CEGUI.toCombobox(g_WinMgr:getWindow(sday))
	
	cmbYear:resetList()
	cmbMonth:resetList()
	cmbDay:resetList()
	
	local i
	for i = mDataTimeYearStart, mDataTimeYearEnd do
		EQGlobaFunc.createComboBoxTextItem(syear, tostring(i), i - mDataTimeYearStart,"")
	end
	
	for i = 1, 12 do
		EQGlobaFunc.createComboBoxTextItem(smonth, tostring(i), i,"")
	end
	
	--控件和对象名关联，便于反向取数据
	cmbYear:getPushButton():setText(name)
	cmbMonth:getPushButton():setText(name)
	cmbDay:getPushButton():setText(name)
	
	cmbYear:subscribeEvent("ListSelectionAccepted", "OnYearCmbListSelectionAccepted")
	cmbMonth:subscribeEvent("ListSelectionAccepted", "OnMonthCmbListSelectionAccepted")
	cmbDay:subscribeEvent("ListSelectionAccepted", "OnDayCmbListSelectionAccepted")
	
	--初使化为当前时间
	SetDataTimeYear(name, tonumber(os.date("%Y")))
	SetDataTimeMonth(name, tonumber(os.date("%m")))
	SetDayList(name)
	SetDataTimeDay(name, tonumber(os.date("%d")))
	
	return true
end

--------------------------------------------
--删除日期控件对象
--name:对象名称
--------------------------------------------
function DelDataTime(name)
	mDataTimeCtrlList[name] = nil
end

--------------------------------------------
--清空现有所有日期控件对象
--------------------------------------------
function ClearDataTime()
	local i
	local n = table.getn(mDataTimeCtrlList)
	
	for i = 1, n do
		table.remove(mDataTimeCtrlList)
	end
end

--------------------------------------------
--获取日期控件对象年份
--name:对象名称
--------------------------------------------
function GetDataTimeYear(name)
	return mDataTimeCtrlList[name].year
end

--------------------------------------------
--获取日期控件对象月份
--name:对象名称
--------------------------------------------
function GetDataTimeMonth(name)
	return mDataTimeCtrlList[name].month
end

--------------------------------------------
--获取日期控件对象日
--name:对象名称
--------------------------------------------
function GetDataTimeDay(name)
	return mDataTimeCtrlList[name].day
end

--------------------------------------------
--设置日期控件对象年份
--name:对象名称
--year:年份
--------------------------------------------
function SetDataTimeYear(name, year)
	if year < mDataTimeYearStart or year > mDataTimeYearEnd then
		return
	end
	
	local cbox = CEGUI.toCombobox(g_WinMgr:getWindow(mDataTimeCtrlList[name].ywn))
	cbox:setItemSelectState(year - mDataTimeYearStart, true)
	mDataTimeCtrlList[name].year = year
end

--------------------------------------------
--设置日期控件对象月份
--name:对象名称
--month:月份
--------------------------------------------
function SetDataTimeMonth(name, month)
	if month < 1 or month > 12 then
		return
	end
	
	local cbox = CEGUI.toCombobox(g_WinMgr:getWindow(mDataTimeCtrlList[name].mwn))
	cbox:setItemSelectState(month - 1, true)
	mDataTimeCtrlList[name].month = month
end

--------------------------------------------
--设置日期控件对象日
--name:对象名称
--day:日
--------------------------------------------
function SetDataTimeDay(name, day)
	local cbox = CEGUI.toCombobox(g_WinMgr:getWindow(mDataTimeCtrlList[name].dwn))
	
	if day < 1 or day > cbox:getItemCount() then
		return
	end
	
	cbox:setItemSelectState(day - 1, true)
	mDataTimeCtrlList[name].day = day
end

--------------------------------------------
--计算某年某月的天数
--year:年份
--month:月份
--------------------------------------------
function CalcDays(year, month)
	local mdays = {31,28,31,30,31,30,31,31,30,31,30,31}
	
	--计算润年天数
	if ((year % 4 == 0 and year % 100 ~= 0) or year % 400 == 0) then
		mdays[2] = 29
	end
	
	return mdays[month]
end

--设置天列表
function SetDayList(name)
	local ds = CalcDays(mDataTimeCtrlList[name].year, mDataTimeCtrlList[name].month)
	local cbox = CEGUI.toCombobox(g_WinMgr:getWindow(mDataTimeCtrlList[name].dwn))
	
	g_Loger:logMessage("update "..tostring(mDataTimeCtrlList[name].year).."-"..tostring(mDataTimeCtrlList[name].month).." days:"..tostring(ds))
	
	cbox:resetList()
	
	local i
	for i = 1, ds do
		EQGlobaFunc.createComboBoxTextItem(mDataTimeCtrlList[name].dwn, tostring(i), i,"")
	end
	
	i = mDataTimeCtrlList[name].day
	
	if i > ds then
		i = 1
	end
	
	cbox:setItemSelectState(i - 1, true)
end

function OnYearCmbListSelectionAccepted(args)
	local cbox = CEGUI.toCombobox(CEGUI.toWindowEventArgs(args).window)
	local name = cbox:getPushButton():getText()
	
	mDataTimeCtrlList[name].year = tonumber(cbox:getSelectedItem():getText())
	
	if 2 == mDataTimeCtrlList[name].month then
		SetDayList(name)
	end
	
	g_Loger:logMessage("current date: "..tostring(mDataTimeCtrlList[name].year).."-"..tostring(mDataTimeCtrlList[name].month).."-"..tostring(mDataTimeCtrlList[name].day))
end

function OnMonthCmbListSelectionAccepted(args)
	local cbox = CEGUI.toCombobox(CEGUI.toWindowEventArgs(args).window)
	local name = cbox:getPushButton():getText()
	
	mDataTimeCtrlList[name].month = tonumber(cbox:getSelectedItem():getText())
	
	SetDayList(name)
	
	g_Loger:logMessage("current date: "..tostring(mDataTimeCtrlList[name].year).."-"..tostring(mDataTimeCtrlList[name].month).."-"..tostring(mDataTimeCtrlList[name].day))
end

function OnDayCmbListSelectionAccepted(args)
	local cbox = CEGUI.toCombobox(CEGUI.toWindowEventArgs(args).window)
	local name = cbox:getPushButton():getText()
	
	mDataTimeCtrlList[name].day = tonumber(cbox:getSelectedItem():getText())
	
	g_Loger:logMessage("current date: "..tostring(mDataTimeCtrlList[name].year).."-"..tostring(mDataTimeCtrlList[name].month).."-"..tostring(mDataTimeCtrlList[name].day))
end
