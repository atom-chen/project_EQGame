#ifndef FarmDecoration_H
#define FarmDecoration_H

class FarmScene;
struct farm_decoration;
struct ModelDisplayRow;

class FarmDecoration
{
public:
	void setFarmScene(FarmScene* fs);

	void setServerData(farm_decoration* fd);

	void updateObjects();
	void changeScene(const std::string& sceneName,bool forceReload=false);

	FarmDecoration(void);
	~FarmDecoration(void);

private:
	const ModelDisplayRow* findModelDisplay(int itemid,int index = 0);
	FarmScene*			mFarmScene;
	farm_decoration*	mServerData, *mOldServerData;
	std::string			mSceneName;
};

#endif