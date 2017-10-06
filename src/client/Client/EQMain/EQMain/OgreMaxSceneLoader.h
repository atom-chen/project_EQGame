/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-18
* ����: OgreMax����.scene�ļ��ĵ�����
******************************************************************************/
#ifndef OgreMaxSceneLoader_H
#define OgreMaxSceneLoader_H

#include "EQMainPrerequisites.h"

#include "HouseRes.h"


/** OgreMax����.scene�ļ��ĵ�����
@remarks Ŀǰֻ���˰����ݶ�Ӧ����Ϊ���ݵ�����, �������ִ���
*/
class OgreMaxSceneLoader
{
public:

	/** 
	@remarks ִ�к���֮ǰ, Ӧ������������·��. 
	@return �������󷵻ؿ�ָ��
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

