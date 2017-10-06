#include "EQCommPCH.h"
#include "PlayProperty.h"
//#include "NetPacket.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "MainPlayer.h"
#include "GameEventMgr.h"
#include "DynamicUIMgr.h"
#include "UIAnimationMgr.h"
#include "EQGameMgr.h"
#include "Avatar.h"
#include "AvatarRTT.h"
#include "AvatarAdjuster.h"

void PlayProperty::reqProper()
{
	req_player_basic_data data;
	sGameMgr.getEQMsgManager()->Send(data);
}

void PlayProperty::notiBasic(ByteArray& recBuff)
{
	notify_player_basic_data data;
	data.decode(recBuff);
	MainPlayer::getSingleton().setPlayerData(data.basic_data);
	mPro = data.property;
	// 更新人物形象中人物预览框模型(hezhr)
	AvatarRTT *rtt = EQGameMgr::getSingleton().getRTT();
	Avatar *avatar = rtt->getAvatar();
	AvatarAdjuster *adj = avatar->getAdjuster();
	adj->doChange(AvatarAdjuster::atHairType, data.basic_data.hair);
	adj->doChange(AvatarAdjuster::atHairColor, data.basic_data.hair_color);
	adj->doChange(AvatarAdjuster::atFace, data.basic_data.face);
	adj->doChange(AvatarAdjuster::atSkin, data.basic_data.skin_color);
	adj->doChange(AvatarAdjuster::atBeard, data.basic_data.beard);

	if (mPro.player_clean < 30)
	{
		// 运行新手教学任务(清洁度低到30以下)
		MainPlayer::getSingleton().getTask().startNewerTeachTask(15010);		
	}
	else if (mPro.player_health < 30)
	{
		// 运行新手教学任务(健康度低到30以下)
		MainPlayer::getSingleton().getTask().startNewerTeachTask(15011);		
	}
}

void PlayProperty::notiProper(ByteArray& recBuff)
{
	notify_player_property_changed data;
	data.decode(recBuff);
	changeProper(data);

}

void PlayProperty::changeProper(notify_player_property_changed data)
{
	if(data.player_clean != 0)
	{
		mPro.player_clean += data.player_clean;
		DynamicUIMgr::getSingleton().createNumberPaoPao(MainPlayer::getSingletonPtr(), ENPPT_CLEAN, data.player_clean);
	}

	if(data.player_health != 0)
	{
		mPro.player_health += data.player_health;
		DynamicUIMgr::getSingleton().createNumberPaoPao(MainPlayer::getSingletonPtr(), ENPPT_HEALTH, data.player_health);
	}

	if(data.player_charm != 0)
	{
		mPro.player_charm += data.player_charm;
		DynamicUIMgr::getSingleton().createNumberPaoPao(MainPlayer::getSingletonPtr(), ENPPT_CHARM, data.player_charm);
	}

	if(data.active_value != 0)
	{
		mPro.active_value += data.active_value;
		DynamicUIMgr::getSingleton().createNumberPaoPao(MainPlayer::getSingletonPtr(), ENPPT_ACTIVE, data.active_value);
	}
}

bool EQMsgManager::handleByProPerty()
{
	switch(mMsgtype)
	{		
	case NetMsgType::msg_notify_player_basic_data:
		{
			MainPlayer::getSingleton().getPropertyModule().notiBasic(mRecebuff);
			GameEventMgr::getSingleton().fireEvent("ON_CLICKED_RECV_PROPERTY");
		}
		break;
	case NetMsgType::msg_notify_player_property_changed:
		{
			MainPlayer::getSingleton().getPropertyModule().notiProper(mRecebuff);
		}
	default:
		return false;
	}
	return true;
}