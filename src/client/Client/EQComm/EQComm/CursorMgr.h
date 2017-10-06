#ifndef   CURSORMGR_H
#define  CURSORMGR_H
#include "CEGUI.h"

#include "InputMgr.h"
#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>
#include "LuaManager.h"
#include "GameState.h"
#include "GameEventMgr.h"
#include "EQGameSceneMgr.h"
#include "ObjMgr.h"
#include "FitmentEvent.h"


class CursorMgr : public EQInputEventListener
{
public:
	void setVisible(bool visible)
	{
		if(visible)
			CEGUI::MouseCursor::getSingleton().show();
		else
			CEGUI::MouseCursor::getSingleton().hide();
	}

	enum CursorState
	{
		CS_NORMAL,
		CS_LBDRAG_SCENE,
		CS_FARM_SCENE,	//ũ�����״̬
		CS_SPLIT_ITEM,  //�����Ʒ
		CS_HYPERLINK,	//��������Ӱ�ť
		CS_CHANGE_FLOOR,	// �����ذ�
		CS_CHANGE_WALL,	// ����ǽֽ
		CS_ON_NPC,
		CS_ON_FITMENT,
	};

	CursorMgr(void)
		: EQInputEventListener(epfCursor)
	{
		mSta = mOldState = CS_NORMAL;
		mSpliting = false;
		mhasTarget = 0;
		mOnTargetType = 0;
		sInputMgr.addListener(this);
	}

	virtual ~CursorMgr(void){}

	static CursorMgr& getSingleton()
	{
		static CursorMgr mgr;
		return mgr;
	}

	void setState(int sta)
	{
// 		std::string err_str("state***");
// 		LOGERR(err_str + Ogre::StringConverter::toString(sta));
		if (sta != mSta)
		{
			mOldState = mSta;
		}
		mSta=sta;
		setDefaultMouseCursor();
	}

	void backtoLastState()
	{
// 		std::string err_str("oldstate***");
// 		LOGERR(err_str + Ogre::StringConverter::toString(mOldState));
		setState(mOldState);
	}

	int	 getState(){return mSta;}
	bool getSpliting(){return (mSpliting!=0);}
	void setSpliting(bool s){ mSpliting = s;}

