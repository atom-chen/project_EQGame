/*********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 脚本接口
**********************************************************************/
#ifndef SCRIPTFUNCTION_H
#define SCRIPTFUNCTION_H
#include "NetPacket.h"
#include "LuaManager.h"
#include "EQLog.h"
#include "CEGUI.h"

struct UIAnimationInfo;
/**
*/
enum eScriptExcept
{
	SE_SHOW_USE_EQLOG,
	SE_SHOW_MESSBOX,
	SE_SHOW_ON_UI,
	SE_WRITE_IN_LOG,
};

/**
*/
class ScriptExcept{
public:	
	static ScriptExcept& getSingleton()
	{
		static ScriptExcept singleton;
		return singleton;
	}
	
	void scriptExcept(int type,std::string except)
	{
		switch(type)
		{
		case SE_SHOW_USE_EQLOG:
			LOGMSG(except.c_str());
			break;
		case SE_SHOW_MESSBOX:
			MessageBoxA(NULL,except.c_str(),"脚本信息",MB_OK);
			break;
		case SE_SHOW_ON_UI:
			CLuaManager::getSingleton().CallFunction("ShowScriptExcept",except);
			break;
		}
	}
private:
	ScriptExcept(){}
	~ScriptExcept(){}
	
};
#define sScriptExcept ScriptExcept::getSingleton()
/**
@brief
登陆请求
@param account
帐号
@param serverIP
服务器IP
@param serverPort
port
*/
bool loginRequest(std::string account, 
				  std::string pw,
				  std::string serverIP, 
				  int serverPort);

/**
@brief
创建角色请求
@param account
@account
@param username
username
*/
bool creatPlayerRequest(std::string account, 
						std::string username);

/**
@brief
载入场景请示
@param
@param
*/
bool loadScene();

/**
@brief
角色请示
@param
@param
*/
bool rolesRequest();

/**
@brief
版本验证请示
@param 帐号
@param IP
@param 端口
*/
bool versionRequest(std::string account,
					std::string pw,
					std::string serverIP, 
					int serverPort);

/**
@brief
设置窗体显示文字，接收从LUA传来的字符串
@param 窗体
@param 文字
*/
void setText(const char* szWindows, 
				 const char* szText);

/**
@brief
聊天框内容显示出来到上面
@param
@param
*/
void setChatText(const char* dest,
				 const char* src);

/**
@brief
RTT激活
@param active 是否激活
@param
*/
void rttSetActive(const char* name, bool active);

/**
@brief
启动RTT
@param x , y, z, 放在一个比较远的地方
@param
*/
void startRTT(int x, int y, int z);

/**
@brief
RTT宽高比，
@param 宽
@param 高
*/
void rttSetAspectRatio(int w, int h);

/**
@brief
RTTLookAt
@param x ,y ,z
@param
*/
void rttLookAt(int x, int y, int z);

/**
@brief
RTT旋转
@param degree 旋转角度
@param
*/
void rttRotate(int degree,int axes);

/**重置RTT的朝向和缩放
*/
void rttReset(void);

/**
	RTT缩放
*/
void rttScale(float scaleX,float scaleY,float scaleZ);

/**RTT缩放
*/
void rttZoom(float scale);

/**
@brief
装上
@param slot
@param 装备显示ID
@param 占据slot
*/
void equipOn(uint slot, 
			 uint equipDisplayID,
			 uint holdSlot);

/**
@brief
脱掉
@param slot
@param
*/
void takeOff(uint slot);

/**
@brief
脱掉全部
*/
void rttTakeOffAll();

/**
@brief
同步主玩家装备
*/
void rttSync();

/**
@brief
rttFurniture
@param itemid 物品ID
@param
*/
void rttFurniture(uint itemid);

/**
@brief
RTT旋转
@param degree 旋转角度
@param
*/
void rttFurniRotate(int degree);

/**
@brief
RTT宽高比，
@param 宽
@param 高
*/
void rttFurniSetAspectRatio(int w, int h);

/**重置RTT的朝向和缩放
*/
void rttFurniReset(void);

/**RTT缩放
*/
void rttFurniZoom(float scale);

/**
@brief
退出游戏
@param
@param
*/
void exitApp();

/**
@brief
显示二楼
@param
@param
*/
void houseUp();

/**
@brief
显示一楼
@param
@param
*/
void houseDown();

/**
@brief
获取房屋显示模式
*/
int getHouseMode();

/**
@brief
切换房屋显示模式
@param sel 选择模式
@param
*/
void changeHouseMode(int sel);

/**
@brief
切换格子显示模式
@param sel 选择模式
@param
*/
void changeGridMode(int sel);

/**
@brief
切换家装模式和正常游戏模式
@param sel 选择模式
@param
*/
void changeFitmentMode(int sel);

/**
@brief
编辑模式下拖拉家具显示
@param id 家具ID
@param
*/
void onMouseFitmentDro(int id);

/**
@brief
编辑模式下拖拉家具旋转
@param degree 旋转角度
@param
*/
void onMouseFitmentRotate(float degree);

/**
@brief
编辑模式下拿起家具
@param
*/
void onMouseFitmentPickup();

/**
@brief
编辑模式下放下家具
@param
*/
void onMouseFitmentPutdown();

