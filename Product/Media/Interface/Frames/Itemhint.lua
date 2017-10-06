-----------------------------------------
-- 文件说明	：创建ItemHint及内容排版
-- 作者		：何展然
-- 修改		：
-- 日期		：10/9/9
-----------------------------------------

local itemhint = nil	-- 动态控件面板容器的父窗口
local nameCtrl = nil	-- 物品名称
local typeCtrl = nil	-- 物品类型
local iconCtrl = nil	-- 物品icon
local describeCtrl = nil	-- 物品说明
local overlapCtrl = nil	-- 物品上限
local stateCtrl = nil	-- 物品状态
local effTimeCtrl = nil	-- 物品有效使用时间
local priceCtrl = nil	-- 物品价格
local clothTypeCtrl = nil	-- 服饰类型
local clothPosCtrl = nil	-- 服饰部位
local gainCtrl = nil	-- 累计收获
local pickCtrl = nil	-- 	累计摘取
local stateFlag = false	-- 是否显示物品归属状态
local overlapFlag = false	-- 是否显示堆叠数
local clothPosFlag = false	-- 是否显示物品服装表里的如：“上衣”，“男装”
local effectTimeFlag = false	-- 是否显示时效
local priceFlag = false	-- 是否显示价格
local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
-----------------------------------------
-- 创建控件
local function _createItemhintCtrl(ctrltype, ctrlname)
	local ctrl = g_WinMgr:createWindow(ctrltype, ctrlname)
	ctrl:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))	-- 设置控件初始大小
	if "EQHomeLook/MultiLineBox2" == ctrltype then
		EQGlobaFunc.toMultiLineBox(ctrl:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(ctrl:getName()):setCols("#FFFFFF")
	end
	return ctrl
end
-----------------------------------------
-- 初始化itemhint各控件
local function _initItemhintCtrl()
	itemhint = g_WinMgr:createWindow("EQHomeLook/ItemhintFrame", "itemhint")
	itemhint:setProperty("BackgroundEnabled", "false")
	itemhint:setAlwaysOnTop(true)
	itemhint:disable()
	g_itemhintVec:initialise(itemhint:getName())
	nameCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/nameCtrl")
	typeCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/typeCtrl")
	iconCtrl = _createItemhintCtrl("EQHomeLook/StaticImage6", "itemhint/iconCtrl")
	describeCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/describeCtrl")
	overlapCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/overlapCtrl")
	stateCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/stateCtrl")
	effTimeCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/effTimeCtrl")
	priceCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/priceCtrl")
	clothTypeCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/clothTypeCtrl")
	clothPosCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/clothPosCtrl")
	gainCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/gainCtrl")
	pickCtrl = _createItemhintCtrl("EQHomeLook/MultiLineBox2", "itemhint/pickCtrl")
end
-- 该调用放在函数_createItemhintCtrl()和_initItemhintCtrl()定义后面
_initItemhintCtrl()
-----------------------------------------
-- 创建商城Itemhint
-- hintType: 0.该物品属于已归属类型，1.该物品未归属(如在商店中)
function createItemhint_shop(itemid, rect, wnd, effTime, hintType, shopcenterRow)
	-- g_Loger:logMessage("--- itemid: "..itemid.." effTime: "..effTime.." ---")
	destroyItemhint(wnd)
	wnd:addChildWindow(itemhint)
	itemhint:setWidth(CEGUI.UDim(0, 250))	-- 控件面板18个汉字宽度
	-- 获取商城物品信息
	local itemrow = g_itemtableMgr:getItemRow(itemid)

	if itemrow == nil then
		return
	end
	-- 物品名称窗体
	EQGlobaFunc.toMultiLineBox(nameCtrl:getName()):setAlign("AT_CENTER_HOR")
	addItemhintText(nameCtrl, itemrow.name, "#FF9C00", "#000000")
	-- 物品类型窗体
	local typeName = checkItemType(itemrow.type)
	if 1 == itemrow.type then
		local itemEquitRow = g_itemtableMgr:getItemEquipRow(itemrow.subID)	-- 获取物品服装表
		addItemhintText(clothTypeCtrl, checkItemhintClothesSex(itemEquitRow.sex), "#FFFFFF", "#045098")
		addItemhintText(clothPosCtrl, checkItemhintClothesPos(itemEquitRow.equip_placeholder), "#FFFFFF", "#045098")
		clothPosFlag = true
	end
	addItemhintText(typeCtrl, typeName, "#FFFFFF", "#045098")
	-- 物品icon窗体
	iconCtrl:setProperty("Image", "set:" ..itemrow.imageset .." image:" ..itemrow.icon)
	-- 物品说明窗体
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setCols("#FFFFFF")
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setWidth(240)	-- 18个汉字宽度
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):clearUp()
	local row = g_logictableMgr:getGoodsDescribeRow(itemrow.tooltip)
	if nil ~= row then
		g_mlbParse:getData(row.value, describeCtrl:getName())
	end
	setItemhintCtrlHeight(describeCtrl)
	-- 物品堆叠数窗体
	if itemrow.overlap > 1 then
		local overlap = "每组上限："..itemrow.overlap
		addItemhintText(overlapCtrl, overlap, "#FFFFFF", "#045098")
		overlapFlag = true
	end
	-- 物品有效时间窗体	
	local remainDays = checkEffectTime(itemrow.effectTimeID, effTime, effTimeCtrl)
	-- 物品归属状态窗体
	addItemhintText(stateCtrl, checkItemhintState(itemrow.bind, remainDays, hintType), "#FFFFFF", "#045098")
	--物品价格
	if shopcenterRow ~= nil then
		EQGlobaFunc.toMultiLineBox(priceCtrl:getName()):setWidth(240)	-- 18个汉字宽度
		EQGlobaFunc.toMultiLineBox(priceCtrl:getName()):clearUp()
		local pricePre = "<clr v=\"#05E5FF\"><sid v=\"#043644\" g=\"1.0\">售价：</sid></clr>"
		local price = ""
		if shopcenterRow.money_type == 1 then
			pricePre = pricePre.."<clr v=\"#FFF600\"><sid v=\"#11110D\" g=\"1.0\">"
			price = pricePre..tostring(shopcenterRow.price).." 金币</sid></clr>"
		else
			pricePre = pricePre.."<clr v=\"#FF9C00\"><sid v=\"#000000\" g=\"1.0\">"
			price = pricePre..tostring(shopcenterRow.price).." EQ币</sid></clr>"
		end
		g_mlbParse:getData(price, priceCtrl:getName())
		setItemhintCtrlHeight(priceCtrl)
		priceFlag = true
	end
	
	-- itemhint排版
	controlLayout_shop()
	g_itemhintVec:controlTypeset()
	
	-- 设置itemhint位置
	local xPos, yPos = checkItemhintPos(rect, wnd, itemhint)
	itemhint:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))
