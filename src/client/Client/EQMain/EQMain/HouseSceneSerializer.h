/******************************************************************************
* 作者: Y3
* 时间: 2010-06-22
* 描述: 房屋和场景的序列化类. 
******************************************************************************/
#ifndef HouseSceneSerializer_H
#define HouseSceneSerializer_H

#include "EQMainPrerequisites.h"

class HouseScene;

//TODO:灯光方面应该从.house移动到.scene文件
class HouseSceneSerializer
{
public:
	HouseSceneSerializer();

	/** 是否能保存
	@remarks 目前调用只有canConvertToResource
	@param reason 返回失败的原因
	*/
	bool canSave(std::string& reason);

	/** 保存资源
	@remarks 请先调用canSave函数, 处理不可保存的情况, 否则会异常
	@param logicName 逻辑名 格式: [Scene下的子目录/]名字[.scene]
	@param scenePath Scene的文件夹路径, 最后字符必须带"/"
	*/
	void save(const std::string& logicName, const std::string& scenePath);

	/** 读取资源
	@param logicName 逻辑名 格式: [Scene下的子目录/]名字[.scene]
	@param inEditor 是否在编辑器状态. 1.编辑器状态下, 房屋带家具. 2.地形会选用优化模式.
	@param withHouse 是否带房屋
	*/
	void load(const std::string& logicName, bool inEditor = false, bool withHouse = true);

	/** 创建场景
	*/
	void createScene(Ogre::Camera* cam);

	/** 从房屋资源创建房屋
	*/
	void createHouseByHouseResName(const std::string& houseResName, Ogre::Camera* cam);

protected:
	void saveHouse();
	void saveTerrain();
	void saveScene();
	void saveServerScene();
	void saveEntityObjects(TiXmlElement* XMLNode);
	void loadTerrain();
	void loadHouse();
	void loadEntityObjects(TiXmlElement* XMLNode);

protected:
	bool				mIsLoad;
	bool				mInEditor;

	std::string			mName;
	std::string			mSceneName;
	std::string			mServerSceneName;
	std::string			mHouseName;
	std::string			mServerHouseName;
	std::string			mEditorHouseName;
	std::string			mTerrainFilename;

	std::string			mSavePath;			//!< 保存目录
	std::string			mShortPath;			//!< 现对于Scene的目录
	std::string			mHouseResName;
};

#endif