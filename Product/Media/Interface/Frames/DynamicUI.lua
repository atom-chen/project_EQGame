-----------------------------------------
-- 文件说明	：动态UI创建
-- 作者		：lf
-- 修改		：
-- 日期		：10/07/08
-----------------------------------------
local counter = 0

local numberpaopao = 
{
	--绿色
	[100] = "Person_data32", --0
	[101] = "Person_data31", --1
	[102] = "Person_data30", --2
	[103] = "Person_data29", --3
	[104] = "Person_data28", --4
	[105] = "Person_data27", --5
	[106] = "Person_data26", --6
	[107] = "Person_data25", --7
	[108] = "Person_data24", --8
	[109] = "Person_data23", --9
	[110] = "Person_data15", -- +
	[111] = "farm_sub01", -- -
	[112] = "Person_data11", --人物清洁度
	[113] = "Person_data12", --健康
	[114] = "Person_data14", --魅力
	[115] = "Person_data9", --活跃
	[116] = "Person_data13", --房屋清洁度
	[117] = "Person_data10", --金币
	[118] = "farm_exp02", --经验
	
	--红色
	[200] = "Person_data42",
	[201] = "Person_data41",
	[202] = "Person_data40",
	[203] = "Person_data39",
	[204] = "Person_data38",
	[205] = "Person_data37",
	[206] = "Person_data36",
	[207] = "Person_data35",
	[208] = "Person_data34",
	[209] = "Person_data33",
	[210] = "farm_add01",
	[211] = "Person_data22",
	[212] = "Person_data18",
	[213] = "Person_data19",
	[214] = "Person_data21",
	[215] = "Person_data16",
	[216] = "Person_data20",
	[217] = "Person_data17",
	[218] = "farm_exp01",
}

--聊天泡泡
function DynamicUI_CreateChatPaoPao(winname, playername, text)
		local name_win,text_win
		local name_winname = winname.."_Name"
		local text_winname = winname.."_Text"

		local lwidth = 133	-- 10个汉字宽度
		local lheight = 19
		
		local win = g_WinMgr:getWindow(winname)
		if win == nil then
				winname = "DynamicUI_Chat_"..counter
				counter = counter + 1
				
				--聊天框
				win = g_WinMgr:createWindow("EQHomeLook/ChatPaoPao", winname)
				g_MainUIWindows:addChildWindow(win)
				
				name_winname = winname.."_Name"
				name_win = g_WinMgr:createWindow("EQHomeLook/StaticText4", name_winname)
				name_win:setProperty("TextColours", "FF00FFFC")
				name_win:setSize(CEGUI.UVector2(CEGUI.UDim(0, lwidth+13), CEGUI.UDim(0, lheight)))
				win:addChildWindow(name_win)
			
				--文本框
				text_winname = winname.."_Text"
				text_win = CEGUI.toMultiLineEditbox(g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", text_winname))
				win:addChildWindow(text_win)
				EQGlobaFunc.toMultiLineBox(text_win:getName()):setAlign("AT_LEFT")
				EQGlobaFunc.toMultiLineBox(text_win:getName()):setAlign("AT_CENTER_VER")		
				EQGlobaFunc.toMultiLineBox(text_win:getName()):setWidth(lwidth)
				EQGlobaFunc.toMultiLineBox(text_win:getName()):setFont("simsun-10")
				EQGlobaFunc.toMultiLineBox(text_win:getName()):setCols("#EBEBEB")
				text_win:subscribeEvent("HyperLink_MouseEnter", "ChatPaoPao_OpenHyperLink")
				text_win:subscribeEvent("HyperLink_MouseLeave", "ChatPaoPao_CloseHyperLink")
		else
				g_MainUIWindows:addChildWindow(win)
				name_win = g_WinMgr:getWindow(name_winname)
				text_win = g_WinMgr:getWindow(text_winname)
				win:setAlpha(1)
		end
		
		win:setAlwaysOnTop(true)
		EQGlobaFunc.SetText(name_winname, playername)
		EQGlobaFunc.toMultiLineBox(text_win:getName()):clearUp()
		parseHyperLink(text_win, text)
		
		--强制重新排版
		name_win:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 1.5), CEGUI.UDim(0, 2.5)))
		text_win:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 8), CEGUI.UDim(0, lheight+5)))
		local height = EQGlobaFunc.toMultiLineBox(text_win:getName()):getFormatHeight()
		EQGlobaFunc.toMultiLineBox(text_win:getName()):setHeight(height)
		
		win:setSize(CEGUI.UVector2(CEGUI.UDim(0, lwidth+15), CEGUI.UDim(0, lheight+height+23)))
		
		return winname
