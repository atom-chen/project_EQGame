/*******************************************************************************
* ����: ��_Mike
* ʱ��: 2010-06-11
* ����: ����ũ��
*******************************************************************************/
#ifndef FUNNYFARM_H
#define FUNNYFARM_H

#include "FarmObjEvent.h"

#include "gamestate.h"
#include "FarmDecoration.h"
#include "CEGUI.h"
#include "CEGUIEx/CEGUIAnimate.h"
#include "CEGUIEx/CEGUIAnimateBox.h"

class FarmScene;
class FarmPlantUnit::PickObjectUnit;

using namespace std;

/**��궯����ʽ
*/
enum FarmMouseAnimate
{
	MA_PUTBUG = 1,
	MA_WATER,
	MA_PICKBUG,
	MA_ONEKEYHARVEST,
	MA_WEED,
	MA_STEAL,
	MA_HARVEST,
	MA_TILL,
	MA_PUTGRASS,
	MA_USE_PROP,
};

enum FarmCursorState
{
	CS_FARM_NORMAL,
	CS_FARM_MOVE,
	CS_FARM_TILL,			// ������ή����
	CS_FARM_WATER,			// ��ˮ
	CS_FARM_WEED,			// ����
	CS_FARM_PICKBUG,		// ����
	CS_FARM_HARVEST,		// ��ժ
	CS_FARM_ONEKEYHARVEST,	// һ����ժ
	CS_FARM_SEEDING,		// ��ֲ
	CS_FARM_PUTBUG,			// �ų�
	CS_FARM_PUTGRASS,		// �Ų�
	CS_FARM_STEAL,			// ͵
	CS_FARM_USE_PROP,
};

//����ũ������
struct SelfFarmInfo 
{
	SelfFarmInfo()
		:mSaccount("")
		,mSusername("")
		,mSlev(0)
		,mSexp(0)
		,mfarmState(5)
	{
	}
	string mSaccount;
	string mSusername;
	int	mSlev;
	//farm_pet mSpet;
	int mSexp;
	farm_pet_food	mSpetFood;
	farm_decoration mSdeco;
	int mfarmState;

};

//һ����ժ��Ϣ
struct AllPickInfo 
{
	AllPickInfo()
		:index(0)
		,dexp(0)
	{
	}
	int index;
	int dexp;
	farm_crop_data cropData;
};

//��ժ������Ϣ
struct PickAwards 
{
	PickAwards()
		:awardIdx(0)
		,money(0)
		,count(0)
	{
	}

	int awardIdx;
	int money;
	int count;
};

//��ժ����ҧ����Ϣ
struct AllHintDatas
{
	AllHintDatas()
		:msg_id(0)
		,hintCount(0)
		,loseGold(0)
	{
	}
	
	int msg_id;
	int hintCount;
	int loseGold;
};
//һ����ժ����ҧ�Ӻ�Ʈ��������Ϣ
struct HintData 
{
	HintData()
	{
	}

	int hint_index;

};
//һ����ժʱƮûǮ͵���Ӻ󶯻�����
struct NomoneyPickData 
{
	NomoneyPickData()
		:plot_id(0)
		,msg_id(0)
	{
	}

	int plot_id;
	int msg_id;
};
//ũ��װ��ʵ��ID����ƷID�ṹ
struct FarmDecID 
{
	FarmDecID()
		:ins_id("")
		,tplt_id(0)
	{
	}

