-----------------------------------------
-- 文件说明	：处理创建人物窗口相关的逻辑
-- 作者		：
-- 修改		：麦
-- 日期		：10/03/25
-----------------------------------------

-------------------2010-04--12huangdj添加----------------------
---
--服饰选择相关数据表
local fashionTable = nil	--服饰选择索引表
local itemTable = nil	--物品表
local subitemTable = nil 	--子表
--男上衣Icon表和男上衣图片集表
maleShangYiIcons 	= {}
maleshangYiImagesets = {}
--男下衣Icon表和男下衣图片集表
maleXiaYiIcons 		= {}
maleXiaYiImagesets 	= {}
--男套装Icon表和男套装图片集表
maleSuitIcons 		= {}
maleSuitImagesets 	= {}
--男正装Icon表和男正装图片集表
malezhengzhuangIcons	= {}
malezhengzhuangImagesets	= {}
--男运动装Icon表和男运动装图片及表
maleYundongIcons	= {}
maleYundongImagesets = {}
--男鞋Icon表和男鞋图片集表
maleShoesIcons		= {}
maleShoesImagesets	= {}
--女上衣Icon表和女上衣图片集表
femaleShangYiIcons 		= {}
femaleShangYiImagesets 	= {}
--女下衣Icon表和女下衣图片集表
femaleXiaYiIcons 		= {}
femaleXiaYiImagesets 	= {}
--女套装Icon表和女套装图片集表
femaleSuitIcons 	= {}
femaleSuitImagesets = {}
--女正装Icon表和女正装图片集表
femalezhengzhuangIcons	= {}
femalezhengzhuangImagesets	= {}
--女运动装Icon表和女运动装图片及表
femaleYundongIcons	= {}
femaleYundongImagesets = {}
--女鞋Icon表和女鞋图片集表
femaleShoesIcons		= {}
femaleShoesImagesets	= {}
--显示缓存
local showIconBuffer = {}
local showImagesetBuffer = {}

local showShangyiBuffer = {}
local showXiayiBuffer = {}
local showShoesBuffer = {}

local shangyiTotalPage = 1
local xiayiTotalPage = 1
local shoesTotalPage = 1

local shangyiCurPage = 1
local xiayiCurPage = 1
local shoesCurPage = 1

local MAXSIZE = 3
--当前选择的是便装还是正装状态
--	0——便装
--	1——正装
local curSeletSuitState = 0

local mSelected1 = ""
local mSelected2 = ""
local mSelected3 = ""
--
local mSelected4 = ""
local mSelected5 = ""

--icon和subID对应表
icon2subID = {}
--icon和物品表Id对应表
icon2dressId = {}

--当前装备状态变量(存放装备上的物品ID)
local curHatState = 0
local curClothState = 0
local curTrouState = 0
local curShoeState = 0
local curGlasseState = 0
local curBackState = 0
local curHandState = 0
local curTrinKetState = 0
--
--当前栏位是否装备标志
local hatFlag = nil
local clothesFlag = nil
local trousersFlag = nil
local shoesFlag = nil
local glassesFlag = nil
local backFlag = nil
local handFlag = nil
local trinketFlag = nil
--
local SkinColorFlag = 1 
-----------------------注册事件段----------------------------------------
--
local mFirstIn = false
--
--g_GameEventMgr:subscribeEvent("ROLE_LIST_RETURN_EMPTY","CreatePlayer_OnCreate")
g_GameEventMgr:subscribeEvent("REQUEST_SCENE_OK","CreatePlayer_OnLoadScene")
g_GameEventMgr:subscribeEvent("REQUEST_SCENE_FAILED","CreatePlayer_OnLoadSceneFail")

g_WinMgr:getWindow("CreateCharacter/Ok"):subscribeEvent("Clicked", 	"CreatePlayer_OnCreatePlayer")
g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):subscribeEvent("Clicked","CreatePlayer_OnSelectGender")
g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):subscribeEvent("Clicked","CreatePlayer_OnSelectGender")
g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("NormalImage", "set:Button6 image:NewImage108")

-----------------------------------------
-- 创建角色处理
-----------------------------------------

local reqcreaterole = EQGlobaClass.req_create_role()


EQGlobaFunc.EQStartRTT(0,50000,0)
local avatar 		= g_rtt:getAvatar()
--当需要取得调整者时，请直接使用  avatar:getAdjuster()
--把之前的adjuster都替换为avatar:getAdjuster()
-------------------------------------------------
--
function CreatePlayer_getFirstInflag()
	return mFirstIn
end
-------------------------------------------------
--
function CreatePlayer_setFirstInflag(flag)
	--if flag == 1 then --dn_day
		mFirstIn = flag
	--else  				--dn_night
	--	mFirstIn = false
	--end
end
--
--
local is_inCreatePlayer_flag = false
--
function CreatePlayer_setInCPFlag(flag)
	is_inCreatePlayer_flag = flag
end
--
function CreatePlayer_getInCPFlag()
	return is_inCreatePlayer_flag
end
--------------------------------------------------------------------------------------------------
function CreatePlayer_OnCreate(name)
	--mFirstIn = true
	CreatePlayer_setInCPFlag(true)
	g_GuiSystem:setGUISheet(g_CreatePlayer)            --创建人物窗口
	avatar:getAdjuster():setForCreateRole(true)
	--旋转处理
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteL"):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteL"):setAutoRepeatRate(0.1)
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteR"):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteR"):setAutoRepeatRate(0.1)
	
	--屏蔽双击按键
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteR"):setWantsMultiClickEvents(false) 
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteL"):setWantsMultiClickEvents(false)
	
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteR"):removeEvent("MouseButtonDown")
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteL"):removeEvent("MouseButtonDown")
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteR"):subscribeEvent("MouseButtonDown", "PlayerPropertyUI_TurnCounterclockwise")	
	g_WinMgr:getWindow("CreateCharacter/BackL/RoteL"):subscribeEvent("MouseButtonDown", "PlayerPropertyUI_TurnClockwise")	
	
	--初始化时默认是男性角色，用的是男性的缩放比例
	g_rtt:changeSex(1)
	curSeletSuitState = 0
	clearState()
	CreatePlayer_clearSelectSta()
	CreatePlayer_TabButtonSta()
	CreatePlayer_hideShoesSlots(0)
	CreatePlayer_setDefSkinBtnStat()
	avatar:getAdjuster():doChange(g_atSkin,1)
	avatar:getAdjuster():reset()
	avatar:takeOffAll()
	EQGlobaFunc.RTTReset()
	EQGlobaFunc.RTTScale(1.03,1.03,1.03)
	EQGlobaFunc.RTTRotate(3,3)
	g_mainplayer:setCeateEquip(0, 0, 0, 0, 0, 0, 0, 0)
	
	g_WinMgr:getWindow("Login/UserEdit"):setText("")
	local sX = g_WinMgr:getWindow("CreatePlayer/BackR/PreView"):getWidth():asAbsolute(640)
	local sY = g_WinMgr:getWindow("CreatePlayer/BackR/PreView"):getHeight():asAbsolute(480)
	EQGlobaFunc.RTTSetAspectRatio(sX,sY)--(138,375)
	EQGlobaFunc.RTTLookAt(0,50090,0)
	print("startRTT")
	local win = g_WinMgr:getWindow("RTTWindow_0")
	local window = g_WinMgr:getWindow("CreatePlayer/BackR/PreView")
	window:addChildWindow(win)
	win:setMousePassThroughEnabled(true)
	CreatePlayer_initialEditbox()
	g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("NormalImage", "set:Button6 image:NewImage107")
	g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("HoverImage", "set:Button6 image:NewImage109")
	g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("NormalImage", "set:Button6 image:NewImage108")
	g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("HoverImage", "set:Button6 image:NewImage108")
	CreatePlayer_itemSubscribeEvent()
	
	CreatePlayer_LoadIcons()
	--
	shangyiTotalPage = getTotalPage(maleShangYiIcons,MAXSIZE)
	CreatePlayer_updateData(1,maleShangYiIcons,maleshangYiImagesets)
	CreatePlayer_OnSelectBianzhuang()
	
	xiayiTotalPage = getTotalPage(maleXiaYiIcons,MAXSIZE)
	CreatePlayer_updateData(1,maleXiaYiIcons,maleXiaYiImagesets)
	CreatePlayer_OnSelectTrousers()
	
	shoesTotalPage = getTotalPage(maleShoesIcons,MAXSIZE)
	CreatePlayer_updateData(1,maleShoesIcons,maleShoesImagesets)
	CreatePlayer_OnSelectShoes()
	
	EQGlobaFunc.RTTSetActive(g_rttAvatarWinName, true)
	
	local namebox = CEGUI.toEditbox(g_WinMgr:getWindow("logoBg/Name/EditPlayerName"))
	if name == "" then
		namebox:setReadOnly(false)
		namebox:setMaxTextLength(8)
		namebox:setText("")
		namebox:activate()
	else
		namebox:setReadOnly(true)
		EQGlobaFunc.SetText(namebox:getName(), name)
		namebox:deactivate()
	end
end

---------------------------------------------------------------------------------------------------
function CreatePlayer_OnEnterEvent(args)    
	local keyvalue = CEGUI.toKeyEventArgs(args).scancode 
	if keyvalue == 28 or keyvalue == 156 then   -- 回车键
		CreatePlayer_OnCreatePlayer()
	end
end

-----------------------------------------------------------------------------------------------------
function CreatePlayer_OnCreatePlayer(args)
	local account	= g_WinMgr:getWindow("Login/UserEdit"):getText()
	local username	= g_WinMgr:getWindow("logoBg/Name/EditPlayerName"):getText()
	
	if username == "" then
		MsgBox_createMsgBox(g_CreatePlayer,0,"","角色名不能为空", "MsgBox_cancelFunc", "")
		return
	end
	
	local content = EQGlobaFunc.stringFilter(username,0)
	if nil ~= content.find(content, "*") then
		MsgBox_createMsgBox(g_CreatePlayer,0,"","名字里含有非法字符", "MsgBox_cancelFunc", "")
		return
	end
	
	--非法字符判断
	--local f = assert(io.open("../Media/Interface/Glues/Rule.txt", "r"))
	--while true do
		--local Rule = f:read("*line")
		--if Rule == nil then break end
		--local n = string.match(username,Rule)
		--if n ~= nil then
			--print(n)
			--return
		--end
	--end
	
	g_mainplayer:setCeateEquip(curHatState, curClothState, curGlasseState, curTrouState, curBackState, curShoeState, curHandState, curTrinKetState)
	EQGlobaFunc.CreatPlayerRequest(account,username)
