/******************************************************************************
* ����: lf
* ʱ��: 2010-07-05
* ����: ��̬UI
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

	EDUI_CHATPAOPAO,	//��������
	EDUI_NUMBERPAOPAO,	//��������
	EDUI_HEADUP,		//ͷ����Ϣ

	EDUI_FARMPAOPAO,	//ũ������
	EDUI_FARMHARVEST,	//ũ���ջ�����
	EDUI_FARMHEADUP,	//ũ��ͷ����Ϣ
	EDUI_CIRCLEMENU,	//���ﻷ�β˵�
	EDUI_BIGEXPRESSION,	//ͷ�������
};

enum NumberPaoPaoType
{
	// 0-11�ֱ�Ϊ0-9��+-��ռ��
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
	EFHT_INSECTPEST=1,	//�溦
	EFHT_PICKABLE=2,	//��ժ
};

struct DynamicUIInfo : public GameEventSet
{
	static const std::string EventTimeOut;

	DynamicUIInfo():life(0),objtype(1),autoRemove(false),autoDel(false),pos(0.0f,0.0f,0.0f){}

	DWORD			time;	//����ʱ��

	DynamicUIType	type;
	std::string		winname;
	uint			life;	//���ʱ��,0Ϊ���Զ�ɾ�������Ƴ�
	bool			autoRemove; //�Զ��Ӹ������Ƴ�
	bool			autoDel; //�Զ�ɾ��

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
	/*	type:	�������ͣ�0����ң�1��NPC��
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
