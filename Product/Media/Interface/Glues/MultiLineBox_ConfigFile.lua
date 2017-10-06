-----------------------------------------
-- �ļ�˵��	��ͼ�Ļ��ſؼ������ļ�
-- ����		����չȻ
-- �޸�		��
-- ����		��10/10/25
-----------------------------------------
--���ֶΡ�
-- Pare��	���������ƣ���:"MainWindow"
-- Xpos��	x���꣬��:50
-- Ypos��	y���꣬��:50
-- Width��	��ȣ���:200
-- Height��	�߶ȣ���:100
-- Font��	�������ƣ���"simsun-10"
-- Cols��	�ı���ɫ����"#FF00FF"
-- Side��	�����ɫ��Cols:��ɫ����:"#FF00FF"��Pix:���أ���:1
-- HorAlign��	ˮƽ���뷽ʽ�����("AT_LEFT")��ˮƽ����("AT_CENTER_HOR")���ұ�("AT_RIGHT")
-- VerAlign��	��ֱ���뷽ʽ������("AT_TOP")����ֱ����("AT_CENTER_VER")���ײ�("AT_BOTTOM")
-- Scrollbar��	��ֱ����������ʾ(true)������(false)

--��ע��
-- ��ֵ��nil��["Pare"]�����ֵ��["Side"]����������ֵ����ʾû�����Ч�����磺["Side"] = {["Cols"] = nil, ["Pix"] = nil}

--�����ӡ�
-- MainWindow/window1
-- g_mlbConfig["MainWindow/window1"] = 
-- {
	-- ["Pare"] = "MainWindow",	-- �����ֵ:nil
	-- ["Xpos"] = 50,
	-- ["Ypos"] = 50,
	-- ["Width"] = 315,
	-- ["Height"] = 15,
	-- ["Font"] = "simsun-10",
	-- ["Cols"] = "#000000",
	-- ["Side"] = {["Cols"] = "#FFFFFF", ["Pix"] = 1},	-- �����ֵ:nil
	-- ["HorAlign"] = "AT_LEFT",
	-- ["VerAlign"] = "AT_CENTER_VER",
	-- ["Scrollbar"] = false
-- }

--�����ú�����
-- mlbox:ͼ�Ļ��ſؼ�
-- cfg:���ñ���:g_mlbConfig["MainWindow/window1"]
function configMultiLineBox(mlbox, cfg)
	-- g_Loger:logMessage("---start config mlbox---")
	local pare = cfg["Pare"]
	local xPos = cfg["Xpos"]
	local yPos = cfg["Ypos"]
	local width = cfg["Width"]
	local height = cfg["Height"]
	local textFont = cfg["Font"]
	local textCols = cfg["Cols"]
	local sideCols = cfg["Side"]["Cols"]
	local sidePix = cfg["Side"]["Pix"]
	local horAlign = cfg["HorAlign"]
	local verAlign = cfg["VerAlign"]
	local scrBarVisi = cfg["Scrollbar"]
	-- g_Loger:logMessage("---get config data over---")
	if nil ~= pare then	--	�����õ�ʱ���踸����
		if nil~= g_WinMgr:getWindow(pare) then
			g_WinMgr:getWindow(pare):addChildWindow(mlbox)
		end
	end
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setPosition(xPos, yPos)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setSize(width, height)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setFont(textFont)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setCols(textCols)
	if nil~=sideCols or nil~=sidePix then	--	�����Ч��
		EQGlobaFunc.toMultiLineBox(mlbox:getName()):setSide(sideCols, sidePix)
	end
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setAlign(horAlign)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setAlign(verAlign)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setScrollbarVisible("SP_OLD", scrBarVisi)
	-- g_Loger:logMessage("---end config mlbox---")
end

--����ʼ���á�




