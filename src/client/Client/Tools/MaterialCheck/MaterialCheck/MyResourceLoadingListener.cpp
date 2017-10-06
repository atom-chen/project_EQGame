#include "MyResourceLoadingListener.h"
#include <assert.h>
#include <iostream>
#include <windows.h>
#include <vector>

// 错误处理 本应只传入一个错误信息
void ErrorInfo(ErrorType type, const Ogre::String & outPutTxt, const char * buf );

//--------------------------------------------------------------------------
bool MyResourceLoadingListener::resourceCollision(Ogre::Resource *resource, Ogre::ResourceManager *resourceManager)
{
	assert(resource);
	assert(resourceManager);
	
	if(resourceManager->resourceExists( resource->getName()))
	{
		// 获取冲突的文件名
		std::string fileName = resourceManager->getByName(resource->getName())->getOrigin();
		
		char buf[1024];
		sprintf(buf, "与%s文件中的 %s 名字重复", fileName.c_str(), resource->getName().c_str() );

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

	// 将当前解析的文件名写入LOG文件
	std::ofstream outPut;
	outPut.open("ResourceCollision.txt", std::ios::app);
	outPut<<"开始检测"<<name<<std::endl;
	outPut.close();

	outPut.open("brackMatch.txt", std::ios::app);
	outPut<<"开始检测"<<name<<std::endl;
	outPut.close();

	outPut.open("noPath.txt", std::ios::app);
	outPut<<"开始检测"<<name<<std::endl;
	outPut.close();

	char buf[1024];

	// 当前行
	int curLine = 0;
	// 当前检测的Material所在行
	int curMaterialLine = 0;
	int materialCount   = 0;

	int leftBrackCount  = 0;
	int rightBrackCount = 0;

	std::vector< Ogre::String > splitResult;
	Ogre::String materialName;

	// 解析dataStream 检测{}匹配 以及.dds 有没有路径前缀
	Ogre::String text;
	while(!dataStream->eof())
	{
		text = dataStream->getLine();
		curLine++;

		Ogre::StringUtil::trim(text);

		if(text == "")
			continue;

		// 处理注释	
		size_t offset =  text.find("//");
		if(offset < text.size())
		{
			// 整行都为注释
			if(Ogre::StringUtil::startsWith(text, "//"))
				continue;
			else
			{// 注释在中间
				text.erase(offset);
			}	
		}

		// 检测.dds有没有前缀路径 只做简单检测
		offset = text.find(".dds");
		if(offset < text.size())
		{
			offset = text.find("/");
			if(offset > text.size())
			{
				sprintf(buf, "第%d行.dss可能没有添加路径", curLine);
				ErrorInfo(WARING_NO_PATH, "noPath.txt", buf);
			}

		}

		// 该行为material起始行
		offset = text.find("material");
		if(offset == 0)
		{
			materialCount ++;
			
			if(materialCount != 1 && leftBrackCount == 0 && leftBrackCount == 0)
			{
				// 输出到控制台与LOG文件
				sprintf(buf, "第%d行 %s 没有括号", curMaterialLine, materialName.c_str() );
				ErrorInfo(NONE_BRACK, "brackMatch.txt", buf);
			}


			if(leftBrackCount != rightBrackCount)
			{
				// 括号不匹配 报错
				sprintf(buf, "第%d行 %s 括号不匹配", curMaterialLine, materialName.c_str());
			    ErrorInfo(BRACK_NO_MATCH, "brackMatch.txt", buf);
			}

			curMaterialLine = curLine;
			leftBrackCount = rightBrackCount = 0;
			materialName = "";

			// 获取材质的名字
			size_t indexL = 0;
			size_t indexR = 0;
			size_t min = 0;

			splitResult = Ogre::StringUtil::split(text, " ");
			if(splitResult.size() <= 1)
			{
				// waring该材质没有名字
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

		// 检测括号数量
		while((offset = text.find("{", offset + 1)) < text.size())
		{
			leftBrackCount++;
		}

		while((offset = text.find("}", offset + 1)) < text.size())
		{
			rightBrackCount++;
		}

	}

	// 检测最后一个Material
	if(materialCount != 1 && leftBrackCount == 0 && leftBrackCount == 0)
	{
		// 输出
		sprintf(buf, "第%d行 %s 没有括号", curMaterialLine, materialName.c_str() );
		ErrorInfo(NONE_BRACK, "brackMatch.txt", buf);
	}


	if(leftBrackCount != rightBrackCount)
	{// 括号不匹配 报错
		sprintf(buf, "第%d行 %s 括号不匹配", curMaterialLine, materialName.c_str());
		ErrorInfo(BRACK_NO_MATCH, "brackMatch.txt", buf);
	}


}

//--------------------------------------------------------------------------
void  MyScriptCompilerListener::handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg)
{
	char buf[1024];
	sprintf(buf, "编译错误 %s in %s  %d line", msg.c_str(), file.c_str(), line );

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

	// 该输出结果太多 暂不打印
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
//	// 由此也能判断 代码中的{} 匹配 
//
//}