end

function DynamicUI_SetChatPaoPaoPos(winname, x, y)
		local win = g_WinMgr:getWindow(winname)
		local dx = CEGUI.UDim(0,x-70)
		local dy = CEGUI.UDim(0,y-5) - win:getHeight()
		win:setPosition(CEGUI.UVector2(dx, dy))
end

function ChatPaoPao_OpenHyperLink(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local id = tonumber(EQGlobaFunc.toMultiLineBox(win:getName()):getHyperLinkId())
	local rectStr = EQGlobaFunc.toMultiLineBox(win:getName()):getTriggerRect()
	local parentPos = {x, y}
	parentPos = win:getParent():getPosition()
	parentXPos = parentPos.x:asAbsolute(g_GuiSystem:getRenderer():getRect():getWidth())
	parentYPos = parentPos.y:asAbsolute(g_GuiSystem:getRenderer():getRect():getHeight())
	local rect = StringToRect(rectStr)
	rect.left = rect.left + parentXPos
	rect.top = rect.top + parentYPos
	rect.right = rect.right + parentXPos
	rect.bottom = rect.bottom + parentYPos
	--时效相关
	local effectTime = ""
	local itemrow = g_itemtableMgr:getItemRow(id)
	if 1 == itemrow.effectTimeID then
		effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
	elseif 2 == itemrow.effectTimeID then
		effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
	end
	createItemhint_shop(id, rect, g_MainUIWindows, effectTime, 0, nil)
end

function ChatPaoPao_CloseHyperLink(args)
	destroyItemhint(g_MainUIWindows)
end

--数字泡泡
function DynamicUI_CreateNumberPaoPao(type, num)
		local scale = 0.8

		--num = math.random(-10000,10000)
		--type = math.random(11,16)
		local imageset = "common2"

		local winname = "DynamicUI_Number_"..counter
		counter = counter + 1
	
		local win = g_WinMgr:createWindow("EQHomeLook/StaticImage", winname)
		g_MainUIWindows:addChildWindow(win)
		
		local offset
		if num >= 0 then
				offset = 100 --绿色
		else
				offset = 200 --红色
				num = -num
		end
		
		local width
		local height = g_ImagesetMgr:getImageset(imageset):getImageHeight(numberpaopao[100])*scale
		local pos = 0
		
		--标题
		local tmpwin = g_WinMgr:createWindow("EQHomeLook/StaticImage")
		win:addChildWindow(tmpwin)
		
		index = offset+type
		tmpwin:setProperty("Image", "set:" ..imageset .." image:" ..numberpaopao[index])
		tmpwin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,pos), CEGUI.UDim(0,0)))
		width = g_ImagesetMgr:getImageset(imageset):getImageWidth(numberpaopao[index])*scale
		tmpwin:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
		pos = pos + width
		
		--正负号
		tmpwin = g_WinMgr:createWindow("EQHomeLook/StaticImage")
		win:addChildWindow(tmpwin)
		
		if offset == 100 then
				index = 110 --绿+
		else
				index = 211 --红-
		end
		tmpwin:setProperty("Image", "set:" ..imageset .." image:" ..numberpaopao[index])
		tmpwin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,pos), CEGUI.UDim(0,0)))
		width = g_ImagesetMgr:getImageset(imageset):getImageWidth(numberpaopao[index])*scale
		tmpwin:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
		pos = pos + width
		
		--数字
		local snum = tostring(num)
		local i,inum
		for i=1,string.len(snum) do
				tmpwin = g_WinMgr:createWindow("EQHomeLook/StaticImage")
				win:addChildWindow(tmpwin)
				
				inum = tonumber(string.sub(snum, i, i))
				index = offset+inum
				tmpwin:setProperty("Image", "set:" ..imageset .." image:" ..numberpaopao[index])
				tmpwin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,pos), CEGUI.UDim(0,0)))
				width = g_ImagesetMgr:getImageset(imageset):getImageWidth(numberpaopao[index])*scale
				tmpwin:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
				pos = pos + width
				--g_Loger:logDebug("DynamicUI_CreateNumberPaoPao "..inum.." "..numberpaopao[inum])
		end
		
		win:setSize(CEGUI.UVector2(CEGUI.UDim(0, pos), CEGUI.UDim(0, height)))
		
		return winname