/**
@brief
编辑模式下家具拾取状态
@param
*/
int getPickState();


/**
@brief
装载MESH
@param MESH 文件名
@param 是否发射阴影
*/
void loadStaticMesh(std::string meshfile, 
					bool castShadows);

/**
XiaoMing Add
@brief
装载MESH
@param MESH 文件名
@param 是否发射阴影
@param x, y, z MESH位置
@param rotationX, rotationY, rotationZ MESH旋转值
*/
// void loadStaticMesh_Outdoor(std::string meshfile, 
// 					bool castShadows,
// 					float x ,
// 					float y ,
// 					float z ,
// 					float rotationX ,
// 					float rotationY ,
// 					float rotationZ ,
// 					float rotationW );
/**
XiaoMing Add
@brief
缩放MESH
*/
void outdoorMeshScale(std::string nodeName, 
				float x, 
				float y, 
				float z);

/**
@brief
调整相机LOOKAT到主角色
@param
@param
*/
void lookAtMainPlayer();

/**
@brief
主角色播放动作
@param ani 动作名称
@param
*/
void playAnimation(std::string ani, std::string target);
void addAnimation(std::string ani,std::string action,bool loop);
void clear_allAnimaList();
void clearLoopAnima();
void addAnimation_loop(std::string ani,std::string action,bool loop);
void setAnimaLoopNum(int loopnum);
void check_Animation_List();
std::string getCurrentAnimation();

class NumberPrinter;
/**
*/
bool sendNetPacket(/*NumberPrinter&*/ INetPacket& packet);

/**
*/
void avatarAdjPrev(int slot);

/**
*/
void avatarAdjNext(int slot);
/**
得到当前编辑中的家具ID
*/
int getCurrFurID();

/**得到环形菜单ID
*/
uint getLoopMenuID();
std::string getLoopMenuDelTime();	//获取家具时效 (2011/9/22 add by hezhr)
uint getInteractingFitmentID();
/**
*/
void selLoopMenu(int menu);

/** 目前是白天或者黑夜
*/
bool getDayNight();

/** 改变为白天或者黑夜
*/
void changeDayNight(bool daynight, bool immediately);
/**恢复黑白正常交替
*/
void revertDayNight();

/** 白天、黑夜替换
*/
void switchDayNight(bool immediately);
/**
*/
int posToSlot(int pos);

/** 
*/
void setSysMsgText(std::string );

/** 
*/
std::string UIntToStringBitsetScript(uint);

/**	
*/
uint getTakeOffSlots(uint);

/*
*/
void reqPlayerBasicData();

/**
*/
void setTooltipProperty(const std::string szWindow, const std::string szText);

/**
*/
int getMultiLineEditboxLineCount(const char* winname);

/**
*/
void createListboxTextItem(const char* listname, const char* title, int id);
/**
*/
void setListboxItemSelect(std::string listbox_name,std::string itm_txt);

/**
*/
bool locateListboxTextItem(std::string listname, int id);

/**
	创建ComboBox Item
*/
void createComboBoxTextItem(const char* comboBoxName, const char* text, int id,std::string txtCol);
/** 
*/
bool isKeyDown(int keycode);
void setKeyFlag(int keycode, bool flag);
void playAnimationByName(std::string winName , UIAnimationInfo&info);
bool uiAnimationIsPlaying(std::string winName);

/**
*/
std::string UTF8ToGB2312Bylua( const char * str );

/**
*/
std::string GB2312ToUTF8Bylua( const char * str );

/**
*/
void showMsgPaoPao(int index,int x,int y, int z, const char* content, const char* imageset, const char* icon);

/**
*/
void showDexpDynamicUI(int index,int exp,int x_3d,int y_3d,int z_3d);
/**
*/
int getUTF8Len(const char * str);
/**
*/
std::string returnValidString(std::string fullStr);
/**
*/
std::string stringFilter(std::string txt,int mode);

DWORD getTickCount();

void setCreateMsgBoxTime();
//
class EQCamera;
EQCamera* getMainCamera();

void openFarmExe();
bool runExe(const char *lpOperation, const char *lpFile, const char *lpParameters, const char *lpDirectory, int nShowCmd);

int createLuaTimer(int delay, int repeatCount);
void startLuaTimer(int flag);
void stopLuaTimer(int flag);
bool isLuaTimerRunning(int flag);
int getLuaTimerCurrentCount(int flag);

/**
*/
void ExitClient(void);

void OpenURL(std::string url);

// 获取当前的模态对话窗体(2011/6/20 add by hezhr)
std::string getModalWindowName(void);

// 设置窗体淡入浅出(2011/7/7 add by hezhr)
void setWindowFade(std::string winname, int delayTime, int fadeTime, bool autoDel);

// 发包
void reqFlowerLogs();
void reqEggLogs();
void reqOpenGuide();
void reqCloseGuide();
void reqStartBuyHouse();
void reqEndBuyHouse();

//-----------------------------------------------------------------
// 调试命令
void debugUseFit(float x, float y, float z);
void continueWalkForUseFurni();
void continueWalkForPickMagicbox();
void setComoboxTextItemTxtColors(std::string combobox_name);
//-----------------------------------------------------------------
#endif