	std::string ins_id;
	int			tplt_id;
};
//����ũ��
class FunnyFarm :
	public GameState
{
public:

	static FunnyFarm& getSingleton()
	{
		static FunnyFarm farm;
		return farm;
	}

	virtual void init();
	virtual void update();
	virtual void Fupdate();
	virtual void cleanup();
public:
	/*������볡��
	@param ����ĳ���������*/
	void reqEnterFarm(string owner);
	
	/*֪ͨũ������*/
	void notiFarmData();

	/**
	*/
	void assartPlot(int index);
	/*���󿪿�ũ��*/
	void reqAssartPlot();

	/* �������*/
	void reqHoeing(int plotIdx);

	/*������ֲ
	@param plotIdx ũ������
	@param cropTepid ũ�����ģ��ID*/
	void reqSow(int plotIdx,uint64 item_inst_id);

	/**
	*/
	void reqUsePropStr(int plotIdx, std::string item_inst_id);


	/*����ʹ�õ���
	@param plotIdx ũ������
	@param cropTepid ũ�����ģ��ID*/
	void reqUseProp(int plotIdx, uint64 item_inst_id);

	/*�����ժũ����
	@param owner ũ����
	@param plotIdx �ؿ�����*/
	void reqPickCrop(string owner, int plotIdx);
	/**
	*/
	void reqPickAllCrops(string owner);
	
	/*�����޸Ĺ���
	@param id ����ID*/
	void reqChgBulletin(int id);

	/*�����ú���ũ����״̬
	@param page �����б��ҳ��*/
	void reqFriendFarmState();
	void notiFriendFarmState();

	/*֪ͨ����ũ����״̬
	@param data ״̬�б�
	*/
	//void notiFriendFarmState(string) 

	/*������ݳ��潽ˮ,�Ųݷų�
	@param ũ����
	@param �ؿ�����
	*/
	void reqWeed(string owner, int plotIdx);
	void reqWorn(string owner, int plotIdx);
	void reqWater(string owner, int plotIdx);
	void reqPutGrass(string owner, int plotIdx);
	void reqPutPest(string owner, int plotIdx);

	void reqAnnounceList();
	/*
	������־�б������־���������ԣ����������б�
	����������ԣ�����ظ�����
	*/
	void reqLogList(int type, const std::string & account);
	void reqclearLog();
	void reqComment(string account,string comment);
	void reqCommentList(const std::string & account);
	void reqClearComment();
	void reqReplyComment(string account,string comment);

	/*ҧ��
	@param hurt �˺�ֵ
	*/
	void notiBite();
	void notiUsePet();
	void notiStopPet();
	void notiFoodEmpty();
	void notiChgBulletin();

	/*
	֪ͨ��־�б������б����Խ��
	*/
	void notiLogList();
	void notiCommentList();
	void notiComment();

	/*
	���������������ֿ���Ʒ�����������ֿ�������Ʒ����������һ����Ʒ��֪ͨȫ�������ļ۸�
	*/
	void reqLockStorageItem( std::string instanceID);
	void reqUnLockStorageItem( std::string instanceID);
	void reqAllSell(int type);
	void reqOneSell( std::string instanceID, int count);

	// ��������
	void reqBuyGoods(int templateId, int type, int count,int pay_type);

	//�����ظ�����
	void notiExistPurchase();

	void notiAllSell();

	/*ũ������
	����ũ������  ֪ͨũ������ ��������ũ������ 
	*/
	void reqSetting();
	void reqSetSetting();
	std::string& replacestring(std::string& source_str,std::string find_str);
	void reqSettingDefault();
	void notiFarmSetting();
	// ũ�����ûָ�Ĭ��ֵ
	void notiSettingDefault();
	void setSetting(const notify_farm_setting & data);
	void setSettingFlag(bool flag){ mResetFlag = flag;}
	bool getSettingFlag(){return mResetFlag;}
	void notiSetResult();
	void notiExpData();
	void notiFarmInfo();
	void notiAllPickData();
	void notiFarmMoney();
	void notiFarmEqGold();
	void notifyFarmAwards();
	/**�������װ��
	*/
	void reqChangeDec(std::string instanceID,int tpltID);

	/**�����Ƴ�װ��
	*/
	void reqRemoveDec(int tpltID);

	/**���������
	*/
	void reqCanUpgradeLand();
	void reqUpgradeLand();

	/**���EQ��Ϣ
	*/
	void notiPlayerEqInfo();

/// ũ�����߼�
public:
	//�Ǹ���
	bool isPlot(int index);
	//������
	bool hasCrop(int index);
//	bool hasCrop(int index);
	void onClickedUnit(FarmPlantUnit::PickObjectUnit* objunit);

	//���ò�������״̬
	void setAction(int act){mAction = act;}

	//��ȡ����״̬
	int getAction(){return mAction;}

	//��ȡ����ũ������Ϣ
	SelfFarmInfo getSelfFarmInfo(){return mSelfFarmInfo;}
	void removePest(int index);
	void removePickUI(int index);
	void removeFarmPaoPao(int index);
	void removeHavestUI(int index);
	void removeDexpUI(int index);
	void clearLand(FarmScene* farmScene,int index);
private:
	void _updateCrops();

	void _startupPlantUnits();
	int _calculSecs(CropsCtrl& cropctrl);
	void _reqSowAction(string owner, int plotIdx);
	void _reqUseProp(int plotIdx);
	void _addMouseAnimateWnd();
	void _initFarmUI();
	void _addAnimateImg(int num,int time, std::string imgset,std::string imgName,int dx = 10,int dy =50);
	void pushAllPickInfo(int index,int aexp,farm_crop_data cropData);
	void pushNomoneyPickInfo(int plot_id,int msg_id);
	bool popAllPickInfo();
	bool popNomoneyPickInfo();

	DWORD			mCropsStartPos
					,mLastElapsed
					;

	int				mAction;					//��������
	CEGUI::Window*	mMouseAnimateWnd;			//��궯������
	CEGUI::Animate	mMouseAnimate;
	CEGUI::AnimateBox *mAniWnd;

public:
	string			mAccount;					//ũ������
	string			mLastAcc;					//�ϴν����ũ�����
	string			mUsername;
	string			mSeedingImgset;
	string			mSeedingIcon;
	SelfFarmInfo	mSelfFarmInfo;			//����ũ����Ϣ
	int				mLev						//ũ���ȼ�
					,mExp						//ũ������
					,mBulletin					//��������
					,mState						// ũ��״̬
					,mCropNum					//ũ��������
					,mGameGold
					,money						//ũ�����
					,mEqmoney					//ũ��EQ��
					;
	std::queue<AllPickInfo> mAllPickBuffer;
	std::vector<PickAwards> mPickAwardsVec;
	AllHintDatas			mAllHintData;
	std::queue<HintData>	mHint_index_que;
	std::queue<NomoneyPickData> mNomoneyPickData;

	typedef std::vector<CropsCtrl> CropsCtrlList;
	typedef std::vector<CropsCtrl>::iterator CropsCtrlListIter;
	CropsCtrlList	mCropsCtrl;//ũ����

	FarmDecoration	mDecoration;	//ũ��װ��
	//farm_pet		mPet;							//���ﹷ

	//ũ������
	FarmScene*		mFarmScene;

	FunnyFarm(void)
		:mResetFlag(false),
		mSetResult(0)
		,mCropsStartPos(0)
		,mpushFlag(0)
		,mIsOneKeyHavSta(false)
		,m_curOperateType(CS_FARM_NORMAL)
//		,mClickHavest(false)
	{}
	
	virtual ~FunnyFarm(void)
	{}

	//---------------ũ������------------------
	bool mResetFlag;
	int  mSetResult;
	//
	int mpushFlag;
	bool mIsOneKeyHavSta;
	stime mCommPetFoodPeriod;
//	bool mClickHavest;

public:
	///*�õ�ũ����
	//@param idx ũ��������
	//*/
	//farm_crop_data& getCrop(int idx)
	//{
	//	assert(idx>=0 && idx <mCropNum);
	//	return mCropsCtrl[idx].crop;
	//}
	/*�õ�ũ���������
	@param idx ũ��������
	*/
	const CropsCtrl& getCropCtrl(int idx)
	{
		assert(idx>=0 && idx <mCropNum);
		return mCropsCtrl[idx];
	}

public:
	/**����ũ�����״̬
	*/
	bool hasDamage(CropsCtrlList& cropCtrl,std::vector<farm_crop_data>& data,bool& hasDrought,bool& hasPest,bool& hasGrass,bool& hasGain);
	bool setSelfFarmState(CropsCtrlList& cropCtrl,std::vector<farm_crop_data>& data);
	void setFarmCurState(int sta, std::string imageset = "", std::string icon ="");
	bool setCurStaCur(bool hasPest, bool hasGrass,bool hasDrought);
	void setSeedingCur(int sta, std::string imageset, std::string icon);
	void _updatePos(POINT posabs);
	void hideMouseAnimateWnd();
	void playMouseAnimate();
	void getMouseAniInfoFromXml(int id,int dx=10,int dy=50);
	bool canPick(int index);
	void setPushFlag(int fg);
	void setHarvestDymanicUI(int index,int templtID,int cropsnum);
	void setHintDymanicUI(int index,int imageID,int lose_gold);
	int getFarmMoney(){return money;}
	int getFarmEqMoney(){return mEqmoney;}
	void checkAllpickAward();
	void checkAllHintData();
	void saveCurDec_tolua(farm_decoration dec);
	int getSelfDogFoodRemainSecs();
	int getCommDogFoodRemainSecs();

// ũ������ͷ���õ�(add by hezhr)
public:
	void insertAccountList(std::string account);
	void reqEqUsersInfo(void);
protected:
	std::vector<std::string> m_accountList;

// ũ������(2011/4/14 add by hezhr)
public:
	void reqFarmTaskList(void);
	void handle_farm_task_list(void);
	void reqFarmTaskComplete(int task_id);
	void handle_farm_task_complete(void);
	int getFarmTaskCount(void);
	int getFarmTaskIdByIndex(int index);
	bool existFarmTask(int task_id);

// ũ��������(2011/6/23 add by hezhr)
public:
	void handle_wallow_time(void);

protected:
	std::vector<player_task> m_farmTaskList;
	FarmCursorState m_curOperateType;	// ��ǰ����������

};

#define sFunnyFarm FunnyFarm::getSingleton()
#endif