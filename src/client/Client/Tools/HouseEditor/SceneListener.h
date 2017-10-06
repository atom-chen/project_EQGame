/******************************************************************************
* ����: Y3
* ʱ��: 2005-5-31
* ����: ���������ݺܶ�, ����һ����������. ����TD
******************************************************************************/
#ifndef __FAIRYSCENELISTENER_H__
#define __FAIRYSCENELISTENER_H__




struct TerrainInfo
{
	int x, z;
	Real oldHeight;
	Real nowHeight;
};


class SceneListener
{
public:
	SceneListener(void) {}
	virtual ~SceneListener() {}


	virtual void onSceneReset(void);

	virtual void onTerrainHeightChanged(const std::vector<TerrainInfo>& terrainInfo);


};

class SceneListenerManager : public Ogre::Singleton<SceneListenerManager> 
{
protected:
	typedef std::list<SceneListener*> Listeners;
	Listeners mListeners;

public:
	SceneListenerManager(void);
	~SceneListenerManager();

	void addSceneListener(SceneListener* listener);
	void removeSceneListener(SceneListener* listener);

	void _fireSceneReset(SceneListener* exclude = 0);

	void _fireTerrainHeightChanged(const std::vector<TerrainInfo>& terrainInfo,SceneListener* exclude =0);





};


#endif
