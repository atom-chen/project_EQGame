/******************************************************************************
* ����: Y3
* ʱ��: 2010-06-22
* ����: ���ݺͳ��������л���. 
******************************************************************************/
#ifndef HouseSceneSerializer_H
#define HouseSceneSerializer_H

#include "EQMainPrerequisites.h"

class HouseScene;

//TODO:�ƹⷽ��Ӧ�ô�.house�ƶ���.scene�ļ�
class HouseSceneSerializer
{
public:
	HouseSceneSerializer();

	/** �Ƿ��ܱ���
	@remarks Ŀǰ����ֻ��canConvertToResource
	@param reason ����ʧ�ܵ�ԭ��
	*/
	bool canSave(std::string& reason);

	/** ������Դ
	@remarks ���ȵ���canSave����, �����ɱ�������, ������쳣
	@param logicName �߼��� ��ʽ: [Scene�µ���Ŀ¼/]����[.scene]
	@param scenePath Scene���ļ���·��, ����ַ������"/"
	*/
	void save(const std::string& logicName, const std::string& scenePath);

	/** ��ȡ��Դ
	@param logicName �߼��� ��ʽ: [Scene�µ���Ŀ¼/]����[.scene]
	@param inEditor �Ƿ��ڱ༭��״̬. 1.�༭��״̬��, ���ݴ��Ҿ�. 2.���λ�ѡ���Ż�ģʽ.
	@param withHouse �Ƿ������
	*/
	void load(const std::string& logicName, bool inEditor = false, bool withHouse = true);

	/** ��������
	*/
	void createScene(Ogre::Camera* cam);

	/** �ӷ�����Դ��������
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

	std::string			mSavePath;			//!< ����Ŀ¼
	std::string			mShortPath;			//!< �ֶ���Scene��Ŀ¼
	std::string			mHouseResName;
};

#endif