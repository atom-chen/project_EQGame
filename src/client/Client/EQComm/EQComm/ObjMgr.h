/******************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ��ϷOBJ����
*******************************************************************/
#ifndef ObjMgr_H
#define ObjMgr_H

#include <list>
#include <string>
#include <map>

#include <Ogre.h>

class GameObj;
class Player;
class Npc;
enum GAMEOBJ_TYPE;
class ModelGcs;
class NPC;

typedef std::list<Player*>	PlayerList;
typedef std::list<NPC*>		NPCList;

class ObjMgr : public Ogre::Singleton<ObjMgr>
{
public:
	enum GAMEOBJ_TYPE
	{
		GOT_OBJ			= 1,
		GOT_PLAYER		= 2,
		GOT_MAINPLAYER	= 3,
		GOT_NPC			= 4,
		GOT_FITMENT		= 5,
	};

	// �����ʾ����(2011/6/17 add by hezhr)
	enum VisibleType
	{
		VT_ALL = 1,		// ģ�ͺ�ͷ�����ֶ���ʾ
		VT_NAME,		// ֻ��ʾͷ������
		VT_NONE,		// ģ�ͺ�ͷ�����ֶ�����ʾ
	};

	ObjMgr(void);
	virtual ~ObjMgr(void);

	GameObj*	reqestObj(GAMEOBJ_TYPE type,std::string account="");
	GameObj*	findObj(GAMEOBJ_TYPE type,std::string key);
	GameObj*	findObj(ModelGcs* model);
	void			update();
	void			cleanup();

	void			setPickObj(GameObj* obj){mPickObj = obj;}
	GameObj*	getPickObj(){return mPickObj;}
	template <typename T>
	T *	getPickObjAs() { return dynamic_cast<T *>(mPickObj); }
	NPC*			getPickNPC();
	Player*		getPickPlayer();
	// XiaoMing ADD
	void			AllPlayerGosNone();

	bool			isMainPlayer(std::string owner);

	GAMEOBJ_TYPE getObjType(GameObj* obj);

	const NPCList& getNPCList(void){return mNpc;}	// (2011/5/9 add by hezhr)

	// ���ڳ�����������ж�(2011/6/13 add by hezhr)
	int getPosFloorNO(const Ogre::Vector3& pos);		// ��ȡλ������¥����
	int getViewFloorNO(void);							// ��ȡ��ǰ���۲�¥����
	bool isPosInViewFloor(const Ogre::Vector3& pos);	// λ���Ƿ������۲��¥��
	bool isPosInStair(const Ogre::Vector3& pos);		// λ���Ƿ���¥�ݶ�Ӧ����ͨ��
	// ���ڽ�ɫ����(2011/6/17 add by hezhr)
	void setPlayerVisible(Player* p);
	void setVisibleType(VisibleType type);

private:
	PlayerList	mPlayer;
	PlayerList	mPlayerCache;
	
	NPCList		mNpc;
	NPCList		mNpcCache;

	GameObj*	mPickObj;

	GameObj*	_getCache(GAMEOBJ_TYPE type);

	VisibleType	mVisibleType;
};
#define sObjMgr ObjMgr::getSingleton()
#endif