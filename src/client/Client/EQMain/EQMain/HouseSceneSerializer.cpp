#include "EQMainPCH.h"
#include "HouseSceneSerializer.h"

#include "HouseScene.h"
#include "TerrainData.h"
#include "CommFunc.h"
#include "HouseResManager.h"
#include "SceneEntityObject.h"
#include "SceneManager.h"
#include "SceneInfo.h"


//-----------------------------------------------------------------------
HouseSceneSerializer::HouseSceneSerializer() :
mIsLoad(false),
mInEditor(false)
{

}
//-----------------------------------------------------------------------
bool HouseSceneSerializer::canSave(std::string& reason)
{
	HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	if ( scene )
	{
		bool f = scene->canConvertToResource(reason);
		if ( !f )
		{
			if ( reason != "房屋未被加载!" )
				return false;
		}
	}


	return true;
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::save(const std::string& logicName, const std::string& scenePath)
{
	//这段代码还会调整, 保存应该有个更强大的保存对话框
	assert( scenePath[scenePath.length()-1] == '/' );
	mIsLoad = false;

	std::string logicBaseName;
	Ogre::StringUtil::splitFilename(logicName, logicBaseName, mShortPath);
	if (Ogre::StringUtil::endsWith(logicBaseName, ".house"))
		logicBaseName.erase(logicBaseName.length() - (sizeof(".house") - 1));
	if (Ogre::StringUtil::endsWith(logicBaseName, ".scene"))
		logicBaseName.erase(logicBaseName.length() - (sizeof(".scene") - 1));
	assert( !logicBaseName.empty() );
	if ( !mShortPath.empty() )
		mSavePath = scenePath + mShortPath;
	else
		mSavePath = scenePath;

	//
	mName = logicBaseName;
	mSceneName = logicBaseName + ".scene";
	mServerSceneName = "common_scene" + logicBaseName + ".xml";
	mHouseName = logicBaseName + ".house";
	mEditorHouseName = logicBaseName + "_editor.house";
	mServerHouseName = logicBaseName + ".xml";
	mTerrainFilename = logicBaseName + ".Terrain";

	//
	ForceDir(mSavePath);
	saveHouse();
	saveTerrain();
	saveScene();
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::saveHouse()
{
	HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	if (!scene)
		return;

	if ( !scene->isLoadHouse() )
		return;

	HouseResPtr houseResPtr = scene->convertToResource();

	// 客户端用数据
	houseResPtr->exportClientXML(mSavePath + mHouseName, true);

	// 编辑器用数据
	houseResPtr->exportClientXML(mSavePath + mEditorHouseName, false);

	// 服务端用数据
	houseResPtr->exportServerXML(mSavePath + mServerHouseName);
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::saveTerrain()
{
	TerrainData* data = EQSceneManager::getSingleton().getTerrainData();
	if ( data->isEmpty() )
		return;

	data->save(mSavePath + mTerrainFilename);
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::saveScene()
{
	TiXmlDocument doc;
	TiXmlElement *ele;
	char buf[256];

	TiXmlDeclaration dec;
	dec.Parse( "<?xml version='1.0' encoding='UTF-8'standalone='yes' ?>", 0, TIXML_ENCODING_UNKNOWN );
	doc.InsertEndChild( dec );

	TiXmlElement* root = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement("EQScene")));
	root->SetAttribute("name", GB2312ToUTF8(mName.c_str()));
	//是否还需要版本号?

	{
		//Ambient
		TiXmlElement* ambientEle = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("Ambient")));

		Ogre::ColourValue color;

		//这个已经没用了
		//ambientEle->SetAttribute("DayNight", AmbientMgr::getSingleton().mDayNight);

		ambientEle->SetAttribute("ChangeTime", AmbientMgr::getSingleton().mChangetime);

		color = AmbientMgr::getSingleton().mDayAmbient;
		ambientEle->SetAttribute("DayAmbient", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		color = AmbientMgr::getSingleton().mDayDiffuse;
		ambientEle->SetAttribute("DayDiffuse", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		color = AmbientMgr::getSingleton().mDaySpecular;
		ambientEle->SetAttribute("DaySpecular", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		ambientEle->SetAttribute("DayMainDir", Ogre::StringConverter::toString(AmbientMgr::getSingleton().mDayMainDir));

		color = AmbientMgr::getSingleton().mDayFogColor;
		ambientEle->SetAttribute("DayFogColor", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		ambientEle->SetAttribute("DayFogParam", Ogre::StringConverter::toString(AmbientMgr::getSingleton().mDayFogParam));

		ambientEle->SetAttribute("DaySkybox", AmbientMgr::getSingleton().mDaySkybox);

		color = AmbientMgr::getSingleton().mNightAmbient;
		ambientEle->SetAttribute("NightAmbient", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		color = AmbientMgr::getSingleton().mNightDiffuse;
		ambientEle->SetAttribute("NightDiffuse", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		color = AmbientMgr::getSingleton().mNightSpecular;
		ambientEle->SetAttribute("NightSpecular", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		ambientEle->SetAttribute("NightMainDir", Ogre::StringConverter::toString(AmbientMgr::getSingleton().mNightMainDir));

		color = AmbientMgr::getSingleton().mNightFogColor;
		ambientEle->SetAttribute("NightFogColor", Ogre::StringConverter::toString(Ogre::Vector3(color.r, color.g, color.b)));

		ambientEle->SetAttribute("NightFogParam", Ogre::StringConverter::toString(AmbientMgr::getSingleton().mNightFogParam));

		ambientEle->SetAttribute("NightSkybox", AmbientMgr::getSingleton().mNightSkybox);

		ambientEle->SetAttribute("Contrast", Ogre::StringConverter::toString(AmbientMgr::getSingleton().mContrast));
	}

	HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	if ( scene && scene->isLoadHouse() )
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("House")));
		ele->SetAttribute("filename", GB2312ToUTF8(mHouseName.c_str()));
	}

	if ( !EQSceneManager::getSingleton().getTerrainData()->isEmpty() )
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("Terrain")));
		ele->SetAttribute("filename", GB2312ToUTF8(mTerrainFilename.c_str()));
	}

	Ogre::Vector2 bp = EQSceneManager::getSingleton().getSceneInfo()->mSceneBeginPos;
	Ogre::Vector2 ep = EQSceneManager::getSingleton().getSceneInfo()->mSceneEndPos;
	sprintf_s(buf, sizeof(buf), "%f %f %f %f", bp.x, bp.y, ep.x, ep.y);
	ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("Edge")));
	ele->SetAttribute("value", buf);

	//
	saveEntityObjects(root);

	//保存
	doc.SaveFile(mSavePath + mSceneName);

	//生成室外家具交互用服务端表
	saveServerScene();
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::saveEntityObjects(TiXmlElement* XMLNode)
{
	//排序算法, 模型名 材质名 位置
	struct SceneEntityObjectMore 
	{
		bool operator()( SceneEntityObject* a, SceneEntityObject* b) const
		{
			if ( a->getEntityObjId() == 0 && b->getEntityObjId() != 0 )
				return false;
			else if ( a->getEntityObjId() != 0 && b->getEntityObjId() == 0 )
				return true;
			else if ( a->getEntityObjId() != 0 && b->getEntityObjId() != 0 )
				return (a->getEntityObjId() < b->getEntityObjId());

			if ( a->getMesh() > b->getMesh() )
				return true;
			else if ( a->getMesh() < b->getMesh() )
				return false;

			if ( a->getMaterial() > b->getMaterial() )
				return true;
			else if ( a->getMaterial() < b->getMaterial() )
				return false;

			if ( a->getPosition() > b->getPosition() )
				return true;
			else if ( a->getPosition() < b->getPosition() )
				return false;

			return false;
		}
	};


	//
	std::map<uint, SceneEntityObject*>& oldObjs = EQSceneManager::getSingleton().getSceneInfo()->getSceneEntityObjects();
	std::vector<SceneEntityObject*> objs;

	objs.reserve(oldObjs.size());
	for ( std::map<uint, SceneEntityObject*>::iterator it = oldObjs.begin(); it != oldObjs.end(); ++it )
		objs.push_back(it->second);

	//排序
	std::sort( objs.begin(), objs.end(), SceneEntityObjectMore() );

	//写入xml
	uint interact_id=0;
	for (std::vector<SceneEntityObject*>::iterator it = objs.begin(); it !=objs.end(); ++it)
	{
		SceneEntityObject* obj = *it;
		TiXmlElement *ele;

		ele = static_cast<TiXmlElement*>(XMLNode->InsertEndChild(TiXmlElement("Entity")));

		if (obj->getEntityObjId() == 0 )
		{
			ele->SetAttribute("mesh", GB2312ToUTF8(obj->getMesh().c_str()));
			if ( !obj->getMaterial().empty() )
				ele->SetAttribute("material", GB2312ToUTF8(obj->getMaterial().c_str()));
			if ( obj->getLightId() != 0 )
				ele->SetAttribute("light", Ogre::StringConverter::toString(obj->getLightId()));
			if ( obj->getCutDistanceLevel() != CDL_NONE )
				ele->SetAttribute("CutLevel", Ogre::StringConverter::toString(obj->getCutDistanceLevel()));
			if ( obj->getParticleId() != 0 )
				ele->SetAttribute("particle", Ogre::StringConverter::toString(obj->getParticleId()));
		}
		else
		{
			ele->SetAttribute("id", Ogre::StringConverter::toString(obj->getEntityObjId()));
		}
		ele->SetAttribute("pos", Ogre::StringConverter::toString(obj->getPosition()));
		if ( obj->getOrientation() != Ogre::Quaternion::IDENTITY )
			ele->SetAttribute("rot", Ogre::StringConverter::toString(obj->getOrientation()));
		if ( obj->getScale() != Ogre::Vector3::UNIT_SCALE )
			ele->SetAttribute("scale", Ogre::StringConverter::toString(obj->getScale()));

		if( obj->getLoopId() )
		{
			ele->SetAttribute("loop", Ogre::StringConverter::toString(obj->getLoopId()));

			obj->setInteractId(++interact_id);
			ele->SetAttribute("interact", Ogre::StringConverter::toString(obj->getInteractId()));
		}
	}
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::load(const std::string& logicName, bool inEditor, bool withHouse)
{
	mIsLoad = false;
	mInEditor = inEditor;

	mName = "";
	mSceneName = "";
	mHouseName = "";
	mServerHouseName = "";
	mEditorHouseName = "";
	mTerrainFilename = "";

	mSavePath = "";
	mShortPath = "";

	//

	std::string logicBaseName;
	Ogre::StringUtil::splitFilename(logicName, logicBaseName, mShortPath);
	if (Ogre::StringUtil::endsWith(logicBaseName, ".scene"))
		logicBaseName.erase(logicBaseName.length() - (sizeof(".scene") - 1));
	assert( !logicBaseName.empty() );

	//
	mSceneName = logicBaseName + ".scene";

	TiXmlElement *ele;
	TiXmlDocument doc;
	Ogre::DataStreamPtr stream = OpenResource(mShortPath + mSceneName);
	doc.Parse( stream->getAsString().c_str() );

	TiXmlElement* root = doc.FirstChildElement("EQScene");
	if ( root == NULL )
	{
		EQ_EXCEPT( mShortPath + mSceneName + " is not a EQ scene!", 
			"HouseSceneSerializer::load");
		return;
	}

	mName = UTF8ToGB2312(xmlGetAttrib(root, "name").c_str());

	//
	ele = root->FirstChildElement("House");
	if( withHouse && ele )
	{
		mHouseName = UTF8ToGB2312(xmlGetAttrib(ele, "filename").c_str());
		mEditorHouseName = mHouseName;
		if (Ogre::StringUtil::endsWith(mEditorHouseName, ".house"))
			mEditorHouseName.erase(mEditorHouseName.length() - (sizeof(".house") - 1));
		mEditorHouseName += "_editor.house";
	}

	//
	ele = root->FirstChildElement("Terrain");
	if( ele )
	{
		mTerrainFilename = UTF8ToGB2312(xmlGetAttrib(ele, "filename").c_str());
	}

	//
	ele = root->FirstChildElement("Edge");
	if ( ele )
	{
		std::string buf = xmlGetAttrib(ele, "value").c_str();
		Ogre::Vector2 bp;
		Ogre::Vector2 ep;
		sscanf_s(buf.c_str(), "%f %f %f %f", &bp.x, &bp.y, &ep.x, &ep.y);
		EQSceneManager::getSingleton().getSceneInfo()->mSceneBeginPos = bp;
		EQSceneManager::getSingleton().getSceneInfo()->mSceneEndPos = ep;

	}

	loadEntityObjects(root);

	//
	loadTerrain();
	loadHouse();

	ele = root->FirstChildElement("Ambient");
	if(ele)
	{
		Ogre::Vector3 vec;

		//这个已经没用了
		//AmbientMgr::getSingleton().mDayNight = xmlGetAttribBool(ele, "DayNight");

		AmbientMgr::getSingleton().mChangetime = xmlGetAttribInt(ele, "ChangeTime");

		vec = xmlGetAttribVector3(ele, "DayAmbient");
		AmbientMgr::getSingleton().mDayAmbient = Ogre::ColourValue(vec.x, vec.y, vec.z);

		vec = xmlGetAttribVector3(ele, "DayDiffuse");
		AmbientMgr::getSingleton().mDayDiffuse = Ogre::ColourValue(vec.x, vec.y, vec.z);

		vec = xmlGetAttribVector3(ele, "DaySpecular");
		AmbientMgr::getSingleton().mDaySpecular = Ogre::ColourValue(vec.x, vec.y, vec.z);

		AmbientMgr::getSingleton().mDayMainDir = xmlGetAttribVector3(ele, "DayMainDir");

		vec = xmlGetAttribVector3(ele, "DayFogColor");
		AmbientMgr::getSingleton().mDayFogColor = Ogre::ColourValue(vec.x, vec.y, vec.z);

		AmbientMgr::getSingleton().mDayFogParam = xmlGetAttribVector4(ele, "DayFogParam");

		AmbientMgr::getSingleton().mDaySkybox = xmlGetAttrib(ele, "DaySkybox");

		vec = xmlGetAttribVector3(ele, "NightAmbient");
		AmbientMgr::getSingleton().mNightAmbient = Ogre::ColourValue(vec.x, vec.y, vec.z);

		vec = xmlGetAttribVector3(ele, "NightDiffuse");
		AmbientMgr::getSingleton().mNightDiffuse = Ogre::ColourValue(vec.x, vec.y, vec.z);

		vec = xmlGetAttribVector3(ele, "NightSpecular");
		AmbientMgr::getSingleton().mNightSpecular = Ogre::ColourValue(vec.x, vec.y, vec.z);

		AmbientMgr::getSingleton().mNightMainDir = xmlGetAttribVector3(ele, "NightMainDir");

		vec = xmlGetAttribVector3(ele, "NightFogColor");
		AmbientMgr::getSingleton().mNightFogColor = Ogre::ColourValue(vec.x, vec.y, vec.z);

		AmbientMgr::getSingleton().mNightFogParam = xmlGetAttribVector4(ele, "NightFogParam");

		AmbientMgr::getSingleton().mNightSkybox = xmlGetAttrib(ele, "NightSkybox");

		AmbientMgr::getSingleton().mContrast = xmlGetAttribFloat(ele, "Contrast");

		AmbientMgr::getSingleton().refresh();
	}
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::loadEntityObjects(TiXmlElement* XMLNode)
{
	TiXmlElement *ele;

	ele = XMLNode->FirstChildElement("Entity");
	while(ele)
	{
		uint id = (uint)xmlGetAttribInt(ele, "id");
		std::string mesh = UTF8ToGB2312(xmlGetAttrib(ele, "mesh").c_str());
		std::string mat = UTF8ToGB2312(xmlGetAttrib(ele, "material").c_str());
		uint lightId = (uint)xmlGetAttribInt(ele, "light");
		int cut = xmlGetAttribInt(ele, "CutLevel");
		uint particleId = (uint)xmlGetAttribInt(ele, "particle");
		uint interact = (uint)xmlGetAttribInt(ele, "interact");
		uint loop = (uint)xmlGetAttribInt(ele, "loop");

		SceneEntityObject* obj = new SceneEntityObject;
		if ( id == 0 )
		{
			obj->setMesh(mesh);
			obj->setMaterial(mat);
			obj->setLightId(lightId);
			obj->setCutDistanceLevel(cut);
			obj->setParticleId(particleId);
		}
		else
			obj->setEntityObjId(id);

		obj->setPosition(xmlGetAttribVector3(ele, "pos"));
		obj->setOrientation(xmlGetAttribQuaternion(ele, "rot"));
		obj->setScale(xmlGetAttribVector3(ele, "scale", Ogre::Vector3::UNIT_SCALE));

		if(loop)
		{
			obj->setLoopId(loop);
			obj->setInteractId(interact);
		}

		EQSceneManager::getSingleton().getSceneInfo()->addSceneObject(obj);

		//
		ele = ele->NextSiblingElement("Entity");
	}

}
//-----------------------------------------------------------------------
void HouseSceneSerializer::loadTerrain()
{
	if ( !mTerrainFilename.empty() )
	{
		EQSceneManager::getSingleton().getTerrainData()->load(mShortPath + mTerrainFilename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false );
	}
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::loadHouse()
{
	std::string name;
	if ( mInEditor )
		name = mEditorHouseName;
	else
		name = mHouseName;

	if ( name.empty() )
		return;

	name = mShortPath + name;

	if ( mInEditor )
		HouseResManager::getSingleton().unload(name); //为了确保加载同一个资源是最新的
	HouseResManager::getSingleton().load(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mHouseResName = name;
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::createScene(Ogre::Camera* cam)
{
	//初始化场景信息
	EQSceneManager::getSingleton().getSceneInfo()->initialise(mInEditor);

	//创建房屋
	if ( !mHouseResName.empty() )
	{
		//EQSceneManager::getSingleton().destroyHouseScene();
		HouseScene* scene = EQSceneManager::getSingleton().createNullHouseScene(cam);
		scene->mHouseResName = mHouseResName;
		scene->buildHouse();
	}
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::createHouseByHouseResName(const std::string& houseResName, Ogre::Camera* cam)
{
	//HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	//assert(scene->isLoadHouse()==false);

	HouseResManager::getSingleton().load(houseResName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	HouseScene* scene = EQSceneManager::getSingleton().createNullHouseScene(cam);
	scene->mHouseResName = houseResName;
	scene->buildHouse();
}
//-----------------------------------------------------------------------
void HouseSceneSerializer::saveServerScene()
{
	TiXmlDocument doc;

	TiXmlDeclaration dec;
	dec.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
	doc.InsertEndChild( dec );

	doc.InsertEndChild(TiXmlElement("root"));
	TiXmlElement* root = doc.RootElement();

	{
		std::map<uint, SceneEntityObject*>& objs = EQSceneManager::getSingleton().getSceneInfo()->getSceneEntityObjects();
		for (std::map<uint, SceneEntityObject*>::iterator it = objs.begin(); it !=objs.end(); ++it)
		{
			SceneEntityObject* obj = it->second;
			if(obj->getLoopId() == 0)
				continue;

			TiXmlElement* tplt = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("common_scene_furniture")));

			TiXmlElement* ele;
			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("id")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(obj->getInteractId())));
			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("object_id")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(obj->getEntityObjId())));
		}
	}

	doc.SaveFile(mSavePath + mServerSceneName);
}