/******************************************************************************
* 作者: 何展然
* 时间: 2011-4-22
* 描述: 小地图
******************************************************************************/
#include "EQCommPCH.h"
#include "SmallMap.h"
#include "CommFunc.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "ObjMgr.h"
#include "NPC.h"
#include "MainPlayer.h"
#include "EQGameSceneMgr.h"


template<> SmallMapManager* Ogre::Singleton<SmallMapManager>::ms_Singleton = 0;
//-----------------------------------------------------------------------
SmallMapManager::SmallMapManager(void)
:m_actualMapLeftUp(Ogre::Vector3::ZERO)
,m_actualMapLeftDown(Ogre::Vector3::ZERO)
,m_actualMapRightUp(Ogre::Vector3::ZERO)
,m_actualMapRightDown(Ogre::Vector3::ZERO)
,m_actualMapWidth(0.0f)
,m_actualMapHeight(0.0f)
,m_playerPos3D(Ogre::Vector3::ZERO)
,m_commonSceneId(0)
,m_commonSceneName("")
{
}
//-----------------------------------------------------------------------
SmallMapManager::~SmallMapManager(void)
{
}
//-----------------------------------------------------------------------
void SmallMapManager::update()
{
	switch (sEQGameSceneMgr.getSceneType())
	{
	case EQGameSceneMgr::SCENE_HOUSE:	// 室内场景
		{
			// 屏蔽地图功能
			luabind::call_function<void>(sLuaMgr.getL(), "SmallMap_shieldMap");
		}
		break;
	case EQGameSceneMgr::SCENE_COMMON:	// 室外场景
		{
			// 改变地图（第一步）
			if (0 != m_commonSceneId)
			{
				luabind::call_function<void>(sLuaMgr.getL(), "SmallMap_changeMap", m_commonSceneId, m_commonSceneName);
			}
			// 更新位置（第二步）
			Ogre::Vector3 playerPos = sMainPlayer.getPos();
			if (playerPos != m_playerPos3D)
			{
				m_playerPos3D = playerPos;
				luabind::call_function<void>(sLuaMgr.getL(), "SmallMap_updatePos", getPos2D(Vector3ToString(m_playerPos3D)));
			}
		}
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------
void SmallMapManager::setActualMapCorner3D(std::string leftUp3D, std::string leftDown3D, std::string rightUp3D, std::string rightDown3D)
{
	m_actualMapLeftUp = StringToVector3(leftUp3D);
	m_actualMapLeftDown = StringToVector3(leftDown3D);
	m_actualMapRightUp = StringToVector3(rightUp3D);
	m_actualMapRightDown = StringToVector3(rightDown3D);
	m_actualMapWidth = m_actualMapLeftUp.distance(m_actualMapRightUp);
	m_actualMapHeight = m_actualMapLeftUp.distance(m_actualMapLeftDown);
}
//-----------------------------------------------------------------------
float SmallMapManager::getActualMapWidth2D(void)
{
	return m_actualMapWidth;
}
//-----------------------------------------------------------------------
float SmallMapManager::getActualMapHeight2D(void)
{
	return m_actualMapHeight;
}
//-----------------------------------------------------------------------
std::string SmallMapManager::getPos2D(std::string pos3D)
{
	Ogre::Vector3 pos = StringToVector3(pos3D);
	float toLeftUp = pos.distance(m_actualMapLeftUp);	// 该点到实际2D地图左上角的距离
	float toLeftDown = pos.distance(m_actualMapLeftDown);	// 该点到实际2D地图左下角的距离

	// 根据以下两个公式：
	// （1）x*x + y*y = d1*d1			// d1，表该点到实际2D地图左上角的距离
	// （2）x*x + (h-y)*(h-y) = d2*d2	// h，表实际2D地图的高；d2，表该点到实际2D地图左下角的距离
	// 得出：y = (h*h + d1*d1 -d2*d2)/(2*h)；x = sqrt(d1*d1 - y*y)

	// 绝对坐标
	float offsetY = (m_actualMapHeight*m_actualMapHeight + toLeftUp*toLeftUp - toLeftDown*toLeftDown)/(2*m_actualMapHeight);
	float offsetX = sqrt(toLeftUp*toLeftUp - offsetY*offsetY);
	// 相对坐标
	float scaleX = offsetX/m_actualMapWidth;
	float scaleY = offsetY/m_actualMapHeight;

	char pos2D[256] = "";
	_snprintf_s(pos2D, sizeof(pos2D), "scale:%f,%f;offset:%f,%f", scaleX, scaleY, offsetX, offsetY);
	return pos2D;
}
//-----------------------------------------------------------------------
void SmallMapManager::enterCommonScene(int scene_id)
{
	const SceneRow* sceneRow = LogicTableManager::getSingleton().getSceneRow(scene_id);
	if (NULL == sceneRow)
	{
		return;
	}
	m_commonSceneId = scene_id;
	m_commonSceneName = GB2312ToUTF8(sceneRow->name.c_str());
}
//-----------------------------------------------------------------------