end
-----------------------------------------
-- 创建农场Itemhint
-- type：0表示蔬果类型
function createItemhint_farm(itemid, rect, wnd, type, effTime)
	destroyItemhint(wnd)
	wnd:addChildWindow(itemhint)
	-- 获取农场商店物品信息
	local itemrow = g_itemtableMgr:getItemRow(itemid)
	-- 物品名称窗体
	addItemhintText(nameCtrl, itemrow.name, "#FF9C00", "#000000")
	-- 物品说明窗体
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setCols("#FFFFFF")	
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setWidth(240)
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):clearUp()
	local row = g_logictableMgr:getGoodsDescribeRow(itemrow.tooltip)
	if nil ~= row then
		g_mlbParse:getData(row.value, describeCtrl:getName())
	end
	local length = EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):getLength()
	if 240 > length then
		local width = EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):getFitWidth(length)
		EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setWidth(width)
	end
	setItemhintCtrlHeight(describeCtrl)
	-- 物品有效时间窗体	
	checkEffectTime(itemrow.effectTimeID, effTime, effTimeCtrl)
	-- 动态设置面板宽度
	local nameWidth = nameCtrl:getWidth():asAbsolute(screenW)
	local describeWidth = describeCtrl:getWidth():asAbsolute(screenW)
	if nameWidth > describeWidth then
		itemhint:setWidth(CEGUI.UDim(0, nameWidth+13))
	else
		itemhint:setWidth(CEGUI.UDim(0, describeWidth+13))
	end
	if 0 == type then	-- 蔬果
		itemhint:setWidth(CEGUI.UDim(0, nameWidth+13))
	end
	-- itemhint排版
	controlLayout_farm(type)
	g_itemhintVec:controlTypeset()
	-- 设置itemhint位置
	local xPos, yPos = checkItemhintPos(rect, wnd, itemhint)
	itemhint:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))	