end

function DynamicUI_SetNumberPaoPaoPos(winname, x, y)
		local win = g_WinMgr:getWindow(winname)
		local screenX = g_GuiSystem:getRenderer():getRect():getWidth()
		local dx = CEGUI.UDim(0, x - win:getWidth():asAbsolute(screenX)/2)
		local dy = CEGUI.UDim(0,y) - win:getHeight()
		win:setPosition(CEGUI.UVector2(dx, dy))
end

--头顶信息
function DynamicUI_CreateHeadup(playername, flag)
		local winname = "DynamicUI_Headup_"..counter
		counter = counter + 1
		
		local win = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", winname)
		EQGlobaFunc.toMultiLineBox(win:getName()):setFont("simsun-10")
		g_MainUIWindows:addChildWindow(win)
		--win:setZOrderingEnabled(true)
		win:moveToBack()
		--win:disable()
		
		win:setProperty("MousePassThroughEnabled", "true")
		local width = EQGlobaFunc.toMultiLineBox(win:getName()):getFitWidth("simsun-10", playername)
		
		local screenX = g_GuiSystem:getRenderer():getRect():getWidth()
		if width > screenX then
			width = screenX
		end
		win:setWidth(CEGUI.UDim(0, width))
		
		EQGlobaFunc.toMultiLineBox(win:getName()):setAlign("AT_CENTER_VER")
		EQGlobaFunc.toMultiLineBox(win:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(win:getName()):setSpecSide("#000000", 1)
		if 0 == flag then	--	玩家
			EQGlobaFunc.toMultiLineBox(win:getName()):addElementText(playername, "#FFFFFF", false)
		elseif 1 == flag then	-- NPC
			EQGlobaFunc.toMultiLineBox(win:getName()):addElementText(playername, "#00CC00", false)
		end
		local height = EQGlobaFunc.toMultiLineBox(win:getName()):getFormatHeight()
		EQGlobaFunc.toMultiLineBox(win:getName()):setHeight(height)
		
		return winname
end

function DynamicUI_SetHeadupPos(winname, x, y)
		local win = g_WinMgr:getWindow(winname)
		local screenX = g_GuiSystem:getRenderer():getRect():getWidth()
		local dx = CEGUI.UDim(0, x - win:getWidth():asAbsolute(screenX)/2)
		local dy = CEGUI.UDim(0,y) - win:getHeight() - CEGUI.UDim(0, 10)
		win:setPosition(CEGUI.UVector2(dx, dy))
end



--农场泡泡
local farm_win = nil
function DynamicUI_CreateFarmPaoPao(index,set, image)
	local winname = "DynamicUI_PestPaoPao_"..index
	g_WinMgr:destroyWindow(winname)
	farm_win = g_WinMgr:createWindow("EQHomeLook/StaticImage", winname)
	g_FarmUI:addChildWindow(farm_win)

	farm_win:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 30)))
	farm_win:setProperty("Image", "set:" ..set .." image:" ..image)
	
	return farm_win:getName()
end

function DynamicUI_SetFarmPaoPaoPos(winname, x, y)
		local win = g_WinMgr:getWindow(winname)
		local dx = CEGUI.UDim(0,x - win:getWidth():asAbsolute(640)/2)
		local dy = CEGUI.UDim(0,y) - win:getHeight()
		win:setPosition(CEGUI.UVector2(dx, dy))