end                                                                                                                               

--------------------------------------------------------------------------------------------
function OnCreateFail(args)
	print("创建角色失败 error code ="..tostring(args))
end

-----------------------------------------
function OnCreateCancel(args)
	print("取消创建角色操作！")
end
-----------------------------------------
-- 玩家创建人物信息收集
-----------------------------------------
--性别-----------------------------------------------------------------------------------------------
local GenderFlag = 1  --1:男,2:女
BeardNum = 0
--g_WinMgr:getWindow("CreateCharacter/BackL/GoateeText"):setEnabled(false)

function CreatePlayer_OnSelectGender(args)
	CreatePlayer_removeAllIcon("CreateCharacter/BackR/shangyiIcon")
	CreatePlayer_removeAllIcon("CreateCharacter/BackR/shoesIcon")
	CreatePlayer_removeAllIcon("CreateCharacter/BackR/xiayiIcon")
	shangyiCurPage = 1
	xiayiCurPage = 1
	shoesCurPage = 1
	curSeletSuitState = 0
	
	CreatePlayer_TabButtonSta()
	CreatePlayer_hideShoesSlots(0)
	CreatePlayer_setDefSkinBtnStat()
	avatar:getAdjuster():doChange(g_atSkin,1)
	local sex = CEGUI.toWindowEventArgs(args).window:getName()
	if sex == "CreateCharacter/BackL/Btn_Female" then
		if GenderFlag == 2 then
			SkinColorFlag =	1--avatar:getAdjuster():getCurrentVal(g_atSkin)
			reSetSelectTab()
			
			shangyiTotalPage = getTotalPage(femaleShangYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,femaleShangYiIcons,femaleShangYiImagesets)
			CreatePlayer_OnSelectBianzhuang()
			
			xiayiTotalPage = getTotalPage(femaleXiaYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,femaleXiaYiIcons,femaleXiaYiImagesets)
			CreatePlayer_OnSelectTrousers()
			
			shoesTotalPage = getTotalPage(femaleShoesIcons,MAXSIZE)
			CreatePlayer_updateData(1,femaleShoesIcons,femaleShoesImagesets)
			CreatePlayer_OnSelectShoes()
			
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("NormalImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("HoverImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("PushedImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("NormalImage", "set:Button6 image:NewImage108")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("HoverImage", "set:Button6 image:NewImage110")
			return
		else
			clearState()
			CreatePlayer_clearSelectSta()
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("NormalImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("HoverImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("PushedImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("NormalImage", "set:Button6 image:NewImage108")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("HoverImage", "set:Button6 image:NewImage110")
			GenderFlag = 2
			g_rtt:changeSex(2)
			
			EQGlobaFunc.RTTScale(1.12,1.12,1.12)
			EQGlobaFunc.RTTRotate(5,3)
			reSetSelectTab()
			
			shangyiTotalPage = getTotalPage(femaleShangYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,femaleShangYiIcons,femaleShangYiImagesets)
			CreatePlayer_OnSelectBianzhuang()
			
			xiayiTotalPage = getTotalPage(femaleXiaYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,femaleXiaYiIcons,femaleXiaYiImagesets)
			CreatePlayer_OnSelectTrousers()
			
			shoesTotalPage = getTotalPage(femaleShoesIcons,MAXSIZE)
			CreatePlayer_updateData(1,femaleShoesIcons,femaleShoesImagesets)
			CreatePlayer_OnSelectShoes()

			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowL"):removeEvent("Clicked")
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowR"):removeEvent("Clicked")
--			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeText"):setVisible(false)
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowL"):setVisible(false)
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowR"):setVisible(false)
		end
	end	
	if sex == "CreateCharacter/BackL/Btn_Male" then
		if GenderFlag == 1 then
			SkinColorFlag =	1
			reSetSelectTab()
			
			shangyiTotalPage = getTotalPage(maleShangYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,maleShangYiIcons,maleshangYiImagesets)
			CreatePlayer_OnSelectBianzhuang()
			
			xiayiTotalPage = getTotalPage(maleXiaYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,maleXiaYiIcons,maleXiaYiImagesets)
			CreatePlayer_OnSelectTrousers()
			
			shoesTotalPage = getTotalPage(maleShoesIcons,MAXSIZE)
			CreatePlayer_updateData(1,maleShoesIcons,maleShoesImagesets)
			CreatePlayer_OnSelectShoes()

			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("NormalImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("HoverImage", "set:Button6 image:NewImage109")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("NormalImage", "set:Button6 image:NewImage108")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("HoverImage", "set:Button6 image:NewImage108")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("PushedImage", "set:Button6 image:NewImage108")
			return
		else
			clearState()
			CreatePlayer_clearSelectSta()
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("NormalImage", "set:Button6 image:NewImage107")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Female"):setProperty("HoverImage", "set:Button6 image:NewImage109")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("NormalImage", "set:Button6 image:NewImage108")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("HoverImage", "set:Button6 image:NewImage108")
			g_WinMgr:getWindow("CreateCharacter/BackL/Btn_Male"):setProperty("PushedImage", "set:Button6 image:NewImage108")
			GenderFlag = 1
			g_rtt:changeSex(1)
			
			EQGlobaFunc.RTTScale(1.03,1.03,1.03)
			EQGlobaFunc.RTTRotate(3,3)
			reSetSelectTab()
			
			shangyiTotalPage = getTotalPage(maleShangYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,maleShangYiIcons,maleshangYiImagesets)
			CreatePlayer_OnSelectBianzhuang()
			
			xiayiTotalPage = getTotalPage(maleXiaYiIcons,MAXSIZE)
			CreatePlayer_updateData(1,maleXiaYiIcons,maleXiaYiImagesets)
			CreatePlayer_OnSelectTrousers()
			
			shoesTotalPage = getTotalPage(maleShoesIcons,MAXSIZE)
			CreatePlayer_updateData(1,maleShoesIcons,maleShoesImagesets)
			CreatePlayer_OnSelectShoes()

			BeardNum = 0
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowL"):removeEvent("Clicked")
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowR"):removeEvent("Clicked")
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowL"):subscribeEvent("Clicked","CreatePlayer_OnSelectBeardL")
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowR"):subscribeEvent("Clicked","CreatePlayer_OnSelectBeardR")
--			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeText"):setVisible(true)
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowL"):setVisible(true)
			g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowR"):setVisible(true)

		end
	end
end

--肤色-------------------------------------------------------------------------------------------------
g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):subscribeEvent("Clicked","CreatePlayer_OnSelectSkinColor")
g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):subscribeEvent("Clicked","CreatePlayer_OnSelectSkinColor")
g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):subscribeEvent("Clicked","CreatePlayer_OnSelectSkinColor")
g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):subscribeEvent("Clicked","CreatePlayer_OnSelectSkinColor")
g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage15")
g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage15")
g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage15")

function CreatePlayer_OnSelectSkinColor(args)
	
	local skincolor = CEGUI.toWindowEventArgs(args).window:getName()
	if skincolor == "CreateCharacter/BackL/ComplexionButton1" then
		if SkinColorFlag == 1 then
			print("fuSe:", SkinColorFlag)
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage15")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage15")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage15")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage8")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage12")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage8")

			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage9")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage13")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage9")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage10")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage14")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage10")
			
			return
		else
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage15")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage15")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage15")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage8")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage12")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage8")

			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage9")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage13")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage9")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage10")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage14")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage10")

			SkinColorFlag = 1
			avatar:getAdjuster():doChange(g_atSkin,1)
		end
	end
	if skincolor == "CreateCharacter/BackL/ComplexionButton2" then
		if SkinColorFlag == 2 then
			print("fuSe:", SkinColorFlag)
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage7")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage11")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage7")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage16")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage16")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage16")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage9")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage13")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage9")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage10")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage14")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage10")

			return
		else
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage7")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage11")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage7")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage16")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage16")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage16")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage9")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage13")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage9")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage10")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage14")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage10")
			SkinColorFlag = 2
			avatar:getAdjuster():doChange(g_atSkin,2)
		end
	end
	if skincolor == "CreateCharacter/BackL/ComplexionButton3" then
		if SkinColorFlag == 3 then
			print("fuSe:", SkinColorFlag)
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage7")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage11")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage7")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage8")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage12")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage8")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage17")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage17")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage17")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage10")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage14")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage10")

			return
		else
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage7")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage11")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage7")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage8")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage12")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage8")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage17")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage17")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage17")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage10")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage14")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage10")
			
			SkinColorFlag = 3
			print("fuSe:", SkinColorFlag)
			avatar:getAdjuster():doChange(g_atSkin,3)
			print(avatar:getAdjuster():getCurrentVal(g_atSkin))
		end
	end
	if skincolor == "CreateCharacter/BackL/ComplexionButton4" then
		if SkinColorFlag == 4 then
			print("fuSe:", SkinColorFlag)
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage7")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage11")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage7")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage8")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage12")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage8")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage9")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage13")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage9")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage18")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage18")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage18")

			return
		else
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage7")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage11")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage7")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage8")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage12")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage8")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage9")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage13")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage9")
			
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage18")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage18")
			g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage18")
			
			SkinColorFlag = 4
			avatar:getAdjuster():doChange(g_atSkin,4)
		end
	end

end

--发型----------------------------------------------------------------------------------------------
g_WinMgr:getWindow("CreateCharacter/BackL/HairstyleArrowL"):subscribeEvent("Clicked","CreatePlayer_OnSelectHairStyleL")
g_WinMgr:getWindow("CreateCharacter/BackL/HairstyleArrowR"):subscribeEvent("Clicked","CreatePlayer_OnSelectHairStyleR")
local HairStyleNum = 0

------------------------------------------------------------------------------------------------------
function CreatePlayer_OnSelectHairStyleL()
	avatar:getAdjuster():doPrev(g_atHairType)
end

--------------------------------------------------------------------------------------------------------
function CreatePlayer_OnSelectHairStyleR()
	avatar:getAdjuster():doNext(g_atHairType)
