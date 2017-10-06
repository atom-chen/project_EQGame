#include "EQCommPCH.h"
#include "PlayerSearchMode.h"
#include "NetPacket.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"

//-------------------------------------
void PlayerSearchModeBase::searchMode(std::string key)
{
	
}

//-------------------------------------
void PlayerSearchByNick::searchMode(std::string key)
{
	req_search_player_by_nick req;
	req.nick = key;
	sGameMgr.getEQMsgManager()->Send(req);

}
//-------------------------------------
void PlayerSearchByMail::searchMode(std::string key)
{
	req_search_player_by_mail req;
	req.mail = key;
	sGameMgr.getEQMsgManager()->Send(req);
}