end

--农场收获泡泡
function DynamicUI_CreateFarmHarvest(index,type, num, set, image)
		local scale = 0.8

		--num = math.random(-10000,10000)
		--type = math.random(11,16)
		local imageset = "common2"

		local winname
		if type == 17 then
			winname = "DynamicUI_FarmGold_"..index--counter
		elseif type == 18 then
			winname = "DynamicUI_FarmDexp_"..counter
		elseif type == 99 then
			winname = "DynamicUI_FarmHarvest_"..index--counter
		end
		counter = counter + 1
		g_WinMgr:destroyWindow(winname)
		local win = g_WinMgr:createWindow("EQHomeLook/StaticImage", winname)
		g_FarmUI:addChildWindow(win)
		win:setZOrderingEnabled(true)
		win:moveToBack()
		
		local offset
		if type == 18 then
				offset = 200 --经验用红色
		else
				if num < 0 then
					offset = 200 --红色
					num = -num
				elseif num == 0 then 
					offset = 200 --红色
				else
					offset = 100 --绿色
				end
		end
		
		local width
		local height = g_ImagesetMgr:getImageset(imageset):getImageHeight(numberpaopao[100])*scale
		local pos = 0
		
		--标题
		local tmpwin = g_WinMgr:createWindow("EQHomeLook/StaticImage")
		win:addChildWindow(tmpwin)
		
		if type == 99 then
			tmpwin:setProperty("Image", "set:" ..set .." image:" ..image)
			width = g_ImagesetMgr:getImageset(set):getImageWidth(image)
			local h = g_ImagesetMgr:getImageset(set):getImageHeight(image)
			tmpwin:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, h)))
		else
			index = offset+type
			tmpwin:setProperty("Image", "set:" ..imageset .." image:" ..numberpaopao[index])
			width = g_ImagesetMgr:getImageset(imageset):getImageWidth(numberpaopao[index])*scale
			tmpwin:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
		end
		tmpwin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,pos), CEGUI.UDim(0,0)))
		pos = pos + width
		
		--正负号
		height = g_ImagesetMgr:getImageset(imageset):getImageHeight(numberpaopao[100])*scale
		
		tmpwin = g_WinMgr:createWindow("EQHomeLook/StaticImage")
		win:addChildWindow(tmpwin)
		
		if type == 18 then
				index = 210
		else
			if offset == 100 then
					index = 110
			else
					index = 211
			end
		end
		
		--数字
		--if num >0 then
			tmpwin:setProperty("Image", "set:" ..imageset .." image:" ..numberpaopao[index])
			tmpwin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,pos), CEGUI.UDim(0,0)))
			width = g_ImagesetMgr:getImageset(imageset):getImageWidth(numberpaopao[index])*scale
			tmpwin:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
			pos = pos + width

			local snum = tostring(num)
			local i,inum
			for i=1,string.len(snum) do
					tmpwin = g_WinMgr:createWindow("EQHomeLook/StaticImage")
					win:addChildWindow(tmpwin)
					
					inum = tonumber(string.sub(snum, i, i))
					index = offset+inum
					tmpwin:setProperty("Image", "set:" ..imageset .." image:" ..numberpaopao[index])
					tmpwin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,pos), CEGUI.UDim(0,0)))
					width = g_ImagesetMgr:getImageset(imageset):getImageWidth(numberpaopao[index])*scale
					tmpwin:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
					pos = pos + width
					--g_Loger:logDebug("DynamicUI_CreateNumberPaoPao "..inum.." "..numberpaopao[inum])
			end
		--end
		
		if type == 99 then 
				height = g_ImagesetMgr:getImageset(set):getImageHeight(image)
		end
		win:setSize(CEGUI.UVector2(CEGUI.UDim(0, pos), CEGUI.UDim(0, height)))
		
		return winname
end