	virtual void LButtonUp( UINT& flags, POINT point )
	{ 
		if ((flags & epfUI) != 0)
			return;

		if (sInputMgr.mLMousedown)
		{
			if (sStateMgr.isFunnyFarm())
			{
				//��ũ���¼���������ע�ˣ�todo
				//sFunnyFarm.setFarmCurState(sFunnyFarm.getAction());
			}
			else
			{
				//setState(CS_NORMAL);
				//resetFlag();
				if (mhasTarget)
				{
					if (mOnTargetType == ObjMgr::GOT_NPC)
					{
						setState(CS_ON_NPC);
					}
					else if(mOnTargetType == ObjMgr::GOT_FITMENT)
					{
						setState(CS_ON_FITMENT);
					}
				}
				else
				{
					// ���ָ���ǽ�ڵذ�ģʽ�µ����״̬(2011/7/29 add by hezhr)
					if (sFitMgr.isFiting() && luabind::call_function<bool>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_setChangeWallFloorCursorState"))
					{
						return;
					}
					setState(CS_NORMAL);
				}
			}
		}
	}

	virtual void LButtonDown( UINT& flags, POINT point )
	{
		if (sStateMgr.isFunnyFarm())
		{
			//û�ط�д�����Ç塣������ʱд�����
			luabind::call_function<void>(sLuaMgr.getL(), "CloseSeedBag");
			//
			setState(CS_FARM_SCENE);
		}
	}

	////
	virtual void MouseMove	( UINT& flags, POINT delta, POINT point )
	{
		if (sStateMgr.isFunnyFarm())
		{
			//��ũ����������ע�ˣ�todo
			//sFunnyFarm._updatePos(point);
			if ((flags & epfUI) != 0)
			{
				//��ũ���¼���������ע�ˣ�todo
				//sFunnyFarm.hideMouseAnimateWnd();
				CEGUI::System::getSingleton().setDefaultMouseCursor("common1","FarmMouse01");
			}
			else
			{
				//��ũ���¼���������ע�ˣ�todo
				//sFunnyFarm.setFarmCurState(sFunnyFarm.getAction());
			}
		}
		else
		{
			if(getState()==CS_SPLIT_ITEM || getState()==CS_HYPERLINK)
			{
				return;
			}
			else
			{
				//setState(CS_NORMAL);
				//resetFlag();
				if (mhasTarget)
				{
					if ((flags & epfUI) != 0)
					{
						setState(CS_NORMAL);
					}
					// 				if (mOnTargetType == ObjMgr::GOT_NPC)
					// 				{
					// 					setState(CS_ON_NPC);
					// 				}
				}
				else
				{
					if ((flags & epfUI) != 0)
					{
						setState(CS_NORMAL);
					}
				}
			}
		}

	}
	
	//
	void setCurTarget(int has)
	{
		mhasTarget = has;
	}
	//
	void setCurTargetType(int type)
	{
		mOnTargetType = type;
	}

private:
	int	mSta,
		mSpliting,
		mOldState,
		mhasTarget,		//��������Ƿ�����ĳ�������ϣ�����������Ӧ�������ʽ��(1�����������ڣ�0��������û��)
		mOnTargetType;	//������ڵĶ�������ͣ�����or NPC or ��� or �Ҿ� �ȵȵģ�	

	void setDefaultMouseCursor()
	{
		string set,
			   image;
		switch (mSta)
		{
		case CS_NORMAL:
			set		= "MouseArrow";
			image	= "MouseArrow";
			break;
		case CS_LBDRAG_SCENE:
			set		= "TaharezLook";
			image	= "MouseMoveCursor";
			break;
		case CS_SPLIT_ITEM:
			set		= "MouseArrow";
			image	= "MouseChaifen";
			break;
		case CS_HYPERLINK:
			set		= "MouseArrow";
			image	= "MouseLink";
			break;
		case CS_CHANGE_FLOOR:
			set		= "MouseArrow";
			image	= "NewImage2";
			break;
		case CS_CHANGE_WALL:
			set		= "MouseArrow";
			image	= "NewImage1";
			break;
		case CS_ON_NPC:
			set		= "MouseArrow";
			image	= "MouseDuihua";
			break;
		case CS_ON_FITMENT:
			set		= "MouseArrow";
			image	= "MouseZhuaqu";
			break;

		case CS_FARM_SCENE:
			//��ũ����������ע�ˣ�todo
			//switch (sFunnyFarm.getAction())
			//{
			//case CS_FARM_NORMAL:
			//	sFunnyFarm.setFarmCurState(CS_FARM_NORMAL);
			//case CS_FARM_MOVE:
			//	sFunnyFarm.setFarmCurState(CS_FARM_MOVE);
			//	break;
			//case CS_FARM_TILL:
			//	sFunnyFarm.setFarmCurState(CS_FARM_TILL);
			//	break;
			//case CS_FARM_WATER:
			//	sFunnyFarm.setFarmCurState(CS_FARM_WATER);
			//	break;
			//case CS_FARM_WEED:
			//	sFunnyFarm.setFarmCurState(CS_FARM_WEED);
			//	break;
			//case CS_FARM_PICKBUG:
			//	sFunnyFarm.setFarmCurState(CS_FARM_PICKBUG);
			//	break;
			//case CS_FARM_HARVEST:
			//	sFunnyFarm.setFarmCurState(CS_FARM_HARVEST);
			//	break;
			//case CS_FARM_ONEKEYHARVEST:
			//	sFunnyFarm.setFarmCurState(CS_FARM_ONEKEYHARVEST);
			//	break;
			//case CS_FARM_SEEDING:
			//	sFunnyFarm.setFarmCurState(CS_FARM_SEEDING);
			//	break;
			//case CS_FARM_PUTBUG:
			//	sFunnyFarm.setFarmCurState(CS_FARM_PUTBUG);
			//	break;
			//case CS_FARM_PUTGRASS:
			//	sFunnyFarm.setFarmCurState(CS_FARM_PUTGRASS);
			//	break;
			//case CS_FARM_STEAL:
			//	sFunnyFarm.setFarmCurState(CS_FARM_STEAL);
			//	break;

			//}

			return;
			break;
		}
		CEGUI::System::getSingleton().setDefaultMouseCursor(set, image);
	}
};
#define  sCursorMgr CursorMgr::getSingleton()
#endif