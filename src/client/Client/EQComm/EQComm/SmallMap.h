/******************************************************************************
* ����: ��չȻ
* ʱ��: 2011-4-22
* ����: С��ͼ
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

	// ����ʵ��3D��ͼ���ĸ��������꣨�����ɲ߻��Ǳ߻�ȡ�����ַ�����ʽΪ��"0, 0, 0"
	void setActualMapCorner3D(std::string leftUp3D, std::string leftDown3D, std::string rightUp3D, std::string rightDown3D);

	// ��ȡʵ��2D��ͼ�Ŀ���setActualMapCorner����ã�
	float getActualMapWidth2D(void);

	// ��ȡʵ��2D��ͼ�ĸߣ���setActualMapCorner����ã�
	float getActualMapHeight2D(void);

	// ��ȡʵ��3D��ͼ��ĳ������2D��ͼ�ϵ����꣬�ַ�����ʽΪ��"0, 0, 0"������ֵ��ʽΪ��"scale:0,0;offset:0,0"
	std::string getPos2D(std::string pos3D);

	// ���빫������
	void enterCommonScene(int scene_id);

	// ��ȡ��ͼ������ 
	float getSceneBeginPosX(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMinX;}
	float getSceneBeginPosY(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMinZ;}
	float getSceneEndPosX(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMaxX;}
	float getSceneEndPosY(void){return EQSceneManager::getSingleton().getSceneInfo()->getTerrainData()->mMaxZ;}

protected:
	// ʵ�ʵ�ͼ���ĸ���������
	Ogre::Vector3 m_actualMapLeftUp;
	Ogre::Vector3 m_actualMapLeftDown;
	Ogre::Vector3 m_actualMapRightUp;
	Ogre::Vector3 m_actualMapRightDown;
	// ʵ�ʵ�ͼ�Ŀ���
	float m_actualMapWidth;
	float m_actualMapHeight;
	// ��ҵ�λ������
	Ogre::Vector3 m_playerPos3D;
	// ��������id����Ӧmini_map.xml��
	int m_commonSceneId;
	// ������������
	std::string m_commonSceneName;
};


#endif // end of guard SmallMap_H
