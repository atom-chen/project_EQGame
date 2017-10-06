/******************************************************************
* 作者: XiaoMing
* 时间: 2010-12-11
* 描述: 礼物记录管理
*******************************************************************/
#ifndef Gift_Log_Manager_H
#define Gift_Log_Manager_H

typedef unsigned __int64  uint64;
typedef std::vector<uint64> LogList;

struct gift_box;
struct gift_express;
struct GiftInfo;

typedef std::map<uint64, GiftInfo*> GiftInfoMap;
typedef std::vector<GiftInfo*> GiftInfoBuffer;

class GiftLogManager:public Ogre::Singleton<GiftLogManager>
{
public:
	static const uint ReserveSize = 100;
	static const uint PerPageLogCount = 7; // 每页记录数量
public:
	enum GiftLogType
	{
		NIL = 0,
		DIY_SEND = 1,
		DIY_RECIVE_NO_VIEW = 2,
		DIY_RECIVE_VIEW = 3,
		MAIL_SEND = 4,
		MAIL_RECIVE_NO_VIEW = 5,
		MAIL_RECIVE_VIEW = 6,
	};
public:
	// 接收礼物记录
	void recGiftLogData(GiftLogType type, std::vector<gift_box> data, int totalDataCount);
	void recGiftLogData(GiftLogType type, std::vector<gift_express> data, int totalDataCount);

	// 看看记录是否已经请求过了，如果请求过了就不再请求。直接显示。
	// 未收的快递礼物着要实时请求。
	bool needReqLog(const GiftLogType type, const uint pageNum) const;

	void showLog(const GiftLogType type, const uint pageNum) const;

	GiftInfo* createGiftLogInfo(const GiftInfo &info );
	void deleteGiftLog(uint64 giftInstanceId);
	void deleteAllLog();

	const GiftInfo* getGiftInfo(uint64 instanceId) const;

	// 当前查看的礼物记录的实例ID
	void setCurGiftInstanceId(uint64 instanceId){mCurGiftInstanceId = instanceId;}
	const uint64 getCurGiftInstanceId() {return mCurGiftInstanceId;}
	
	// 清空记录 mGiftLog
	void cleanLog(); 
	void cleanLog(LogList & data);
	/** 清空 mMailSendLog mDIYSendLog等表
		@remarks
			只是清空 mMailSendLog mDIYSendLog 等表中
		的数据。并不清空mGiftLog中的数据。因为mGiftLog
		中原来的礼物信息并不会改变，只会变多。只有当
		玩家主动清空mGiftLog中的数据时才清空
	*/
	void cleanIndex();

	GiftLogManager();
	~GiftLogManager();
private:
	GiftInfo* _reqGiftInfoBuffer(const GiftInfo& giftInfo);  // 请求分配缓存，如果有
	void  _addGiftInfoBuffer(GiftInfo* giftInfo);
	void  _recycleGiftInfo(GiftInfo* giftInfo);

	uint _getTotalPage(const LogList & data, uint perPageLogCount) const;
	uint _getStartIndex( uint pageNum, uint perPageLogCount) const;
	uint _getEndIndex(const LogList & list, uint pageNum, uint perPageLogCount) const;

	void _showSendLog(const LogList & logData, const uint pageNum) const;
	void _showReciveLog(const LogList & logData, const uint pageNum) const;

	bool _needReqLog(const LogList & data, uint pageNum, uint perPageLogCount) const;
private:
	// 记录信息在Map中的索引
	LogList mDIYSendLog;
	LogList mDIYReciveLog;			// 未查看
	LogList mDIYViewedReciveLog;	// 已查看

	LogList mMailSendLog;
	LogList mMailReciveLog;			// 未查看
	LogList mMailViewedReciveLog;	// 已查看

	uint64 mCurGiftInstanceId;

	GiftInfoMap mGiftLog;
	GiftInfoBuffer mBuffer;
};
	
#endif