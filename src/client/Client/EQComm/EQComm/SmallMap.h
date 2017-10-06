/******************************************************************************
* 作者: 何展然
* 时间: 2011-4-22
* 描述: 小地图
******************************************************************************/
#ifndef SmallMap_H
#define SmallMap_H

#include "EQMainPrerequisites.h"
#include "SceneInfo.h"
#include "TerrainData.h"
#include "SceneManager.h"

class SmallMapManager : public Ogre::Singleton<SmallMapManager>
{
public:
	SmallMapManager(void);

	~SmallMapManager(void);

	void update(void);

	// 设置实际3D地图的四个顶点坐标（坐标由策划那边获取），字符串格式为："0, 0, 0"
	void setActualMapCorner3D(std::string leftUp3D, std::string leftDown3D, std::string rightUp3D, std::string rightDown3D);

	// 获取实际2D地图的宽（在setActualMapCorner后调用）
	float getActualMapWidth2D(void);

	// 获取实际2D地图的高（在setActualMapCorner后调用）
	float getActualMapHeight2D(void);

	// 获取实际3D地图上某个点在2D地图上的坐标，字符串格式为："0, 0, 0"，返回值格式为："scale:0,0;offset:0,0"
	std::string getPos2D(std::string pos3D);

	// 进入公共场景
	void enterCommonScene(int scene_id);

	// 获取地图的坐标 
	float getSceneBeginPosX(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMinX;}
	float getSceneBeginPosY(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMinZ;}
	float getSceneEndPosX(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMaxX;}
	float getSceneEndPosY(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMaxZ;}

protected:
	// 实际地图的四个顶点坐标
	Ogre::Vector3 m_actualMapLeftUp;
	Ogre::Vector3 m_actualMapLeftDown;
	Ogre::Vector3 m_actualMapRightUp;
	Ogre::Vector3 m_actualMapRightDown;
	// 实际地图的宽、高
	float m_actualMapWidth;
	float m_actualMapHeight;
	// 玩家的位置坐标
	Ogre::Vector3 m_playerPos3D;
	// 公共场景id（对应mini_map.xml）
	int m_commonSceneId;
	// 公共场景名字
	std::string m_commonSceneName;
};


#endif // end of guard SmallMap_H
