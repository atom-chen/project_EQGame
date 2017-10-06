/******************************************************************************
* 作者: 锦旭
* 时间: 2011-08-16
* 描述: 室内寻路
******************************************************************************/

#ifndef IndoorPath_H
#define IndoorPath_H

class HouseScene;

class IndoorPath
{
public:
	static void findPath(const Ogre::Vector3 & startPos, const Ogre::Vector3 & endPos, 
		std::queue<Ogre::Vector3> & path, HouseScene * hs);
};

#endif