end

--发色-----------------------------------------------------------------------------------------------
g_WinMgr:getWindow("CreateCharacter/BackL/HairColourArrowL"):subscribeEvent("Clicked","CreatePlayer_OnSelectHairColorL")
g_WinMgr:getWindow("CreateCharacter/BackL/HairColourArrowR"):subscribeEvent("Clicked","CreatePlayer_OnSelectHairColorR")
local HairColorNum = 0

function CreatePlayer_OnSelectHairColorL()
	avatar:getAdjuster():doPrev(g_atHairColor)
end

function CreatePlayer_OnSelectHairColorR()
	avatar:getAdjuster():doNext(g_atHairColor)
end

--脸型----------------------------------------------------------------------------------------------
g_WinMgr:getWindow("CreateCharacter/BackL/FaceTypeArrowL"):subscribeEvent("Clicked","CreatePlayer_OnSelectFaceTypeL")
g_WinMgr:getWindow("CreateCharacter/BackL/FaceTypeArrowR"):subscribeEvent("Clicked","CreatePlayer_OnSelectFaceTypeR")

function CreatePlayer_OnSelectFaceTypeL()
	avatar:getAdjuster():doPrev(g_atFace)
end
function CreatePlayer_OnSelectFaceTypeR()
	avatar:getAdjuster():doNext(g_atFace)
end

--胡子----------------------------------------------------------------
g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowL"):subscribeEvent("Clicked","CreatePlayer_OnSelectBeardL")
g_WinMgr:getWindow("CreateCharacter/BackL/GoateeArrowR"):subscribeEvent("Clicked","CreatePlayer_OnSelectBeardR")

function CreatePlayer_OnSelectBeardL()
	avatar:getAdjuster():doPrev(g_atBeard)
end

function CreatePlayer_OnSelectBeardR()
	avatar:getAdjuster():doNext(g_atBeard)
end

--------------------------------------------------------------
-- 登录场景处理
--------------------------------------------------------------
function CreatePlayer_OnLoadScene()
	CreatePlayer_setInCPFlag(false)
	if g_clientGameInited == false then
		initialClientGame()
		clientGameLoads()
		g_clientGameInited = true
	end
	
	EQGlobaFunc.LoadScene()
	
	avatar:getAdjuster():setForCreateRole(false)

	--已不用这种做法了
	--LoadMesh_LoadMesh()
	g_GuiSystem:setGUISheet(g_UIRoot)
	g_UIRoot:activate()
	g_UIRoot:setWantsMultiClickEvents(false)
	g_UIRoot:setProperty("MousePassThroughEnabled", "true")
	--g_MainUIWindows:setProperty("MousePassThroughEnabled", "true")
	g_FriendSys:reqFriendList()
	
	EQGlobaFunc.RTTSetActive(g_rttAvatarWinName, false)
	BigExpression_loadBigExpressionResource()
	UIHelpBtnShowFunc_loadTableSource()
	task_subscribe_scripts()	-- 注册所有新手任务脚本(add by hezhr)
end

function CreatePlayer_OnLoadSceneFail(args)
	print("加载场景失败！")
end

--------------------------------------------------------------
--界面右部分服装选择逻辑处理部分
--------------------------------------------------------------
--格子名
local cslotName = "CreateCharacter/BackR/shangyiIcon"
--订阅相关按钮事件
g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 1"):subscribeEvent("Clicked","CreatePlayer_updateBianZhuangBufferData")
g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 2"):subscribeEvent("Clicked","CreatePlayer_updateZhengZhuangBufferData")

g_WinMgr:getWindow("CreateCharacter/BackL/shangyiArrowL"):subscribeEvent("Clicked","CreatePlayer_shangyiprePage")
g_WinMgr:getWindow("CreateCharacter/BackL/shangyiArrowR"):subscribeEvent("Clicked","CreatePlayer_shangyiNextPage")

g_WinMgr:getWindow("CreateCharacter/BackL/xiayiArrowL"):subscribeEvent("Clicked","CreatePlayer_xiayiprePage")
g_WinMgr:getWindow("CreateCharacter/BackL/xiayiArrowR"):subscribeEvent("Clicked","CreatePlayer_xiayiNextPage")

g_WinMgr:getWindow("CreateCharacter/BackL/shoesArrowL"):subscribeEvent("Clicked","CreatePlayer_shoesprePage")
g_WinMgr:getWindow("CreateCharacter/BackL/shoesArrowR"):subscribeEvent("Clicked","CreatePlayer_shoesNextPage")

--g_WinMgr:getWindow("CreateCharacter/TabContrl__auto_TabPane__Buttons__auto_btnTab 2"):subscribeEvent("Clicked","CreatePlayer_OnSelectZhengzhuang")
	--g_WinMgr:getWindow("CreateCharacter/BackR/Btn_YunDong"):subscribeEvent("Clicked","CreatePlayer_OnSelectYundong")
--g_WinMgr:getWindow("CreateCharacter/TabContrl__auto_TabPane__Buttons__auto_btnTab 3"):subscribeEvent("Clicked","CreatePlayer_OnSelectShoes")
--g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):subscribeEvent("Clicked","CreatePlayer_OnSelectClothes")
--g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):subscribeEvent("Clicked","CreatePlayer_OnSelectTrousers")
--g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):subscribeEvent("Clicked","CreatePlayer_OnSelectSuit")

--订阅物品格按钮事件
clickFlag = false
function CreatePlayer_itemSubscribeEvent()
	for i=1, 3 do
		g_WinMgr:getWindow("CreateCharacter/BackR/shangyiIcon" ..i):removeEvent("MouseClick")
		g_WinMgr:getWindow("CreateCharacter/BackR/xiayiIcon" ..i):removeEvent("MouseClick")
		g_WinMgr:getWindow("CreateCharacter/BackR/shoesIcon" ..i):removeEvent("MouseClick")
		g_WinMgr:getWindow("CreateCharacter/BackR/shangyiIcon" ..i):subscribeEvent("MouseClick","CreatePlayer_OnClickItem")
		g_WinMgr:getWindow("CreateCharacter/BackR/xiayiIcon" ..i):subscribeEvent("MouseClick","CreatePlayer_OnClickItem")
		g_WinMgr:getWindow("CreateCharacter/BackR/shoesIcon" ..i):subscribeEvent("MouseClick","CreatePlayer_OnClickItem")
	end
end

--自动加载ICON函数
function CreatePlayer_DynamicLoadItem()

end

--相关按钮事件函数
--------------------------------
---------------------------------------------------
--更新显示缓存数据
function CreatePlayer_updateBianZhuangBufferData()
	curSeletSuitState = 0
	
	shangyiCurPage = 1
	xiayiCurPage = 1
	shoesCurPage = 1
	
	CreatePlayer_TabButtonSta()
	CreatePlayer_hideShoesSlots(0)
	--上衣
	
	if g_rtt:getSex() == 1 then
		shangyiTotalPage = getTotalPage(maleShangYiIcons,MAXSIZE)
		CreatePlayer_updateData(shangyiCurPage,maleShangYiIcons,maleshangYiImagesets)
	else
		shangyiTotalPage = getTotalPage(femaleShangYiIcons,MAXSIZE)
		CreatePlayer_updateData(shangyiCurPage,femaleShangYiIcons,femaleShangYiImagesets)
	end
	CreatePlayer_OnSelectBianzhuang()
	--下衣
	if g_rtt:getSex() == 1 then
		xiayiTotalPage = getTotalPage(maleXiaYiIcons,MAXSIZE)
		CreatePlayer_updateData(xiayiCurPage,maleXiaYiIcons,maleXiaYiImagesets)
	else
		xiayiTotalPage = getTotalPage(femaleXiaYiIcons,MAXSIZE)
		CreatePlayer_updateData(xiayiCurPage,femaleXiaYiIcons,femaleXiaYiImagesets)
	end
	CreatePlayer_OnSelectTrousers()
	--鞋子
	if g_rtt:getSex() == 1 then
		shoesTotalPage = getTotalPage(maleShoesIcons,MAXSIZE)
		CreatePlayer_updateData(shoesCurPage,maleShoesIcons,maleShoesImagesets)
	else
		shoesTotalPage = getTotalPage(femaleShoesIcons,MAXSIZE)
		CreatePlayer_updateData(shoesCurPage,femaleShoesIcons,femaleShoesImagesets)
	end
	CreatePlayer_OnSelectShoes()
end
---------
--
function CreatePlayer_updateZhengZhuangBufferData()
	curSeletSuitState = 1
	
	shangyiCurPage = 1
	xiayiCurPage = 1
	shoesCurPage = 1
	
	CreatePlayer_TabButtonSta()
	CreatePlayer_hideShoesSlots(1)
	
	--上衣
	if g_rtt:getSex() == 1 then
		shangyiTotalPage = getTotalPage(malezhengzhuangIcons,MAXSIZE)
		CreatePlayer_updateData(shangyiCurPage,malezhengzhuangIcons,malezhengzhuangImagesets)
	else
		shangyiTotalPage = getTotalPage(femalezhengzhuangIcons,MAXSIZE)
		CreatePlayer_updateData(shangyiCurPage,femalezhengzhuangIcons,femalezhengzhuangImagesets)
	end
	CreatePlayer_OnSelectBianzhuang()
	--鞋子
	if g_rtt:getSex() == 1 then
		shoesTotalPage = getTotalPage(maleShoesIcons,MAXSIZE)
		CreatePlayer_updateData(shoesCurPage,maleShoesIcons,maleShoesImagesets)
	else
		shoesTotalPage = getTotalPage(femaleShoesIcons,MAXSIZE)
		CreatePlayer_updateData(shoesCurPage,femaleShoesIcons,femaleShoesImagesets)
	end
	CreatePlayer_OnSelectTrousers()
	CreatePlayer_removeAllIcon("CreateCharacter/BackR/shoesIcon")
