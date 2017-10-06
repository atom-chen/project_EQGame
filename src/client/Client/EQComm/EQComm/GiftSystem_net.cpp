#include "EQCommPCH.h"
#include "GiftSystem.h"
#include "NetPacket.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"
#include "MainPlayer.h"
#include "HouseScene.h"
#include "SceneManager.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "EnumDef.h"
#include "CommFunc.h"

//-----------------------------------------------
bool EQMsgManager::handleGiftsystemMsg()
{
	return GiftSystem::getSingleton().handleMsg(mMsgtype, mRecebuff);
}
//-----------------------------------------------
bool GiftSystem::handleMsg(UINT16 msgType, ByteArray& recBuff)
{
	switch(msgType)
	{
	case NetMsgType::msg_notify_house_gift_box:
		{
			notify_house_gift_box data;
			data.decode(recBuff);

			recGiftData(data.gift_box_vec);
		}
		break;

	case NetMsgType::msg_notify_start_gift_service:
		{// ֪ͨ�������ʼ
			notify_start_gift_service data;
			data.decode(recBuff);

			//����LUA����
			startGiftService(static_cast<GiftServiceType>(data.type));
		}
		break;

	case NetMsgType::msg_notify_end_gift_service:
		{// ֪ͨ����������
			notify_end_gift_service data;
			data.decode(recBuff);

			//����LUA����
			luabind::call_function<void>(sLuaMgr.getL(), "CloseAllGiftUI");
		}
		break;

	case NetMsgType::msg_notify_pack_gift_box:
		{// ֪ͨ����������
			notify_pack_gift_box data;
			data.encode(recBuff);
			data.gift.inst_id = mCurMakingGiftInfo.instanceId;

			reqEndGiftService();
		}
		break;
	
	case NetMsgType::msg_notify_unpack_gift_box:
		{// ȡ����ǰ�������������
			notify_unpack_gift_box data;
			data.decode(recBuff);

			reqEndGiftService();
		}
		break;

	case NetMsgType::msg_notify_put_gift_box:
		{// ֪ͨ������
			notify_put_gift_box data;
			data.decode(recBuff);

			reqEndGiftService();
		}
		break;

	case NetMsgType::msg_notify_pickup_gift_box:
		{// ֪ͨʰȡ����
			notify_pickup_gift_box data;
			data.decode(recBuff);
			
			reqEndGiftService();
			luabind::call_function<void>(sLuaMgr.getL(), "GiftSystem_EndPickUpGift");	// (2011/6/15 add by hezhr)
		}	
		break;
		
	case NetMsgType::msg_notify_gift_box:
		{// ֪ͨ�Ƿ����Ѿ����������
			notify_gift_box data;
			data.decode(recBuff);

			if (0 != data.gift.inst_id) 
			{
				converDataToGiftInfo(data.gift, mCurMakingGiftInfo);
				luabind::call_function<void>(sLuaMgr.getL(), "NotifyHavePacked");
				luabind::call_function<void>(sLuaMgr.getL(), "SetPutGiftUI"
											,mCurMakingGiftInfo.giverName
											,mCurMakingGiftInfo.greetings
											,mCurMakingGiftInfo.itemTemplateId
											,mCurMakingGiftInfo.effecTime
											,mCurMakingGiftInfo.itemOverlay
											,mCurMakingGiftInfo.cardId);
			}
			else
			{
				luabind::call_function<void>(sLuaMgr.getL(), "NotifyWithoutPacked");
			}
		}
		break;

		case NetMsgType::msg_notify_send_gift_express:
			{
				notify_send_gift_express data;
				data.decode(recBuff);

				reqEndGiftService();
			}
			break;

		case NetMsgType::msg_notify_receive_gift_express:
			{
				reqEndGiftService();
				luabind::call_function<void>(sLuaMgr.getL(), "OnDelUnViewGift");
			}
			break;

	default:
		return false;
	}

	return true;
}
//-----------------------------------------------
void GiftSystem::reqStartGiftService(const GiftServiceType type) const
{
	req_start_gift_service req;
	req.type = type;
	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------
void GiftSystem::reqEndGiftService() const
{
	req_end_gift_service req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------
void GiftSystem::reqPackGift() const
{
	req_pack_gift_box req;
	req.gift.box_id = mCurMakingGiftInfo.boxId;
	req.gift.card_id = mCurMakingGiftInfo.cardId;

	if( -1 != mCurMakingGiftInfo.storageIndex)
	{
		req.index1 = 0;
		req.index2 = mCurMakingGiftInfo.storageIndex;
		req.pack1_type = ItemPack::PT_GIFT;
		req.pack2_type = ItemPack::PT_WAREHOUSE;
	}
	else
	{
		req.index1 = -1;
		req.index2 = -1;
	}
	
	if("" == mCurMakingGiftInfo.giverName)
	{
		req.gift.is_anonymous = at_yes;
	}
	else
	{
		req.gift.is_anonymous = at_no;
		req.gift.sender_name = mCurMakingGiftInfo.giverName;
	}

	req.gift.name = mCurMakingGiftInfo.name;
	req.gift.sender = mCurMakingGiftInfo.giverAcc;
	req.gift.receiver = mCurMakingGiftInfo.recipientAcc;
	req.gift.comment = mCurMakingGiftInfo.greetings;

	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------
void GiftSystem::reqCanclePackGift() const
{
	req_unpack_gift_box req;
	req.inst_id = mCurMakingGiftInfo.instanceId;
	
	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------
void GiftSystem::reqPutGift() const
{		
	// ��������վ����λ��
	Ogre::Vector3 position = MainPlayer::getSingleton().getPos();
	//Ogre::Ray ray(position+Ogre::Vector3(0,1,0), 
	//	Ogre::Vector3::NEGATIVE_UNIT_Y);

	HouseScene * hs = EQSceneManager::getSingleton().getHouseScene();
	HouseSceneGrid * grid = NULL;
	// ���صĽ��㣨�˴������ã�
	Ogre::Vector3 pt(Ogre::Vector3::ZERO);

	//if(hs->rayGrid(ray, grid, pt, true))
	if((grid = hs->gridByPosition(position)))
	{// ���Ӵ���
		int floorNum = grid->getCreator()->getFloorNO();
		int gridX	 = grid->getGridX();
		int gridZ	 = grid->getGridZ();

		// �жϸ�λ���ܷ��������
		if(!_canPutGift(floorNum, gridX, gridZ))
		{
			// ��ʾ�޷�����
			luabind::call_function<void>(sLuaMgr.getL(), "NotifyCannotPutGift");
			return;
		}
			
		req_put_gift_box req;	
		req.inst_id = mCurMakingGiftInfo.instanceId;
		req.pos.x = gridX;
		req.pos.z = gridZ;
		// yֵ���ڴ��¥���
		req.pos.y = floorNum;
		sGameMgr.getEQMsgManager()->Send(req);
	}
	else
	{
		return;
	}
}
//-----------------------------------------------
void GiftSystem::reqPickUpGift() const
{
	if(NULL == mCurPickGift)
		return;

	req_pickup_gift_box req;
	req.inst_id = mCurPickGift->getInstanceId();

	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------
void GiftSystem::reqExpressGift() const
{
	req_send_gift_express req;
	req.gift.card_id = mCurMakingGiftInfo.cardId;
	req.gift.comment = mCurMakingGiftInfo.greetings;
	
	if( -1 != mCurMakingGiftInfo.storageIndex)
	{
		req.index1 = 0;
		req.index2 = mCurMakingGiftInfo.storageIndex;
		req.pack1_type = ItemPack::PT_GIFT;
		req.pack2_type = ItemPack::PT_WAREHOUSE;
	}
	else
	{
		req.index1 = -1;
		req.index2 = -1;
	}

	req.gift.receiver = mCurMakingGiftInfo.recipientAcc;
	req.gift.sender = mCurMakingGiftInfo.giverAcc;
	req.gift.sender_name = mCurMakingGiftInfo.giverName;

	req.gift.send_date.year = mCurMakingGiftInfo.sendTime._year;
	req.gift.send_date.month = mCurMakingGiftInfo.sendTime._month;
	req.gift.send_date.day = mCurMakingGiftInfo.sendTime._day;
	req.gift.send_date.minute = 0;
	req.gift.send_date.hour = 0;
	req.gift.send_date.second = 0;


	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------
void GiftSystem::converDataToGiftInfo(const gift_box &giftBox, GiftInfo& giftInfo) const
{
	giftInfo.boxId = giftBox.box_id;
	giftInfo.giverAcc = giftBox.sender;
	giftInfo.cardId = giftBox.card_id;
	giftInfo.greetings = giftBox.comment;
	giftInfo.instanceId = giftBox.inst_id;
	giftInfo.name = giftBox.name;

	if(!giftBox.grid_vec.empty())
	{// Ŀǰֻȡһ�����ݡ��Ժ������������޸�
		giftInfo.itemInstanceId = giftBox.grid_vec[0].item_data.instance_id;
		giftInfo.itemTemplateId = giftBox.grid_vec[0].item_data.template_id;
		giftInfo.itemOverlay	= giftBox.grid_vec[0].count;
		int year = giftBox.grid_vec[0].item_data.del_time.year;
		int month = giftBox.grid_vec[0].item_data.del_time.month;
		int day = giftBox.grid_vec[0].item_data.del_time.day;
		int hour = giftBox.grid_vec[0].item_data.del_time.hour;
		int minute= giftBox.grid_vec[0].item_data.del_time.minute;
		int second = giftBox.grid_vec[0].item_data.del_time.second;
		giftInfo.effecTime		= FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
	}
	giftInfo.recipientAcc = giftBox.receiver;
	giftInfo.reciverName = giftBox.receiver_name;

	if(at_no == giftBox.is_anonymous)
			giftInfo.giverName = giftBox.sender_name;

	// y�������¥������
	giftInfo.floorNum = giftBox.pos.y;
	giftInfo.gridX	= giftBox.pos.x;
	giftInfo.gridZ = giftBox.pos.z;

	giftInfo.reciveTime.set(giftBox.date.year, giftBox.date.month, giftBox.date.day);
}
//-----------------------------------------------
void GiftSystem::converDataToGiftInfo(const gift_express &giftExpress, GiftInfo& giftInfo) const
{
	giftInfo.expressType = giftExpress.type;
	giftInfo.cardId =  giftExpress.card_id;
	giftInfo.giverAcc = giftExpress.sender;
	giftInfo.giverName = giftExpress.sender_name;
	giftInfo.greetings = giftExpress.comment;
	giftInfo.instanceId = giftExpress.inst_id;
	giftInfo.recipientAcc = giftExpress.receiver;
	giftInfo.reciverName = giftExpress.receiver_name;
	giftInfo.reciveTime.set(giftExpress.date.year, giftExpress.date.month, giftExpress.date.day);

	if(!giftExpress.grid_vec.empty())
	{// Ŀǰֻȡһ�����ݡ��Ժ������������޸�
		giftInfo.itemInstanceId = giftExpress.grid_vec[0].item_data.instance_id;
		giftInfo.itemTemplateId = giftExpress.grid_vec[0].item_data.template_id;
		giftInfo.itemOverlay	= giftExpress.grid_vec[0].count;
		int year = giftExpress.grid_vec[0].item_data.del_time.year;
		int month = giftExpress.grid_vec[0].item_data.del_time.month;
		int day = giftExpress.grid_vec[0].item_data.del_time.day;
		int hour = giftExpress.grid_vec[0].item_data.del_time.hour;
		int minute= giftExpress.grid_vec[0].item_data.del_time.minute;
		int second = giftExpress.grid_vec[0].item_data.del_time.second;
		giftInfo.effecTime		= FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
	}

	giftInfo.reciveTime.set(giftExpress.date.year, giftExpress.date.month, giftExpress.date.day);
}