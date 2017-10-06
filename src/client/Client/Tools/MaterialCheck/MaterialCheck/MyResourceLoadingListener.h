/******************************************************************************
* ����: XiaoMing
* ʱ��: 2010-05-10
* ����: Material��Դ�ظ������������ʾ
******************************************************************************/
#ifndef MyResourceLoadingListener_H
#define MyResourceLoadingListener_H

#include "OgreResourceGroupManager.h"
#include "OgreScriptCompiler.h"


// ��������
enum ErrorType
{
	BRACK_NO_MATCH, // ���Ų�ƥ��
	NONE_BRACK,		// û������
	COMPLIER_ERROR, // �������
	COLLISION_NAME, // �����ظ�

	WARING_NO_PATH, // ���� .dssǰû�м�·��

};



class MyResourceLoadingListener:public Ogre::ResourceLoadingListener
{
public:
	Ogre::DataStreamPtr resourceLoading(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource);

	// һ����Դ�ļ����� �� all.material
	void resourceStreamOpened(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource, Ogre::DataStreamPtr& dataStream);
	// ��Դ��ײ���
	bool resourceCollision(Ogre::Resource *resource, Ogre::ResourceManager *resourceManager);
};



// �ü�������OGRE����ű��ļ���ʱ��ʹ��
class MyScriptCompilerListener:public Ogre::ScriptCompilerListener
{
	// ���������
	void handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg);
	//void preConversion(Ogre::ScriptCompiler *compiler, Ogre::ConcreteNodeListPtr nodes);
};

#endif