end
--------------------------------------------------------------
--便装按钮逻辑
--这个默认显示上装的icon
function CreatePlayer_OnSelectBianzhuang()
	CreatePlayer_removeAllIcon("CreateCharacter/BackR/shangyiIcon")
	print("1")
	
	--显示便装下的上衣，下衣，套装按钮
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("Visible", "True")
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("Visible", "True")
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("Visible", "True")
	
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("NormalImage","set:Button1 image:ImageButton50_Pushed")
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("HoverImage","set:Button1 image:ImageButton50_Pushed")
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("NormalImage","set:Button1 image:ImageButton51_Normal")
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("HoverImage","set:Button1 image:ImageButton51_Hover")
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("NormalImage","set:Button1 image:ImageButton52_Normal")
--	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("HoverImage","set:Button1 image:ImageButton52_Hover")
	--男
	if 1 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("11")
		for i,v in ipairs(showIconBuffer) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, showImagesetBuffer[i], v, "False")
--			g_Loger:logError("maleShangYiIcons..." ..i .." " ..showIconBuffer[i])
--			g_Loger:logError("maleshangYiImagesets..." ..i .." " ..showImagesetBuffer[i])
		end
		
	--女	
	elseif 2 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("22")
		for i,v in ipairs(showIconBuffer) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, showImagesetBuffer[i], v, "False")
--			g_Loger:logDebug("femaleShangYiIcons..." ..i .." " ..showIconBuffer[i])
--			g_Loger:logDebug("femaleShangYiImagesets..." ..i .." " ..showImagesetBuffer[i])
		end
	end
	
	--移除选中ICON的框
	removeSelectBlock("CreateCharacter/BackR/shangyiIcon")
	if curSeletSuitState == 0 then
		CreatePlayer_checkSelectSta1()
	else
		CreatePlayer_checkSelectSta4()
	end
end

--正装按钮------------------------------------------
function CreatePlayer_OnSelectZhengzhuang()
	CreatePlayer_removeAllIcon()
	--隐藏便装下的上衣，下衣，套装按钮
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("Visible", "False")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("Visible", "False")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("Visible", "False")
	
		--男
	if 1 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("11")
		for i,v in ipairs(malezhengzhuangIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, malezhengzhuangImagesets[i], v, "False")
--			g_Loger:logDebug("malezhengzhuangIcons..." ..i .." " ..malezhengzhuangIcons[i])
--			g_Loger:logDebug("malezhengzhuangImagesets..." ..i .." " ..malezhengzhuangImagesets[i])
		end
		
	--女	
	elseif 2 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("22")
		for i,v in ipairs(femalezhengzhuangIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, femalezhengzhuangImagesets[i], v, "False")
--			g_Loger:logDebug("femalezhengzhuangIcons..." ..i .." " ..femalezhengzhuangIcons[i])
--			g_Loger:logDebug("femalezhengzhuangImagesets..." ..i .." " ..femalezhengzhuangImagesets[i])
		end
	end

	--移除选中ICON的框
	removeSelectBlock("CreateCharacter/BackR/shangyiIcon")
	CreatePlayer_checkSelectSta1()
end

--运动装按钮------------------------------------------
function CreatePlayer_OnSelectYundong()
	CreatePlayer_removeAllIcon()
	--隐藏便装下的上衣，下衣，套装按钮
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("Visible", "False")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("Visible", "False")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("Visible", "False")
	
			--男
	if 1 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		for i,v in ipairs(maleYundongIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, maleYundongImagesets[i], v, "False")
--			g_Loger:logDebug("maleYundongIcons..." ..i .." " ..maleYundongIcons[i])
--			g_Loger:logDebug("maleYundongImagesets..." ..i .." " ..maleYundongImagesets[i])
		end
		
	--女	
	elseif 2 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		for i,v in ipairs(femaleYundongIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, femaleYundongImagesets[i], v, "False")
--			g_Loger:logDebug("femaleYundongIcons..." ..i .." " ..femaleYundongIcons[i])
--			g_Loger:logDebug("femaleYundongImagesets..." ..i .." " ..femaleYundongImagesets[i])
		end
	end

	--移除选中ICON的框
	removeSelectBlock("CreateCharacter/BackR/shangyiIcon")
end

--脚部按钮----------------------------------------------
local cslotName2 = "CreateCharacter/BackR/shoesIcon"
function CreatePlayer_OnSelectShoes()
	CreatePlayer_removeAllIcon("CreateCharacter/BackR/shoesIcon")
	--隐藏便装下的上衣，下衣，套装按钮
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("Visible", "False")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("Visible", "False")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("Visible", "False")
	
	--男
	if 1 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		for i,v in ipairs(showIconBuffer) do
			StorageUI_DynamicLoadItem(cslotName2 ..i, "item"..i, i, showImagesetBuffer[i], v, "False")
--			g_Loger:logDebug("maleShoesIcons..." ..i .." " ..showIconBuffer[i])
--			g_Loger:logDebug("maleShoesImagesets..." ..i .." " ..showImagesetBuffer[i])
		end
		
	--女	
	elseif 2 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		for i,v in ipairs(showIconBuffer) do
			StorageUI_DynamicLoadItem(cslotName2 ..i, "item"..i, i, showImagesetBuffer[i], v, "False")
--			g_Loger:logDebug("femaleShoesIcons..." ..i .." " ..showIconBuffer[i])
--			g_Loger:logDebug("femaleShoesImagesets..." ..i .." " ..showImagesetBuffer[i])
		end
	end
	
	--移除选中ICON的框
	removeSelectBlock("CreateCharacter/BackR/shoesIcon")
	CreatePlayer_checkSelectSta3()
end

--上衣按钮-----------------------------------------------
function CreatePlayer_OnSelectClothes()
	CreatePlayer_removeAllIcon()
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("NormalImage","set:Button1 image:ImageButton50_Pushed")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("HoverImage","set:Button1 image:ImageButton50_Pushed")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("NormalImage","set:Button1 image:ImageButton51_Normal")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("HoverImage","set:Button1 image:ImageButton51_Hover")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("NormalImage","set:Button1 image:ImageButton52_Normal")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("HoverImage","set:Button1 image:ImageButton52_Hover")
	--男
	if 1 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("11")
--		g_Loger:logError("maleShangYiIcons count..." ..table.getn(maleShangYiIcons))
		for i,v in ipairs(maleShangYiIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, maleshangYiImagesets[i], v, "False")
--			g_Loger:logDebug("maleShangYiIcons..." ..i .." " ..maleShangYiIcons[i])
--			g_Loger:logDebug("maleshangYiImagesets..." ..i .." " ..maleshangYiImagesets[i])
		end
	--女	
	elseif 2 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("22")
		for i,v in ipairs(femaleShangYiIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, femaleShangYiImagesets[i], v, "False")
--			g_Loger:logDebug("femaleShangYiIcons..." ..i .." " ..femaleShangYiIcons[i])
--			g_Loger:logDebug("femaleShangYiImagesets..." ..i .." " ..femaleShangYiImagesets[i])
		end
	end
	--移除选中ICON的框
	removeSelectBlock("CreateCharacter/BackR/shangyiIcon")
	CreatePlayer_checkSelectSta1()
end

--下衣按钮-----------------------------------------------
local cslotName1 = "CreateCharacter/BackR/xiayiIcon"
function CreatePlayer_OnSelectTrousers()
	CreatePlayer_removeAllIcon("CreateCharacter/BackR/xiayiIcon")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("NormalImage","set:Button1 image:ImageButton50_Normal")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("HoverImage","set:Button1 image:ImageButton50_Hover")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("NormalImage","set:Button1 image:ImageButton51_Pushed")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("HoverImage","set:Button1 image:ImageButton51_Pushed")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("NormalImage","set:Button1 image:ImageButton52_Normal")
	--g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("HoverImage","set:Button1 image:ImageButton52_Hover")
	--男
	if 1 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		for i,v in ipairs(showIconBuffer) do
			StorageUI_DynamicLoadItem(cslotName1 ..i, "item"..i, i, showImagesetBuffer[i], v,"False")
--			g_Loger:logDebug("maleXiaYiIcons..." ..i .." " ..showIconBuffer[i])
--			g_Loger:logDebug("maleXiaYiImagesets..." ..i .." " ..showImagesetBuffer[i])
		end
	--女	
	elseif 2 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("22")
		for i,v in ipairs(showIconBuffer) do
			StorageUI_DynamicLoadItem(cslotName1 ..i, "item"..i, i, showImagesetBuffer[i], v, "False")
--			g_Loger:logDebug("femaleXiaYiIcons..." ..i .." " ..showIconBuffer[i])
--			g_Loger:logDebug("femaleXiaYiImagesets..." ..i .." " ..showImagesetBuffer[i])
		end
	end
	--移除选中ICON的框
	removeSelectBlock("CreateCharacter/BackR/xiayiIcon")
	if curSeletSuitState == 0 then
		CreatePlayer_checkSelectSta2()
	else
		CreatePlayer_checkSelectSta5()
	end
end

--套装按钮-------------------------------------------------
function CreatePlayer_OnSelectSuit()
	CreatePlayer_removeAllIcon()
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("NormalImage","set:Button1 image:ImageButton50_Normal")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("HoverImage","set:Button1 image:ImageButton50_Hover")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("NormalImage","set:Button1 image:ImageButton51_Normal")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectTrousers"):setProperty("HoverImage","set:Button1 image:ImageButton51_Hover")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("NormalImage","set:Button1 image:ImageButton52_Pushed")
	g_WinMgr:getWindow("CreateCharacter/BackR/SelectSuit"):setProperty("HoverImage","set:Button1 image:ImageButton52_Pushed")
	--男
	if 1 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		for i,v in ipairs(maleSuitIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, maleSuitImagesets[i], v, "False")
--			g_Loger:logDebug("maleSuitIcons..." ..i .." " ..maleSuitIcons[i])
--			g_Loger:logDebug("maleSuitImagesets..." ..i .." " ..maleSuitImagesets[i])
		end
	--女	
	elseif 2 == avatar:getAdjuster():getCurrentVal(g_atSex) then
		print("22")
		for i,v in ipairs(femaleSuitIcons) do
			StorageUI_DynamicLoadItem(cslotName ..i, "item"..i, i, femaleSuitImagesets[i], v, "False")
--			g_Loger:logDebug("femaleSuitIcons..." ..i .." " ..femaleSuitIcons[i])
--			g_Loger:logDebug("femaleSuitImagesets..." ..i .." " ..femaleSuitImagesets[i])
		end
	end

	--移除选中ICON的框
	removeSelectBlock("CreateCharacter/BackR/shangyiIcon")
end