function DynamicUI_SetFarmHarvestPos(winname, x, y)
		local win = g_WinMgr:getWindow(winname)
		local dx = CEGUI.UDim(0,x - win:getWidth():asAbsolute(640)/2)
		local dy = CEGUI.UDim(0,y) - win:getHeight()
		win:setPosition(CEGUI.UVector2(dx, dy))
end

--农场头顶信息
function DynamicUI_CreateFarmHeadup(index,type, set1, image1, set2, image2, set3, image3)
		local winname
		if type == 1 then
			winname = "DynamicUI_PestHeadup_"..index--counter
		end
		if type == 2 then
			winname = "DynamicUI_PickHeadup_"..index--counter
		end
		
		--counter = counter + 1
		g_WinMgr:destroyWindow(winname)
		local win = g_WinMgr:createWindow("EQHomeLook/StaticImage", winname)
		g_FarmUI:addChildWindow(win)
		win:setZOrderingEnabled(true)
		win:moveToBack()

		
		local img1=nil
		if set1 ~= nil then
				img1 = g_WinMgr:createWindow("EQHomeLook/StaticImage")
				img1:setProperty("Image", "set:" ..set1 .." image:" ..image1)
				win:addChildWindow(img1)
		end
		
		local img2=nil
		if set2 ~= nil then
				img2 = g_WinMgr:createWindow("EQHomeLook/StaticImage")
				img2:setProperty("Image", "set:" ..set2 .." image:" ..image2)
				win:addChildWindow(img2)
		end
		
		local img3=nil
		if set3 ~= nil then
				img3 = g_WinMgr:createWindow("EQHomeLook/StaticImage")
				img3:setProperty("Image", "set:" ..set3 .." image:" ..image3)
				win:addChildWindow(img3)
		end
		
		if type == 1 then
				if image3 ~= nil then
						img1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						img2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						img3:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						
						img1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 5), CEGUI.UDim(0, 5)))
						img2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 5)))
						img3:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 55), CEGUI.UDim(0, 5)))
				elseif image2 ~= nil then
						img1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						img2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						
						img1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 15), CEGUI.UDim(0, 5)))
						img2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 45), CEGUI.UDim(0, 5)))
				else
						img1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						
						img1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 5)))
				end
				
				win:setSize(CEGUI.UVector2(CEGUI.UDim(0, 80), CEGUI.UDim(0, 30)))
		elseif type == 2 then
				if image3 ~= nil then
						img1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 40)))
						img2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						img3:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						
						img1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 5), CEGUI.UDim(0, 5)))
						img2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 15)))
						img3:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 75), CEGUI.UDim(0, 15)))
				elseif image2 ~= nil then
						img1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 40)))
						img2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 20)))
						
						img1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 15), CEGUI.UDim(0, 5)))
						img2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 75), CEGUI.UDim(0, 15)))
				else
						img1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 40)))
						
						img1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 5)))
				end
				
				win:setSize(CEGUI.UVector2(CEGUI.UDim(0, 100), CEGUI.UDim(0, 50)))
		end
		
		return winname
end

function DynamicUI_SetFarmHeadupPos(winname, x, y)
		local win = g_WinMgr:getWindow(winname)
		local dx = CEGUI.UDim(0,x - win:getWidth():asAbsolute(640)/2)
		local dy = CEGUI.UDim(0,y) - win:getHeight()
		win:setPosition(CEGUI.UVector2(dx, dy))
end

function Mosaic_CreateWin(winname)
		local win = g_WinMgr:createWindow("EQHomeLook/StaticImage", winname)
		win:setProperty("Image", "set:" .."masaike" .." image:" .."masaike")
		g_MainUIWindows:addChildWindow(win)
end

function Mosaic_SetPos(winname, left, top, right, bottom)
		local win = g_WinMgr:getWindow(winname)
		win:setArea( CEGUI.URect( CEGUI.UDim(0,left), CEGUI.UDim(0,top), CEGUI.UDim(0,right), CEGUI.UDim(0,bottom) ) )
end

function Mosaic_DestroyWin(winname)
		g_WinMgr:destroyWindow(winname)
end