-----------------------------------------
-- 文件说明	：UI窗口名称统一管理
-- 			  其它文件请不要直接使用硬编码，
-- 			  窗口名称统一在这里取得
-- 作者		：麦
-- 修改		:
-- 日期		：10/03/25
-----------------------------------------

--角色装备栏窗口 + ..1 到 ..8 共8个窗口
--class Player : public GameObj
--{
--public:
--	//角色装备位置
--	enum eBody
--	{
--		BODY_HAT,//hat, 1).       % 帽子
--		BODY_CLOTHES,//	-define(pd_clothes, 2).   % 衣服
--		BODY_TROUSERS,//-define(pd_trousers, 3).  % 裤子
--		BODY_SHOES,//-define(pd_shoes, 4).     % 鞋子
--		BODY_GLASSES,//-define(pd_glasses, 5).   % 眼镜
--		BODY_BACK,//-define(pd_back, 6).      % 背部
--		BODY_HAND,//-define(pd_hand, 7).      % 手持
--		BODY_TRINKET,//-define(pd_trinket, 8).   % 饰品
--	};
g_tmpTalkSuffix = 	  "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
					.. "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
					.. "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
					.. "\t\t\n"
function UIW_getTmpTalkSuff()
	return g_tmpTalkSuffix
end
g_bodyslot = {
				["none"] 	= "PlayerProperty/PropertyTabCtrl/Property/Slot",
				["hat"] 	= "PlayerProperty/PropertyTabCtrl/Property/Slot1",
				["clothes"] = "PlayerProperty/PropertyTabCtrl/Property/Slot2",
				["glasses"] = "PlayerProperty/PropertyTabCtrl/Property/Slot3",
				["trouses"] = "PlayerProperty/PropertyTabCtrl/Property/Slot4",
				["back"] 	= "PlayerProperty/PropertyTabCtrl/Property/Slot5",
				["shoes"] 	= "PlayerProperty/PropertyTabCtrl/Property/Slot6",
				["hand"] 	= "PlayerProperty/PropertyTabCtrl/Property/Slot7",
				["trinket"] = "PlayerProperty/PropertyTabCtrl/Property/Slot8"
			}

g_storageslot = {
					["none"] 	= "CkFrmWnd/BgPanel/Slot"--0/DragContainer"
				}
--页面
g_StoragePage = {
				"CkFrmWnd/BgPanel/Btn_NoncurBag"
				,"CkFrmWnd/BgPanel/Btn_LockBag1"
				,"CkFrmWnd/BgPanel/Btn_LockBag2"
				,"CkFrmWnd/BgPanel/Btn_LockBag3"
				}

--g_StoragePageCur 	= "set: Button6 image: NewImage24"
g_StoragePageOpenNormal = "set: Button6 image: NewImage24"
g_StoragePageOpenHover = "set: Button6 image: NewImage115"
g_StoragePageNotOpen = "set: Button6 image: NewImage25"
g_StoragePageActive	= "set: Button6 image: NewImage116"

g_fitstorageslot =  {
						["none"] = "FitmodeStorageRoot/Slot" -- 0
					}

--交易栏格子
--分左栏 右栏
g_dealPanelSlot = {
					["leftNone"] 		= "CkFrmWnd1/BgPanel/LSlot",
					["rightNone"]	= "CkFrmWnd1/BgPanel/RSlot"
					}
--
--农场仓库
g_farmStorage = {
					["lockText"] = "Root/maichu/zhuyemian/txt4",
					["lockBtn"] = "Root/maichu/zhuyemian/anjian1",
					["lockIcon"] = "Root/maichu/zhuyemian/icon2",
					["saleBtn"] = "Root/maichu/zhuyemian/anjian2"	,
					["tabName"] = "Root/zhuchuangkou/biaoqian",
					["totalPrice"] = "Root/zhuchuangkou/txt3",
					["pageText"] = "Root/zhuchuangkou/fanye3",
					["perPageBtn"] = "Root/zhuchuangkou/fanye1",
					["nextPageBtn"] = "Root/zhuchuangkou/fanye2",
					["sellAllBtn"] = "Root/zhuchuangkou/anjian2",
					["inputBagBtn"] = "Root/zhuchuangkou/anjian1",
					["tab1Btn"] = "Root/zhuchuangkou/biaoqian__auto_TabPane__Buttons__auto_btnTab 1",
					["tab2Btn"] = "Root/zhuchuangkou/biaoqian__auto_TabPane__Buttons__auto_btnTab 2",
					["tab3Btn"] = "Root/zhuchuangkou/biaoqian__auto_TabPane__Buttons__auto_btnTab 3",
					["closeBtn"] = "Root/zhuchuangkou__auto_closebutton__",
					["saleCloseBtn"] =  "Root/maichu__auto_closebutton__",
					["salePerNum"] = "Root/maichu/zhuyemian/xuanze1",
					["saleNextNum"] = "Root/maichu/zhuyemian/xuanze2",
					["saleCancle"] = "Root/maichu/zhuyemian/anjian3",
					["saleNum"] = "Root/maichu/zhuyemian/shuliang",
					["emptyTxt"] = "Root/zhuchuangkou/tishitxt",
					["toShopBtn"] = "Root/zhuchuangkou/anjian11",
					["emptyTipWnd"] = "Root/zhuchuangkou/tishitxt",
					
					["UseDogFoodConfirm"] = "use_dog_food/background/use_dogfood",
					["UseDogFoodCancel"] = "use_dog_food/background/cancel",
					["UseDogFoodCloseBtn"] = "use_dog_food__auto_closebutton__"
					}
--农场商店
g_farmShop = {
					["tabName"] = "shangdian/zhuchuangkou/biaoqian",
					["tab1Btn"] = "shangdian/zhuchuangkou/biaoqian__auto_TabPane__Buttons__auto_btnTab 1",
					["tab2Btn"] = "shangdian/zhuchuangkou/biaoqian__auto_TabPane__Buttons__auto_btnTab 2",
					["tab3Btn"] = "shangdian/zhuchuangkou/biaoqian__auto_TabPane__Buttons__auto_btnTab 3",
					["closeBtn"] = "shangdian/zhuchuangkou__auto_closebutton__",
					["seedCloseBtn"] = "FShopSeedSale__auto_closebutton__",
					["propCloseBtn"] = "FShopPropSale__auto_closebutton__",
					["ornaCloseBtn"] = "FShopOrnamentSale__auto_closebutton__",
					["perPageBtn"] = "shangdian/zhuchuangkou/fanye1",
					["nextPageBtn"] = "shangdian/zhuchuangkou/fanye2",
					["pageText"] = "shangdian/zhuchuangkou/fanye3",
					["seedSureBtn"] = "FShopSeedSale/bg/sure",
					["seedCancleBtn"] = "FShopSeedSale/bg/cancle",
					["seedCount"] = "FShopSeedSale/bg/count",
					["seedTitle"] = "shangdian/zhuchuangkou/biaoqian/Tab 1/beijing",
					["propSureBtn"] = "",
					["propCancleBtn"] = "FShopPropSale/bg/cancle",
					["propCount"] = "FShopPropSale/bg/count",
					["propTitle"] = "shangdian/zhuchuangkou/biaoqian/Tab 1/beijing1",

					["propBuyGold"] = "FShopPropSale/bg/Textbygold" ,
					["propBuyEq"]   = "FShopPropSale/bg/TextbyE" ,
					["propGamePrice"] = "FShopPropSale/bg/goldPrice",
					["propEqPrice"] = "FShopPropSale/bg/EqPrice",
					["propDescript"] = "FShopPropSale/bg/descript",
					["propType"] = "FShopPropSale/bg/typecontent",
					["propImage"] = "FShopPropSale/bg/imageSmall",

					["seedImage"] = "FShopSeedSale/bg/imagesmall",
					["seedPerNum"] = "FShopSeedSale/bg/perNum",
					["seedNextNum"] = "FShopSeedSale/bg/nextNum",
					["propPerNum"] = "FShopPropSale/bg/perNum",
					["propNextNum"] = "FShopPropSale/bg/nextNum",
					["dogFoodNextNum"] = "buy_dog_food/background/add",
					["dogFoodPerNum"] = "buy_dog_food/background/minus",

					["seedEptPick"] = "FShopSeedSale/bg/expectGain",
					["seedTime"] = "FShopSeedSale/bg/time",
					["seedPerPrice"] = "FShopSeedSale/bg/perPrice",
					["seedPrice"] = "FShopSeedSale/bg/price2",
					["seedEptMoney"] = "FShopSeedSale/bg/expectMoney",
					["seedExp"] = "FShopSeedSale/bg/gainExp",
					["seedLevel"] = "FShopSeedSale/bg/level",
					["seedName"] = "FShopSeedSale/bg/name",
					["seedSpecialUse"] = "FShopSeedSale/bg/specielUse",
					["seedInfo"] = "FShopSeedSale/bg/flightTips",

					["ornaName"] = "FShopOrnamentSale/bg/name",
					["ornaEqPrice"] = "FShopOrnamentSale/bg/eq",
					["ornaGamePrice"] = "FShopOrnamentSale/bg/gold",
					["ornaSpecial"] = "FShopOrnamentSale/bg/premiumcontent",
					["ornaExp"] = "FShopOrnamentSale/bg/expunit",
					["ornaPreView"] = "FShopOrnamentSale/bg/Preview",
					["ornaGameGoldBtn"] = "FShopOrnamentSale/bg/bygold",
					["ornaEqGoldBtn"] = "FShopOrnamentSale/bg/byeq",
					["ornaValidatyTime"] = "FShopOrnamentSale/validatyTime",
					["ornaTitle"] = "shangdian/zhuchuangkou/biaoqian/Tab 1/beijing2",
					["ornaCancleBtn"] = "FShopOrnamentSale/bg/cancle",
					["ornaMsg"] = "FShopOrnamentSale/bg/enlighten",
					["ornaImage"] = "FShopOrnamentSale/bg/image",
					["ornaType"]  = "shangdian/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian",
					
					["BuyDogFoodCloseBtn"] = "buy_dog_food__auto_closebutton__",
					["BuyDogFoodCancelBtn"] = "buy_dog_food/background/cancel",
					["BuyDogFoodImg"] = "buy_dog_food/background/icon",
					["BuyDogFoodEqGold"] = "buy_dog_food/background/count",
					["DogFoodName"] = "buy_dog_food/background/name",
					["DogFoodIntro"] = "buy_dog_food/background/intro",
					["DogFoodToStorage"] = "buy_dog_food/background/tostorage",
					["DogFoodToDogHouse"] = "buy_dog_food/background/todoghouse",
					["DogFoodCount"] = "buy_dog_food/background/counts",
					
					["BuyDogCancel"] = "buy_dog/background/cancel",
					["BuyDogCloseBtn"] = "buy_dog__auto_closebutton__",
					["BuyDogImg"] = "buy_dog/background/icon",
					["BuyDogName"] = "buy_dog/background/name",
					["BuyDogGameGold"] = "buy_dog/background/count",
					["BuyDogConfirm"] = "buy_dog/background/done",
					}
--农场设置
g_farmSetting = {
					["closeBtn"] = "MSystemSetting__auto_closebutton__",
					["settingBtn"] = "MSystemSetting/bg/SettingSure",
					["setDefaultBtn"] = "MSystemSetting/bg/setDefualtBtn",
					["waterText"] = "MSystemSetting/bg/TextInput0",
					["weedText"] = "MSystemSetting/bg/TextInput2",
					["wormText"] = "MSystemSetting/bg/TextInput1",
					["putGrassText"] = "MSystemSetting/bg/TextInput3",
					["putPestText"] = "MSystemSetting/bg/TextInput4",
					}
--交易
g_deal = {
					["myGold"] = "DealWnd/Inventory1/Text3",
					["otherGold"] = "DealWnd/Inventory2/Text3",
					["LBlock"] = "DealWnd/Inventory1/Covering1",
					["RBlock"] = "DealWnd/Inventory2/Covering1",
					["sureBtn"] = "DealWnd/Done",
					["cancleBtn"] = "DealWnd/Cancel",
					["leftNone"] 		= "DealWnd/Inventory1/Grille",
					["rightNone"]	= "DealWnd/Inventory2/Grille",
					["myName"] = "DealWnd/Inventory1/Text1",
					["otherName"] = "DealWnd/Inventory2/Text1",
					["closeBtn"] = "DealWnd__auto_closebutton__"
				    }

--房屋买卖
g_houseSales = {	[""] = "",
					["tabName"] = "houseshop/background",
					["tab"] = "houseshop/background__auto_TabPane__Buttons__auto_btnTab ",
					["closeBtn"] = "houseshop__auto_closebutton__",
					["slotName"] = "houseshop/SlotBg/Slot",
					["info"] = "houseshop/SlotBg/Txt_Slot/Txt",
					["nextPage"] = "houseshop/SlotBg/Pagedown",
					["prePage"] = "houseshop/SlotBg/Pageup",
					["nextLevel"] = "houseshop/SlotBg/Level_Down",
					["preLevel"] = "houseshop/SlotBg/Level_Up",
					["levelInfo"] = "houseshop/SlotBg/Level_Txt",
					["pageInfo"] = "houseshop/SlotBg/Page",
					["buyHouseBtn"] = "houseshop/SlotBg/Pay",
					["viewHosueBtn"] = "houseshop/SlotBg/Perview",
					}
--农场个人信息
g_farmPlayerInfo = {
					["image"] = "FarmPlayerInfo/bg/photo",
					["name"] = "FarmPlayerInfo/bg/name",
					["level"] = "FarmPlayerInfo/bg/level",
					["exp"] = "FarmPlayerInfo/bg/exp",
					["gameGold"] = "FarmPlayerInfo/bg/gold",
					["tabName"] = "FarmPlayerInfo/bg/farmMsg",
					["tab1"] = "FarmPlayerInfo/bg/farmMsg__auto_TabPane__Buttons__auto_btnmsg",
					["tab2"] = "FarmPlayerInfo/bg/farmMsg__auto_TabPane__Buttons__auto_btnharvest",
					["tab3"] = "FarmPlayerInfo/bg/farmMsg__auto_TabPane__Buttons__auto_btnconsume",
					["tab4"] = "FarmPlayerInfo/bg/farmMsg__auto_TabPane__Buttons__auto_btnleaveWord",
					["clearMsg"] = "FarmPlayerInfo/bg/clear1",
					["clearLeaveWord"] = "FarmPlayerInfo/bg/clear2",
					["prePage"] = "FarmPlayerInfo/bg/left",
					["nextPage"] = "FarmPlayerInfo/bg/right",
					["pageText"] = "FarmPlayerInfo/bg/pageInfo",
					["closeBtn"] = "FarmPlayerInfo__auto_closebutton__",
					["msgText"] = "FarmPlayerInfo/bg/farmMsg/msg/msgText",
					["leaveWordText"] = "FarmPlayerInfo/bg/farmMsg/leaveWord/",
					["harvestSlot"] = "FarmPlayerInfo/bg/farmMsg/harvest/grille",
					["consumeText"] = "FarmPlayerInfo/bg/farmMsg/consume/consumeText",
					["leaveWord"] = "FarmPlayerInfo/bg/farmMsg/leaveWord/lw",
					["leaveWordBtn"] = "FarmPlayerInfo/bg/farmMsg/leaveWord/liuyan",
}
--房屋信息
g_houseInfo = {
				[""] = "",
				["closeBtn"] = "housedata__auto_closebutton__",
				["tabName"] = "housedata/tab1",
				["name"] = "housedata/tab1/Tab 1/txt1a",
				["type"] = "housedata/tab1/Tab 1/txt4a",
				["level"] = "housedata/tab1/Tab 1/txt3a",
				["buyTime"] = "housedata/tab1/Tab 1/txt6a",
				["clean"] = "housedata/tab1/Tab 1/txt7a",
				["price"] = "housedata/tab1/Tab 1/txt5a",
				["contain"] = "housedata/tab1/Tab 1/txt2a",
				["welcomeTxt"] = "housedata/tab1/Tab 1/txt8a",
				["sureBtn"] = "housedata/queding",
				["cancleBtn"] = "housedata/quxiao",
				["accessY"] = "housedata/tab1/Tab 2/xuanxiang1",
				["accessN"] = "housedata/tab1/Tab 2/xuanxiang2",
				["accessYTxt"] = "housedata/tab1/Tab 2/shuomingtxt1",
				["accessNTxt"] = "housedata/tab1/Tab 2/shuomingtxt2",
				["useFur"] = "housedata/tab1/Tab 2/xuanxiang3",
				["useYTxt"] = "housedata/tab1/Tab 2/shuomingtxt3",
				["useNTxt"] = "housedata/tab1/Tab 2/shuomingtxt4",
				["clear"] = "housedata/tab1/Tab 3/qingkongjilu",
				["log"] = "housedata/tab1/Tab 3/txt1",
				["sendLogComboBox"] = "housedata/tab1/Tab 5/leixing",
				["recLogComboBox"] = "housedata/tab1/Tab 6/leixing",
				["sendGiftLog"] = "housedata/tab1/Tab 5/leixing2",
				["recGiftLog"] = "housedata/tab1/Tab 6/leixing2",
				["prePageSendLog"] = "housedata/tab1/Tab 5/fanye1",
				["nextPageSendLog"] = "housedata/tab1/Tab 5/fanye2",
				["sendLogPageText"] = "housedata/tab1/Tab 5/fanye3",
				["recLogPageText"] = "housedata/tab1/Tab 6/fanye3",
				["prePageRecLog"] = "housedata/tab1/Tab 6/fanye1",
				["nextPageRecLog"] = "housedata/tab1/Tab 6/fanye2",
				["clearSendLog"] = "housedata/tab1/Tab 5/qingkongjilu",
				["clearRecLog"] = "housedata/tab1/Tab 6/qingkongjilu",
				["sendLogTip"] = "housedata/tab1/Tab 5/leixing1",
				["recLogTip"] = "housedata/tab1/Tab 6/leixing1",
				["root"] = "housedata",
}
--娱乐选择界面
g_funnyPanel = {
			["btnBaseName"] = "zhuchuangkou1/icon",
			["textBaseName"] = "zhuchuangkou1/txt",
			["closeBtn"] = "zhuchuangkou__auto_closebutton__",
			["nextPage"] = "zhuchuangkou1/fanye2",
			["prePage"] = "zhuchuangkou1/fanye1",
			["pageText"] = "zhuchuangkou1/fanye3"
			}
--打工
g_works = {
			["slotName"] = "work/zhuchuangkou/icon",
			["prePage"] = "work/zhuchuangkou/fanye1",
			["nextPage"] = "work/zhuchuangkou/fanye2",
			["pageText"] = "work/zhuchuangkou/fanye3",
			["closeBtn"] = "work__auto_closebutton__",
			["sureBtn"] = "",
			["cancleBtn"] = "",
			["imageWindow"] = "Works/full/play",
			["textWindow"] = "Works/full/list/txt",
			["bgWindow"] = "Works/full",
			["textBack"] = "Works/full/list",
			["textFrame"] = "Works/full/listbiankuang",
			["imageFrame"] = "Works/full/biankuang",

		}

--角色装备位置表
g_body = {
			["hat"]		= 0, --0x0001,
			["clothes"]	= 1,--0x0002,
			["trousers"]	= 3,--0x0008,
			["shoes"]		= 5,--0x0020,
			["glasses"]	= 2,--0x0004,
			["back"]		= 4,--0x0010,
			["hand"]		= 6,--0x0040,
			["trinket"]	= 7,--0x0080,
		 }

--窗体ID
g_uitype = {
				["sysshop"] = 1,
				["npcshop"]	= 2,
				["uit_farmdepot"] = 3,
				["uit_depot"] = 4,
				["uit_bag"] = 5,
			}

--服务返回的打开窗口类型
g_openUiType =
	{
	["sysshop"] = 1,
	["npcshop"] = 2,
	["farmdepot"] = 3,
	["depot"] = 4,
	["bag"] = 5,
	["housetransaction"] = 6
	}

--
--农场鼠标状态枚举
farmCurStaEnum = {
				  ["CS_FARM_NORMAL"] = 0,
				  ["CS_FARM_MOVE"] = 1,
				  ["CS_FARM_TILL"] = 2,
                  ["CS_FARM_WATER"] = 3,
                  ["CS_FARM_WEED"] = 4,
                  ["CS_FARM_PICKBUG"] = 5,
                  ["CS_FARM_HARVEST"] = 6,
                  ["CS_FARM_ONEKEYHARVEST"] = 7,
				  ["CS_FARM_SEEDING"] = 8,
				  ["CS_FARM_PUTBUG"] = 9,
				  ["CS_FARM_PUTGRASS"] = 10,
				  ["CS_FARM_STEAL"] = 11,
                 }

--农场播种界面
g_farmSeedBag = {
					["toFarmStorage"] = "farm_seedbag/tostore",
					["slotName"] 	  = "farm_seedbag/seed",
					["preBtn"] 	      = "farm_seedbag/leftbutton",
					["nextBtn"] 	  = "farm_seedbag/rightbutton",
					["info"]		  = "farm_seedbag/nothing",
					["toShopBtn"]	  = "farm_seedbag/nothing/toshop",
				}

--邮件界面
g_mail = {

			["mailBox"] = "mail/open_mailbox1",
			["tabName"] = "mail/open_mailbox/open_mail",
			["mail"] = "mail/open_mailbox2",
			["mailSlot"] = "mail/open_mailbox/Tab 1/mail",
			["prePage"] = "mail/open_mailbox/Tab 1/pageup",
			["nextPage"] = "mail/open_mailbox/Tab 1/pagedown",
			["pageText"] = "mail/open_mailbox/Tab 1/pagenumber",
			["closeBtn"] = "mail/open_mailbox1__auto_closebutton__",
			["closeBtn2"] = "mail/open_mailbox2__auto_closebutton__",
			["addressee"] = "mail/open_mailbox/Tab 2/receivemanedit",
			["recSubject"] = "mail/open_mailbox2/read_mail/title_name",
			["sendSubject"] = "mail/open_mailbox/Tab 2/titleedit",
			["sender"] = "mail/open_mailbox2/read_mail/fromwho_name",
			["sendSlot"] = "mail/open_mailbox/Tab 2/senditem",
			["recSlot"] = "mail/open_mailbox2/read_mail/mailitem2",
			["sendContent"] = "mail/open_mailbox/Tab 2/contentsedit",
			["recContent"] = "mail/open_mailbox2/read_mail/mail_contents",
			["replyBtn"] = "mail/open_mailbox2/read_mail/remail",
			["sendBtn"] = "mail/open_mailbox/Tab 2/sendbutton",
			["deleteBtn"] = "mail/open_mailbox2/read_mail/delete",
			["returnBtn"] = "mail/open_mailbox2/read_mail/sendback",
			["titleSlot"] = "mail/open_mailbox/Tab 1/contents",
			["waringTextWin"] = "BulletinBoard/Bg/Text",
			["waringCloseBtn"] = "BulletinBoard__auto_closebutton__",
			["waringSureBtn"] = "BulletinBoard/Bg/Sure",
			["tab1Btn"] = "mail/open_mailbox/open_mail__auto_TabPane__Buttons__auto_btnTab 1",

		}

--家政界面
g_HomeService = {
			["text"] = "Root/full/list/txt",
			["image"] = "Root/full/play",
			["bgWindow"] = "Root/full",
			["tipCloseBtn"] = "check__auto_closebutton__",
			["tipSureBtn"] = "check/check1/queren",
			["tipCancleBtn"] = "check/check1/quxiao",
			["tipText"] = "check/check1/checktxt",
			["imageFrame"] = "Root/full/biankuang",
			["textFrame"] = "Root/full/listbiankuang",
			["textBack"]  = "Root/full/list"
				}
--电话本界面
g_addressBook = {
			["slotName"] = "addressbook/zhuchuangkou/dianhuahaoma",
			["closeBtn"] = "addressbook__auto_closebutton__",
			["leaveBtn"] = "addressbook/zhuchuangkou/likai",
			}
--仓库界面
g_storageUIName = {
			["root"] = "Root/store",
			["slotBaseName"] = "Root/store/Background/grille",
			["closeBtn"] = "Root/store__auto_closebutton__",
			["saveMoneyBtn"] = "Root/store/Background/deposit",
			["getMoneyBtn"] = "Root/store/Background/withdraw",
			["lockBtn"] = "Root/store/Background/lock",
			["moneyTextBox"] = "Root/store/Background/Money",
			["inputBoxSureBtn"] = "store_money/put_in",
			["inputBoxCancleBtn"] = "store_money/take_out",
			["inputBoxCloseBtn"] = "store_money__auto_closebutton__",
			["moneyInputEditBox"] = "store_money/editbox/edit",
				}
				
--红土地升级面板
g_upgradeLandUIName = {
			["image"] = "farmland_upgrade/groundimg/img",
			["tips"] = "farmland_upgrade/groundimg/words",
			["condition"] = "farmland_upgrade/groundimg/msg",
			["sureBtn"] = "farmland_upgrade/groundimg/button",
			["closeBtn"] = "farmland_upgrade__auto_closebutton__",
					}

--选择制作礼物面板
g_giftUIName_choose = {
			["diyBtn"] = "lwchuangkou/chuangkou1/anjian1",
			["mailBtn"] = "lwchuangkou/chuangkou1/anjian2",
			["cancleBtn"] = "lwchuangkou/chuangkou1/likai",
			}
			
--手工制作礼盒面板
g_giftUIName_diy = {
			["closeBtn"] = "lwchuangkou2__auto_closebutton__",
			["anonymousBtn"] = "lwchuangkou2/chuangkou2/niming",
			["greetings"] = "lwchuangkou2/chuangkou2/zengyan2",
			["giftName"] = "lwchuangkou2/chuangkou2/mingzishuru",
			["sureBtn"] = "lwchuangkou2/chuangkou2/queding",
			["cancleBtn"] = "lwchuangkou2/chuangkou2/queding1",
			["itemSlot"] = "lwchuangkou2/chuangkou2/liwugezi2",
			["boxSlot"] = "lwchuangkou2/chuangkou2/liwuhezi1",
			["cardSlot"] = "lwchuangkou2/chuangkou2/liwuka1",		
}	
--礼盒选择界面
g_giftBoxUIName = {
			["slotBase"] = "liwuhe/biaoqianmianban/icon",
			["preBtn"] = "liwuhe/biaoqianmianban/zuoyou1",
			["nextBtn"] = "liwuhe/biaoqianmianban/zuoyou11",
			["showSlot"] = "liwuhe/biaoqianmianban/daicon1",
			["name"] = "liwuhe/biaoqianmianban/mingzitxt",
			["desc"] = "liwuhe/biaoqianmianban/jieshaotxt",
			["sureBtn"] = "liwuhe/biaoqianmianban/querenanjian",
			["price"] = "liwuhe/biaoqianmianban/jiaqiantxt",
			["tab1"] = "liwuhe/biaoqian__auto_TabPane__Buttons__auto_btnTab 1",
			["tab2"] = "liwuhe/biaoqian__auto_TabPane__Buttons__auto_btnTab 2",
			["tab3"] = "liwuhe/biaoqian__auto_TabPane__Buttons__auto_btnTab 3",
			["tab4"] = "liwuhe/biaoqian__auto_TabPane__Buttons__auto_btnTab 4",
			["tabName"] = "liwuhe/biaoqian",
			["closeBtn"] = "liwuhe/biaoqianmianban/liwuhechuangkou__auto_closebutton__",
					}
--礼卡选择界面
g_giftCardUIName = {
			["slotBase"] = "liwuka/biaoqianmianban/icon",
			["preBtn"] = "liwuka/biaoqianmianban/zuoyou1",
			["nextBtn"] = "liwuka/biaoqianmianban/zuoyou11",
			["showSlot"] = "liwuka/biaoqianmianban/daicon1",
			["name"] = "liwuka/biaoqianmianban/mingzitxt",
			["desc"] = "liwuka/biaoqianmianban/jieshaotxt",
			["sureBtn"] = "liwuka/biaoqianmianban/querenanjian",
			["price"] = "liwuka/biaoqianmianban/jiaqiantxt",
			["tab1"] = "liwuka/biaoqian__auto_TabPane__Buttons__auto_btnTab 1",
			["tab2"] = "liwuka/biaoqian__auto_TabPane__Buttons__auto_btnTab 2",
			["tab3"] = "liwuka/biaoqian__auto_TabPane__Buttons__auto_btnTab 3",
			["tab4"] = "liwuka/biaoqian__auto_TabPane__Buttons__auto_btnTab 4",
			["tabName"] = "liwuka/biaoqian",
			["closeBtn"] = "liwuka/biaoqianmianban/liwukachuangkou__auto_closebutton__",
					}
--快递形式制作礼物
g_giftUIName_mail = {
			["reciverAcc"] = "lwchuangkou3/chuangkou2/mingzishuru",
			["searchBtn"] = "lwchuangkou3/chuangkou2/tianjiahaoyou",
			["itemSlot"] = "lwchuangkou3/chuangkou2/liwugezi2",
			["cardSlot"] = "lwchuangkou3/chuangkou2/liwuka1",
			["greetings"] = "lwchuangkou3/chuangkou2/zengyan2",
			["date"] = "lwchuangkou3/chuangkou2/shijian2",
			["sureBtn"] = "lwchuangkou3/chuangkou2/queding",
			["cancleBtn"] = "lwchuangkou3/chuangkou2/queding1",
			["closeBtn"] = "lwchuangkou3__auto_closebutton__",
			
			["year"] = "lwchuangkou3/chuangkou2/nian",
			["month"] = "lwchuangkou3/chuangkou2/yue",
			["day"] = "lwchuangkou3/chuangkou2/ri",
			
}
--放置回收礼物界面
g_putGiftUIName = {
			["reciverName"] = "shouliwu/mingzi",
			["senderName"] = "shouliwu/mingzi2",
			["itemSlot"] = "shouliwu/liwubeijing/gezi",
			["greetings"] = "shouliwu/mingzi1",
			["putBtn"] = "shouliwu/anjian1",
			["takeBackBtn"] = "shouliwu/anjian2",
			["cancleBtn"] = "shouliwu/anjian3",
			["hyperLinker"] = "shouliwu/mingzi21",
}
--礼物界面(当前查看的)
g_DIYGiftUIName = {
			["greetings"] = "shouliwu1/mingzi1",
			["senderName"] = "shouliwu1/mingzi2",
			["hyperLinker"] = "shouliwu1/mingzi21",
			["itemSlot"] = "shouliwu1/liwubeijing/gezi",
			["pickUpBtn"] = "shouliwu1/anjian1",
			["cancleBtn"] = "shouliwu1/anjian2",
				}
--礼物仓库			
g_GiftStorageUI = 
				{
				["slotBaseName"] = "CkFrmWnd1/BgPanel/Slot",
				["page1"] = "CkFrmWnd1/BgPanel/Btn_NoncurBag",
				["page2"] = "CkFrmWnd1/BgPanel/Btn_LockBag1",
				["page3"] = "CkFrmWnd1/BgPanel/Btn_LockBag2",
				["page4"] = "CkFrmWnd1/BgPanel/Btn_LockBag3",
				["closeBtn"] = "CkFrmWnd1__auto_closebutton__",
					}
--记录显示礼物面板					
g_MailGiftUIName = 
				{
				["greetings"] = "shouliwu2/mingzi1",
				["senderName"] = "shouliwu2/mingzi2",
				["hyperLinker"] = "shouliwu2/mingzi21",
				["itemSlot"] = "shouliwu2/liwubeijing/gezi",
				["reply"] = "shouliwu2/anjian1",
				["pickUpBtn"] = "shouliwu2/anjian21",
				["cancleBtn"] = "shouliwu2/anjian2",
				 }
--界面互斥
g_GuideUINames =
	{
	["root"] = "Goto/Windows"
	}
--人物动作
g_ActionUINames =
	{
	["root"] = "ActionWnd"
	}
--人物属性小面版
g_PlayerAttrUINames =
	{
	["root"] = "PlayerAttribute_Root/Player/"
	}
--背包
g_PackUINames =
	{
	["root"] = "CkFrmWnd"
	}
--人物属性详细面版
g_PlayerPropertyUINames =
	{
	["root"] = "PlayerProperty"
	}
--商场
g_MarketUINames =
	{
	["root"] = "Marketroot/Market"
	}
--好友列表
g_FriendListUINames =
	{
	["root"] = "freindlist"
	}
--系统设置
g_SysConfigUINames =
	{
	["root"] = "Systemconfig/Systemconfig"
	}
--娱乐
g_FunnyUINames =
	{
	["root"] = "zhuchuangkou"
	}
--小地图
g_SmallMapUINames =
	{
	["root"] = "map"
	}
--照相机
g_CameraUINames = 
	{
	["root"] = "zhaoxiangji"
	}
--系统公告
g_BroadUINames =
	{
	["root"] = "broadcast"
	}
--礼物制作选择
g_GiftSelUINames =
	{
	["root"] = "lwchuangkou1"
	}
--电话薄
g_AddBookUINames =
	{
	["root"] = "addressbook"
	}
--聊天表情栏
g_ChatExpUINames =
	{
	["root"] = "ChatExpressionUI"
	}
--npc
g_NpcDlgUINames =
	{
	["root"] = "npcDlg"
	}
--任务
g_TaskUINames =
	{
	["root"] = "Taskboardroot/Taskboard"
	}
--NPC商场
g_NPCMarketUINames =
	{
	["root"] = "Npcshoproot/Npcshop"
	}
--房屋买卖
g_HouseSalesUINames =
	{
	["root"] = "houseshop"
	}
--摆家具，商城模式
g_FitModeMktUINames =
	{
	["root"] = "up1Root"
	}
--美容
g_BeautyUINames =
	{
	["root"] = "Root/beauty"
	}
--乐园和其他系统的物品交互
g_ItemInteractUIName = 
{
	["root"] = "ItemInteractWnd",
	["atuostaicimage"] = "ItemInteractWnd__auto_staticimage__",
	["slotBaseName"] = "ItemInteractWnd/BgPanel/Slot",
	["closeBtn"] = "ItemInteractWnd__auto_closebutton__",
	["prePageBtn"] = "ItemInteractWnd/BgPanel/fanye3",
	["nextPageBtn"] = "ItemInteractWnd/BgPanel/fanye4",
	["pageTxtbox"] = "ItemInteractWnd/BgPanel/fanye5",
	["moneyBox"] = "ItemInteractWnd/BgPanel/Txt_GoldBox",
	["moneyImgBtn"] = "ItemInteractWnd/BgPanel/qian"
}
--离线消息
g_OfflineMsgUIName = 
{
	["root"] = "friend_list_sys",
	["closeBtn"] = "friend_list_sys__auto_closebutton__",
	["tabBtn1"] = "friend_list_sys/control/Tab 1",
	["tabBtn2"] ="friend_list_sys/control/Tab 2",
	["msgPanel1"] = "friend_list_sys/control/Tab 1/msg1/sys_msg",
	["msgPanel2"] = "friend_list_sys/control/Tab 1/msg1/sys_msg1",
	["confirmBtn1"] = "friend_list_sys/control/Tab 1/msg1/agree1",
	["confirmBtn2"] = "friend_list_sys/control/Tab 1/msg1/agree11",
	["cancelBtn1"] = "friend_list_sys/control/Tab 1/msg1/refuse1",
	["cancelBtn2"] = "friend_list_sys/control/Tab 1/msg1/refuse11",
	["checkBox1"] = "friend_list_sys/control/Tab 1/msg1/receive_again",
	["checkBox2"] = "friend_list_sys/control/Tab 1/msg1/receive_again1",
	["prePageBtn"] = "friend_list_sys/control/Tab 1/pg_up",
	["nextPageBtn"] = "friend_list_sys/control/Tab 1/pg_down",
	["pageBox"] = "friend_list_sys/control/Tab 1/pages",
	["ignoreAllOption"] = "friend_list_sys/control/Tab 2/allow",
	["notifyAllOption"] = "friend_list_sys/control/Tab 2/refuse"
}
--打工确定
g_workConfirmUINames =
{
	["root"] = "dagongtishi",
	["labelNeed"] = "dagongtishi/neirong4",
	["labelExpend"] = "dagongtishi/neirong5",
	["labelGain"] = "dagongtishi/neirong6",
	["okBtn"] = "dagongtishi/queding",
	["cancelBtn"] = "dagongtishi/queding1"
}

g_diseaseSpecialEventUINames =
{
	["root"] = "DiseaseSpecialEvent/root",
	["image"] = "DiseaseSpecialEvent/root/event"
}
--打工背景
g_housesweepUINames =
	{
	["root"] = "Root"
	}