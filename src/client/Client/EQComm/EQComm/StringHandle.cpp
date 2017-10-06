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
		//�������㷨1����
		break;
	case FILTERALG2:
		//�������㷨2����
		break;
	default:
		//Ĭ��ԭ�����
		output = source_s;
		break;
	}
	std::string _outputStr(output);
	std::string::size_type _outputLen = _outputStr.length();
	_outputStr.erase(input_len,(_outputLen-input_len));
	return _outputStr;

	
	//���ڴ�й¶bug, û�޸ĺ�֮ǰֱ�ӷ���  Y3
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