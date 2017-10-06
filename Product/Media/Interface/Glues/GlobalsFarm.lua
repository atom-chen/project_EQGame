-----------------------------------------
-- 文件说明	：网络消息接收处理
-- 作者		：麦
-- 修改		：
-- 日期		：10/03/25
-----------------------------------------

g_clientGameInited = false
g_testImgset	= "10600001Imageset"
g_testImgName	= "01"
g_rttAvatarWinName = "RTTWindow_0"
g_rttFurnitureWinName = "RTTWindow_1"
g_rttOtherPlayerWinName = "RTTWindow_2"

--最大属性值 暂时不读表
g_max_pro_player_clean  = 100--人物清洁
g_max_pro_player_active = 700--人物活跃
g_max_pro_player_health = 100--人物健康
g_max_pro_house_clean   = 100--房屋清洁
g_max_pro_player_charm	= 100--人物魅力

-- CEGUI相关
g_GuiSystem 	= CEGUI.System:getSingleton()
g_SchemeMgr 	= CEGUI.SchemeManager:getSingleton()
g_WinMgr    	= CEGUI.WindowManager:getSingleton()
g_MouseCur		= CEGUI.MouseCursor:getSingleton()
g_ImagesetMgr 	= CEGUI.ImagesetManager:getSingleton()
g_FontMgr 		= CEGUI.FontManager:getSingleton()

--luaManager接口对象
g_LuaMgr 		= EQGlobaClass.getLuaManager()

--GameEventManager接口对象
g_GameEventMgr 	= EQGlobaClass.getGameEventMgr()

--gameManager接口
g_gamemgr		= EQGlobaClass.getGameMgr()

--游戏状态管理器
g_GameStateMgr = EQGlobaClass.getGameStateMgr()

--objmgr
g_objmgr		= EQGlobaClass.getObjMgr()
--mainPlayer接口
g_mainplayer 	= EQGlobaClass.getMainPlayer()

--sceneMgr接口
g_sceneMgr = EQGlobaClass.getSceneMgr()

g_eqoisMgr = EQGlobaClass.getEQOIS()

--rendertotxture接口
g_rtt			= g_gamemgr:getRTT()

--smallModules接口
g_smallModules = EQGlobaClass.getSmallModules()

--storage仓库
g_StorageUIFlag = false  --全局变量

--场景管理
g_SceneMgr = EQGlobaClass.getEQSceneMgr()

--物品管理
g_itemtableMgr = EQGlobaClass.getItemTableManager()
--
g_housetableMgr = EQGlobaClass.getHouseTableManager()
--
g_logictableMgr = EQGlobaClass.getLogicTableManager()
--
g_tasktableMgr = EQGlobaClass.getTaskTableManager()

--网络消息管理
g_msgmgr = EQGlobaClass.getMsgMgr()
--NPC
g_NPCTable = EQGlobaClass.getNPCTable()
g_npctalk = {}
--交换标识
g_ExchgFlag	= false

--交换物品窗口
g_ExchTargetWin = nil

--删掉物品
g_Btn_Destroy = nil

g_ChatSys		= EQGlobaClass.getChatSystem()
g_FriendSys 	= EQGlobaClass.getFriendSys()
--
----农场表格管理器
g_FarmTableMgr	= EQGlobaClass.getFarmTableMgr()
----农场管理器
g_FarmMgr = EQGlobaClass.getFunnyFarm()

--网络消息
g_req_swap_item 	= EQGlobaClass.req_swap_item()
g_req_move_item 	= EQGlobaClass.req_move_item()
g_req_delete_item	= EQGlobaClass.req_delete_item()
g_req_move_item		= EQGlobaClass.req_move_item()
g_req_use_item		= EQGlobaClass.req_use_item()

g_req_start_edit_house 	= EQGlobaClass.req_start_edit_house()
g_req_end_edit_house 	= EQGlobaClass.req_end_edit_house()

g_req_buy_sys_shop_goods = EQGlobaClass.req_buy_sys_shop_goods()
g_req_open_ui = EQGlobaClass.req_open_ui()
g_req_buy_npc_shop_goods = EQGlobaClass.req_buy_npc_shop_goods()

g_dynamicUIInfo = EQGlobaClass.DynamicUIInfo()
g_uiAnimationInfo = EQGlobaClass.UIAnimationInfo()

g_req_task_list = EQGlobaClass.req_task_list()
g_req_give_up_task = EQGlobaClass.req_give_up_task()

g_req_split_item			= EQGlobaClass.req_split_item()
g_req_resize_player_pack 	= EQGlobaClass.req_resize_player_pack()
--请求铲除地块作物
g_req_hoeing				= EQGlobaClass.req_hoeing()
--
g_req_assart_plot			= EQGlobaClass.req_assart_plot()

g_loadedmesh = false

--dragitem
g_DragItem = EQGlobaClass.getDragItem()
g_chaifenBtnCliked = false

g_PT_WAREHOUSE 	= EQGlobaClass.ItemPack.PT_WAREHOUSE
g_PT_BODY		= EQGlobaClass.ItemPack.PT_BODY
g_PT_TRADE		= EQGlobaClass.ItemPack.PT_TRADEPACK
g_PT_OUTBOX		= EQGlobaClass.ItemPack.PT_OUTBOX
g_PT_INBOX		= EQGlobaClass.ItemPack.PT_INBOX
g_PT_STORAGE    = EQGlobaClass.ItemPack.PT_STORAGE

g_PT_FITMODE	= 99--客户端用

g_CS_NORMAL		= EQGlobaClass.CursorMgr.CS_NORMAL
g_CS_SPLIT_ITEM	= EQGlobaClass.CursorMgr.CS_SPLIT_ITEM
g_CS_HYPERLINK	= EQGlobaClass.CursorMgr.CS_HYPERLINK

--hotkey
g_isKeydown	= EQGlobaFunc.isKeydown
g_KC_LSHIFT = 42
g_KC_RSHIFT	= 54

--邮件
g_mailManager =  EQGlobaClass.getMailManager()

--
g_CursorMgr = EQGlobaClass.getCursorMgr()

-- Itemhint面板容器(hezhr)
g_itemhintVec = EQGlobaClass.createDynamicCtrlVector()
-- npc面板容器(hezhr)
g_npcVec = EQGlobaClass.createDynamicCtrlVector()
-- 任务面板容器(hezhr)
g_taskVec = EQGlobaClass.createDynamicCtrlVector()
-- 图文混排控件解析对象(hezhr)
g_mlbParse = EQGlobaClass.createMultiLineBoxParse()
-- 图文混排控件配置表(hezhr)
g_mlbConfig = {}
--Avatar 调整类型
g_atSex 		= EQGlobaClass.AvatarAdjuster.atSex
g_atHairType 	= EQGlobaClass.AvatarAdjuster.atHairType
g_atHairColor 	= EQGlobaClass.AvatarAdjuster.atHairColor
g_atFace 		= EQGlobaClass.AvatarAdjuster.atFace
g_atSkin 		= EQGlobaClass.AvatarAdjuster.atSkin
g_atBeard 		= EQGlobaClass.AvatarAdjuster.atBeard
--