/*********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: �ű��ӿ�
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
			MessageBoxA(NULL,except.c_str(),"�ű���Ϣ",MB_OK);
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
��½����
@param account
�ʺ�
@param serverIP
������IP
@param serverPort
port
*/
bool loginRequest(std::string account, 
				  std::string pw,
				  std::string serverIP, 
				  int serverPort);

/**
@brief
������ɫ����
@param account
@account
@param username
username
*/
bool creatPlayerRequest(std::string account, 
						std::string username);

/**
@brief
���볡����ʾ
@param
@param
*/
bool loadScene();

/**
@brief
��ɫ��ʾ
@param
@param
*/
bool rolesRequest();

/**
@brief
�汾��֤��ʾ
@param �ʺ�
@param IP
@param �˿�
*/
bool versionRequest(std::string account,
					std::string pw,
					std::string serverIP, 
					int serverPort);

/**
@brief
���ô�����ʾ���֣����մ�LUA�������ַ���
@param ����
@param ����
*/
void setText(const char* szWindows, 
				 const char* szText);

/**
@brief
�����������ʾ����������
@param
@param
*/
void setChatText(const char* dest,
				 const char* src);

/**
@brief
RTT����
@param active �Ƿ񼤻�
@param
*/
void rttSetActive(const char* name, bool active);

/**
@brief
����RTT
@param x , y, z, ����һ���Ƚ�Զ�ĵط�
@param
*/
void startRTT(int x, int y, int z);

/**
@brief
RTT��߱ȣ�
@param ��
@param ��
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
RTT��ת
@param degree ��ת�Ƕ�
@param
*/
void rttRotate(int degree,int axes);

/**����RTT�ĳ��������
*/
void rttReset(void);

/**
	RTT����
*/
void rttScale(float scaleX,float scaleY,float scaleZ);

/**RTT����
*/
void rttZoom(float scale);

/**
@brief
װ��
@param slot
@param װ����ʾID
@param ռ��slot
*/
void equipOn(uint slot, 
			 uint equipDisplayID,
			 uint holdSlot);

/**
@brief
�ѵ�
@param slot
@param
*/
void takeOff(uint slot);

/**
@brief
�ѵ�ȫ��
*/
void rttTakeOffAll();

/**
@brief
ͬ�������װ��
*/
void rttSync();

/**
@brief
rttFurniture
@param itemid ��ƷID
@param
*/
void rttFurniture(uint itemid);

/**
@brief
RTT��ת
@param degree ��ת�Ƕ�
@param
*/
void rttFurniRotate(int degree);

/**
@brief
RTT��߱ȣ�
@param ��
@param ��
*/
void rttFurniSetAspectRatio(int w, int h);

/**����RTT�ĳ��������
*/
void rttFurniReset(void);

/**RTT����
*/
void rttFurniZoom(float scale);

/**
@brief
�˳���Ϸ
@param
@param
*/
void exitApp();

/**
@brief
��ʾ��¥
@param
@param
*/
void houseUp();

/**
@brief
��ʾһ¥
@param
@param
*/
void houseDown();

/**
@brief
��ȡ������ʾģʽ
*/
int getHouseMode();

/**
@brief
�л�������ʾģʽ
@param sel ѡ��ģʽ
@param
*/
void changeHouseMode(int sel);

/**
@brief
�л�������ʾģʽ
@param sel ѡ��ģʽ
@param
*/
void changeGridMode(int sel);

/**
@brief
�л���װģʽ��������Ϸģʽ
@param sel ѡ��ģʽ
@param
*/
void changeFitmentMode(int sel);

/**
@brief
�༭ģʽ�������Ҿ���ʾ
@param id �Ҿ�ID
@param
*/
void onMouseFitmentDro(int id);

/**
@brief
�༭ģʽ�������Ҿ���ת
@param degree ��ת�Ƕ�
@param
*/
void onMouseFitmentRotate(float degree);

/**
@brief
�༭ģʽ������Ҿ�
@param
*/
void onMouseFitmentPickup();

/**
@brief
�༭ģʽ�·��¼Ҿ�
@param
*/
void onMouseFitmentPutdown();

/**
@brief
�༭ģʽ�¼Ҿ�ʰȡ״̬
@param
*/
int getPickState();


/**
@brief
װ��MESH
@param MESH �ļ���
@param �Ƿ�����Ӱ
*/
void loadStaticMesh(std::string meshfile, 
					bool castShadows);

/**
XiaoMing Add
@brief
װ��MESH
@param MESH �ļ���
@param �Ƿ�����Ӱ
@param x, y, z MESHλ��
@param rotationX, rotationY, rotationZ MESH��תֵ
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
����MESH
*/
void outdoorMeshScale(std::string nodeName, 
				float x, 
				float y, 
				float z);

/**
@brief
�������LOOKAT������ɫ
@param
@param
*/
void lookAtMainPlayer();

/**
@brief
����ɫ���Ŷ���
@param ani ��������
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
�õ���ǰ�༭�еļҾ�ID
*/
int getCurrFurID();

/**�õ����β˵�ID
*/
uint getLoopMenuID();
std::string getLoopMenuDelTime();	//��ȡ�Ҿ�ʱЧ (2011/9/22 add by hezhr)
uint getInteractingFitmentID();
/**
*/
void selLoopMenu(int menu);

/** Ŀǰ�ǰ�����ߺ�ҹ
*/
bool getDayNight();

/** �ı�Ϊ������ߺ�ҹ
*/
void changeDayNight(bool daynight, bool immediately);
/**�ָ��ڰ���������
*/
void revertDayNight();

/** ���졢��ҹ�滻
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
	����ComboBox Item
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

// ��ȡ��ǰ��ģ̬�Ի�����(2011/6/20 add by hezhr)
std::string getModalWindowName(void);

// ���ô��嵭��ǳ��(2011/7/7 add by hezhr)
void setWindowFade(std::string winname, int delayTime, int fadeTime, bool autoDel);

// ����
void reqFlowerLogs();
void reqEggLogs();
void reqOpenGuide();
void reqCloseGuide();
void reqStartBuyHouse();
void reqEndBuyHouse();

//-----------------------------------------------------------------
// ��������
void debugUseFit(float x, float y, float z);
void continueWalkForUseFurni();
void continueWalkForPickMagicbox();
void setComoboxTextItemTxtColors(std::string combobox_name);
//-----------------------------------------------------------------
#endif