citem = g_WinMgr:createWindow("TaharezLook/StaticImage","tip")
			citem:setProperty("Image", "set:windows1 image:BackGround14")
			citem:setProperty("MousePassThroughEnabled", "true")
			citem:setProperty("WantsMultiClickEvents", "false")
			citem:setProperty("FrameEnabled", "false")
			citem:setProperty("BackgroundEnabled", "false")
			citem:setProperty("AlwaysOnTop", "true")
citem1 = g_WinMgr:createWindow("TaharezLook/StaticImage","tip1")
			citem1:setProperty("Image", "set:windows1 image:BackGround14")
			citem1:setProperty("MousePassThroughEnabled", "true")
			citem1:setProperty("WantsMultiClickEvents", "false")
			citem1:setProperty("FrameEnabled", "false")
			citem1:setProperty("BackgroundEnabled", "false")
			citem1:setProperty("AlwaysOnTop", "true")
citem2 = g_WinMgr:createWindow("TaharezLook/StaticImage","tip2")
			citem2:setProperty("Image", "set:windows1 image:BackGround14")
			citem2:setProperty("MousePassThroughEnabled", "true")
			citem2:setProperty("WantsMultiClickEvents", "false")
			citem2:setProperty("FrameEnabled", "false")
			citem2:setProperty("BackgroundEnabled", "false")
			citem2:setProperty("AlwaysOnTop", "true")

local oldEquipSlotA0 = {}
local oldEquipSlotA1 = {}
local oldEquipSlotA2 = {}
local oldEquipSlotA3 = {}
local oldEquipSlotA4 = {}
local oldEquipSlotA5 = {}
local oldEquipSlotA6 = {}
local oldEquipSlotA7 = {}
for i = 0,7 do
	oldEquipSlotA0[i] = 0
	oldEquipSlotA1[i] = 0
	oldEquipSlotA2[i] = 0
	oldEquipSlotA3[i] = 0
	oldEquipSlotA4[i] = 0
	oldEquipSlotA5[i] = 0
	oldEquipSlotA6[i] = 0
	oldEquipSlotA7[i] =	0
end
local oldEquipSlot1 = nil
local oldEquipSlotSuit = nil
local oldEquipSlotShangyi = nil
local oldEquipSlotXiayi = nil
local oldEquipSlotShoes = nil
local oldEquipSlotHat = nil

--物品格点击事件函数-----------------------------------------
function CreatePlayer_OnClickItem(args)
	cslot = CEGUI.toWindowEventArgs(args).window
	--测试
--	g_Loger:logDebug("cslot..." ..cslot:getName())
--	g_Loger:logDebug("ChildCount..." ..cslot:getChildCount())
	------
	if cslot:getChildCount() ~= 0 then
		if cslot ~= cslotPre then
		local winNameLen = string.len(cslot:getName()) - 1
		local slotBaseName = string.sub(cslot:getName(), 1,winNameLen)
--		g_Loger:logError("slotBaseName..." ..slotBaseName)
		if slotBaseName == "CreateCharacter/BackR/shangyiIcon" then
			if cslot:getChildAtIdx(0):getChildCount() > 0 then
				iocnWnd = cslot:getChildAtIdx(0):getChildAtIdx(0)
				iocnWnd:addChildWindow(citem)
			end
		end
		if slotBaseName == "CreateCharacter/BackR/xiayiIcon" then
			if cslot:getChildAtIdx(0):getChildCount() > 0 then
				iocnWnd = cslot:getChildAtIdx(0):getChildAtIdx(0)
				iocnWnd:addChildWindow(citem1)
			end
		end
		if slotBaseName == "CreateCharacter/BackR/shoesIcon" then
			if cslot:getChildAtIdx(0):getChildCount() > 0 then
				iocnWnd = cslot:getChildAtIdx(0):getChildAtIdx(0)
				iocnWnd:addChildWindow(citem2)
			end
		end
			
			propertystr = cslot:getChildAtIdx(0):getChildAtIdx(0):getProperty("Image")
		--保存选的是哪个Slot
		if slotBaseName == "CreateCharacter/BackR/shangyiIcon" then
			if curSeletSuitState == 0 then
				CreatePlayer_saveSelectItem1(propertystr)
				mSelected4 = ""
			else
				CreatePlayer_saveSelectItem4(propertystr)
				mSelected1 = ""
				mSelected2 = ""
			end
		end
		if slotBaseName == "CreateCharacter/BackR/xiayiIcon" then
			if curSeletSuitState == 0 then
				CreatePlayer_saveSelectItem2(propertystr)
				mSelected4 = ""
			else
				CreatePlayer_saveSelectItem5(propertystr)
				CreatePlayer_saveSelectItem3(propertystr)
			end
		end
		if slotBaseName == "CreateCharacter/BackR/shoesIcon" then
			CreatePlayer_saveSelectItem3(propertystr)
			CreatePlayer_saveSelectItem5(propertystr)
		end

			--测试
			property1 = cslot:getChildAtIdx(0):getName()
			--pro1 = cslot:getChildAtIdx(1):getName()
--			g_Loger:logDebug("propertystr..." ..propertystr)
--			g_Loger:logDebug("property1..." ..property1)
--			--g_Loger:logDebug("pro1..." ..pro1)
			------
			idx = string.len(propertystr) - 1
			substr = string.sub(propertystr, idx)
			substr1 = string.sub(propertystr, 5, 20)
--			g_Loger:logDebug("substr1..." ..substr1)
			--测试
			if substr ~= nil then
				print(substr)
--				g_Loger:logDebug("icon2subID..." ..substr1 ..substr .." " ..icon2subID[substr1 ..substr])
				----
				local equipMap = g_itemtableMgr:getItemEquipRow(icon2subID[substr1 ..substr])
				equipSlot = equipMap.equip_placeholder
				--装备挂接栏位
				holdSlot = equipMap.equip_pos
				local equipDisID = equipMap.equip_disp_id
				local type1 = equipMap.type1

				
				--
--				g_Loger:logDebug("equipSlot..." ..equipSlot)
--				g_Loger:logDebug("equipDisID..." ..equipDisID)
				--
				local returnStr = EQGlobaFunc.UIntToStringBitsetScript(equipSlot)
				local returnStrLen = string.len(returnStr)
				--
--				g_Loger:logDebug("returnStr......" ..returnStr)
--				g_Loger:logDebug("returnStrLen......" ..returnStrLen)
				--
				local slotTab = Split(returnStr, ",")
				local slotArray = {}
				for i,v in ipairs(slotTab) do
--					g_Loger:logDebug("slotTab..." ..i .."..." ..v)
					slotArray[i] = tonumber(slotTab[i])
--					g_Loger:logDebug("slotArray..." ..i .."..." ..slotArray[i])
				end
				if holdSlot == 0 then
					for i,v in ipairs(slotArray) do
						EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						if checkElemInArray(slotArray[i],oldEquipSlotA0) then
							EQGlobaFunc.RTTtakeOff(oldEquipSlot1)
						end
					end
					curHatState = 0
				elseif holdSlot == 1 then
					for i,v in ipairs(oldEquipSlotA1) do
						--EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						EQGlobaFunc.RTTtakeOff(2^oldEquipSlotA1[i])
						if oldEquipSlotSuit ~= nil then
							EQGlobaFunc.RTTtakeOff(oldEquipSlotSuit)
							oldEquipSlotSuit = nil
						end
					end
					if type1 == 1 then
						curClothState = 0
						curTrouState = 0
					end
				elseif holdSlot == 2 then
					for i,v in ipairs(slotArray) do
						EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						EQGlobaFunc.RTTtakeOff(2^oldEquipSlotA2[i])
						if checkElemInArray(slotArray[i],oldEquipSlotA2) then
							EQGlobaFunc.RTTtakeOff(oldEquipSlot1)
						end
					end
				elseif holdSlot == 3 then
					for i,v in ipairs(slotArray) do
						EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						EQGlobaFunc.RTTtakeOff(2^oldEquipSlotA3[i])
						
						for i =0,7 do
						oldEquipSlotA1[i] = 0
						end	
						if oldEquipSlotXiayi ~= nil then
						EQGlobaFunc.RTTtakeOff(oldEquipSlotXiayi)
						end
						if oldEquipSlotSuit ~= nil then
							EQGlobaFunc.RTTtakeOff(oldEquipSlotSuit)
							oldEquipSlotSuit = nil
							EQGlobaFunc.RTTtakeOff(2)
							curClothState = 0
						end
					end
					curTrouState = 0
				elseif holdSlot == 4 then
					for i,v in ipairs(slotArray) do
						EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						EQGlobaFunc.RTTtakeOff(2^oldEquipSlotA4[i])
						if checkElemInArray(slotArray[i],oldEquipSlotA4) then
							EQGlobaFunc.RTTtakeOff(oldEquipSlot1)
						end
					end
				elseif holdSlot == 5 then
					for i,v in ipairs(slotArray) do
						EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						EQGlobaFunc.RTTtakeOff(2^oldEquipSlotA5[i])
						if oldEquipSlotShoes then
						EQGlobaFunc.RTTtakeOff(oldEquipSlotShoes)
						end
					end
					curShoeState = 0 
				elseif holdSlot == 6 then
					for i,v in ipairs(slotArray) do
						EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						if checkElemInArray(slotArray[i],oldEquipSlotA6) then
							EQGlobaFunc.RTTtakeOff(oldEquipSlot1)
						end
					end
				elseif holdSlot == 7 then
					for i,v in ipairs(slotArray) do
						EQGlobaFunc.RTTtakeOff(2^slotArray[i])
						if checkElemInArray(slotArray[i],oldEquipSlotA7) then
							EQGlobaFunc.RTTtakeOff(oldEquipSlot1)
						end
					end

				end

				EQGlobaFunc.RTTequipOn(equipSlot,equipDisID,equipMap.equip_placeholder)
				
				if holdSlot == 0 then
					for i,v in ipairs(slotArray) do
						oldEquipSlotA0[v] = v
					end                             
				elseif holdSlot == 1 then
					for i,v in ipairs(slotArray) do
				    	oldEquipSlotA1[v] = v
