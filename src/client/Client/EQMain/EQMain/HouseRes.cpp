#include "EQMainPCH.h"

#include "HouseRes.h"
#include "SubHouseRes.h"
#include "CommFunc.h"
#include "AmbientMgr.h"
#include "HouseTable.h"
#include "ItemTable.h"

//-----------------------------------------------------------------------
HouseRes::HouseRes(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle,
				   const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader ) :
Resource(creator, name, handle, group, isManual, loader),
mFloorRootNO(0)
{
	mAddMatFullName.clear();
}
//-----------------------------------------------------------------------
HouseRes::~HouseRes()
{
	unload();
}
//-----------------------------------------------------------------------
FloorHouseRes* HouseRes::createFloorHouseRes(int no)
{
	if ( mFloorHouseResMap.find(no) != mFloorHouseResMap.end() )
	{
		EQ_EXCEPT("Duplicate NO",
			"HouseRes::createFloorHouseRes");
	}

	FloorHouseRes* sub = new FloorHouseRes;
	sub->mNO = no;

	mFloorHouseResMap.insert(FloorHouseResMap::value_type(no, sub));

	return sub;
}
//-----------------------------------------------------------------------
bool HouseRes::hasFloorHouseRes(int no) const
{
	return mFloorHouseResMap.find(no) != mFloorHouseResMap.end();
}
//-----------------------------------------------------------------------
FloorHouseRes* HouseRes::getFloorHouseRes(int no)
{
	FloorHouseResMap::iterator it = mFloorHouseResMap.find(no);
	if ( it == mFloorHouseResMap.end() )
		return NULL;
	else
		return it->second;
}
//-----------------------------------------------------------------------
ComponenetHouseRes* HouseRes::createComponenetHouseRes(const std::string& name)
{
	if ( mComponenetHouseResMap.find(name) != mComponenetHouseResMap.end() )
	{
		EQ_EXCEPT("Duplicate name '" + name + "'",
			"HouseRes::createComponenetHouseRes");
	}

	ComponenetHouseRes* sub = new ComponenetHouseRes;
	sub->mName = name;

	mComponenetHouseResMap.insert(ComponenetHouseResMap::value_type(name, sub));

	return sub;
}
//-----------------------------------------------------------------------
bool HouseRes::hasComponenetHouseRes(const std::string& name) const
{
	if ( mComponenetHouseResMap.find(name) == mComponenetHouseResMap.end() )
		return false;
	else
		return true;
}
//-----------------------------------------------------------------------
void HouseRes::exportClientXML(const std::string& fileName, bool clientUse)
{
	if ( !isLoaded() )
		EQ_EXCEPT("Resoucre is no loaded!", "HouseRes::exportClientXML");

	TiXmlDocument doc;

	TiXmlDeclaration dec;
	dec.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
	doc.InsertEndChild( dec );

	doc.InsertEndChild(TiXmlElement("House"));
	TiXmlElement* houseEle = doc.RootElement();

	{
		//Material
		TiXmlElement* materialEle = static_cast<TiXmlElement*>(houseEle->InsertEndChild(TiXmlElement("Material")));
		materialEle->SetAttribute("file", mAddMatFullName);

		//Floors
		TiXmlElement* flrsEle = static_cast<TiXmlElement*>(houseEle->InsertEndChild(TiXmlElement("Floors")));
		flrsEle->SetAttribute("rootNO", mFloorRootNO);

		for ( FloorHouseResMap::iterator it = mFloorHouseResMap.begin(); it != mFloorHouseResMap.end(); ++it )
		{
			FloorHouseRes* flr = it->second;
			TiXmlElement* flrEle = static_cast<TiXmlElement*>(flrsEle->InsertEndChild(TiXmlElement("Floor")));
			flrEle->SetAttribute("NO", flr->mNO);
			if ( flr->mUpFloorNO < 256 )
				flrEle->SetAttribute("upFloorNO", flr->mUpFloorNO);
			flrEle->SetAttribute("height", flr->mHeight);

			{//Grids
				TiXmlElement* gridsEle = static_cast<TiXmlElement*>(flrEle->InsertEndChild(TiXmlElement("Grids")));
				for ( std::vector<FloorHouseRes::Grid>::iterator it = flr->mGrids.begin(); it != flr->mGrids.end(); ++it )
				{
					FloorHouseRes::Grid& grid = *it;
					TiXmlElement* gridEle = static_cast<TiXmlElement*>(gridsEle->InsertEndChild(TiXmlElement("Grid")));
					gridEle->SetAttribute("type", grid.mType);
					gridEle->SetAttribute("x", grid.mX);
					gridEle->SetAttribute("z", grid.mZ);
					gridEle->SetAttribute("room", grid.mRoom);
				}
			}

			{//Walls
				TiXmlElement* wallsEle = static_cast<TiXmlElement*>(flrEle->InsertEndChild(TiXmlElement("Walls")));
				for ( std::vector<FloorHouseRes::Wall>::iterator it = flr->mWalls.begin(); it != flr->mWalls.end(); ++it )
				{
					FloorHouseRes::Wall& wall = *it;
					TiXmlElement* wallEle = static_cast<TiXmlElement*>(wallsEle->InsertEndChild(TiXmlElement("Wall")));
					wallEle->SetAttribute("x0", wall.mX0);
					wallEle->SetAttribute("z0", wall.mZ0);
					wallEle->SetAttribute("x1", wall.mX1);
					wallEle->SetAttribute("z1", wall.mZ1);
				}
			}
		}
	}

	{
		//Components
		TiXmlElement* compsEle = static_cast<TiXmlElement*>(houseEle->InsertEndChild(TiXmlElement("Components")));

		for ( ComponenetHouseResMap::iterator it = mComponenetHouseResMap.begin(); it != mComponenetHouseResMap.end(); ++it )
		{
			ComponenetHouseRes* comp = it->second;

			//客户端不保存非框架、非门窗类部件
			if(clientUse)
			{
				if(!comp->mFrame)
				{
					const HouseCompRow* comprow = HouseTableManager::getSingleton().getHouseCompRow(comp->mCompId);
					const HouseCompPropRow* prorow = HouseTableManager::getSingleton().getHouseCompPropRow(comprow->PropID);
					bool isDoorOrWin = prorow->CanRep && (prorow->HoldType != 0);
					if(!isDoorOrWin)
						continue;
				}
			}

			TiXmlElement* compEle = static_cast<TiXmlElement*>(compsEle->InsertEndChild(TiXmlElement("Comp")));

			compEle->SetAttribute("name", GB2312ToUTF8(comp->mName.c_str()));
			if ( comp->mFrame )
			{
				compEle->SetAttribute("frame", "true");
				compEle->SetAttribute("compTyp", comp->mFrmCompType);
				compEle->SetAttribute("mesh", GB2312ToUTF8(comp->mFrmMesh.c_str()));
				if ( comp->mFrmPos != Ogre::Vector3::ZERO )
					compEle->SetAttribute("pos", Ogre::StringConverter::toString(comp->mFrmPos));
				if ( comp->mFrmRot != Ogre::Quaternion::IDENTITY )
					compEle->SetAttribute("rot", Ogre::StringConverter::toString(comp->mFrmRot));
				if ( comp->mFrmScale != Ogre::Vector3::UNIT_SCALE )
					compEle->SetAttribute("scale", Ogre::StringConverter::toString(comp->mFrmScale));
			}
			else
			{
				compEle->SetAttribute("compId", comp->mCompId);
			}
			if ( !comp->mFrame )
			{
				compEle->SetAttribute("x", comp->mX);
				compEle->SetAttribute("z", comp->mZ);
			}
			if ( comp->mBottom != 0 ) compEle->SetAttribute("height", comp->mBottom);
			compEle->SetAttribute("flr", comp->mFlr);
			compEle->SetAttribute("room", comp->mRoom);
			if ( comp->mFace != 0 ) compEle->SetAttribute("face", comp->mFace);
			if ( comp->mLayer != 0 ) compEle->SetAttribute("layer", comp->mLayer);
		}
	}

	{
		//连接格子
		TiXmlElement* compsEle = static_cast<TiXmlElement*>(houseEle->InsertEndChild(TiXmlElement("LinkGrids")));
		for ( LinkGridList::iterator it = mLinkGridList.begin(); it != mLinkGridList.end(); ++it )
		{
			TiXmlElement* compEle = static_cast<TiXmlElement*>(compsEle->InsertEndChild(TiXmlElement("LinkGrid")));

			compEle->SetAttribute("startFloorNo", it->mStartGrid.mFloorNo );
			compEle->SetAttribute("startX", it->mStartGrid.mX );
			compEle->SetAttribute("startZ", it->mStartGrid.mZ );

			compEle->SetAttribute("endFloorNo", it->mEndGrid.mFloorNo );
			compEle->SetAttribute("endX", it->mEndGrid.mX );
			compEle->SetAttribute("endZ", it->mEndGrid.mZ );
		}
	}

	doc.SaveFile(fileName);
}
//-----------------------------------------------------------------------
void HouseRes::exportServerXML(const std::string& fileName)
{
	TiXmlDocument doc;

	TiXmlDeclaration dec;
	dec.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
	doc.InsertEndChild( dec );

	doc.InsertEndChild(TiXmlElement("root"));
	TiXmlElement* root = doc.RootElement();

	{
		uint id=0;
		for ( ComponenetHouseResMap::iterator it = mComponenetHouseResMap.begin(); it != mComponenetHouseResMap.end(); ++it )
		{
			ComponenetHouseRes* comp = it->second;

			//服务端不保存框架、门窗类部件
			{
				if(comp->mFrame)
					continue;

				const HouseCompRow* comprow = HouseTableManager::getSingleton().getHouseCompRow(comp->mCompId);
				const HouseCompPropRow* prorow = HouseTableManager::getSingleton().getHouseCompPropRow(comprow->PropID);
				bool isDoorOrWin = prorow->CanRep && (prorow->HoldType != 0);
				if(isDoorOrWin)
					continue;
			}

			TiXmlElement* tplt = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("house_furniture_tplt")));

			TiXmlElement* ele;
			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("id")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(++id)));

			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("furniture_id")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(comp->mCompId)));
 
 			ItemTable table = ItemTableManager::getSingleton().getItemTable();
 			ItemTable::const_iterator cit;
 			for(cit=table.begin(); cit!=table.end(); cit++)
 			{
 				if(cit->second.type != 2)
 					continue;
 
 				if(cit->second.subID == comp->mCompId)
 				{
					ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("item_temp_id")));
					ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(cit->second.id)));
 					break;
 				}
 			}

			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("x")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(comp->mX)));

			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("z")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(comp->mZ)));

			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("height")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(comp->mBottom)));

			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("flr")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(comp->mFlr)));

			ele = static_cast<TiXmlElement*>(tplt->InsertEndChild(TiXmlElement("face")));
			ele->InsertEndChild(TiXmlText(Ogre::StringConverter::toString(comp->mFace)));
		}
	}

	doc.SaveFile(fileName);
}
//-----------------------------------------------------------------------
void HouseRes::importXML(Ogre::DataStreamPtr stream)
{
	TiXmlDocument doc;
	doc.Parse( stream->getAsString().c_str() );

	TiXmlElement* eleRoot;

	eleRoot = doc.FirstChildElement("House");
	if(eleRoot)
	{
		TiXmlElement* ele;
		ele = eleRoot->FirstChildElement("Material");
		if( ele )
		{
			std::string mat = xmlGetAttrib(ele, "file", "");
			if(!mat.empty())
				mAddMatFullName = mat;
		}

		ele = eleRoot->FirstChildElement("Floors");
		if ( ele )
			processFloors(ele);

		ele = eleRoot->FirstChildElement("Components");
		if ( ele )
			processComponents(ele);

		ele = eleRoot->FirstChildElement("LinkGrids");
		if ( ele )
			processLinkGrids(ele);
	}
	else
		LOGERR(mName + " is not a HouseRes!");

}
//-----------------------------------------------------------------------
void HouseRes::processFloors(TiXmlElement* node)
{
	TiXmlElement* ele;

	mFloorRootNO = xmlGetAttribInt(node, "rootNO", 256);

	ele = node->FirstChildElement("Floor");
	while(ele)
	{
		FloorHouseRes* flr = createFloorHouseRes(xmlGetAttribInt(ele, "NO"));
		flr->mUpFloorNO = xmlGetAttribInt(ele, "upFloorNO", 256);
		flr->mHeight = xmlGetAttribInt(ele, "height");

		TiXmlElement* el;
		el = ele->FirstChildElement("Grids");
		if ( el )
			processGrids(el, flr);

		el = ele->FirstChildElement("Walls");
		if ( el )
			processWalls(el, flr);

		ele = ele->NextSiblingElement("Floor");
	}
}
//-----------------------------------------------------------------------
void HouseRes::processGrids(TiXmlElement* node, FloorHouseRes* flr)
{
	TiXmlElement* ele;

	ele = node->FirstChildElement("Grid");
	while(ele)
	{
		FloorHouseRes::Grid grid;
		grid.mType = (ushort)xmlGetAttribUint(ele, "type");
		grid.mX = xmlGetAttribInt(ele, "x");
		grid.mZ = xmlGetAttribInt(ele, "z");
		grid.mRoom = xmlGetAttribInt(ele, "room");
		flr->mGrids.push_back(grid);

		ele = ele->NextSiblingElement("Grid");
	}
}
//-----------------------------------------------------------------------
void HouseRes::processWalls(TiXmlElement* node, FloorHouseRes* flr)
{
	TiXmlElement* ele;

	ele = node->FirstChildElement("Wall");
	while(ele)
	{
		FloorHouseRes::Wall wall;
		wall.mX0 = xmlGetAttribInt(ele, "x0");
		wall.mZ0 = xmlGetAttribInt(ele, "z0");
		wall.mX1 = xmlGetAttribInt(ele, "x1");
		wall.mZ1 = xmlGetAttribInt(ele, "z1");
		flr->mWalls.push_back(wall);

		ele = ele->NextSiblingElement("Wall");
	}
}
//-----------------------------------------------------------------------
void HouseRes::processComponents(TiXmlElement* node)
{
	TiXmlElement* ele;

	ele = node->FirstChildElement("Comp");
	while(ele)
	{
		processComponent(ele);

		ele = ele->NextSiblingElement("Comp");
	}
}
//-----------------------------------------------------------------------
void HouseRes::processLinkGrids(TiXmlElement* node)
{	
	mLinkGridList.clear();

	for ( TiXmlElement* ele = node->FirstChildElement("LinkGrid"); ele != NULL; ele = ele->NextSiblingElement("LinkGrid") )
	{
		LinkGridRes lgr;
		lgr.mStartGrid.mFloorNo = xmlGetAttribInt(ele, "startFloorNo");
		lgr.mStartGrid.mX = xmlGetAttribInt(ele, "startX");
		lgr.mStartGrid.mZ = xmlGetAttribInt(ele, "startZ");

		lgr.mEndGrid.mFloorNo = xmlGetAttribInt(ele, "endFloorNo");
		lgr.mEndGrid.mX = xmlGetAttribInt(ele, "endX");
		lgr.mEndGrid.mZ = xmlGetAttribInt(ele, "endZ");
		
		mLinkGridList.push_back( lgr );
	}
}
//-----------------------------------------------------------------------
void HouseRes::processComponent(TiXmlElement* node)
{
	ComponenetHouseRes* res = createComponenetHouseRes(UTF8ToGB2312(xmlGetAttrib(node, "name").c_str()));
	res->mFrame = xmlGetAttribBool(node, "frame");

	if ( res->mFrame )
	{
		res->mFrmCompType = (ushort)xmlGetAttribUint(node, "compTyp");
		res->mFrmMesh = UTF8ToGB2312(xmlGetAttrib(node, "mesh").c_str());
		res->mFrmPos = xmlGetAttribVector3(node, "pos");
		res->mFrmRot = xmlGetAttribQuaternion(node, "rot");
		res->mFrmScale = xmlGetAttribVector3(node, "scale", Ogre::Vector3::UNIT_SCALE);
	}
	else
	{
		res->mCompId = xmlGetAttribUint(node, "compId");
		res->mBottom = xmlGetAttribUint(node, "height");
	}

	res->mFlr = xmlGetAttribInt(node, "flr");
	res->mRoom = xmlGetAttribInt(node, "room");
	res->mX = xmlGetAttribInt(node, "x");
	res->mZ = xmlGetAttribInt(node, "z");
	res->mFace = xmlGetAttribInt(node, "face");
	res->mLayer = xmlGetAttribInt(node, "layer");
}

//-----------------------------------------------------------------------
void HouseRes::loadImpl(void)
{
	std::string baseName;
	std::string ExtName;
	std::string pathName;
	Ogre::StringUtil::splitFullFilename(mName, baseName, ExtName, pathName);
	mAddPath = pathName;

	if(mAddMatFullName.empty())
		mAddMatFullName = pathName + baseName + ".material";

	importXML(OpenResource(mName));
}
//-----------------------------------------------------------------------
void HouseRes::unloadImpl()
{
	//
	for (ComponenetHouseResMap::iterator i = mComponenetHouseResMap.begin();
		i != mComponenetHouseResMap.end(); ++i)
	{
		delete i->second;
	}
	mComponenetHouseResMap.clear();

	//
	for (FloorHouseResMap::iterator i = mFloorHouseResMap.begin();
		i != mFloorHouseResMap.end(); ++i)
	{
		delete i->second;
	}
	mFloorHouseResMap.clear();

	//
	mFloorRootNO = 0;

	mAddPath = "";
	mAddMatFullName = "";
}
//-----------------------------------------------------------------------
