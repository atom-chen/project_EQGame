-----------------------------------------
-- 文件说明	：图文混排控件配置文件
-- 作者		：何展然
-- 修改		：
-- 日期		：10/10/25
-----------------------------------------
--【字段】
-- Pare：	父窗口名称，如:"MainWindow"
-- Xpos：	x坐标，如:50
-- Ypos：	y坐标，如:50
-- Width：	宽度，如:200
-- Height：	高度，如:100
-- Font：	字体名称，如"simsun-10"
-- Cols：	文本颜色，如"#FF00FF"
-- Side：	描边颜色，Cols:颜色，如:"#FF00FF"，Pix:像素，如:1
-- HorAlign：	水平对齐方式，左边("AT_LEFT")，水平居中("AT_CENTER_HOR")，右边("AT_RIGHT")
-- VerAlign：	垂直对齐方式，顶部("AT_TOP")，垂直居中("AT_CENTER_VER")，底部("AT_BOTTOM")
-- Scrollbar：	垂直滚动条，显示(true)，隐藏(false)

--【注】
-- 空值填nil（["Pare"]可填空值，["Side"]的子项可填空值，表示没有描边效果）如：["Side"] = {["Cols"] = nil, ["Pix"] = nil}

--【例子】
-- MainWindow/window1
-- g_mlbConfig["MainWindow/window1"] = 
-- {
	-- ["Pare"] = "MainWindow",	-- 可填空值:nil
	-- ["Xpos"] = 50,
	-- ["Ypos"] = 50,
	-- ["Width"] = 315,
	-- ["Height"] = 15,
	-- ["Font"] = "simsun-10",
	-- ["Cols"] = "#000000",
	-- ["Side"] = {["Cols"] = "#FFFFFF", ["Pix"] = 1},	-- 可填空值:nil
	-- ["HorAlign"] = "AT_LEFT",
	-- ["VerAlign"] = "AT_CENTER_VER",
	-- ["Scrollbar"] = false
-- }

--【配置函数】
-- mlbox:图文混排控件
-- cfg:配置表，如:g_mlbConfig["MainWindow/window1"]
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
	if nil ~= pare then	--	在配置的时候赋予父窗口
		if nil~= g_WinMgr:getWindow(pare) then
			g_WinMgr:getWindow(pare):addChildWindow(mlbox)
		end
	end
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setPosition(xPos, yPos)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setSize(width, height)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setFont(textFont)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setCols(textCols)
	if nil~=sideCols or nil~=sidePix then	--	有描边效果
		EQGlobaFunc.toMultiLineBox(mlbox:getName()):setSide(sideCols, sidePix)
	end
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setAlign(horAlign)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setAlign(verAlign)
	EQGlobaFunc.toMultiLineBox(mlbox:getName()):setScrollbarVisible("SP_OLD", scrBarVisi)
	-- g_Loger:logMessage("---end config mlbox---")
end

--【开始配置】




