#include "EQMainPCH.h"
#include "FarmScene.h"
#include "ModelManagerGcs.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "SystemGcs.h"

//-----------------------------------------------------------------------
const std::string FARM_XML = "FarmScene.xml";

//-----------------------------------------------------------------------
const std::string& FarmPlantUnit::PickObjectUnit::getClassName()
{
	static std::string name = "FarmPlantUnit::PickObjectUnit";
	return name;
}
//-----------------------------------------------------------------------
FarmPlantUnit::FarmPlantUnit(FarmScene& creator, int index)
: mCreator(creator)
, mIndex(index)
, mLand(NULL)
, mPlant(NULL)
, mSignExpand(NULL)
{
	for (int i=0; i<GRASS_MAX; i++)
	{
		mGrass[i] = NULL;
		mGrassPo[i].mCreator = this;
		mGrassPo[i].mName = "Grass";
		mGrassPxy[i].setObject(&mGrassPo[i]);
	}
	calcPos();

	mLandPo.mCreator = this;
	mLandPo.mName = "Land";
	mPlantPo.mCreator = this;
	mPlantPo.mName = "Plant";
	mSignExpandPo.mCreator = this;
	mSignExpandPo.mName = "SignExpand";

	mLandPxy.setObject(&mLandPo);
	mPlantPxy.setObject(&mPlantPo);
	mSignExpandPxy.setObject(&mSignExpandPo);
}
//-----------------------------------------------------------------------
FarmPlantUnit::~FarmPlantUnit()
{
	changeLand("","");
	removeGrass();
	removePlant();
	removeSignExpand();
}
//-----------------------------------------------------------------------
void FarmPlantUnit::calcPos()
{
	mPos = Ogre::Vector3::ZERO;

	int ix = mIndex;
	for ( std::vector<FarmLandAreaInfo>::iterator it = mCreator.mLandAreaInfos.begin(); it != mCreator.mLandAreaInfos.end(); ++it )
	{
		FarmLandAreaInfo& info = *it;
		int size = info.xSize*info.zSize;
		int ls = ix+1;		//剩下的大小
		if ( ls > size )
			ix -= size;
		else
		{
			int z = ix / info.zSize;
			int x = ix % info.zSize;

			mPos.z = info.beginPos.z + z*mCreator.mLandInterval + mCreator.mLandInterval/2.0f;
			mPos.y = info.beginPos.y;
			mPos.x = info.beginPos.x + x*mCreator.mLandInterval + mCreator.mLandInterval/2.0f;
			break;
		}
	}
}
//-----------------------------------------------------------------------
void FarmPlantUnit::changeLand(const std::string& mesh, const std::string& material)
{
	if ( mLand )
	{
		ModelManagerGcs::getSingleton().destroyModel(mLand);
		mLand = NULL;
	}

	if ( !mesh.empty() )
	{
		SimpleCreateModelRule rule(mesh, material);
		mLand = ModelManagerGcs::getSingleton().createModel(&rule);
		mLand->setParentNode(mCreator.getSceneNode());
		mLand->setPickObjectProxy(&mLandPxy);

		mLand->getMainNode()->setPosition(mPos);
	}
}
//-----------------------------------------------------------------------
void FarmPlantUnit::resetLand()
{
	changeLand(mCreator.mInitialLandMesh, mCreator.mInitialLandMaterial);
}
//-----------------------------------------------------------------------
void FarmPlantUnit::seedPlant(const std::string& mesh, const std::string& material )
{
	if ( mPlant )
	{
		ModelManagerGcs::getSingleton().destroyModel(mPlant);
		mPlant = NULL;
	}

	if ( !mesh.empty() )
	{
		SimpleCreateModelRule rule(mesh, material);
		mPlant = ModelManagerGcs::getSingleton().createModel(&rule);
		mPlant->setParentNode(mCreator.getSceneNode());
		mPlant->setPickObjectProxy(&mPlantPxy);

		mPlant->getMainNode()->setPosition(mPos+Ogre::Vector3(0,mCreator.mSeedHeight,0));
	}
}
//-----------------------------------------------------------------------
void FarmPlantUnit::removePlant()
{
	seedPlant("");
}
//-----------------------------------------------------------------------
void FarmPlantUnit::seedGrass()
{
	//int rand;
	for (int i=0; i<GRASS_MAX; i++)
	{
		//rand = GetTickCount() % 3;
		if ( mGrass[i] )
		{
			continue;
		}

		SimpleCreateModelRule rule(mCreator.mGrassMesh, mCreator.mGrassMaterial);
		mGrass[i] = ModelManagerGcs::getSingleton().createModel(&rule);
		mGrass[i]->setParentNode(mCreator.getSceneNode());
		mGrass[i]->setPickObjectProxy(&mGrassPxy[i]);

		mGrass[i]->getMainNode()->setPosition(mPos+Ogre::Vector3(0,mCreator.mSeedHeight,0) + mCreator.mGrassPos[i]);
		break;
	}
}
//-----------------------------------------------------------------------
void FarmPlantUnit::removeGrass()
{
	for (int i=0; i<GRASS_MAX; i++)
	{
		if ( mGrass[i] )
		{
			ModelManagerGcs::getSingleton().destroyModel(mGrass[i]);
			mGrass[i] = NULL;
		}
	}
}
//-----------------------------------------------------------------------
void FarmPlantUnit::signExpand()
{
	if ( mSignExpand )
	{
		return;
	}

	SimpleCreateModelRule rule(mCreator.mExpandSignMesh, mCreator.mExpandSignMaterial);
	mSignExpand = ModelManagerGcs::getSingleton().createModel(&rule);
	mSignExpand->setParentNode(mCreator.getSceneNode());
	mSignExpand->setPickObjectProxy(&mSignExpandPxy);

	mSignExpand->getMainNode()->setPosition(mPos+Ogre::Vector3(0,mCreator.mSeedHeight,0));
}
//-----------------------------------------------------------------------
void FarmPlantUnit::removeSignExpand()
{
	if ( mSignExpand )
	{
		ModelManagerGcs::getSingleton().destroyModel(mSignExpand);
		mSignExpand = NULL;
	}
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
FarmObject::FarmObject(FarmScene& creator, const std::string& name)
: mCreator(creator)
, mName(name)
, mObject(NULL)
{
	mObjectPxy.setObject(this);
}
//-----------------------------------------------------------------------
FarmObject::~FarmObject()
{
	if ( mObject )
	{
		ModelManagerGcs::getSingleton().destroyModel(mObject);
		mObject = NULL;
	}
}
//-----------------------------------------------------------------------
const std::string& FarmObject::getClassName()
{
	static const std::string name = "FarmObject";
	return name;
}
//-----------------------------------------------------------------------
void FarmObject::changeObject(const std::string& mesh, const std::string& material)
{
	if ( mObject )
	{
		ModelManagerGcs::getSingleton().destroyModel(mObject);
		mObject = NULL;
	}

	if ( !mesh.empty() )
	{
		SimpleCreateModelRule rule(mesh, material);
		mObject = ModelManagerGcs::getSingleton().createModel(&rule);
		mObject->setParentNode(mCreator.getSceneNode());
		mObject->setPickObjectProxy(&mObjectPxy);

		FarmObjectInfo& foi = mCreator.mObjectInfos.find(mName)->second;
		mObject->getMainNode()->setPosition(foi.mPos);
		mObject->getMainNode()->yaw(Ogre::Radian(Ogre::Degree(foi.mYDegree)));
	}
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
FarmScene::FarmScene()
: mLandInterval(80.0f)
, mSeedHeight(10.0f)
{

}
//-----------------------------------------------------------------------
FarmScene::~FarmScene()
{
	for ( std::vector<FarmPlantUnit*>::iterator it = mFarmPlantUnits.begin(); it != mFarmPlantUnits.end(); ++it )
		delete *it;
	mFarmPlantUnits.clear();

	for (std::map<std::string, FarmObject*>::iterator it = mFarmObjects.begin(); it != mFarmObjects.end(); ++it)
		delete it->second;
	mFarmObjects.clear();

	mLandAreaInfos.clear();
	mObjectInfos.clear();
}
//-----------------------------------------------------------------------
void FarmScene::changeLand(int index, const std::string& mesh, const std::string& material)
{
	mFarmPlantUnits[index]->changeLand(mesh, material);
}
//-----------------------------------------------------------------------
void FarmScene::resetLand(int index)
{
	mFarmPlantUnits[index]->resetLand();
}
//-----------------------------------------------------------------------
void FarmScene::seedPlant(int index , const std::string& mesh, const std::string& material)
{
	mFarmPlantUnits[index]->seedPlant(mesh, material);
}
//-----------------------------------------------------------------------
void FarmScene::removePlant(int index)
{
	mFarmPlantUnits[index]->removePlant();
}
//-----------------------------------------------------------------------
void FarmScene::seedGrass(int index)
{
	mFarmPlantUnits[index]->seedGrass();
}
//-----------------------------------------------------------------------
void FarmScene::removeGrass(int index)
{
	mFarmPlantUnits[index]->removeGrass();
}
//-----------------------------------------------------------------------
void FarmScene::signExpand(int index)
{
	mFarmPlantUnits[index]->signExpand();
}
//-----------------------------------------------------------------------
void FarmScene::removeSignExpand(int index)
{
	mFarmPlantUnits[index]->removeSignExpand();
}
//-----------------------------------------------------------------------
void FarmScene::changeFarmObject(const std::string& name, const std::string& mesh, const std::string& material )
{
	std::map<std::string, FarmObject*>::iterator it;
	it = mFarmObjects.find(name);
	if ( it == mFarmObjects.end() )
	{
		EQ_EXCEPT(name + " is invaild!", "FarmScene::changeFarmObject");
	}
	
	it->second->changeObject(mesh, material);
}
//-----------------------------------------------------------------------
void FarmScene::removeFarmObject(const std::string& name)
{
	changeFarmObject(name, "");
}
//-----------------------------------------------------------------------
FarmPlantUnit* FarmScene::getFarmPlantUnit(int index)
{
	assert(index < (int)mFarmPlantUnits.size() && index >= 0);

	return mFarmPlantUnits[index];
}
//-----------------------------------------------------------------------
FarmObject* FarmScene::getFarmObject(const std::string& name)
{
	std::map<std::string, FarmObject*>::iterator it = mFarmObjects.find(name);
	if ( it == mFarmObjects.end() )
	{
		EQ_EXCEPT(name + " is invaild!", "FarmScene::getFarmObject");
	}

	return it->second;
}
//-----------------------------------------------------------------------
void FarmScene::setInitialState()
{
	for(std::vector<FarmPlantUnit*>::iterator it = mFarmPlantUnits.begin(); it != mFarmPlantUnits.end(); ++it)
	{
		FarmPlantUnit* fpu = *it;
		fpu->resetLand();
		fpu->removePlant();
		fpu->removeGrass();
	}

	for ( std::map<std::string, FarmObject*>::iterator it = mFarmObjects.begin(); it != mFarmObjects.end(); ++it )
	{
		FarmObject* fo = it->second;
		fo->changeObject("");
	}
}
//-----------------------------------------------------------------------
int FarmScene::getLandSize()
{
	int s = 0;

	if ( mFarmPlantUnits.empty() )
	{
		for ( std::vector<FarmLandAreaInfo>::iterator it = mLandAreaInfos.begin(); it != mLandAreaInfos.end(); ++it )
		{
			FarmLandAreaInfo& info = *it;
			s += info.xSize * info.zSize;
		}
	}
	else
		s = mFarmPlantUnits.size();

	return s;
}
//-----------------------------------------------------------------------
void FarmScene::initPlantUnit()
{
	assert(mFarmPlantUnits.empty());

	int c = getLandSize();
	mFarmPlantUnits.resize(c);
	for ( int i = 0; i < c; ++i )
	{
		FarmPlantUnit* unit = new FarmPlantUnit(*this,i);
		mFarmPlantUnits[i] = unit;
	}
}
//-----------------------------------------------------------------------
void FarmScene::initFarmObject()
{
	assert(mFarmObjects.empty());

	for (std::map<std::string, FarmObjectInfo>::iterator it = mObjectInfos.begin(); it != mObjectInfos.end(); ++it)
	{
		FarmObjectInfo& foi = it->second;
		FarmObject* fo = new FarmObject(*this, foi.mName);
		mFarmObjects[foi.mName] = fo;
	}
}
//-----------------------------------------------------------------------
Ogre::SceneNode* FarmScene::getSceneNode()
{
	static const std::string name = "__FarmScene__";
	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
	if(!mgr->hasSceneNode(name))
		mgr->getRootSceneNode()->createChildSceneNode(name);

	return mgr->getSceneNode(name);
}
//-----------------------------------------------------------------------
void FarmScene::initialFromXml()
{
	TiXmlDocument doc;
	Ogre::DataStreamPtr stream = OpenResource(FARM_XML);
	doc.Parse( stream->getAsString().c_str() );

	TiXmlElement *ele, *ele1;
	TiXmlElement* root = doc.FirstChildElement("FarmScene");
	if ( root == NULL )
		EQ_EXCEPT( "Invaild farm scene!",  "FarmScene::initialFromXml");

	//土地信息
	ele = root->FirstChildElement("Land");
	if ( ele == NULL )
		EQ_EXCEPT( "Can't find 'Land'",  "FarmScene::initialFromXml");
	mLandInterval = xmlGetAttribFloat(ele, "interval", 80.0f);
	mSeedHeight = xmlGetAttribFloat(ele, "seedHeight", 80.0f);
	ele1 = ele->FirstChildElement("Area");
	while(ele1)
	{
		FarmLandAreaInfo info;
		info.beginPos = xmlGetAttribVector3(ele1, "pos");
		info.xSize = xmlGetAttribInt(ele1, "xSize");
		info.zSize = xmlGetAttribInt(ele1, "zSize");
		assert( info.xSize > 0 );
		assert( info.zSize > 0 );
		mLandAreaInfos.push_back(info);

		//
		ele1 = ele1->NextSiblingElement("Area");
	}

	//物件信息
	ele = root->FirstChildElement("Object");
	while(ele)
	{
		FarmObjectInfo foi;
		foi.mName = UTF8ToGB2312(xmlGetAttrib(ele, "name").c_str());
		foi.mPos = xmlGetAttribVector3(ele, "pos");
		foi.mYDegree = xmlGetAttribFloat(ele, "YDegree");

		foi.mMesh = UTF8ToGB2312(xmlGetAttrib(ele, "mesh").c_str());
		foi.mMtrl = UTF8ToGB2312(xmlGetAttrib(ele, "material").c_str());

		mObjectInfos[foi.mName] = foi;

		//
		ele = ele->NextSiblingElement("Object");
	}

	//misc
	ele = root->FirstChildElement("DefaultLand");
	mInitialLandMesh = UTF8ToGB2312(xmlGetAttrib(ele, "mesh").c_str());
	mInitialLandMaterial = UTF8ToGB2312(xmlGetAttrib(ele, "material").c_str());

	ele = root->FirstChildElement("Grass");
	mGrassMesh = UTF8ToGB2312(xmlGetAttrib(ele, "mesh").c_str());
	mGrassMaterial = UTF8ToGB2312(xmlGetAttrib(ele, "material").c_str());

	//
	int count = 0;
	ele = root->FirstChildElement("GrassPos");
	while(ele && count < GRASS_MAX)
	{
		FarmObjectInfo foi;
		mGrassPos[count] = xmlGetAttribVector3(ele, "pos");
		//
		ele = ele->NextSiblingElement("GrassPos");
		count++;
	}


	//扩建牌子
	ele = root->FirstChildElement("ExpandSign");
	mExpandSignMesh = UTF8ToGB2312(xmlGetAttrib(ele, "mesh").c_str());
	mExpandSignMaterial = UTF8ToGB2312(xmlGetAttrib(ele, "material").c_str());

	std::string elename="WasteLand";
	for(int i=LS_WASTE; i < LS_NUM; i++)
	{
		switch (i)
		{
		case LS_WASTE:
			elename = "WasteLand";
			break;
		case LS_NORMAL:
			elename = "NormalLand";
			break;
		case LS_DROUGHT_NORMAL:
			elename = "DroughtNormalLand";
			break;
		case LS_RED:
			elename = "RedLand";
			break;
		case LS_DROUGHT_RED:
			elename = "DroughtRedLand";
			break;
		}
		ele = root->FirstChildElement(elename);
		mLandStateMesh[i] = UTF8ToGB2312(xmlGetAttrib(ele, "mesh").c_str());
		mLandStateMaterial[i] = UTF8ToGB2312(xmlGetAttrib(ele, "material").c_str());
	}
	//初始化
	initPlantUnit();
	initFarmObject();
}
//-----------------------------------------------------------------------
void FarmScene::changeLand(int index, int landstate)
{
	assert(landstate>=0 && landstate < LS_NUM);

	mFarmPlantUnits[index]->changeLand(mLandStateMesh[landstate], mLandStateMaterial[landstate]);
}
//-----------------------------------------------------------------------
const Ogre::Vector3& FarmScene::getLandUintPos(int index)
{
	return mFarmPlantUnits[index]->getPos();
}



#if 0
//-----------------------------------------------------------------------
FarmLandAreaInfo newFarmLandAreaInfo(Ogre::Vector3 v, int x, int z)
{
	FarmLandAreaInfo info;
	info.beginPos = v;
	info.xSize = x;
	info.zSize = z;
	return info;
}

void FarmScene::tempInit()
{
	mInitialLandMesh = "nc_00.mesh";
	mGrassMesh = "outside/60000001.mesh";

	std::vector<FarmLandAreaInfo> lands;
	lands.push_back(newFarmLandAreaInfo(Ogre::Vector3(0,0,0), 4, 4));
	lands.push_back(newFarmLandAreaInfo(Ogre::Vector3(500,0,0), 5, 2));

	setFarmLandAreaInfo(lands);

	//
	FarmObjectInfo foi;
	foi.mName = "chiwei";
	foi.mPos = Ogre::Vector3(-400,0, -400);
	foi.mYDegree = -90;
	mObjectInfos[foi.mName] = foi;

	initFarmObject();

	setInitialState();

	changeFarmObject("chiwei", "Npc5001.mesh");
	seedGrass(1);
}
#endif


