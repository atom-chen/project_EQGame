/******************************************************************************
* ����: Y3
* ʱ��: 2009-01-05
* ����: ���ݲ�����װ����
******************************************************************************/
#ifndef HSCFitmentCtrl_H
#define HSCFitmentCtrl_H

#include "EQMainPrerequisites.h"

#include "HouseSceneComponent.h"
#include "HouseComponentGridGraphics.h"
#include "HouseSceneGrid.h"



/** ���ݲ�����װ����
@remarks ���ṩɾ��mComp����, ����Ϊ����಻Ӧ���������Ϊ
		�ͻ���Ҳ��Ҫ��װ����, ���Ա����й��õ�һ����װ������
*/
class HSCFitmentCtrl
{
public:
	enum PutMode 
	{
		pmEditor = 0,	// �༭��ģʽ
		pmPlayer,		// ���ģʽ
	};
public:
	/// ���Ӳ���
	void attach(HouseSceneComponent* hsc);

	/// �������
	HouseSceneComponent* detach();

	/// ��ȡ���ӵĲ���
	HouseSceneComponent* getAttach() { return mComp; }

	/// ִ��
	void exec(const Ogre::Ray& ray);

	/// ��ʼ��ת
	void rotStart();

	/// ��ת����
	void rotEnd();

	/// ��ת�Ƕ�
	void rotate(float degree);

	/// ���ðڷ�ģʽ
	void setPutMode( PutMode val ) { mPutMode = val; }
	PutMode getPutMode() const { return mPutMode; }

	Ogre::SceneNode* getSceneNode() { return mSceneNode; }
public:
	HSCFitmentCtrl();
	~HSCFitmentCtrl();
	
	class Listener
	{
	public:
		virtual ~Listener() {}

		virtual void notifyCanLay( HouseSceneGrid * grid , int face, int height, HouseSceneComponent * compRep ) = 0; 
		virtual void notifyForbidLay(HouseReturnValue hrv) {}
		virtual void notifyFaceChange(int face) {}
	};

	void setListener( Listener* l ) { mListener = l; }

private:
	HSCFitmentCtrl( const HSCFitmentCtrl & ); //no copy
	HSCFitmentCtrl & operator = ( const HSCFitmentCtrl & );

	void enterScene();
	void leaveScene();

	bool calcLay(HouseSceneGrid* grid);

private:
	bool					mRoting;
	int						mFace;
	/// �Ƿ����ģʽ, ���ģʽ�²��ܰڷ��Ŵ���ֻ���滻�Ŵ�
	PutMode					mPutMode;

	HouseSceneComponent *	mComp;
	Ogre::SceneNode *		mSceneNode;
	HouseSceneGrid *		mLastOperateGrid;
	Listener *				mListener;

	HouseComponentGridGraphics	mCompGridGraphics;
};


class HSCFitmentHelper
{
public:
	// ȡ����Դ����
	static std::string fetchResName();
	// ȡ�ô���ѶϢ
	static std::string getFitmentErrorStr( int err );
};

#endif