end
-----------------------------------------
-- 创建农场成果Itemhint
function createItemhint_farmgain(name, gain, pick, rect, wnd)
	destroyItemhint(wnd)
	wnd:addChildWindow(itemhint)
	-- 物品名称窗体
	addItemhintText(nameCtrl, name, "#FF9C00", "#000000")
	-- 累计收获窗体
	addItemhintText(gainCtrl, "累计收获："..gain, "#05E5FF", "#043644")
	-- 累计摘取窗体
	addItemhintText(pickCtrl, "累计摘取："..pick, "#05E5FF", "#043644")
	-- 动态设置面板宽度
	local gainWidth = gainCtrl:getWidth():asAbsolute(screenW)
	local pickWidth = pickCtrl:getWidth():asAbsolute(screenW)
	if gainWidth > pickWidth then
		itemhint:setWidth(CEGUI.UDim(0, gainWidth+13))
	else
		itemhint:setWidth(CEGUI.UDim(0, pickWidth+13))
	end
	-- itemhint排版
	controlLayout_farmgain()
	g_itemhintVec:controlTypeset()
	-- 设置itemhint位置
	local xPos, yPos = checkItemhintPos(rect, wnd, itemhint)
	itemhint:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))	
end
-----------------------------------------
-- 创建礼物礼盒/礼卡Itemhint
-- flag: 0.礼盒，1.礼卡
function createItemhint_gift(itemid, rect, wnd, flag)
	destroyItemhint(wnd)
	wnd:addChildWindow(itemhint)
	itemhint:setWidth(CEGUI.UDim(0, 250))	-- 控件面板18个汉字宽度
	-- 获取物品信息
	local itemrow
	if 0 == flag then	-- 礼盒
		itemrow = g_logictableMgr:getGiftBoxRow(itemid)	
	elseif 1 == flag then	-- 礼卡
		itemrow = g_logictableMgr:getGiftCardRow(itemid)	
	end
	-- 物品名称窗体
	addItemhintText(nameCtrl, itemrow.name, "#FF9C00", "#000000")
	-- 物品icon窗体
	iconCtrl:setProperty("Image", "set:" ..itemrow.imageset .." image:" ..itemrow.image)
	-- 物品说明窗体
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setWidth(200)
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):setSpecSide("#043644", 1)
	EQGlobaFunc.toMultiLineBox(describeCtrl:getName()):addElementText(itemrow.desc, "#05E5FF", false)
	setItemhintCtrlHeight(describeCtrl)
	-- 使用期限
	if 1 ~= itemrow.type then	-- 收费
		local effTime = "最多可使用5次"
		addItemhintText(effTimeCtrl, effTime, "#FFFFFF", "#045098")
	end
	-- itemhint排版
	controlLayout_gift(itemrow.type)
	g_itemhintVec:controlTypeset()
	-- 设置itemhint位置
	local xPos, yPos = checkItemhintPos(rect, wnd, itemhint)
	itemhint:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))	
end
-----------------------------------------
-- 创销毁Itemhint
function destroyItemhint(wnd)
	if nil ~= g_WinMgr:getWindow("itemhint") then
		wnd:removeChildWindow("itemhint")
		if nil ~= g_itemhintVec then
			g_itemhintVec:removeAllControl()
		end
	end
