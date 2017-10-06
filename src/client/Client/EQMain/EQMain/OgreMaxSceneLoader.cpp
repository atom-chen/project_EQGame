#include "EQMainPCH.h"

#include "OgreMaxSceneLoader.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "HouseResManager.h"
#include "SubHouseRes.h"
#include "HouseTable.h"


//-----------------------------------------------------------------------
ulong OgreMaxSceneLoader::mCreateResNum = 0;
//-----------------------------------------------------------------------
HouseResPtr OgreMaxSceneLoader::directLoadToHouseType( const std::string& fileName )
{
	std::string baseName;
	std::string ExtName;
	std::string pathName;
	Ogre::StringUtil::splitFullFilename(fileName, baseName, ExtName, pathName);
	std::string matName = pathName + baseName + ".material";
	if ( !Ogre::ResourceGroupManager::getSingleton().resourceExists(
		"OgreMax", matName) )
		matName = "";

	Ogre::DataStreamPtr stream = OpenResource(fileName);

	TiXmlDocument xml;

	xml.Parse( stream->getAsString().c_str() );

	TiXmlElement* pRoot = xml.RootElement();


	if( !pRoot || std::string( pRoot->Value()) != "scene"  ) {
		LOGWAR("[OgreMaxDotScene] Error: Invalid .scene File. Missing <scene>");
		return HouseResPtr();
	}

	LOG("Begin load OgreMax .scene file '" + fileName + "'");
	printHeaderMsg(pRoot);

	std::string resName = fileName + "-Manual-" + Ogre::StringConverter::toString(++mCreateResNum);
	HouseResPtr ptr = HouseResManager::getSingleton().createManual(resName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, &g_NullManualResourceLoader );

	processScene(pRoot, ptr);

	ptr->mAddMatFullName = matName;

	//这句是必须的, 否则内存泄露
	ptr->load();

	LOG("Load OgreMax .scene file '" + fileName + "' OK!");
	return ptr;
}
//-----------------------------------------------------------------------
void OgreMaxSceneLoader::printHeaderMsg(TiXmlElement* XMLRoot)
{
	// Process the scene parameters
	std::string version = xmlGetAttrib(XMLRoot, "formatVersion", "unknown");

	std::string message = "[DotSceneLoader] Parsing dotScene file with version " + version;
	if(XMLRoot->Attribute("upAxis"))
		message += ", upAxis " + std::string(XMLRoot->Attribute("upAxis"));
	if(XMLRoot->Attribute("unitsPerMeter"))
		message += ", unitsPerMeter " + std::string(XMLRoot->Attribute("unitsPerMeter"));
	if(XMLRoot->Attribute("minOgreVersion"))
		message += ", min. Ogre version " + std::string(XMLRoot->Attribute("minOgreVersion"));
	if(XMLRoot->Attribute("author"))
		message += ", author " + std::string(XMLRoot->Attribute("author"));

	LOG(message);
}
//-----------------------------------------------------------------------
bool OgreMaxSceneLoader::parseOldDefMeshFile( const std::string& baseName, ushort& type, uchar& floor, uchar& room )
{
	std::vector<std::string> strVec = Ogre::StringUtil::split(baseName, "_");

	bool flag = false;
	std::vector<std::string>::iterator it;
	for ( it = strVec.begin(); it != strVec.end(); ++it )
	{
		if ( *it == "groundbase" || *it == "static")
		{
			type = HCP_STATIC;
			flag = true;
		}
		else if ( *it == "innerwall" || *it == "inneredge" )
		{
			type = HCP_IN_WALL;
			flag = true;
		}
		else if ( *it == "outwall" || *it == "outedge" )
		{
			type = HCP_OUT_WALL;
			flag = true;
		}
		else if ( *it == "floor" )
		{
			type = HCP_FLOOR;
			flag = true;
		}
		else if ( *it == "roof" )
		{
			type = HCP_ROOF;
			flag = true;
		}
		else if ( *it == "tcx" || *it == "edgetcx" )
		{
			type = HCP_TCX;
			flag = true;
		}
		else
		{
			type = HCP_NONE;
			flag = false;
		}

		if ( flag )
		{
			std::vector<std::string>::iterator itt = it;
			++itt;
			if ( itt == strVec.end() || !Ogre::StringConverter::isNumber(*itt) )
			{
				flag = false;
				break;
			}

			floor = (uchar)(Ogre::StringConverter::parseInt(*itt)/100);
			room = (uchar)(Ogre::StringConverter::parseInt(*itt)%100);
			break;
		}
	}

	return flag;
}
//-----------------------------------------------------------------------
void OgreMaxSceneLoader::processScene(TiXmlElement* XMLRoot, HouseResPtr ptr)
{
	TiXmlElement *pElement;

	pElement = XMLRoot->FirstChildElement("nodes");
	if(pElement)
		processNodes(pElement, ptr);

	buildHouseFloorsRelation(ptr);
}
//-----------------------------------------------------------------------
void OgreMaxSceneLoader::processNodes(TiXmlElement* XMLNode, HouseResPtr ptr)
{
	TiXmlElement *pElement;

	pElement = XMLNode->FirstChildElement("node");
	while(pElement)
	{
		processNode(pElement, ptr);
		pElement = pElement->NextSiblingElement("node");
	}
}
//-----------------------------------------------------------------------
void OgreMaxSceneLoader::processNode(TiXmlElement* XMLNode, HouseResPtr ptr)
{
	std::string name = xmlGetAttrib(XMLNode, "name");

	Ogre::Vector3 pos = Ogre::Vector3::ZERO;
	Ogre::Quaternion rot = Ogre::Quaternion::IDENTITY;
	Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE;

	if ( ptr->hasComponenetHouseRes(name) )
	{
		EQ_EXCEPT(".scene has duplicate node name '" + name + "'",
			"OgreMaxSceneLoader::processNode");
	}

	ComponenetHouseRes* compRes = NULL;

	//
	TiXmlElement *pElement;

	pElement = XMLNode->FirstChildElement("scale");
	if(pElement)
	{
		scale = parseVector3(pElement);
	}

	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
	{
		pos = parseVector3(pElement);
	}

	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
	{
		rot = parseQuaternion(pElement);
	}

	pElement = XMLNode->FirstChildElement("entity");
	if(pElement)
	{
		compRes = processEntity(pElement,ptr,name);
	}

	//没有找到对应的房屋资源则退出.
	if ( !compRes )
		return;


	compRes->mFrmPos = pos;
	compRes->mFrmRot = rot;
	compRes->mFrmScale = scale;

}
//-----------------------------------------------------------------------
ComponenetHouseRes* OgreMaxSceneLoader::processEntity(TiXmlElement* XMLNode, HouseResPtr ptr, const std::string& nodeName)
{
	//std::string name = xmlGetAttrib(XMLNode, "name");
	std::string meshFile = xmlGetAttrib(XMLNode, "meshFile");
	bool castShadows = xmlGetAttribBool(XMLNode, "castShadows");
	bool receiveShadows = xmlGetAttribBool(XMLNode, "receiveShadows");

	//分析房屋类型
	std::string baseName;
	std::string tmp1;
	std::string tmp2;
	Ogre::StringUtil::splitFullFilename(meshFile, baseName, tmp1, tmp2);

	ushort type;
	uchar floor;
	uchar room;
	if ( !parseOldDefMeshFile(baseName, type, floor, room) )
		return NULL;

	ComponenetHouseRes* comp = ptr->createComponenetHouseRes(nodeName);

	comp->mFrame = true;
	comp->mFrmCompType = type;
	comp->mFrmMesh = meshFile;
	comp->mFlr = floor;
	comp->mRoom = room;

	// 确保有这个楼层且有这个框架网格
	if ( !ptr->hasFloorHouseRes(floor) )
	{
		ptr->createFloorHouseRes(floor);
	}


	TiXmlElement *pElement;

	pElement = XMLNode->FirstChildElement("subentities");
	while(pElement)
	{
		processSubEntity(pElement, ptr);
		pElement = pElement->NextSiblingElement("subentities");
	}

	return comp;
}
//-----------------------------------------------------------------------
void OgreMaxSceneLoader::processSubEntity(TiXmlElement* XMLNode, HouseResPtr ptr)
{
	// nothing
}
//-----------------------------------------------------------------------
void OgreMaxSceneLoader::buildHouseFloorsRelation(HouseResPtr ptr)
{
	HouseRes::FloorHouseResMap::reverse_iterator it;
	int a = 256;
	for ( it = ptr->mFloorHouseResMap.rbegin(); it != ptr->mFloorHouseResMap.rend(); ++it )
	{
		if ( a < 256 )
			it->second->mUpFloorNO = a;

		a = it->second->mNO;
	}

	ptr->mFloorRootNO = a;
}
//-----------------------------------------------------------------------
Ogre::Vector3 OgreMaxSceneLoader::parseVector3(TiXmlElement *XMLNode)
{
	return Ogre::Vector3(
		Ogre::StringConverter::parseReal(XMLNode->Attribute("x")),
		Ogre::StringConverter::parseReal(XMLNode->Attribute("y")),
		Ogre::StringConverter::parseReal(XMLNode->Attribute("z"))
		);
}
//-----------------------------------------------------------------------
Ogre::Quaternion OgreMaxSceneLoader::parseQuaternion(TiXmlElement *XMLNode)
{
	Ogre::Quaternion orientation;

	if(XMLNode->Attribute("qx"))
	{
		orientation.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("qx"));
		orientation.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("qy"));
		orientation.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("qz"));
		orientation.w = Ogre::StringConverter::parseReal(XMLNode->Attribute("qw"));
	}
	else if(XMLNode->Attribute("axisX"))
	{
		assert( 0 && "还没碰到这情况, 如果你在这里断言了, 检查下正确性, 就断言删了!" );

		Ogre::Vector3 axis;
		axis.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisX"));
		axis.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisY"));
		axis.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisZ"));
		Ogre::Real angle = Ogre::StringConverter::parseReal(XMLNode->Attribute("angle"));;
		orientation.FromAngleAxis(Ogre::Angle(angle), axis);
	}
	else if(XMLNode->Attribute("angleX"))
	{
		assert( 0 && "还没碰到这情况, 如果你在这里断言了, 检查下正确性, 就断言删了!" );

		Ogre::Vector3 axis;
		axis.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleX"));
		axis.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleY"));
		axis.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleZ"));
		//orientation.FromAxes(&axis);
		//orientation.F
	}

	return orientation;
}
//-----------------------------------------------------------------------
Ogre::ColourValue OgreMaxSceneLoader::parseColour(TiXmlElement *XMLNode)
{
	return Ogre::ColourValue(
		Ogre::StringConverter::parseReal(XMLNode->Attribute("r")),
		Ogre::StringConverter::parseReal(XMLNode->Attribute("g")),
		Ogre::StringConverter::parseReal(XMLNode->Attribute("b")),
		XMLNode->Attribute("a") != NULL ? Ogre::StringConverter::parseReal(XMLNode->Attribute("a")) : 1
		);
}