--						g_Loger:logDebug("oldEquipSlotA1" .."[" ..i .."]" ..oldEquipSlotA1[i])
				    end                             
				    
				elseif holdSlot == 2 then
					for i,v in ipairs(slotArray) do
				    	oldEquipSlotA2[v] = v
				    end                             
				elseif holdSlot == 3 then
					for i,v in ipairs(slotArray) do
				    	oldEquipSlotA3[v] = v
				    end                             
				elseif holdSlot == 4 then
					for i,v in ipairs(slotArray) do
				    	oldEquipSlotA4[v] = v
				    end                             
				elseif holdSlot == 5 then
					for i,v in ipairs(slotArray) do
				    	oldEquipSlotA5[v] = v
				    end                             
				elseif holdSlot == 6 then
				for i,v in ipairs(slotArray) do
			    	oldEquipSlotA6[v] = v
				end                             
				elseif holdSlot == 7 then
					for i,v in ipairs(slotArray) do
				    	oldEquipSlotA7[v] = v
				    end                             
				end 
				
				if type1 == 1 then
					oldEquipSlotSuit = equipSlot
				elseif type1 == 2 then
					oldEquipSlotShangyi = equipSlot
				elseif type1 == 3 then
					oldEquipSlotXiayi = equipSlot
				elseif type1 == 9 then
					oldEquipSlotShoes = equipSlot
				end
			end
			
			--保存当前装备信息
			
			if holdSlot == g_body.hat then
			 
				 curHatState = icon2dressId[substr1 ..substr]
				 hatFlag = 1
				--
