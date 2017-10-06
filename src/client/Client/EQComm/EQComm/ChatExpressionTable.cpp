/******************************************************************************
* 作者: 何展然
* 时间: 2010-10-29
* 描述: 聊天表情表管理器
******************************************************************************/
#include "EQCommPCH.h"
#include "ChatExpressionTable.h"
#include "CommFunc.h"
#include "XmlDataCenter.h"


//-----------------------------------------------------------------------
//	单件
template<> ChatExpressionTableManager* Ogre::Singleton<ChatExpressionTableManager>::ms_Singleton = 0;
//-----------------------------------------------------------------------
//	构造函数
ChatExpressionTableManager::ChatExpressionTableManager() :
mExpressionCount(0),
mLoadFlag(true)
{
}
//-----------------------------------------------------------------------
//	析构函数
ChatExpressionTableManager::~ChatExpressionTableManager()
{
}
//-----------------------------------------------------------------------
//	加载聊天表情表
void ChatExpressionTableManager::load(void)
{
	if (mLoadFlag)
	{
		XmlDataTable x;
		x.loadResource(OpenResource("chat_expression.xml"));

		const int colId = x.getColumnIndex("id");
		const size_t colTransferred = x.getColumnIndex("transferred");
		const int colPlaytime = x.getColumnIndex("playtime");
		const size_t colFirstf = x.getColumnIndex("firstf");
		const size_t colSecondf = x.getColumnIndex("secondf");
		const size_t colThirdf = x.getColumnIndex("thirdf");
		const size_t colForthf = x.getColumnIndex("forthf");
		const size_t colFifthf = x.getColumnIndex("fifthf");
		const size_t colSixthf = x.getColumnIndex("sixthf");
		const size_t colSeventhf = x.getColumnIndex("seventhf");
		const size_t colEightf = x.getColumnIndex("eighth");

		for (size_t iRow=0; iRow<x.getRowCount(); ++iRow)
		{
			ChatExpressionRow cer;
			cer.id = Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, colId, true).c_str());
			cer.transferred = GB2312ToUTF8(x.cell(iRow, colTransferred, true).c_str());
			cer.playtime = Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, colPlaytime, true).c_str());
			cer.firstf = GB2312ToUTF8(x.cell(iRow, colFirstf, true).c_str());
			cer.secondf = GB2312ToUTF8(x.cell(iRow, colSecondf, true).c_str());
			cer.thirdf = GB2312ToUTF8(x.cell(iRow, colThirdf, true).c_str());
			cer.forthf = GB2312ToUTF8(x.cell(iRow, colForthf, true).c_str());
			cer.fifthf = GB2312ToUTF8(x.cell(iRow, colFifthf, true).c_str());
			cer.sixthf = GB2312ToUTF8(x.cell(iRow, colSixthf, true).c_str());
			cer.seventhf = GB2312ToUTF8(x.cell(iRow, colSeventhf, true).c_str());
			cer.eighthf = GB2312ToUTF8(x.cell(iRow, colEightf, true).c_str());

			mDataMap.insert(std::make_pair(cer.id, cer));	//	插入到映射表
		}
		mExpressionCount = x.getRowCount();
		mLoadFlag = false;
	}
}
//-----------------------------------------------------------------------
//	截取描述帧的字符串
void ChatExpressionTableManager::_addFrameFromStr(CEGUI::Animate& animate, std::string frame)
{
	assert("" != frame);
	int pos = frame.find(" ");
	assert(std::string::npos != pos);
	std::string imagesetName = frame.substr(0, pos);
	std::string imageName = frame.substr(pos+1, frame.size());
	animate.addAnimateFrame(imagesetName, imageName);
}
//-----------------------------------------------------------------------
//	从映射表获取聊天表情
int ChatExpressionTableManager::getChatExpression(CEGUI::Animate& animate, int index)
{
	stdext::hash_map<int, ChatExpressionRow>::iterator iter;
	iter = mDataMap.find(index);
	if (mDataMap.end() == iter)
	{
		return -1;
	}
	ChatExpressionRow cer = iter->second;
	animate.initAnimate(cer.playtime, 16, 16);
	if ("0" != cer.firstf)
	{
		_addFrameFromStr(animate, cer.firstf);
	}
	if ("0" != cer.secondf)
	{
		_addFrameFromStr(animate, cer.secondf);
	}
	if ("0" != cer.thirdf)
	{
		_addFrameFromStr(animate, cer.thirdf);
	}
	if ("0" != cer.forthf)
	{
		_addFrameFromStr(animate, cer.forthf);
	}
	if ("0" != cer.fifthf)
	{
		_addFrameFromStr(animate, cer.fifthf);
	}
	if ("0" != cer.sixthf)
	{
		_addFrameFromStr(animate, cer.sixthf);
	}
	if ("0" != cer.seventhf)
	{
		_addFrameFromStr(animate, cer.seventhf);
	}
	if ("0" != cer.eighthf)
	{
		_addFrameFromStr(animate, cer.eighthf);
	}
	return 0;
}
//-----------------------------------------------------------------------
//	从映射表获取表情转义符
std::string ChatExpressionTableManager::getTransfer(int index)
{
	stdext::hash_map<int, ChatExpressionRow>::iterator iter;
	iter = mDataMap.find(index);
	if (mDataMap.end() == iter)
	{
		return "";
	}
	ChatExpressionRow cer = iter->second;
	return cer.transferred;
}
//-----------------------------------------------------------------------
//	获取表情的数量
int ChatExpressionTableManager::getExpressionCount(void)
{
	return mExpressionCount;
}
