#include "EQCommPCH.h"
#include "PlayerSearch.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "PlayerSearchOutput.h"

//---------------------------------------------
PlayerSearch::PlayerSearch()
{
	subscribe(&PlayerSearchOutputToListbox::getSingleton());
	regist_msgHandle();
}
//--------------------------------------------
PlayerSearch::~PlayerSearch()
{
	desubscribe(&PlayerSearchOutputToListbox::getSingleton());
}
//--------------------------------------------
void PlayerSearch::regist_msgHandle()
{
	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_search_player, NewMsgHandle(PlayerSearch, rec_msg_searchResult));
}
//-------------------------------------------
void PlayerSearch::req_search(std::string search_key, int mode)
{
	switch (mode)
	{
	case BY_NICK:
		searchway(&PlayerSearchByNick::getSingleton(),search_key);
		break;
	case BY_MAIL:
		searchway(&PlayerSearchByMail::getSingleton(),search_key);
		break;
	}
}
//-----------------------------------------
void PlayerSearch::searchway(PlayerSearchModeBase* psmb_ptr, 
							 std::string search_key)
{
	assert(psmb_ptr != NULL);
	psmb_ptr->searchMode(search_key);
}
//------------------------------------------
void PlayerSearch::rec_msg_searchResult(MsgPack &recBuff)
{
	notify_search_player result;
	result.decode(recBuff);

	std::vector<std::string>::iterator iter = result.players.begin();
	clearPlayerList();
	for ( ; iter != result.players.end(); ++iter)
	{
		save_searchResult(*iter);
	}
	updateSearchResult(mPlayer_list);
}
//-------------------------------------------
void PlayerSearch::save_searchResult(std::string acc)
{
	SearchPlayerResult spr(acc);
	mPlayer_list.push_back(spr);
}
//-----------------------------------------
void PlayerSearch::addEQInfo(std::string eq_acc,
							 std::string full_name,
							 std::string nick,
							 std::string eq_province,
							 std::string eq_city)
{
	if (isSearchEmpty())
	{
		return;
	}

	if (eq_province == "")
	{
		eq_province = "(None)";
	}
	if (eq_city == "")
	{
		eq_city = "(None)";
	}
	if (nick == "" && full_name != "")
	{
		nick = full_name;
	}
	if (nick == "" && full_name == "")
	{
		nick = "(None)";
	}

	PlayerList::iterator iter = mPlayer_list.begin();
	for ( ; iter != mPlayer_list.end(); ++iter)
	{
		if (-1 != iter->getAccount().find(eq_acc))
		{
			iter->setNickname(nick);
			iter->setArea(eq_province,eq_city);
		}
	}
	updateSearchResult(mPlayer_list);
}
//-----------------------------------------
void PlayerSearch::subscribe(PlayerSearchOutput* subscriber)
{
	msubscribers.push_back(subscriber);
}
//-----------------------------------------
void PlayerSearch::desubscribe(PlayerSearchOutput* subscriber)
{
	msubscribers.erase(std::remove(msubscribers.begin(),msubscribers.end(),subscriber),msubscribers.end());
}
//-----------------------------------------
void PlayerSearch::updateSearchResult(PlayerList& playerList)
{
	for (std::list<PlayerSearchOutput*>::iterator iter = msubscribers.begin();
		 iter != msubscribers.end(); ++iter )
	{
		(*iter)->output(&mPlayer_list);
	}
}
//-----------------------------------------
//
//
//-----------------------------------------
SearchPlayerResult::SearchPlayerResult(std::string acc)
:mAccount(acc)
,mNick_name("")
{
}