end
-----------------------------------------
-- 添加图文混排控件内容
function addItemhintText(ctrl, text, cols, side)
	if nil == ctrl then
		--g_Loger:logError("--- Itemhint.lua -> addItemhintText() -> ctrl is nil ---")
		return
	end
	if nil == text then
		--g_Loger:logError("--- Itemhint.lua -> addItemhintText() -> text in "..ctrl:getName().." is nil ---")
		return
	end
	EQGlobaFunc.toMultiLineBox(ctrl:getName()):clearUp()
	setItemhintCtrlWidth(ctrl, text)
	EQGlobaFunc.toMultiLineBox(ctrl:getName()):setAlign("AT_CENTER_VER")
	EQGlobaFunc.toMultiLineBox(ctrl:getName()):setSpecSide(side, 1)
	EQGlobaFunc.toMultiLineBox(ctrl:getName()):addElementText(text, cols, false)
	setItemhintCtrlHeight(ctrl)	-- 设置控件高度
end
-----------------------------------------
-- 根据内容设置控件宽度
function setItemhintCtrlWidth(ctrl, text)
	local len = EQGlobaFunc.toMultiLineBox(ctrl:getName()):getFitWidth("simsun-10", text)
	if "itemhint/nameCtrl" == ctrl:getName() then	-- 名字长度不超过16个汉字
		if len > 210 then
			len = 210
		end
	else	-- 其他内容不超过18个汉字
		if len > 240 then
			len = 240
		end
	end
	ctrl:setWidth(CEGUI.UDim(0, len))