--				g_Loger:logDebug("curHatState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--
			elseif holdSlot == g_body.clothes then
			
				 curClothState = icon2dressId[substr1 ..substr]
				 clothesFlag = 1
				--
--				g_Loger:logDebug("curClothState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--
			elseif holdSlot == g_body.trousers then
			
				curTrouState = icon2dressId[substr1 ..substr]
				trousesFlag = 1
				--
--				g_Loger:logDebug("curTrouState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--				
			
			elseif holdSlot == g_body.shoes then
				
				curShoeState = icon2dressId[substr1 ..substr]
				shoesFlag = 1
				
				--
--				g_Loger:logDebug("curShoeState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--
			
			elseif holdSlot == g_body.glasses then
			
				curGlasseState = icon2dressId[substr1 ..substr]
				glassesFlag = 1
				--
--				g_Loger:logDebug("curGlasseState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--
			
			elseif holdSlot == g_body.back then
				
				curBackState = icon2dressId[substr1 ..substr]
				backFlag = 1
				--
--				g_Loger:logDebug("curBackState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--
			
			elseif holdSlot == g_body.hand then
				
				curHandState = icon2dressId[substr1 ..substr]
				handFlag = 1
				--
--				g_Loger:logDebug("curHandState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--
			
			elseif holdSlot == g_body.trinket then
			
				curTrinKetState = icon2dressId[substr1 ..substr]
				trinketFlag = 1
				--
--				g_Loger:logDebug("curTrinKetState..." ..substr1 ..substr .." " ..icon2dressId[substr1 ..substr])
				--
			
			end
			
			if curHatState == nil then
				curHatState = 0
			end
			if curClothState == nil then
				curClothState = 0
			end
			if curTrouState == nil then
				curTrouState = 0
			end
			if curShoeState == nil then
				curShoeState = 0 
			end
			if curGlasseState== nil then
				curGlasseState = 0
			end
			if curBackState == nil then
				curBackState= 0 
			end
			if curHandState == nil then 
				curHandState = 0
			end
			if curTrinKetState == nil then
				curTrinKetState = 0
			end
			--
--			g_Loger:logDebug("curHatState..." ..curHatState)
--			g_Loger:logDebug("curClothState..." ..curClothState)
--			g_Loger:logDebug("curTrouState..." ..curTrouState)
--			g_Loger:logDebug("curShoeState..." ..curShoeState)
--			g_Loger:logDebug("curGlasseState..." ..curGlasseState)
--			g_Loger:logDebug("curBackState..." ..curBackState)
--			g_Loger:logDebug("curHandState..." ..curHandState)
--			g_Loger:logDebug("curTrinKetState..." ..curTrinKetState)
			--
			
			g_mainplayer:setCeateEquip(curHatState, curClothState, curGlasseState, curTrouState, curBackState, curShoeState, curHandState, curTrinKetState)
		else
		
		
		end
	end
	
end

cslotPre = cslot

--加载服饰选择的ICON-------------------------------------------------
function CreatePlayer_LoadIcons()
	local l = 1
	local p = 1
	local j = 1
	local m = 1
	local k = 1
	local n = 1
	local a = 1
	local b = 1
	local c = 1
	local e = 1
	local f = 1
	local g = 1
	
	local rowCount = g_itemtableMgr:getFashionRowCount()
	--测试
--	g_Loger:logDebug("rowCount..."..rowCount)
	------
	for	i=1,rowCount do
		--得到创建角色时的服饰选择索引表
		fashionTable = g_itemtableMgr:getFashionRow(i)
		if fashionTable ~= nil then
			local id    = fashionTable.id
			local sex 	= fashionTable.sex
			local sort1 = fashionTable.sort1
			local sort2 = fashionTable.sort2
			local dressId 	= fashionTable.dressId
			--测试
--			g_Loger:logDebug("id..."..id)
--			g_Loger:logDebug("sex..."..sex)
--			g_Loger:logDebug("sort1..."..sort1)
--			g_Loger:logDebug("sort2..."..sort2)
--			g_Loger:logDebug("dressId..."..dressId)
			---------
			--得到物品表的信息
			itemTable = g_itemtableMgr:getItemRow(dressId)
			if itemTable ~= nil then
				local icon = itemTable.icon
				local imageset = itemTable.imageset
				local subid = itemTable.subID
				
				icon2dressId[imageset ..icon] = dressId
				icon2subID[imageset ..icon] = subid
				--测试
--				g_Loger:logDebug("icon..."..icon)
--				g_Loger:logDebug("imageset..."..imageset)
--				g_Loger:logDebug("subid..."..subid)
--				g_Loger:logDebug("icon2subID..." ..imageset ..icon .." " ..icon2subID[imageset ..icon])
--				g_Loger:logDebug("icon2dressId..." ..imageset ..icon .." " ..icon2dressId[imageset ..icon])
				-------
				--得到服装表的信息
				local subitemTable = g_itemtableMgr:getItemEquipRow(subid)
				if subitemTable ~= nil then
					--性别分类
					if 1 == subitemTable.sex then
						--男便装分类
						if 1 == sort1 then
							--男便装上装
							if 1 == sort2 then
								
								maleShangYiIcons[j] = icon
								maleshangYiImagesets[j] = imageset
								
								--测试
--								g_Loger:logDebug("maleShangYiIcons..." ..j .." " ..maleShangYiIcons[j])
--								g_Loger:logDebug("maleshangYiImagesets..." ..j .." " ..maleshangYiImagesets[j])
								-------
								j = j + 1
							--男便装下衣	
							elseif 2 == sort2 then
								
								maleXiaYiIcons[l] =icon 
								maleXiaYiImagesets[l] = imageset
								
								--测试
--								g_Loger:logDebug("maleXiaYiIcons..." ..l .." " ..maleXiaYiIcons[l])
--								g_Loger:logDebug("maleXiaYiImagesets..." ..l .." " ..maleXiaYiImagesets[l])
								------
								l = l+1
							--男便装套装	
							elseif 3 == sort2 then
							 
								maleSuitIcons[a] =icon 
								maleSuitImagesets[a] = imageset
								
								--测试
--								g_Loger:logDebug("maleSuitIcons..." ..a .." " ..maleSuitIcons[a])
--								g_Loger:logDebug("maleSuitImagesets..." ..a .." " ..maleSuitImagesets[a])
								------
								a = a+1
							end
						--男正装分类
						elseif 2 == sort1 then
						
							malezhengzhuangIcons[b] = icon 
							malezhengzhuangImagesets[b] = imageset
							
							--测试
--							g_Loger:logDebug("malezhengzhuangIcons..." ..b .." " ..malezhengzhuangIcons[b])
--							g_Loger:logDebug("malezhengzhuangImagesets..." ..b .." " ..malezhengzhuangImagesets[b])
							------
							b = b+1
						--男运动装分类
						elseif 3 == sort1 then
							maleYundongIcons[c] =icon 
							maleYundongImagesets[c] = imageset
							
							--测试
--							g_Loger:logDebug("maleYundongIcons..." ..c .." " ..maleYundongIcons[c])
--							g_Loger:logDebug("maleYundongImagesets..." ..c .." " ..maleYundongImagesets[c])
							------
							c = c+1
						
						--男鞋分类
						elseif 4 == sort1 then
							
							maleShoesIcons[k]		= icon
							maleShoesImagesets[k]	= imageset
							
							--测试
--							g_Loger:logDebug("maleShoesIcons..." ..k .." " ..maleShoesIcons[k])
--							g_Loger:logDebug("maleShoesImagesets..." ..k .." " ..maleShoesImagesets[k])
							------
							k = k+1
						end
						
					elseif 2 == subitemTable.sex then
						--女便装分类
						if 1 == sort1 then
							--女便装上衣
							if 1 == sort2 then
								femaleShangYiIcons[m] = icon
								femaleShangYiImagesets[m] = imageset
								
								--测试
--								g_Loger:logDebug("femaleShangYiIcons..." ..m .." " ..femaleShangYiIcons[m])
--								g_Loger:logDebug("femaleShangYiImagesets..." ..m .." " ..femaleShangYiImagesets[m])
								-------
								m = m + 1
							--女便装下衣
							elseif 2 == sort2 then
								femaleXiaYiIcons[p] = icon
								femaleXiaYiImagesets[p] = imageset
								--测试
--								g_Loger:logDebug("femaleXiaYiIcons..." ..p .." " ..femaleXiaYiIcons[p])
--								g_Loger:logDebug("femaleXiaYiImagesets..." ..p .." " ..femaleXiaYiImagesets[p])
								-------
								p = p + 1
							--女便装套装
							elseif 3 == sort2 then
								femaleSuitIcons[e] =icon 
								femaleSuitImagesets[e] = imageset
								
								--测试
--								g_Loger:logDebug("femaleSuitIcons..." ..e .." " ..femaleSuitIcons[e])
--								g_Loger:logDebug("femaleSuitImagesets..." ..e .." " ..femaleSuitImagesets[e])
								------
								e = e+1
							
							end
						--女正装
						elseif 2 == sort1 then
							femalezhengzhuangIcons[f] = icon 
							femalezhengzhuangImagesets[f] = imageset
							
							--测试
--							g_Loger:logDebug("femalezhengzhuangIcons..." ..f .." " ..femalezhengzhuangIcons[f])
--							g_Loger:logDebug("femalezhengzhuangImagesets..." ..f .." " ..femalezhengzhuangImagesets[f])
							------
							f = f+1
						--女运动装
						elseif 3 == sort1 then
							femaleYundongIcons[g] =icon 
							femaleYundongImagesets[g] = imageset
							
							--测试
--							g_Loger:logDebug("femaleYundongIcons..." ..g .." " ..femaleYundongIcons[g])
--							g_Loger:logDebug("femaleYundongImagesets..." ..g .." " ..femaleYundongImagesets[g])
							------
							g = g+1
						--女鞋分类
						elseif 4 == sort1 then
							femaleShoesIcons[n] = icon
							femaleShoesImagesets[n] = imageset
							--测试
--							g_Loger:logDebug("femaleShoesIcons..." ..n .." " ..femaleShoesIcons[n])
--							g_Loger:logDebug("femaleShoesImagesets..." ..n .." " ..femaleShoesImagesets[n])
							-------
							n = n + 1
						end
					end	
				end
			end
		end
	end
	
end

--移除所有物品栏内的ICON
function CreatePlayer_removeAllIcon(slotWinBase)
	for i = 1, 3 do
		if g_WinMgr:getWindow(slotWinBase ..i):getChildCount() > 0 then
			local TempSource = g_WinMgr:getWindow(slotWinBase ..i):getChildAtIdx(0)
			g_WinMgr:getWindow(slotWinBase ..i):removeChildWindow(TempSource)
		end
	end
end
--
function checkElemInArray(list1,list2)
	for i,v in ipairs(list2) do
		if list1 == v then
--			g_Loger:logDebug("true")
			list2[list1] = 0
			return true
			
		else
--			g_Loger:logDebug("false")
			
			
		end
	end
	return false
end

--切换性别时清空装备状态
function clearState()
	curHatState = 0
	curClothState = 0
	curTrouState = 0
	curShoeState = 0
	curGlasseState = 0
	curBackState = 0
	curHandState = 0
	curTrinKetState = 0
--			g_Loger:logDebug("curHatState..." ..curHatState)
--			g_Loger:logDebug("curClothState..." ..curClothState)
--			g_Loger:logDebug("curTrouState..." ..curTrouState)
--			g_Loger:logDebug("curShoeState..." ..curShoeState)
--			g_Loger:logDebug("curGlasseState..." ..curGlasseState)
--			g_Loger:logDebug("curBackState..." ..curBackState)
--			g_Loger:logDebug("curHandState..." ..curHandState)
--			g_Loger:logDebug("curTrinKetState..." ..curTrinKetState)

end

function selectItemFram(args)
	cslot = CEGUI.toWindowEventArgs(args).window
	--测试
--	g_Loger:logDebug("cslot..." ..cslot:getName())
--	g_Loger:logDebug("ChildCount..." ..cslot:getChildCount())
	------
	if cslot:getChildCount() ~= 0 then
		--if cslot ~= cslotPre then
			print("succ")
			citem:setProperty("Image", "set:windows1 image:BackGround14")
			citem:setProperty("MousePassThroughEnabled", "true")
			citem:setProperty("WantsMultiClickEvents", "false")
			citem:setProperty("FrameEnabled", "false")
			citem:setProperty("BackgroundEnabled", "false")
			citem:setProperty("AlwaysOnTop", "true")
			if cslot:getChildAtIdx(0):getChildCount() > 0 then
				iocnWnd = cslot:getChildAtIdx(0):getChildAtIdx(0)
--				g_Loger:logDebug("iocnWnd..." ..iocnWnd:getName())
				iocnWnd:addChildWindow(citem)
			end
		--end
	end
end

--移除选中框
function removeSelectBlock(winNameBase)
	for i = 1,3 do
		local slotwin = g_WinMgr:getWindow(winNameBase ..i)
		if slotwin:getChildCount() > 0 then
			if slotwin:getChildAtIdx(0):getChildCount() > 0 then
				if slotwin:getChildAtIdx(0):getChildAtIdx(0):getChildCount() > 0 then
					local child = slotwin:getChildAtIdx(0):getChildAtIdx(0)
					child:removeChildWindow(child:getChildAtIdx(0))
				end
			end
		end
	end
end

--设置输入框文字长度
function CreatePlayer_initialEditbox()
	--g_editbox = g_WinMgr:getWindow("logoBg/Name/EditPlayerName")
	--g_editbox:subscribeEvent("KeyDown", "CreatePlayer_OnEnterEvent")   --触发Tab事件
	--g_editbox:activate()

	--CEGUI.toEditbox(g_editbox):setMaxTextLength(8)
end

--设置默认皮肤选中按钮状体啊
function CreatePlayer_setDefSkinBtnStat()
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("NormalImage", "set:chuangjian image:NewImage15")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("HoverImage", "set:chuangjian image:NewImage15")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton1"):setProperty("PushedImage", "set:chuangjian image:NewImage15")
	
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("NormalImage", "set:chuangjian image:NewImage8")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("HoverImage", "set:chuangjian image:NewImage12")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton2"):setProperty("PushedImage", "set:chuangjian image:NewImage8")

	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("NormalImage", "set:chuangjian image:NewImage9")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("HoverImage", "set:chuangjian image:NewImage13")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton3"):setProperty("PushedImage", "set:chuangjian image:NewImage9")
	
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("NormalImage", "set:chuangjian image:NewImage10")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("HoverImage", "set:chuangjian image:NewImage14")
	g_WinMgr:getWindow("CreateCharacter/BackL/ComplexionButton4"):setProperty("PushedImage", "set:chuangjian image:NewImage10")
end

--
function reSetSelectTab()
	--CEGUI.toTabControl(g_WinMgr:getWindow("CreateCharacter/TabContrl")):setSelectedTab(0)
end
---------------------------------------------------
--
function CreatePlayer_updateData(page,iconDataPool,imagesetDataPool)
	showIconBuffer = {}
	showImagesetBuffer = {}
	showIconBuffer = getData(iconDataPool, page, MAXSIZE)
	showImagesetBuffer = getData(imagesetDataPool, page, MAXSIZE)
end
---------------------------------------------------
--
function CreatePlayer_shangyiNextPage()
	if shangyiCurPage >= shangyiTotalPage then
		shangyiCurPage = shangyiTotalPage
	else
		shangyiCurPage = shangyiCurPage + 1
	end
--	g_Loger:logError("shangyiCurPage..." ..shangyiCurPage)
	if curSeletSuitState == 0 then
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(shangyiCurPage,maleShangYiIcons,maleshangYiImagesets)
		else
			CreatePlayer_updateData(shangyiCurPage,femaleShangYiIcons,femaleShangYiImagesets)
		end
	else
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(shangyiCurPage,malezhengzhuangIcons,malezhengzhuangImagesets)
		else
			CreatePlayer_updateData(shangyiCurPage,femalezhengzhuangIcons,femalezhengzhuangImagesets)
		end
	end
	CreatePlayer_OnSelectBianzhuang()
	if curSeletSuitState == 0 then
		CreatePlayer_checkSelectSta1()
	else
		CreatePlayer_checkSelectSta4()
	end
end
---------------------------------------------------
--
function CreatePlayer_shangyiprePage()
	if shangyiCurPage <= 1 then
		shangyiCurPage = 1
	else
		shangyiCurPage = shangyiCurPage - 1
	end
--	g_Loger:logError("shangyiCurPage..." ..shangyiCurPage)
	if curSeletSuitState == 0 then
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(shangyiCurPage,maleShangYiIcons,maleshangYiImagesets)
		else
			CreatePlayer_updateData(shangyiCurPage,femaleShangYiIcons,femaleShangYiImagesets)
		end
	else
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(shangyiCurPage,malezhengzhuangIcons,malezhengzhuangImagesets)
		else
			CreatePlayer_updateData(shangyiCurPage,femalezhengzhuangIcons,femalezhengzhuangImagesets)
		end
	end
	CreatePlayer_OnSelectBianzhuang()
	if curSeletSuitState == 0 then
		CreatePlayer_checkSelectSta1()
	else
		CreatePlayer_checkSelectSta4()
	end
end
---------------------------------------------------
--
function CreatePlayer_xiayiNextPage()
	if curSeletSuitState == 0 then
	
		if xiayiCurPage >= xiayiTotalPage then
			xiayiCurPage = xiayiTotalPage
		else
			xiayiCurPage = xiayiCurPage + 1
		end
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(xiayiCurPage,maleXiaYiIcons,maleXiaYiImagesets)
		else
			CreatePlayer_updateData(xiayiCurPage,femaleXiaYiIcons,femaleXiaYiImagesets)
		end
	else
	
		if xiayiCurPage >= shoesTotalPage then
			xiayiCurPage = shoesTotalPage
		else
			xiayiCurPage = xiayiCurPage + 1
		end
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(xiayiCurPage,maleShoesIcons,maleShoesImagesets)
		else
			CreatePlayer_updateData(xiayiCurPage,femaleShoesIcons,femaleShoesImagesets)
		end
	end
	CreatePlayer_OnSelectTrousers()
	if curSeletSuitState == 0 then
		CreatePlayer_checkSelectSta2()
	else
		CreatePlayer_checkSelectSta5()
	end
end
---------------------------------------------------
--
function CreatePlayer_xiayiprePage()
	if xiayiCurPage <= 1 then
		xiayiCurPage = 1
	else
		xiayiCurPage = xiayiCurPage - 1
	end
	if curSeletSuitState == 0 then
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(xiayiCurPage,maleXiaYiIcons,maleXiaYiImagesets)
		else
			CreatePlayer_updateData(xiayiCurPage,femaleXiaYiIcons,femaleXiaYiImagesets)
		end
	else
		if g_rtt:getSex() == 1 then
			CreatePlayer_updateData(xiayiCurPage,maleShoesIcons,maleShoesImagesets)
		else
			CreatePlayer_updateData(xiayiCurPage,femaleShoesIcons,femaleShoesImagesets)
		end
	end
	CreatePlayer_OnSelectTrousers()
	if curSeletSuitState == 0 then
		CreatePlayer_checkSelectSta2()
	else
		CreatePlayer_checkSelectSta5()
	end
end
---------------------------------------------------
--
function CreatePlayer_shoesNextPage()
	if shoesCurPage >= shoesTotalPage then
		shoesCurPage = shoesTotalPage
	else
		shoesCurPage = shoesCurPage + 1
	end
--	g_Loger:logError("shoesCurPage..." ..shoesCurPage)
--	g_Loger:logError("shoesTotalPage..." ..shoesTotalPage)
	if g_rtt:getSex() == 1 then
		CreatePlayer_updateData(shoesCurPage,maleShoesIcons,maleShoesImagesets)
	else
		CreatePlayer_updateData(shoesCurPage,femaleShoesIcons,femaleShoesImagesets)
	end
	CreatePlayer_OnSelectShoes()
	CreatePlayer_checkSelectSta3()
end
---------------------------------------------------
--
function CreatePlayer_shoesprePage()
	if shoesCurPage <= 1 then
		shoesCurPage = 1
	else
		shoesCurPage = shoesCurPage - 1
	end
	if g_rtt:getSex() == 1 then
		CreatePlayer_updateData(shoesCurPage,maleShoesIcons,maleShoesImagesets)
	else
		CreatePlayer_updateData(shoesCurPage,femaleShoesIcons,femaleShoesImagesets)
	end
	CreatePlayer_OnSelectShoes()
	CreatePlayer_checkSelectSta3()
end
-----------------------------------------------------
--
function CreatePlayer_hideShoesSlots(enableFlag)
	if enableFlag == 1 then
		g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("Image","set:chuangjian image:NewImage58")
		g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes2"):setProperty("Image","set:chuangjian image:NewImage57")
		g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes3"):setVisible(false)
		g_WinMgr:getWindow("CreateCharacter/BackL/shoesArrowL"):setVisible(false)
		g_WinMgr:getWindow("CreateCharacter/BackL/shoesArrowR"):setVisible(false)
		for i = 1 ,3 do
			g_WinMgr:getWindow("CreateCharacter/BackR/shoesIcon" ..i):setVisible(false)
		end
	else
		g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes"):setProperty("Image","set:chuangjian image:NewImage55")
		g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes2"):setProperty("Image","set:chuangjian image:NewImage56")
		g_WinMgr:getWindow("CreateCharacter/BackR/SelectClothes3"):setVisible(true)
		g_WinMgr:getWindow("CreateCharacter/BackL/shoesArrowL"):setVisible(true)
		g_WinMgr:getWindow("CreateCharacter/BackL/shoesArrowR"):setVisible(true)
		for i = 1 ,3 do
			g_WinMgr:getWindow("CreateCharacter/BackR/shoesIcon" ..i):setVisible(true)
		end
	end
end
-----------------------------------------------------
--
function CreatePlayer_saveSelectItem1(item)
	mSelected1 = item
end
-----------------------------------------------------
--
function CreatePlayer_saveSelectItem2(item)
	mSelected2 = item
end
-----------------------------------------------------
--
function CreatePlayer_saveSelectItem3(item)
	mSelected3 = item
end
-----------------------------------------------------
--
function CreatePlayer_saveSelectItem4(item)
	mSelected4 = item
end
-----------------------------------------------------
--
function CreatePlayer_saveSelectItem5(item)
	mSelected5 = item
end
------------------------------------------------------
--
function CreatePlayer_checkSelectSta1()
	local cSlotBase = "CreateCharacter/BackR/shangyiIcon"
	for i = 1, 3 do
		local cSlotWin = g_WinMgr:getWindow(cSlotBase ..i)
		if cSlotWin:getChildCount() > 0 then
			if cSlotWin:getChildAtIdx(0):getChildCount() > 0 then
				local child = cSlotWin:getChildAtIdx(0):getChildAtIdx(0)
				local properStr = child:getProperty("Image")
				if mSelected1 ~= "" then
					if properStr == mSelected1 then
						child:addChildWindow(citem)
					end
				end
			end
		end
	end
			--if cSlotWin:getChildAtIdx(0):getChildAtIdx(0):getChildCount() > 0 then
			--	local child = cSlotWin:getChildAtIdx(0):getChildAtIdx(0)
			--	child:removeChildWindow(child:getChildAtIdx(0))
			--end

end
------------------------------------------------------
--
function CreatePlayer_checkSelectSta2()
	local cSlotBase = "CreateCharacter/BackR/xiayiIcon"
	for i = 1, 3 do
		local cSlotWin = g_WinMgr:getWindow(cSlotBase ..i)
		if cSlotWin:getChildCount() > 0 then
			if cSlotWin:getChildAtIdx(0):getChildCount() > 0 then
				local child = cSlotWin:getChildAtIdx(0):getChildAtIdx(0)
				local properStr = child:getProperty("Image")
				if mSelected2 ~= "" then
					if properStr == mSelected2 then
						child:addChildWindow(citem1)
					end
				end
			end
		end
	end
end
------------------------------------------------------
--
function CreatePlayer_checkSelectSta3()
	local cSlotBase = "CreateCharacter/BackR/shoesIcon"
	for i = 1, 3 do
		local cSlotWin = g_WinMgr:getWindow(cSlotBase ..i)
		if cSlotWin:getChildCount() > 0 then
			if cSlotWin:getChildAtIdx(0):getChildCount() > 0 then
				local child = cSlotWin:getChildAtIdx(0):getChildAtIdx(0)
				local properStr = child:getProperty("Image")
				if mSelected3 ~= "" then
					if properStr == mSelected3 then
						child:addChildWindow(citem2)
					end
				end
			end
		end
	end
end
-------------------------------------------------------
--
function CreatePlayer_checkSelectSta4()
	local cSlotBase = "CreateCharacter/BackR/shangyiIcon"
	for i = 1, 3 do
		local cSlotWin = g_WinMgr:getWindow(cSlotBase ..i)
		if cSlotWin:getChildCount() > 0 then
			if cSlotWin:getChildAtIdx(0):getChildCount() > 0 then
				local child = cSlotWin:getChildAtIdx(0):getChildAtIdx(0)
				local properStr = child:getProperty("Image")
				if mSelected4 ~= "" then
					if properStr == mSelected4 then
						child:addChildWindow(citem)
					end
				end
			end
		end
	end
end
-------------------------------------------------------
--
function CreatePlayer_checkSelectSta5()
	local cSlotBase = "CreateCharacter/BackR/xiayiIcon"
	for i = 1, 3 do
		local cSlotWin = g_WinMgr:getWindow(cSlotBase ..i)
		if cSlotWin:getChildCount() > 0 then
			if cSlotWin:getChildAtIdx(0):getChildCount() > 0 then
				local child = cSlotWin:getChildAtIdx(0):getChildAtIdx(0)
				local properStr = child:getProperty("Image")
				if mSelected5 ~= "" then
					if properStr == mSelected5 then
						child:addChildWindow(citem1)
					end
				end
			end
		end
	end
end
-------------------------------------------------------
--
function CreatePlayer_clearSelectSta()
	mSelected1 = ""
	mSelected2 = ""
	mSelected3 = ""
	mSelected4 = ""
	mSelected5 = ""
end
---------------------------------------------------------
--
function CreatePlayer_TabButtonSta()
	if curSeletSuitState == 0 then
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 1"):setProperty("NormalImage", "set:chuangjian image:NewImage36")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 1"):setProperty("HoverImage", "set:chuangjian image:NewImage36")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 1"):setProperty("PushedImage", "set:chuangjian image:NewImage36")

		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 2"):setProperty("NormalImage", "set:chuangjian image:NewImage37")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 2"):setProperty("HoverImage", "set:chuangjian image:NewImage38")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 2"):setProperty("PushedImage", "set:chuangjian image:NewImage39")

	else
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 1"):setProperty("NormalImage", "set:chuangjian image:NewImage34")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 1"):setProperty("HoverImage", "set:chuangjian image:NewImage35")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 1"):setProperty("PushedImage", "set:chuangjian image:NewImage36")

		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 2"):setProperty("NormalImage", "set:chuangjian image:NewImage39")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 2"):setProperty("HoverImage", "set:chuangjian image:NewImage39")
		g_WinMgr:getWindow("CreateCharacter/TabContrl/Tab 2"):setProperty("PushedImage", "set:chuangjian image:NewImage39")
	end
end
----------------------------------------------------------
--
g_WinMgr:getWindow("CreatePlayer/BackR/PreView"):subscribeEvent("MouseButtonDown","CreatePlayer_setMousePushed")
g_WinMgr:getWindow("CreatePlayer/BackR/PreView"):subscribeEvent("MouseButtonUp","CreatePlayer_setMouseUped")
g_WinMgr:getWindow("CreatePlayer/BackR/PreView"):subscribeEvent("MouseMove","CreatePlayer_rotateView")
g_WinMgr:getWindow("CreatePlayer/BackR/PreView"):subscribeEvent("MouseEnter","CreatePlayer_setMouseUped")
--
local viewWinPushed = false
--------------------------------------------------------------
function CreatePlayer_setMousePushed(args)
	viewWinPushed = true
end
-----------------------------------------------------------------
function CreatePlayer_setMouseUped(args)
	viewWinPushed = false
end
---------------------------------------------------------------
--
local lastMousePos = {x,y}
	lastMousePos = g_MouseCur:getPosition()
--
-------------------------------------------------------------------
function CreatePlayer_rotateView(args)
	local curMousePos = {}
	curMousePos = g_MouseCur:getPosition()
	if viewWinPushed == true then
		if curMousePos.x >  lastMousePos.x then
			EQGlobaFunc.RTTRotate(18,2)
		else
			EQGlobaFunc.RTTRotate(-18,2)
		end
	end
	lastMousePos = g_MouseCur:getPosition()
end
---------------------------------------------------------------------
--
local RttAnimaNameTable = {"standby1","standby2","standby3"}
--
function CreatePlayer_getRttAnimaName()
	local table_size = table.getn(RttAnimaNameTable)
	if table_size == 0 then
		return ""
	end
	math.randomseed(os.time())
	math.random(1, table_size)
	math.random(1, table_size)
	local idx = math.random(1, table_size)
	local action_name = RttAnimaNameTable[idx]
	return action_name
end