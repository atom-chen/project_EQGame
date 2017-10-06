#include "MyResourceLoadingListener.h"
#include <assert.h>
#include <iostream>
#include <windows.h>
#include <vector>

// ������ ��Ӧֻ����һ��������Ϣ
void ErrorInfo(ErrorType type, const Ogre::String & outPutTxt, const char * buf );

//--------------------------------------------------------------------------
bool MyResourceLoadingListener::resourceCollision(Ogre::Resource *resource, Ogre::ResourceManager *resourceManager)
{
	assert(resource);
	assert(resourceManager);
	
	if(resourceManager->resourceExists( resource->getName()))
	{
		// ��ȡ��ͻ���ļ���
		std::string fileName = resourceManager->getByName(resource->getName())->getOrigin();
		
		char buf[1024];
		sprintf(buf, "��%s�ļ��е� %s �����ظ�", fileName.c_str(), resource->getName().c_str() );

		ErrorInfo(COLLISION_NAME, "ResourceCollision.txt", buf);
		return false;
	}
	else
		return true;

}

//--------------------------------------------------------------------------
Ogre::DataStreamPtr MyResourceLoadingListener::resourceLoading(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource)
{
	return Ogre::DataStreamPtr();
}

//--------------------------------------------------------------------------
void MyResourceLoadingListener::resourceStreamOpened(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource, Ogre::DataStreamPtr& dataStream)
{

	// ����ǰ�������ļ���д��LOG�ļ�
	std::ofstream outPut;
	outPut.open("ResourceCollision.txt", std::ios::app);
	outPut<<"��ʼ���"<<name<<std::endl;
	outPut.close();

	outPut.open("brackMatch.txt", std::ios::app);
	outPut<<"��ʼ���"<<name<<std::endl;
	outPut.close();

	outPut.open("noPath.txt", std::ios::app);
	outPut<<"��ʼ���"<<name<<std::endl;
	outPut.close();

	char buf[1024];

	// ��ǰ��
	int curLine = 0;
	// ��ǰ����Material������
	int curMaterialLine = 0;
	int materialCount   = 0;

	int leftBrackCount  = 0;
	int rightBrackCount = 0;

	std::vector< Ogre::String > splitResult;
	Ogre::String materialName;

	// ����dataStream ���{}ƥ�� �Լ�.dds ��û��·��ǰ׺
	Ogre::String text;
	while(!dataStream->eof())
	{
		text = dataStream->getLine();
		curLine++;

		Ogre::StringUtil::trim(text);

		if(text == "")
			continue;

		// ����ע��	
		size_t offset =  text.find("//");
		if(offset < text.size())
		{
			// ���ж�Ϊע��
			if(Ogre::StringUtil::startsWith(text, "//"))
				continue;
			else
			{// ע�����м�
				text.erase(offset);
			}	
		}

		// ���.dds��û��ǰ׺·�� ֻ���򵥼��
		offset = text.find(".dds");
		if(offset < text.size())
		{
			offset = text.find("/");
			if(offset > text.size())
			{
				sprintf(buf, "��%d��.dss����û�����·��", curLine);
				ErrorInfo(WARING_NO_PATH, "noPath.txt", buf);
			}

		}

		// ����Ϊmaterial��ʼ��
		offset = text.find("material");
		if(offset == 0)
		{
			materialCount ++;
			
			if(materialCount != 1 && leftBrackCount == 0 && leftBrackCount == 0)
			{
				// ���������̨��LOG�ļ�
				sprintf(buf, "��%d�� %s û������", curMaterialLine, materialName.c_str() );
				ErrorInfo(NONE_BRACK, "brackMatch.txt", buf);
			}


			if(leftBrackCount != rightBrackCount)
			{
				// ���Ų�ƥ�� ����
				sprintf(buf, "��%d�� %s ���Ų�ƥ��", curMaterialLine, materialName.c_str());
			    ErrorInfo(BRACK_NO_MATCH, "brackMatch.txt", buf);
			}

			curMaterialLine = curLine;
			leftBrackCount = rightBrackCount = 0;
			materialName = "";

			// ��ȡ���ʵ�����
			size_t indexL = 0;
			size_t indexR = 0;
			size_t min = 0;

			splitResult = Ogre::StringUtil::split(text, " ");
			if(splitResult.size() <= 1)
			{
				// waring�ò���û������
			}
			else
			{
				materialName = splitResult[1];
				
				size_t size = materialName.size();

				indexL = materialName.find("{");
				indexR = materialName.find("}");
			
				min = (indexL<indexR) ? indexL:indexR;

				if(min < size)
					materialName.erase(min);

				Ogre::StringUtil::trim(materialName);
				
			}

			while((offset = text.find("{", offset + 1)) < text.size())
			{
				leftBrackCount++;
			}

			while((offset = text.find("}", offset + 1)) < text.size())
			{
				rightBrackCount++;
			}
			continue;
		}	

		// �����������
		while((offset = text.find("{", offset + 1)) < text.size())
		{
			leftBrackCount++;
		}

		while((offset = text.find("}", offset + 1)) < text.size())
		{
			rightBrackCount++;
		}

	}

	// ������һ��Material
	if(materialCount != 1 && leftBrackCount == 0 && leftBrackCount == 0)
	{
		// ���
		sprintf(buf, "��%d�� %s û������", curMaterialLine, materialName.c_str() );
		ErrorInfo(NONE_BRACK, "brackMatch.txt", buf);
	}


	if(leftBrackCount != rightBrackCount)
	{// ���Ų�ƥ�� ����
		sprintf(buf, "��%d�� %s ���Ų�ƥ��", curMaterialLine, materialName.c_str());
		ErrorInfo(BRACK_NO_MATCH, "brackMatch.txt", buf);
	}


}

//--------------------------------------------------------------------------
void  MyScriptCompilerListener::handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg)
{
	char buf[1024];
	sprintf(buf, "������� %s in %s  %d line", msg.c_str(), file.c_str(), line );

	ErrorInfo(COMPLIER_ERROR, "CompilerError.txt", buf);
}

//--------------------------------------------------------------------------
void ErrorInfo(ErrorType type, const Ogre::String & outPutTxt, const char * buf)
{
	HANDLE handleOfCon = GetStdHandle(STD_OUTPUT_HANDLE); 

	std::ofstream outPut;
	outPut.open(outPutTxt.c_str(), std::ios::app);

	WORD color = 0;

	switch(type)
	{
	case BRACK_NO_MATCH:
		color = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;	
		break;
	case NONE_BRACK:
		color = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;	
		break;
	case COMPLIER_ERROR:
		color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case COLLISION_NAME:
		color = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case WARING_NO_PATH:
		break;
	default:
		return;
	}
	
	SetConsoleTextAttribute(handleOfCon,  color);

	// ��������̫�� �ݲ���ӡ
	if(type != WARING_NO_PATH)
		std::cout<<buf<<std::endl;

	outPut<<buf<<std::endl;
	outPut.close();

	SetConsoleTextAttribute(handleOfCon, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);

	//delete handleOfCon;
}
////--------------------------------------------------------------------------
//void MyScriptCompilerListener::preConversion(Ogre::ScriptCompiler *compiler, Ogre::ConcreteNodeListPtr nodes)
//{
//	// �ɴ�Ҳ���ж� �����е�{} ƥ�� 
//
//}
