-----------------------------------------
-- �ļ�˵��	�������ڿؼ�
-- ����		��xj
-- ����		��2011/4/28
-----------------------------------------

--���ڿؼ������б�
local mDataTimeCtrlList = {}
--��ʼ���
local mDataTimeYearStart = 1949
--�������
local mDataTimeYearEnd = 2049

--------------------------------------------
--�������ڿؼ�����
--name:��������
--syear:����Ͽ�ؼ�����
--smonth:����Ͽ�ؼ�����
--sday:����Ͽ�ؼ�����
--------------------------------------------
function CreateDataTime(name, syear, smonth, sday)
	if nil ~= mDataTimeCtrlList[name] then
		return false
	end
	
	--��������
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
	
	--�ؼ��Ͷ��������������ڷ���ȡ����
	cmbYear:getPushButton():setText(name)
	cmbMonth:getPushButton():setText(name)
	cmbDay:getPushButton():setText(name)
	
	cmbYear:subscribeEvent("ListSelectionAccepted", "OnYearCmbListSelectionAccepted")
	cmbMonth:subscribeEvent("ListSelectionAccepted", "OnMonthCmbListSelectionAccepted")
	cmbDay:subscribeEvent("ListSelectionAccepted", "OnDayCmbListSelectionAccepted")
	
	--��ʹ��Ϊ��ǰʱ��
	SetDataTimeYear(name, tonumber(os.date("%Y")))
	SetDataTimeMonth(name, tonumber(os.date("%m")))
	SetDayList(name)
	SetDataTimeDay(name, tonumber(os.date("%d")))
	
	return true
end

--------------------------------------------
--ɾ�����ڿؼ�����
--name:��������
--------------------------------------------
function DelDataTime(name)
	mDataTimeCtrlList[name] = nil
end

--------------------------------------------
--��������������ڿؼ�����
--------------------------------------------
function ClearDataTime()
	local i
	local n = table.getn(mDataTimeCtrlList)
	
	for i = 1, n do
		table.remove(mDataTimeCtrlList)
	end
end

--------------------------------------------
--��ȡ���ڿؼ��������
--name:��������
--------------------------------------------
function GetDataTimeYear(name)
	return mDataTimeCtrlList[name].year
end

--------------------------------------------
--��ȡ���ڿؼ������·�
--name:��������
--------------------------------------------
function GetDataTimeMonth(name)
	return mDataTimeCtrlList[name].month
end

--------------------------------------------
--��ȡ���ڿؼ�������
--name:��������
--------------------------------------------
function GetDataTimeDay(name)
	return mDataTimeCtrlList[name].day
end

--------------------------------------------
--�������ڿؼ��������
--name:��������
--year:���
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
--�������ڿؼ������·�
--name:��������
--month:�·�
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
--�������ڿؼ�������
--name:��������
--day:��
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
--����ĳ��ĳ�µ�����
--year:���
--month:�·�
--------------------------------------------
function CalcDays(year, month)
	local mdays = {31,28,31,30,31,30,31,31,30,31,30,31}
	
	--������������
	if ((year % 4 == 0 and year % 100 ~= 0) or year % 400 == 0) then
		mdays[2] = 29
	end
	
	return mdays[month]
end

--�������б�
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
