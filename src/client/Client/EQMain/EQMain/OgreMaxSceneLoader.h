/******************************************************************************
* 作者: Y3
* 时间: 2009-11-18
* 描述: OgreMax导出.scene文件的导入器
******************************************************************************/
#ifndef OgreMaxSceneLoader_H
#define OgreMaxSceneLoader_H

#include "EQMainPrerequisites.h"

#include "HouseRes.h"


/** OgreMax导出.scene文件的导入器
@remarks 目前只做了把数据对应导入为房屋的内容, 其他部分待加
*/
class OgreMaxSceneLoader
{
public:

	/** 
	@remarks 执行函数之前, 应该先设置搜索路径. 
	@return 解析错误返回空指针
	*/
	static HouseResPtr directLoadToHouseType( const std::string& fileName );


protected:
	static void printHeaderMsg(TiXmlElement* XMLRoot);

	static bool parseOldDefMeshFile( const std::string& baseName, ushort& type, uchar& floor, uchar& room );

	//
	static void processScene(TiXmlElement* XMLRoot, HouseResPtr ptr);
	static void processNodes(TiXmlElement* XMLNode, HouseResPtr ptr);
	static void processNode(TiXmlElement* XMLNode, HouseResPtr ptr);
	static ComponenetHouseRes* processEntity(TiXmlElement* XMLNode, HouseResPtr ptr, const std::string& nodeName);
	static void processSubEntity(TiXmlElement* XMLNode, HouseResPtr ptr);
	static void buildHouseFloorsRelation(HouseResPtr ptr);
	

public:
	static Ogre::Vector3 parseVector3(TiXmlElement *XMLNode);
	static Ogre::Quaternion parseQuaternion(TiXmlElement *XMLNode);
	static Ogre::ColourValue parseColour(TiXmlElement *XMLNode);

protected:
	static ulong mCreateResNum;
};

#endif

