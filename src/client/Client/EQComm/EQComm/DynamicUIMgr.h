/******************************************************************************
* 作者: lf
* 时间: 2010-07-05
* 描述: 动态UI
******************************************************************************/
#ifndef DynamicUIMgr_H
#define DynamicUIMgr_H

#include "EQMainPrerequisites.h"
#include "GameEventMgr.h"
namespace CEGUI
{
	class Window;
};
class GameObj;

enum DynamicUIType
{
	EDUI_COMMON,

	EDUI_CHATPAOPAO,	//聊天泡泡
	EDUI_NUMBERPAOPAO,	//数字泡泡
	EDUI_HEADUP,		//头顶信息

	EDUI_FARMPAOPAO,	//农场泡泡
	EDUI_FARMHARVEST,	//农场收获泡泡
	EDUI_FARMHEADUP,	//农场头顶信息
	EDUI_CIRCLEMENU,	//人物环形菜单
	EDUI_BIGEXPRESSION,	//头顶大表情
};

enum NumberPaoPaoType
{
	// 0-11分别为0-9和+-号占用
	ENPPT_CLEAN=12,
	ENPPT_HEALTH,
	ENPPT_CHARM,
	ENPPT_ACTIVE,
	ENPPT_FURNICLEAN,
	ENPPT_GOLD,
	ENPPT_EXP,
};

enum FarmPaoPaoType
{
	EFPPT_GOLD=17,
	EFPPT_EXP=18,
	EFPPT_DEFINE=99,
};

enum FarmHeadupType
{
	EFHT_INSECTPEST=1,	//虫害
	EFHT_PICKABLE=2,	//可摘
};

struct DynamicUIInfo : public GameEventSet
{
	static const std::string EventTimeOut;

	DynamicUIInfo():life(0),objtype(1),autoRemove(false),autoDel(false),pos(0.0f,0.0f,0.0f){}

	DWORD			time;	//创建时间

	DynamicUIType	type;
	std::string		winname;
	uint			life;	//存活时间,0为不自动删除或者移除
	bool			autoRemove; //自动从父窗体移除
	bool			autoDel; //自动删除

	//EDUI_CHATPAOPAO EDUI_NUMBERPAOPAO EDUI_HEADUP
	std::string		objname;
	uint			objtype;

	//EDUI_FARMPAOPAO EDUI_FARMHARVEST EDUI_FARMHEADUP
	Ogre::Vector3	pos;
};
typedef std::map<std::string, DynamicUIInfo> DynamicUIMap;

class DynamicUIMgr : public Ogre::Singleton<DynamicUIMgr>
{
public:
	DynamicUIMgr();
	~DynamicUIMgr();

	std::string createCommon(DynamicUIInfo& info);

	std::string createChatPaoPao(GameObj* obj, std::string& winname, const char* text);
	std::string createNumberPaoPao(GameObj* obj, NumberPaoPaoType type, int num);
	/*	type:	人物类型（0，玩家；1，NPC）
	*/
	std::string createHeadup(GameObj* obj, int type);
	std::string createCircleMenuDynamicInfo(GameObj*obj,int type, std::string winName);
	std::string createFarmPaoPao(int index,int x1,int y1,int z, const char* content, const char* set, const char* image);
	std::string createFarmHarvest(int index,Ogre::Vector3& pos, FarmPaoPaoType type, int num, const char* set=NULL, const char* image=NULL);
	std::string createFarmHeadup(int index,const Ogre::Vector3& pos, FarmHeadupType type, const char* set1=NULL, const char* image1=NULL, 
									const char* set2=NULL, const char* image2=NULL, const char* set3=NULL, const char* image3=NULL);
	std::string createHeadupBigExpression( GameObj* obj,std::string& winname,int id );
	void destroyWindow(std::string& winname);
	bool hasWindow(std::string& winname);
	void clearFarmPaoPaoBuffer();

	void update();

	bool _popFarmPaoPao();

protected:
	bool _popNumberPaoPao();
protected:
	DynamicUIMap	mUIMap;
	uint			mCounter;

	std::queue<DynamicUIInfo> mNumberPaoPaoBuffer;
	std::queue<DynamicUIInfo> mFarmPaoPaoBuffer;
};

#endif
