/******************************************************************
* ����: XiaoMing
* ʱ��: 2010-12-01
* ����: ����ϵͳ
*******************************************************************/
#ifndef GIFT_SYSTEM_H
#define GIFT_SYSTEM_H

#include "..\..\EQMain\EQMain\PickObjectProxy.h"
#include "HouseScene.h"

class ModelGcs;
class ByteArray;
class FlashDlg;
struct gift_box;
struct gift_express;

typedef unsigned __int64  uint64;
typedef unsigned __int32  uint32;
//--------------------------------------------------------
struct GiftDate
{
	GiftDate()
		:_year(1970)
		,_month(1)
		,_day(1)
	{

	}

	void reset()
	{
		_year = 1970;
		_month = 1;
		_day = 1;
	}

	uint _year;
	uint _month;
	uint _day;
public:
	void set(uint year, uint month, uint day);
};
//--------------------------------------------------------

//���������Ϣ
struct GiftInfo
{
	/*��ʹ��NetPacket������ݽṹ��NetPacket����Ƶ��-��-*/
	GiftInfo()
		:expressType(0)
		,itemTemplateId(0)
		,cardId(0)
		,boxId(0)
		,gridX(0)
		,gridZ(0)
		,floorNum(0)
		,itemOverlay(0)
		,itemInstanceId(0)
		,instanceId(0)
		,name("")
		,greetings("")
		,giverName("")
		,giverAcc("")
		,recipientAcc("")
		,reciverName("")
		,effecTime("")
	{}

	~GiftInfo()
	{

	}
public:
	GiftInfo(const GiftInfo &info)
		:expressType(info.expressType)
		,itemTemplateId(info.itemTemplateId)
		,cardId(info.cardId)
		,boxId(info.boxId)
		,gridX(info.gridX)
		,gridZ(info.gridZ)
		,floorNum(info.floorNum)
		,itemOverlay(info.itemOverlay)
		,itemInstanceId(info.itemInstanceId)
		,instanceId(info.instanceId)
		,name(info.name)
		,greetings(info.greetings)
		,giverName(info.giverName)
		,giverAcc(info.giverAcc)
		,reciverName(info.reciverName)
		,recipientAcc(info.recipientAcc)
		,sendTime(info.sendTime)
		,reciveTime(info.reciveTime)
		,effecTime(info.effecTime)
	{

	}

	void reset();
	
	int     expressType;
	int		itemTemplateId;		// ��Ʒģ��ID
	int		cardId;		// �ؿ�
	int		boxId;				// ���ID
	int     gridX;				// ���õĸ��ӵ�X����
	int     gridZ;              // ���õĸ��ӵ�Z����
	int     floorNum;			// ���õĸ��ӵ�¥��
	int		itemOverlay;		// ��Ʒ����
	int		storageIndex;		// ��Ʒ�ڱ����е�����
	uint64	itemInstanceId;		// ��Ʒʵ��ID
	uint64  instanceId;			// ����ʵ��ID���������ˣ�
	std::string name;			// ������
	std::string greetings;		// ף����
	std::string giverName;		// ����������
	std::string giverAcc;		// �������ʺ�
	std::string recipientAcc;	// �������˺�
	std::string reciverName;    // ����������
	GiftDate	sendTime;		// ���﷢��ʱ��
	GiftDate	reciveTime;		// �������ʱ��
	std::string effecTime;		// ������Ʒ��ʱЧ
};
//--------------------------------------------------------
//����
class Gift
{
public:
	// �������ڵ������ʱʹ��
	class PickObjectUnit : public PickObject
	{
	public:
		// ����Ϊ Gift::PickObjectUnit
		const std::string& getClassName();

		void  setCreator(Gift* creator){ mCreator = creator; }
		Gift* getCreator(){return mCreator;}

		PickObjectUnit()
			:mCreator(NULL)
		{}
	private:
		Gift* mCreator;

	};

public:
	ModelGcs* getModel() const { return mModel;}
	void      setModel(ModelGcs* model);
	void	  setPosition();	// ��������ģ��λ��
	void      setGiftInfo(const GiftInfo & info){ mInfo = info; }
	const GiftInfo & getGiftInfo() const {return mInfo;}

	uint32    getIndex() const{return mIndex;}
	void	  setIndex(const uint32 index) { mIndex = index; }

	void	  setInstanceId(const uint64 instanceId){mInfo.instanceId = instanceId;}
	uint64	  getInstanceId() const {return mInfo.instanceId;}
	// ���ã����գ���������
	void      reset();

