/******************************************************************************
* ����: ��չȻ
* ʱ��: 2011-4-22
* ����: С��ͼ
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
	case EQGameSceneMgr::SCENE_HOUSE:	// ���ڳ���
		{
			// ���ε�ͼ����
			luabind::call_function<void>(sLuaMgr.getL(), "SmallMap_shieldMap");
		}
		break;
	case EQGameSceneMgr::SCENE_COMMON:	// ���ⳡ��
		{
			// �ı��ͼ����һ����
			if (0 != m_commonSceneId)
			{
				luabind::call_function<void>(sLuaMgr.getL(), "SmallMap_changeMap", m_commonSceneId, m_commonSceneName);
			}
			// ����λ�ã��ڶ�����
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
	float toLeftUp = pos.distance(m_actualMapLeftUp);	// �õ㵽ʵ��2D��ͼ���Ͻǵľ���
	float toLeftDown = pos.distance(m_actualMapLeftDown);	// �õ㵽ʵ��2D��ͼ���½ǵľ���

	// ��������������ʽ��
	// ��1��x*x + y*y = d1*d1			// d1����õ㵽ʵ��2D��ͼ���Ͻǵľ���
	// ��2��x*x + (h-y)*(h-y) = d2*d2	// h����ʵ��2D��ͼ�ĸߣ�d2����õ㵽ʵ��2D��ͼ���½ǵľ���
	// �ó���y = (h*h + d1*d1 -d2*d2)/(2*h)��x = sqrt(d1*d1 - y*y)

	// ��������
	float offsetY = (m_actualMapHeight*m_actualMapHeight + toLeftUp*toLeftUp - toLeftDown*toLeftDown)/(2*m_actualMapHeight);
	float offsetX = sqrt(toLeftUp*toLeftUp - offsetY*offsetY);
	// �������
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