/******************************************************************
* ����: XiaoMing
* ʱ��: 2010-12-11
* ����: �����¼����
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
	static const uint PerPageLogCount = 7; // ÿҳ��¼����
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
	// ���������¼
	void recGiftLogData(GiftLogType type, std::vector<gift_box> data, int totalDataCount);
	void recGiftLogData(GiftLogType type, std::vector<gift_express> data, int totalDataCount);

	// ������¼�Ƿ��Ѿ�������ˣ����������˾Ͳ�������ֱ����ʾ��
	// δ�յĿ��������Ҫʵʱ����
	bool needReqLog(const GiftLogType type, const uint pageNum) const;

	void showLog(const GiftLogType type, const uint pageNum) const;

	GiftInfo* createGiftLogInfo(const GiftInfo &info );
	void deleteGiftLog(uint64 giftInstanceId);
	void deleteAllLog();

	const GiftInfo* getGiftInfo(uint64 instanceId) const;

	// ��ǰ�鿴�������¼��ʵ��ID
	void setCurGiftInstanceId(uint64 instanceId){mCurGiftInstanceId = instanceId;}
	const uint64 getCurGiftInstanceId() {return mCurGiftInstanceId;}
	
	// ��ռ�¼ mGiftLog
	void cleanLog(); 
	void cleanLog(LogList & data);
	/** ��� mMailSendLog mDIYSendLog�ȱ�
		@remarks
			ֻ����� mMailSendLog mDIYSendLog �ȱ���
		�����ݡ��������mGiftLog�е����ݡ���ΪmGiftLog
		��ԭ����������Ϣ������ı䣬ֻ���ࡣֻ�е�
		����������mGiftLog�е�����ʱ�����
	*/
	void cleanIndex();

	GiftLogManager();
	~GiftLogManager();
private:
	GiftInfo* _reqGiftInfoBuffer(const GiftInfo& giftInfo);  // ������仺�棬�����
	void  _addGiftInfoBuffer(GiftInfo* giftInfo);
	void  _recycleGiftInfo(GiftInfo* giftInfo);

	uint _getTotalPage(const LogList & data, uint perPageLogCount) const;
	uint _getStartIndex( uint pageNum, uint perPageLogCount) const;
	uint _getEndIndex(const LogList & list, uint pageNum, uint perPageLogCount) const;

	void _showSendLog(const LogList & logData, const uint pageNum) const;
	void _showReciveLog(const LogList & logData, const uint pageNum) const;

	bool _needReqLog(const LogList & data, uint pageNum, uint perPageLogCount) const;
private:
	// ��¼��Ϣ��Map�е�����
	LogList mDIYSendLog;
	LogList mDIYReciveLog;			// δ�鿴
	LogList mDIYViewedReciveLog;	// �Ѳ鿴

	LogList mMailSendLog;
	LogList mMailReciveLog;			// δ�鿴
	LogList mMailViewedReciveLog;	// �Ѳ鿴

	uint64 mCurGiftInstanceId;

	GiftInfoMap mGiftLog;
	GiftInfoBuffer mBuffer;
};
	
#endif