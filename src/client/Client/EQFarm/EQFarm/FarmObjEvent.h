#ifndef FARMOBJEVENT_H
#define FARMOBJEVENT_H

#include "FarmScene.h"
#include "NetPacket.h"
#include "InputDef.h"
class FarmScene;
class FarmPlantUnit::PickObjectUnit;

class FarmObjEvent
	: public EQInputEventListener
{
public:
	static FarmObjEvent& getSingleton()
	{
		static FarmObjEvent singleton;
		return singleton;
	}

	/// ����¼���Ӧ
	void 	onMovein(FarmPlantUnit::PickObjectUnit* objunit,
						Ogre::ColourValue& highlight);
	void 	onMoveout(FarmPlantUnit::PickObjectUnit* objunit);
	void 	onClicked(FarmPlantUnit::PickObjectUnit* objunit);
	void	onMoveinFarmObj(FarmObject* farmObj,Ogre::ColourValue& highlight);
	void	onMoveoutFarmObj(FarmObject* farmObj);
	void	onClickFarmObj(FarmObject* farmObj);

	bool	hasObjOnEvent();
	//
	bool getIsPickUI(){return mIsSetPickUI;}
	//
	void setIsPickUI(bool ispickui);

	virtual void LButtonUp( UINT& flags, POINT point );

	virtual void LButtonDown( UINT& flags, POINT point );
	////
	virtual void MouseMove	( UINT& flags, POINT delta, POINT point );






private:
	FarmObjEvent(void);
	~FarmObjEvent(void);

	FarmPlantUnit::PickObjectUnit*	mObjOnEvent;
	int								mIndex;
	bool							mIsSetPickUI;

};

//ũ�������
class CropsCtrl 
{
public:
	friend class FunnyFarm;
	CropsCtrl();

	enum LandType
	{
		NIL    = -1,
		NORMAl = 0,
		RED    = 1,
		COUNT  = 2,
	};

	int hasState(int state)
	{
		int count = 0;
		for (int i=0; i< (int)crop.curr_damage.size(); i++)
		{
			if (crop.curr_damage[i] == state)
				count++;	
		}
		return count;
	}

#define SECONDS_PER_MINUE 60
#define SECONDS_PER_HOUR (60 * 60)
#define SECONDS_PER_DAY (60 * 60 * 24)
	/*
	��ȡ�¸������׶�ʣ��ʱ��*/
	stime getRemainStime()
	{
		stime time;
		time.second = remainsecs % SECONDS_PER_MINUE;
		time.minute = (remainsecs % SECONDS_PER_HOUR) / SECONDS_PER_MINUE;
		time.hour   = (remainsecs % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
		time.day    = remainsecs / SECONDS_PER_DAY;
		return time;
	}
	/*
	��õ�ǰ�����׶�*/
	int getCurStage(){return curStage;}
	/*
	��õ�ǰ��������ֵ*/
	int getHealth(){return crop.health;}
	/*
	��õ�ǰʣ�����*/
	int getRemain(){return crop.remain;}
	/*
	����������*/
	int getOutput(){return crop.output;}
	/**����Ƿ��ѱ���ժ
	*/
	int getPicked(){return crop.picked;}
	/**��ò�ժ��
	*/
	int getPickedNum(){return crop.pick_num;}
	////
	//void init(FarmScene* farmScene);
	void startup(FarmScene* farmScene);
	void update();

	//����ID
	int getCropTpltID() const
	{
		return crop.tempid;
	}

	//�Ƿ�����ֲ��
	bool hasCrop() const
	{
		return crop.tempid > 0;
	}

	//
	int getRemainTime(){return remainsecs;}

protected:
	int  _getCropStageWithRemain();
	void _cropModelDisplay();
	void _getCropModelCfg(int tempid, int stage, string& mesh, string& material);
	//��ʱ�ų�ͳ���Ч��
	void putPest(int index,int num);
	void setPickUI(int index);
	//void removePest(int index);


	farm_crop_data	crop;
	int				curStage;
	int				remainsecs;
	int				index;

	std::string		landMesh
		,landMtrl
		,grassMess
		,grassMtrl
		;
	FarmScene*		mFarmScene;

};

#define sFarmObjEvent FarmObjEvent::getSingleton()

#endif