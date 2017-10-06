#include "EQCommPCH.h"

#include "EQMsgManager.h"
#include "NetMsgType.h"
#include "EnumDef.h"
#include "NetPacket.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"

// 通知打开不同类型的UI在此处理
bool EQMsgManager::handleOpenUIMsg()
{
	if(NetMsgType::msg_notify_open_ui == mMsgtype)
	{
		notify_open_ui data;
		data.decode(mRecebuff);

		luabind::call_function<void>(sLuaMgr.getL(), "HandleOpenUIMsg", data.type);

		return true;
	}
	else if(NetMsgType::msg_notify_open_shop == mMsgtype)
	{
		notify_open_shop data;
		data.decode(mRecebuff);

		//luabind::call_function<void>(sLuaMgr.getL(), "NpcShop_Open", data.type);
		luabind::call_function<void>(sLuaMgr.getL(), "ShopCenter_Open");

		return true;
	}

	return false;
}