	Gift(const GiftInfo& giftInfo);
	~Gift();
private:
	void	  _setPosition(const Ogre::Vector3& position);
	void	  _setPosition(const int floorNum, const int gridX, const int gridZ);
private:
	uint32		mIndex;		 // �ͻ������õ�����������������ϵͳΪ���������ɵ�
	GiftInfo    mInfo;     
	ModelGcs*   mModel;		 // ���ģ��
	PickObjectUnit  mPickObjectUnit;
	PickObjectProxy mPickObjectProxy;

};

//--------------------------------------------------------
// ����ϵͳ
// ����ϵͳ��������ķ��ͣ��Լ��������������ʾ�ȡ�
// �������־��GiftLogManager�й���GiftSystem��
// ������־�Ĺ���
typedef std::map<uint32, Gift*> GiftMap;
typedef std::vector<Gift*> GiftBuffer;

class GiftSystem: public Ogre::Singleton<GiftSystem>
{
public:
	enum GiftServiceType
	{// ͬ���ڷ�����
		Pack = 2,
		PickUp  = 3,
		Put = 4,
		GetExpressGift = 5,
		ReplyMailGift = 6,
	};
public:
	Gift* createGift(const GiftInfo & info);
	void  deleteGift(const uint32 giftId);

	void  deleteAllGift();

	void  putGift(Gift* gift) const;
	Gift* getGift(const uint32 giftId) const;
	// �������ģ��
	void  clickGift(Gift* gift);

	void  cleanCurMakingGiftInfo();
	// ����Ҫ�͵��������Ʒ
	// @parm storageGolbaIndex �ֿ�ȫ������
	void  setGiftItem(int storageGolbaIndex);

	// ������������
	void  recGiftData(const std::vector<gift_box> &giftData);

	// ��Ϣ����
	bool  handleMsg(UINT16 msgType, ByteArray& recBuff);

	void  reqStartGiftService(const GiftServiceType type) const;
	void  reqEndGiftService() const;
	void  reqPackGift() const;
	void  reqCanclePackGift() const;
	// �����ڷ����ڷ�������
	void  reqPutGift() const;
	void  reqPickUpGift() const;
	// ����������
	void  reqExpressGift() const;
	// ������ȡ�������
	void  reqReciveExpressGift() const;

	// ��ʼ������񣬸��ݲ�ͬ�ķ���״̬���򿪲�ͬ�����
	void  startGiftService(const GiftServiceType type) ;

	// �������ʽ���ݶ�ȡ
	void  loadGiftBoxData();

	// ���￨��ʽ���ݶ�ȡ
	void  loadGiftCardData();

	// ��������Ϣ�ṹתΪGiftInfo
	void  converDataToGiftInfo(const gift_box &giftBox, GiftInfo& giftInfo) const;

	void  converDataToGiftInfo(const gift_express &giftExpress, GiftInfo& giftInfo) const;

	// ���õ�ǰ�ֹ�����������Ļ�����Ϣ
	void  setDIYGiftInfo(std::string giftame,
							uint cardId, 
							uint boxId,
							int  storageGolbaIndex,
							std::string greetings, 
							std::string giverName);
	// ���õ�ǰ��ݵ�����Ļ�����Ϣ
	void  setMailGiftInfo( std::string reciveAcc,
						  std::string greetings,
						  uint cardId, 
						  int  storageGolbaIndex,
						  const uint year,
						  const uint month,
						  const uint day);

	const GiftInfo* getCurPickGiftInfo(){return &mCurPickGift->getGiftInfo();}
	// ����Flash����
	void playGiftCardFlash(const char* flashUrl);
	// �ƶ�flash���ڣ�������Ϸ�������ƶ���
	void moveDisplayFlashWin();

	bool hasGiftInGrid(HouseSceneGrid *grid);

	// �����¼���
	GiftSystem();
	~GiftSystem();
private:
	Gift* _reqGiftBuffer();  // ������仺�棬�����
	void  _addGiftBuffer(Gift* gift);
	void  _recycleGift(Gift* gift);
	// �жϵ�ǰ��վ�ĸ����ܷ��������
	bool  _canPutGift(const int floorNum, const int gridX, const int gridZ) const;

	void  _pickUpGift() const;
private:
	uint32 mAutoGiftIndex;		
	GiftMap mGiftMap;		 // �������е������ģ����ʾ
	GiftBuffer mBuffer;
public:
	// ��LUAʹ��
	GiftInfo mCurMakingGiftInfo; // ��ǰ����������
	Gift*	 mCurPickGift;
	bool	 mIsLoadBoxData;
	bool	 mIsLoadCardData;
	FlashDlg* mDisplayFlashDlg;
	HWND	 mFlashHwnd;
	
};
#endif
