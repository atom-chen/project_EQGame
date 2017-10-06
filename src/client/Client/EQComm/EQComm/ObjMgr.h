/******************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 游戏OBJ管理
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

	// 玩家显示类型(2011/6/17 add by hezhr)
	enum VisibleType
	{
		VT_ALL = 1,		// 模型和头顶名字都显示
		VT_NAME,		// 只显示头顶名字
		VT_NONE,		// 模型和头顶名字都不显示
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

	// 室内场景相关条件判定(2011/6/13 add by hezhr)
	int getPosFloorNO(const Ogre::Vector3& pos);		// 获取位置所在楼层编号
	int getViewFloorNO(void);							// 获取当前所观察楼层编号
	bool isPosInViewFloor(const Ogre::Vector3& pos);	// 位置是否在所观察的楼层
	bool isPosInStair(const Ogre::Vector3& pos);		// 位置是否在楼梯对应的连通点
	// 用于角色隐藏(2011/6/17 add by hezhr)
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