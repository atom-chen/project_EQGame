/******************************************************************************
* 作者: XiaoMing
* 时间: 2010-05-10
* 描述: Material资源重复及编译错误提示
******************************************************************************/
#ifndef MyResourceLoadingListener_H
#define MyResourceLoadingListener_H

#include "OgreResourceGroupManager.h"
#include "OgreScriptCompiler.h"


// 错误类型
enum ErrorType
{
	BRACK_NO_MATCH, // 括号不匹配
	NONE_BRACK,		// 没有括号
	COMPLIER_ERROR, // 编译错误
	COLLISION_NAME, // 名字重复

	WARING_NO_PATH, // 警告 .dss前没有加路径

};



class MyResourceLoadingListener:public Ogre::ResourceLoadingListener
{
public:
	Ogre::DataStreamPtr resourceLoading(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource);

	// 一个资源文件被打开 如 all.material
	void resourceStreamOpened(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource, Ogre::DataStreamPtr& dataStream);
	// 资源碰撞检测
	bool resourceCollision(Ogre::Resource *resource, Ogre::ResourceManager *resourceManager);
};



// 该监听器在OGRE编译脚本文件的时候使用
class MyScriptCompilerListener:public Ogre::ScriptCompilerListener
{
	// 编译错误处理
	void handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg);
	//void preConversion(Ogre::ScriptCompiler *compiler, Ogre::ConcreteNodeListPtr nodes);
};

#endif