end
-----------------------------------------
-- 自动设置图文混排控件高度，可根据内容而改变
function setItemhintCtrlHeight(ctrl)
	local height = EQGlobaFunc.toMultiLineBox(ctrl:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(ctrl:getName()):setHeight(height)
end
-----------------------------------------
-- 检查itemhint的显示坐标
function checkItemhintPos(rect, rootWin, win)
	local xPos, yPos = nil, nil
	local rootWinWidth = rootWin:getWidth():asAbsolute(screenW)
	local rootWinHeight = rootWin:getHeight():asAbsolute(screenH)
	local width = win:getWidth():asAbsolute(screenW)
	local height = win:getHeight():asAbsolute(screenH)
	-- 限制x坐标
	if rect.right+width > rootWinWidth then
		xPos = rect.left - width
	else
		xPos = rect.right
	end
	-- 限制y坐标
	if rect.bottom+height > rootWinHeight then
		yPos = rect.top - height
	else
		yPos = rect.bottom
	end
	return xPos, yPos
end
---------------------------------------
-- 截取字符串的各个部分:str(字符串),sep(分隔符),num(分隔几部分)
function cutString(str, sep, num)
	local parts = {}	-- 存放分离的各个部分
	for i=1, num-1 do
		local pos = string.find(str, sep)
		if nil == pos then	-- 没有分隔符
			return
		end
		table.insert(parts, string.sub(str, 1, pos-1))
		str = string.sub(str, pos+1, -1)
	end
	table.insert(parts, str)
	return parts
end
-----------------------------------------
-- 判断时效
function checkEffectTime(showType, timeStr, etCtrl)
	-- g_Loger:logMessage("---showType: "..showType.." timeStr: "..timeStr.."---")
	local serverTime = EQGlobaClass.getTimekeeper():getServerTime()
	-- 截取年,月,日,时,分,秒
	local parts, y, m, d, h, n, s = cutString(timeStr, ",", 7)
	if nil ~= parts then
		y, m, d = tonumber(parts[1]), tonumber(parts[2]), tonumber(parts[3])
		h, n, s = tonumber(parts[4]), tonumber(parts[5]), tonumber(parts[6])	
	end
	local remainDays = -1
	-- 处理不同的时效显示类型
	if 1 == showType then	-- 显示剩余时间
		local remainTime = "剩余时间："
		if nil ~= parts then	-- 已归属状态
			if 0 == y then
				addItemhintText(etCtrl, "剩余时间：0天", "#FFFFFF", "#045098")
				return remainDays
			end
			-- (不知道为什么购买物品后,马上移到物品上面,计算出来的天数会多出一天,把serverTime加上十几秒就不会,非常囧)
			remainDays = EQGlobaFunc.TimeDiffDayInt(EQGlobaFunc.DateToSeconds(y, m, d, h, n, s), serverTime+20)
			if remainDays < 0 then
				remainDays = 0
			end
			remainTime = remainTime..remainDays.."天"
		else	-- 未归属状态(商店中)
			remainTime = remainTime..timeStr.."天"
		end
		addItemhintText(etCtrl, remainTime, "#FFFFFF", "#045098")
		effectTimeFlag = true
	elseif 2 == showType then	-- 显示到期时间
		if nil ~= parts then	-- 已归属状态
			if 0 == y then
				addItemhintText(etCtrl, "到期时间：今天", "#FFFFFF", "#045098")
				return remainDays
			end
			-- (不知道为什么购买物品后,马上移到物品上面,计算出来的天数会多出一天,把serverTime加上十几秒就不会,非常囧)
			remainDays = EQGlobaFunc.TimeDiffDayInt(EQGlobaFunc.DateToSeconds(y, m, d, h, n, s), serverTime+20)
		else	-- 未归属状态(商店中)
			y = tonumber(string.sub(tostring(timeStr), 1, 4))
			m = tonumber(string.sub(tostring(timeStr), 5, 6))
			d = tonumber(string.sub(tostring(timeStr), 7, 8))
		end
		local dueTime = "到期时间："..y.."年"..m.."月"..d.."天"
		addItemhintText(etCtrl, dueTime, "#FFFFFF", "#045098")
		effectTimeFlag = true
	else	-- 0.不显示时效
		etCtrl:setSize(CEGUI.UVector2(CEGUI.UDim(0, 0), CEGUI.UDim(0, 0)))
		effectTimeFlag = false
	end
	return remainDays
end
-----------------------------------------
-- 判断物品归属状态
function checkItemhintState(flag, remainDays, hintType)
	local state = nil
	if 1 == flag then
		state = "不可交易"
	elseif 2 == flag then
		state = "使用后不可交易"
	elseif 3 == flag then
		if 0 == hintType then	-- 物品未归属类型的itemhint
			state = ""
		elseif 1 == hintType then	-- 物品已归属类型的itemhint
			if remainDays <= 10 then
				if -1 == remainDays then
					state = ""
				else 
					state = "剩余时间不足，不可交易"
				end
			else
				state = ""
			end
		end
	elseif 4 == flag then
		state = ""
	elseif 5 == flag then
		state = "可交互，可交易"
	else
		state = ""
	end
	return state
end
-----------------------------------------
-- 判断物品类型
function checkItemType(flag)
	local typeName = nil
	if 1 == flag then
		typeName = "服饰"
	elseif 2 == flag then
		typeName = "家具"
	elseif 3 == flag then
		typeName = "可使用的道具"
	elseif 4 == flag then
		typeName = "农场种子"
	elseif 5 == flag then
		typeName = "农场果实"
	elseif 6 == flag then
		typeName = "农场道具"
	elseif 7 == flag then
		typeName = "农场装饰"
	else
		typeName = "其他"
	end
	return typeName
end
-----------------------------------------
-- 判断物品服装是否为男女装
function checkItemhintClothesSex(flag)
	local sex = nil
	if 1 == flag then
		sex = "男装"
	elseif 2 == flag then
		sex = "女装"
	else
		sex = "男女共用"
	end
	return sex
end
-----------------------------------------
-- 判断物品服装位置
function checkItemhintClothesPos(flag)
	local pos = EQGlobaFunc.UIntToStringBitsetScript(flag)
	local clothes = nil
	if "0" ==  pos then
		clothes = "帽子"
	elseif "1" == pos then
		clothes = "上衣"
	elseif "2" == pos then
		clothes = "眼镜"
	elseif "3" == pos then
		clothes = "下衣"
	elseif "4" == pos then
		clothes = "背部"
	elseif "5" == pos then
		clothes = "鞋子"
	elseif "6" == pos then
		clothes = "手持"
	else
		clothes = "套装"
	end	
	return clothes
end
---------------------------------------
-- 商城itemhint排版配置
function controlLayout_shop()
	g_itemhintVec:setAlign("AT_CENTER")
	g_itemhintVec:setVerInterval(10)
	g_itemhintVec:addControl(nameCtrl:getName())		-- 显示物品名称
	g_itemhintVec:setHorInterval(10)
	g_itemhintVec:addControl(iconCtrl:getName())		-- 显示物品icon
	g_itemhintVec:setAtRight()
	g_itemhintVec:addControl(typeCtrl:getName())		-- 显示物品类型
	g_itemhintVec:setHorInterval(48)
	g_itemhintVec:setVerInterval(-13)
	g_itemhintVec:addControl(stateCtrl:getName())		-- 显示物品归属状态
	if true == clothPosFlag then							-- 是否显示物品服装表的信息，如“上衣”和“男装”
		g_itemhintVec:addControl(clothPosCtrl:getName())
		g_itemhintVec:setAtRight()
		g_itemhintVec:setHorInterval(13)
		g_itemhintVec:addControl(clothTypeCtrl:getName())
		clothPosFlag = false
	end
	if true == overlapFlag then						
		g_itemhintVec:addControl(overlapCtrl:getName())	-- 显示物品堆叠数
		overlapFlag = false
	end
	g_itemhintVec:addControl(describeCtrl:getName())	-- 显示物品描述说明
	if true == effectTimeFlag then
		g_itemhintVec:addControl(effTimeCtrl:getName())	-- 显示物品到期时间
	end
	if true == priceFlag then						
		g_itemhintVec:addControl(priceCtrl:getName())	-- 显示物品价格
		priceFlag = false
	end
end
---------------------------------------
-- 农场itemhint排版配置
function controlLayout_farm(type)
	g_itemhintVec:addControl(nameCtrl:getName())		-- 显示物品名称
	if 0 ~= type then	-- 蔬果
		g_itemhintVec:addControl(describeCtrl:getName())	-- 显示物品描述说明
	end
	if true == effectTimeFlag then
		g_itemhintVec:addControl(effTimeCtrl:getName())	-- 显示物品到期时间
	end
end
---------------------------------------
-- 农场成果itemhint排版配置
function controlLayout_farmgain()
	g_itemhintVec:addControl(nameCtrl:getName())	-- 显示物品名称
	g_itemhintVec:addControl(gainCtrl:getName())	-- 显示累计收获
	g_itemhintVec:addControl(pickCtrl:getName())	-- 显示累计摘取
end
---------------------------------------
-- 礼盒/礼卡itemhint排版配置
function controlLayout_gift(type)
	g_itemhintVec:setAlign("AT_CENTER")
	g_itemhintVec:setVerInterval(10)
	g_itemhintVec:addControl(nameCtrl:getName())		-- 显示物品名称
	g_itemhintVec:setHorInterval(10)
	g_itemhintVec:addControl(iconCtrl:getName())		-- 显示物品icon
	g_itemhintVec:setAtRight()
	g_itemhintVec:addControl(describeCtrl:getName())	-- 显示物品描述说明
	if 1 ~= type then
		g_itemhintVec:addControl(effTimeCtrl:getName())	-- 显示物品到期时间
	end	
end
---------------------------------------
-- 初始化类型为1的itemhint,只有标题和说明
local m_hintWin1, m_headWin1, m_tipWin1 = nil, nil, nil
local m_hintVec1 = EQGlobaClass.createDynamicCtrlVector()
local function _itemhint_initType1()
	m_hintWin1 = g_WinMgr:getWindow("Itemhint/type1")
	if nil == m_hintWin1 then
		m_hintWin1 = g_WinMgr:createWindow("EQHomeLook/ItemhintFrame", "Itemhint/type1")
		m_hintWin1:setWidth(CEGUI.UDim(0, 250))	-- 控件18个汉字宽度
		m_hintVec1:initialise("Itemhint/type1")
	end
	m_headWin1 = g_WinMgr:getWindow("Itemhint/type1/head")
	if nil == m_headWin1 then
		m_headWin1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Itemhint/type1/head")
		EQGlobaFunc.toMultiLineBox(m_headWin1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(m_headWin1:getName()):setCols("#FFFFFF")
		EQGlobaFunc.toMultiLineBox(m_headWin1:getName()):setSide("#045098", 1)
		EQGlobaFunc.toMultiLineBox(m_headWin1:getName()):setWidth(240)	-- 18个汉字宽度
	end
	m_tipWin1 = g_WinMgr:getWindow("Itemhint/type1/tip")
	if nil == m_tipWin1 then
		m_tipWin1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Itemhint/type1/tip")
		EQGlobaFunc.toMultiLineBox(m_tipWin1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(m_tipWin1:getName()):setCols("#05E5FF")
		EQGlobaFunc.toMultiLineBox(m_tipWin1:getName()):setSide("#043644", 1)
		EQGlobaFunc.toMultiLineBox(m_tipWin1:getName()):setWidth(240)	-- 18个汉字宽度
	end	
end
_itemhint_initType1()
---------------------------------------
-- 显示类型为1的itemhint,只有标题和说明
function Itemhint_showType1(pareWin, title, content, x, y)
	-- 重置
	EQGlobaFunc.toMultiLineBox(m_headWin1:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(m_tipWin1:getName()):clearUp()
	Itemhint_hideType1()
	-- 设置标题
	g_mlbParse:getData(title, m_headWin1:getName())
	local height = EQGlobaFunc.toMultiLineBox(m_headWin1:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(m_headWin1:getName()):setHeight(height)
	-- 设置说明
	g_mlbParse:getData(content, m_tipWin1:getName())
	height = EQGlobaFunc.toMultiLineBox(m_tipWin1:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(m_tipWin1:getName()):setHeight(height)
	-- 排版
	m_hintVec1:addControl(m_headWin1:getName())
	m_hintVec1:addControl(m_tipWin1:getName())
	m_hintVec1:controlTypeset()
	local xPos, yPos = Itemhint_checkPos(x, y, pareWin, m_hintWin1)
	m_hintWin1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))
	if nil ~= pareWin then
		pareWin:addChildWindow(m_hintWin1)
	end
end
---------------------------------------
-- 隐藏类型为1的itemhint
function Itemhint_hideType1()
	local pareWin = g_WinMgr:getWindow("Itemhint/type1"):getParent()
	if nil ~= pareWin then
		pareWin:removeChildWindow ("Itemhint/type1")
	end
	if nil ~= m_hintVec1 then
		m_hintVec1:removeAllControl()
	end
end
---------------------------------------
-- 检查itemhint的显示坐标
function Itemhint_checkPos(x, y, rootWin, win)
	local rootWinWidth = rootWin:getWidth():asAbsolute(screenW)
	local rootWinHeight = rootWin:getHeight():asAbsolute(screenH)
	local width = win:getWidth():asAbsolute(screenW)
	local height = win:getHeight():asAbsolute(screenH)
	local gap = 32
	-- 限制x坐标
	if x <= 0 then
		x = gap
	elseif x >= rootWinWidth then
		x = rootWinWidth - width - gap
	elseif x+width > rootWinWidth then
		x = x - width - gap
	end
	-- 限制y坐标
	if y <= 0 then
		y = gap
	elseif y >= rootWinHeight then
		y = rootWinHeight - height - gap
	elseif y+height > rootWinHeight then
		y = y - height - gap
	end
	return x, y
end
---------------------------------------
