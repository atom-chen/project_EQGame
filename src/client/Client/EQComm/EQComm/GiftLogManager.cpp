#include "EQCommPCH.h"
#include "GiftSystem.h"
#include "GiftLogManager.h"
#include "NetPacket.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"
#include "CommFunc.h"
#include <math.h>
//-----------------------------------------------------------------------
template<> GiftLogManager* Ogre::Singleton<GiftLogManager>::ms_Singleton = NULL;
//-----------------------------------------------------------------------
GiftLogManager::GiftLogManager()
:mCurGiftInstanceId(0)
{
	mDIYSendLog.reserve(ReserveSize);
	mDIYReciveLog.reserve(ReserveSize);
	mDIYViewedReciveLog.reserve(ReserveSize);

	mMailSendLog.reserve(ReserveSize);
	mMailReciveLog.reserve(ReserveSize);
	mMailViewedReciveLog.reserve(ReserveSize);

	mBuffer.reserve(200);
}
//-----------------------------------------------------------------------
GiftLogManager::~GiftLogManager()
{
	GiftInfoMap::iterator iter = mGiftLog.begin();
	while(mGiftLog.end() != iter)
	{
		delete iter->second;
		++iter;
	}
	mGiftLog.clear();

	GiftInfoBuffer::iterator bufferIter = mBuffer.begin();
	while(mBuffer.end() != bufferIter)
	{
		delete (*bufferIter);
		++bufferIter;
	}
	mBuffer.clear();


	mDIYSendLog.clear();
	mDIYReciveLog.clear();
	mDIYViewedReciveLog.clear();

	mMailSendLog.clear();
	mMailReciveLog.clear();
	mMailViewedReciveLog.clear();

}
//-----------------------------------------------------------------------
void GiftLogManager::recGiftLogData(GiftLogType type, std::vector<gift_box> data, int totalDataCount)
{
	int totalPageNum = ceil(static_cast<double>(totalDataCount)/PerPageLogCount);
	luabind::call_function<void>(sLuaMgr.getL(), "SetGiftLogTotalPage", type, totalPageNum);

	switch(type)
	{
	case DIY_SEND:
		{
			uint size = data.size();

			// 请求清空记录后会返回空数据，所以要清空缓存
			if(0 == size)
			{
				cleanLog(mDIYSendLog);
				return;
			}

			for(uint i = 0; i < size; ++i)
			{
				GiftInfo info;
				GiftSystem::getSingleton().converDataToGiftInfo(data[i], info);
				createGiftLogInfo(info);
				mDIYSendLog.push_back(info.instanceId);

				std::string time = FormatStr("%d-%d-%d"
					,info.reciveTime._year
					,info.reciveTime._month
					,info.reciveTime._day);

				luabind::call_function<void>(sLuaMgr.getL(), "SetSendGiftLog"
					,info.reciverName
					,time
					,i+1
					,size);
			}
		}
		break;

	case DIY_RECIVE_NO_VIEW:
		{
			uint size = data.size();

			// 请求清空记录后会返回空数据，所以要清空缓存
			if(0 == size)
			{
				cleanLog(mDIYReciveLog);
				return;
			}

			char instanceId[20] = {0};

			for(uint i = 0; i < size; ++i)
			{
				GiftInfo info;
				GiftSystem::getSingleton().converDataToGiftInfo(data[i], info);
				createGiftLogInfo(info);
				mDIYReciveLog.push_back(info.instanceId);

				_ui64toa( info.instanceId, instanceId, 10);

				std::string time = FormatStr("%d-%d-%d"
					,info.reciveTime._year
					,info.reciveTime._month
					,info.reciveTime._day);

				luabind::call_function<void>(sLuaMgr.getL(), "SetReciveGiftLog"
					,info.giverName
					,time
					,i+1
					,size
					,instanceId);
			}
		}
		break;

	case DIY_RECIVE_VIEW:
		{
			uint size = data.size();

			// 请求清空记录后会返回空数据，所以要清空缓存
			if(0 == size)
			{
				cleanLog(mDIYViewedReciveLog);
				return;
			}

			char instanceId[20] = {0};

			for(uint i = 0; i < size; ++i)
			{
				GiftInfo info;
				GiftSystem::getSingleton().converDataToGiftInfo(data[i], info);
				createGiftLogInfo(info);
				mDIYViewedReciveLog.push_back(info.instanceId);

				_ui64toa( info.instanceId, instanceId, 10);

				std::string time = FormatStr("%d-%d-%d"
					,info.reciveTime._year
					,info.reciveTime._month
					,info.reciveTime._day);

				luabind::call_function<void>(sLuaMgr.getL(), "SetReciveGiftLog"
					,info.giverName
					,time
					,i+1
					,size
					,instanceId);
			}
		}
		break;

	default:return;
	}
}
//-----------------------------------------------------------------------
void GiftLogManager::recGiftLogData(GiftLogType type, std::vector<gift_express> data, int totalDataCount)
{
	int totalPageNum = ceil(static_cast<double>(totalDataCount)/PerPageLogCount);
	luabind::call_function<void>(sLuaMgr.getL(), "SetGiftLogTotalPage", type, totalPageNum);

	switch(type)
	{
	case MAIL_SEND:
		{
			uint size = data.size();

			// 请求清空记录后会返回空数据，所以要清空缓存
			if(0 == size)
			{
				cleanLog(mMailSendLog);
				return;
			}

			for(uint i = 0; i < size; ++i)
			{
				GiftInfo info;
				GiftSystem::getSingleton().converDataToGiftInfo(data[i], info);
				createGiftLogInfo(info);
				mMailSendLog.push_back(info.instanceId);

				std::string time = FormatStr("%d-%d-%d"
					,info.reciveTime._year
					,info.reciveTime._month
					,info.reciveTime._day);

				luabind::call_function<void>(sLuaMgr.getL(), "SetSendGiftLog"
					,info.reciverName
					,time
					,i+1
					,size);
			}
		}
		break;

	case MAIL_RECIVE_NO_VIEW:
		{
			uint size = data.size();
			char instanceId[20] = {0};

			// 特殊处理 清空原有的数据
			cleanLog(mMailReciveLog);
			luabind::call_function<void>(sLuaMgr.getL(), "CleanRecGiftLogUI");

			for(uint i = 0; i < size; ++i)
			{
				GiftInfo info;
				GiftSystem::getSingleton().converDataToGiftInfo(data[i], info);
				createGiftLogInfo(info);
				mMailReciveLog.push_back(info.instanceId);

				_ui64toa( info.instanceId, instanceId, 10);

				std::string time = FormatStr("%d-%d-%d"
					,info.reciveTime._year
					,info.reciveTime._month
					,info.reciveTime._day);

				luabind::call_function<void>(sLuaMgr.getL(), "SetReciveGiftLog"
					,info.giverName
					,time
					,i+1
					,size
					,instanceId);
			}
		}
		break;

	case MAIL_RECIVE_VIEW:
		{
			uint size = data.size();

			// 请求清空记录后会返回空数据，所以要清空缓存
			if(0 == size)
			{
				cleanLog(mMailViewedReciveLog);
				return;
			}

			char instanceId[20] = {0};

			for(uint i = 0; i < size; ++i)
			{
				GiftInfo info;
				GiftSystem::getSingleton().converDataToGiftInfo(data[i], info);
				createGiftLogInfo(info);
				mMailViewedReciveLog.push_back(info.instanceId);

				_ui64toa( info.instanceId, instanceId, 10);

				std::string time = FormatStr("%d-%d-%d"
					,info.reciveTime._year
					,info.reciveTime._month
					,info.reciveTime._day);

				luabind::call_function<void>(sLuaMgr.getL(), "SetReciveGiftLog"
					,info.giverName
					,time
					,i+1
					,size
					,instanceId);
			}
		}
		break;

	default:return;
	}
}
//-----------------------------------------------------------------------
GiftInfo* GiftLogManager::_reqGiftInfoBuffer(const GiftInfo& giftInfo)
{
	if(mBuffer.empty())
	{
		return NULL;
	}
	else
	{
		GiftInfo* giftLogInfo = mBuffer.back();
		*giftLogInfo = giftInfo;
		mBuffer.pop_back();

		return giftLogInfo;
	}
}
//-----------------------------------------------------------------------
void GiftLogManager::_addGiftInfoBuffer(GiftInfo *giftInfo)
{
	// 使用前已经判断指针有效性
	mBuffer.push_back(giftInfo);
}
//-----------------------------------------------------------------------
GiftInfo* GiftLogManager::createGiftLogInfo(const GiftInfo &info)
{
	GiftInfoMap::iterator iter = mGiftLog.find(info.instanceId);
	if(mGiftLog.end() != iter )
	{// 已经存在
		return NULL;
		//_recycleGiftInfo(iter->second);
		//mGiftLog.erase(iter);
	}

	GiftInfo* giftInfo = _reqGiftInfoBuffer(info);

	if(NULL == giftInfo)
	{
		giftInfo = new GiftInfo(info);
	}

	mGiftLog.insert(GiftInfoMap::value_type(info.instanceId, giftInfo));

	return giftInfo;
}
//-----------------------------------------------------------------------
void GiftLogManager::_recycleGiftInfo(GiftInfo *giftInfo)
{
	if(NULL == giftInfo)
		return;

	giftInfo->reset();

	_addGiftInfoBuffer(giftInfo);
}
//-----------------------------------------------------------------------
void GiftLogManager::showLog(const GiftLogType type, const uint pageNum) const
{
	switch(type)
	{
	case MAIL_SEND:
		{
			_showSendLog(mMailSendLog, pageNum);
		}
		break;

	case MAIL_RECIVE_VIEW:
		{
			_showReciveLog(mMailViewedReciveLog, pageNum);
		}
		break;

	case MAIL_RECIVE_NO_VIEW:
		{
			_showReciveLog(mMailReciveLog, pageNum);
		}
		break;

	case DIY_SEND:
		{
			_showSendLog(mDIYSendLog, pageNum);
		}
		break;

	case DIY_RECIVE_VIEW:
		{
			_showReciveLog(mDIYViewedReciveLog, pageNum);
		}
		break;

	case DIY_RECIVE_NO_VIEW:
		{
			_showReciveLog(mDIYReciveLog, pageNum);
		}
		break;

	default:return;
	}
}
//-----------------------------------------------------------------------
bool GiftLogManager::needReqLog(const GiftLogType type, const uint pageNum) const
{
	switch(type)
	{
	case DIY_SEND:
		{
			return _needReqLog(mDIYSendLog, pageNum, PerPageLogCount);
		}
		break;

	case DIY_RECIVE_VIEW:
		{
			return _needReqLog(mDIYViewedReciveLog, pageNum, PerPageLogCount);
		}
		break;

	case DIY_RECIVE_NO_VIEW:
		{
			return _needReqLog(mDIYReciveLog, pageNum, PerPageLogCount);
		}
		break;

	case MAIL_SEND:
		{
			return _needReqLog(mMailSendLog, pageNum, PerPageLogCount);
		}
		break;

	case MAIL_RECIVE_VIEW:
		{
			return _needReqLog(mMailViewedReciveLog, pageNum, PerPageLogCount);
		}
		break;

	case MAIL_RECIVE_NO_VIEW:
		{
			return true;//_needReqLog(mMailReciveLog, pageNum, PerPageLogCount);
		}
		break;
	}

	return true;
}
//-----------------------------------------------------------------------
uint GiftLogManager::_getTotalPage(const LogList &data, uint perPageLogCount) const
{
	if(0 == perPageLogCount)
		return 0;

	return ceil(static_cast<double>(data.size()) / perPageLogCount);
}
//-----------------------------------------------------------------------
uint GiftLogManager::_getStartIndex( uint pageNum, Ogre::uint perPageLogCount) const
{
	assert(pageNum > 0);
	return (pageNum - 1) * perPageLogCount;
}
//-----------------------------------------------------------------------
uint GiftLogManager::_getEndIndex(const LogList &list, Ogre::uint pageNum, Ogre::uint perPageLogCount) const
{
	return min(pageNum * perPageLogCount -1 , list.size() - 1);
}
//-----------------------------------------------------------------------
void GiftLogManager::_showSendLog(const LogList &logData, const uint pageNum) const 
{
	uint startIndex = _getStartIndex(pageNum, PerPageLogCount);
	uint endIndex = _getEndIndex(logData, pageNum, PerPageLogCount);

	uint forCount = endIndex - startIndex + 1;

	for(uint i = 0; i < forCount ; ++i)
	{
		uint64 instanceId = logData[startIndex + i];

		GiftInfoMap::const_iterator iter = mGiftLog.find(instanceId);
		GiftInfo* giftInfo = NULL;

		if(mGiftLog.end() != iter)
		{
			giftInfo = iter->second;
			assert(giftInfo);
		}
		else
		{
			continue;
		}

		std::string time = FormatStr("%d-%d-%d"
			,iter->second->reciveTime._year
			,iter->second->reciveTime._month
			,iter->second->reciveTime._day);

		luabind::call_function<void>(sLuaMgr.getL(), "SetSendGiftLog"
			,iter->second->reciverName
			,time
			,i+1
			,forCount);
	}
}
//-----------------------------------------------------------------------
void GiftLogManager::_showReciveLog(const LogList & logData, const uint pageNum) const 
{
	uint startIndex = _getStartIndex(pageNum, PerPageLogCount);
	uint endIndex = _getEndIndex(logData, pageNum, PerPageLogCount);

	uint forCount = endIndex - startIndex + 1;
	char instanceIdStr[20] = {0};

	for(uint i = 0; i < forCount ; ++i)
	{
		uint64 instanceId = logData[startIndex + i];

		GiftInfoMap::const_iterator iter = mGiftLog.find(instanceId);
		GiftInfo* giftInfo = NULL;

		if(mGiftLog.end() == iter)
		{
			continue;
		}

		_ui64toa( instanceId, instanceIdStr, 10);

		std::string time = FormatStr("%d-%d-%d"
			,iter->second->reciveTime._year
			,iter->second->reciveTime._month
			,iter->second->reciveTime._day);

		luabind::call_function<void>(sLuaMgr.getL(), "SetReciveGiftLog"
			,iter->second->giverName
			,time
			,i+1
			,forCount
			,instanceIdStr);
	}
}
//-----------------------------------------------------------------------
bool GiftLogManager::_needReqLog(const LogList &data, Ogre::uint pageNum, Ogre::uint perPageLogCount) const
{
	if(data.empty())
		return true;

	uint totalPage = _getTotalPage(data, PerPageLogCount);

	if(pageNum <= totalPage)
		return false;
	else
		return true;
}
//-----------------------------------------------------------------------
const GiftInfo* GiftLogManager::getGiftInfo(uint64 instanceId) const
{
	GiftInfoMap::const_iterator iter = mGiftLog.find(instanceId);

	if(mGiftLog.end() != iter)
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}
//-----------------------------------------------------------------------
void GiftLogManager::cleanLog()
{
	GiftInfoMap::iterator iter = mGiftLog.begin();

	while(mGiftLog.end() != iter)
	{
		_recycleGiftInfo(iter->second);
		iter = mGiftLog.erase(iter);
	}
}
//-----------------------------------------------------------------------
void GiftLogManager::cleanLog(LogList & data)
{
	LogList::iterator iter = data.begin();

	while(data.end() != iter)
	{
		GiftInfoMap::iterator mapIter = mGiftLog.find(*iter);

		if(mGiftLog.end() != mapIter)
		{
			_recycleGiftInfo(mapIter->second);
			mGiftLog.erase(mapIter);
		}

		iter = data.erase(iter);
	}
}
//-----------------------------------------------------------------------
void GiftLogManager::cleanIndex()
{
	mDIYSendLog.clear();
	mDIYReciveLog.clear();
	mDIYViewedReciveLog.clear();
	
	mMailSendLog.clear();
	mMailReciveLog.clear();
	mMailViewedReciveLog.clear();
}
//-----------------------------------------------------------------------