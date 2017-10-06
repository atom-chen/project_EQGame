#include "EQCommPCH.h"
#include "PlayerSearchOutput.h"
#include "luabind/luabind.hpp"
#include "LuaManager.h"

//-----------------------------------------------------------------------------
void PlayerSearchOutputToListbox::output(PlayerSearch::PlayerList* out_playerList)
{
	PlayerSearch::PlayerList::iterator iter = out_playerList->begin();

	luabind::call_function<void>(sLuaMgr.getL(),"Ps_clearResultList");

	for ( ; iter != out_playerList->end(); ++iter)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "Ps_getResultplayerList",iter->getNickname(),iter->getAccount(),
																				iter->getProvince(),iter->getCity());
	}
//----≤‚ ‘¥˙¬Î
// 	std::string tempNick("MDK");
// 	std::string tempAcc("huangdj1@35info.cn");
// 	std::string tempProvince("FJ");
// 	std::string tempCity("XM");
// 	for ( int i = 0; i < 50; ++i)
// 	{
// 		luabind::call_function<void>(sLuaMgr.getL(), "Ps_getResultplayerList",tempNick,tempAcc,
// 			tempProvince,tempCity);
// 	}
//----≤‚ ‘¥˙¬Î

	luabind::call_function<void>(sLuaMgr.getL(),"Ps_openPsListboxUI");
}