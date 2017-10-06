#include "HouseEditorPCH.h"
#include "FarmScript.h"
#include "FarmScene.h"
#include "SceneManager.h"
#include "EQLog.h"
#include "CommFunc.h"
#include "EQException.h"
#include "OperatorManager.h"
#include "RootManager.h"

static void logerr(int n)
{
	LOGERR("农场脚本-执行指令"+ Ogre::StringConverter::toString(n) +"出错!");
}

static void log(int n)
{
	LOG("农场脚本-执行指令"+ Ogre::StringConverter::toString(n) +"成功!");
}

void runFarmScript()
{
	FarmScene* fs = EQSceneManager::getSingleton().getFarmScene();
	if ( !fs )
		return;

	TiXmlDocument doc;
	Ogre::DataStreamPtr stream = OpenResource("FarmScript.xml");
	doc.Parse( stream->getAsString().c_str() );

	TiXmlElement *ele;
	TiXmlElement* root = doc.FirstChildElement("FarmScript");
	if ( root == NULL )
		EQ_EXCEPT( "Invaild FarmScript.xml!",  "runFarmScript");

	LOG(" ");
	LOG("农场脚本-开始执行!");

	int cn = 1;
	ele = root->FirstChildElement("CMD");
	while(ele)
	{
		std::string f = xmlGetAttrib(ele, "f");
		std::string p0 = xmlGetAttrib(ele, "p0");
		std::string p1 = xmlGetAttrib(ele, "p1");
		std::string p2 = xmlGetAttrib(ele, "p2");
		if(f == "setInitialState")
		{
			fs->setInitialState();
		}
		else if (f == "changeLand")
		{
			int ix = Ogre::StringConverter::parseInt(p0);
			fs->changeLand(ix, p1, p2);
		}
		else if (f == "resetLand")
		{
			int ix = Ogre::StringConverter::parseInt(p0);
			fs->resetLand(ix);
		}
		else if (f == "seedPlant")
		{
			int ix = Ogre::StringConverter::parseInt(p0);
			fs->seedPlant(ix, p1, p2);
		}
		else if (f == "removePlant")
		{
			int ix = Ogre::StringConverter::parseInt(p0);
			fs->removePlant(ix);
		}
		else if (f == "seedGrass")
		{
			int ix = Ogre::StringConverter::parseInt(p0);
			fs->seedGrass(ix);
		}
		else if (f == "removeGrass")
		{
			int ix = Ogre::StringConverter::parseInt(p0);
			fs->removeGrass(ix);
		}
		else if (f == "changeFarmObject")
		{
			fs->changeFarmObject(p0, p1, p2);
		}
		else if (f == "removeFarmObject")
		{
			fs->removeFarmObject(p0);
		}
		else if (f == "loadSceneInfo")
		{
			RootManager::getSingleton().getOperatorManager()->reset();
			EQSceneManager::getSingleton().loadSceneInfo(p0,true);
		}
		else
		{
			logerr(cn);
			return;
		}

		log(cn);

		//
		ele = ele->NextSiblingElement("CMD");
		++cn;
	}

	LOG("农场脚本-执行结束!");
}



