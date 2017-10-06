#include "EQCommPCH.h"
#include "StringHandle.h"
#include "spam.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"


//-----------------------------------------------------------------
StringHandle::StringHandle():mHasload_dict(false)
{

}
StringHandle::~StringHandle()
{

}
//-----------------------------------------------------------------
std::string StringHandle::stringFilter(char* source_s,int mode)
{
	std::string _tmpstr(source_s);
	size_t input_len = _tmpstr.length();
	std::string _tmpTalkSuff = luabind::call_function<std::string>(sLuaMgr.getL(), "UIW_getTmpTalkSuff");
	_tmpstr += _tmpTalkSuff;
	size_t _inputLen = _tmpstr.length();
	if (mHasload_dict == false)
	{
		g_load_dict("../Media/Data/word.txt");
		mHasload_dict = true;
	}
	char buftmp[10000];
	char* output;

	switch (mode)
	{
	case FILTERALG1:

		output = spam_string(_tmpstr.c_str(), _inputLen, buftmp, _inputLen);
		//按过滤算法1过滤
		break;
	case FILTERALG2:
		//按过滤算法2过滤
		break;
	default:
		//默认原样输出
		output = source_s;
		break;
	}
	std::string _outputStr(output);
	std::string::size_type _outputLen = _outputStr.length();
	_outputStr.erase(input_len,(_outputLen-input_len));
	return _outputStr;

	
	//有内存泄露bug, 没修改好之前直接返回  Y3
	//return std::string(source_s);
}
//-----------------------------------------------------
void StringHandle::clearup()
{
	if (mHasload_dict)
	{
		g_free_dict();
	}
}