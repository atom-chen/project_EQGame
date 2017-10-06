/******************************************************************************
* ����: Y3
* ʱ��: 2010-9-30
* ����: ����༭. ����TD
******************************************************************************/
#ifndef __REGIONEDITACTION_H__
#define __REGIONEDITACTION_H__

#include "UnreachRegion.h"
#include "Action.h"

#define		REGION_VER0			"[REGION_00]"
#define		CUR_REGION_VER		REGION_VER0

#define		FUNCTION_KEY        0X00000000
#define		FUNC_KEY_SHIFT		0X00000001
#define		FUNC_KEY_CTRL		0X00000002
#define		FUNC_KEY_ALT		0X00000004

enum OPERATOR_TYPE
{
	NO_OPERATOR   = 0,
	ADD_NEW_POINT,
	MODITY_POINT_POSITION,
};

typedef std::map<unsigned long, CUnreachRegion*> UNREACHREGIONMAP;

class RegionEditManager
{
public:
	class Listener
	{
	public:
		//������������ص�����
		virtual void onRegionAddNewPoint(unsigned long ulRegionId,unsigned long ulPointId, Ogre::Vector3 position) {}

		// ɾ��һ����Ļص�����
		virtual void onRegionDelPoint(unsigned long ulRegionId,unsigned long ulPointId) {}

		// ѡ��һ������
		virtual void onRegionSel(unsigned long ulRegionId) {}
	};
public:

	// �������������б�
	UNREACHREGIONMAP m_UnreachRegionMap;

	// �Ƿ���ʾ����.
	bool  m_bIsShow;

	Listener* mListener;

public:
	RegionEditManager();
	~RegionEditManager();

	void setListener(Listener* l) {mListener = l;}

	////////////////////////////////////////////////////////////////////////////////////
	//
	// ���̺Ͷ��̲���
	//

	// �������ȡ�ļ�
	unsigned long ReadRegionFromFileVer0(std::string strFileName);

	// ������洢���ļ�
	unsigned long SaveRegionToFileVer0(std::string strFileName);

	// �������ȡ�ļ�
	unsigned long ReadRegionFromFile(std::string strFileName);

	// ������洢���ļ�
	unsigned long SaveRegionToFile(std::string strFileName);

public:
	////////////////////////////////////////////////////////////////////////////////////
	//
	// �༭�����������������
	//
	Ogre::SceneNode*	m_pSceneNode;			// ������ʾ����༭���ܵĽڵ�.

	unsigned long m_ulCurMaxId;					// ��ǰ��������ID
	unsigned long m_ulCurEditRegionId;			// ��ǰ���ڱ༭�������id
	unsigned long m_ulCurEditPointId;			// ��ǰ���ڱ༭�ĵ��id
	unsigned long m_ulCurEditLineId;			// ��ǰ���ڱ༭���ߵ�id

	// �õ���ǰ���ڱ༭�������id
	int GetCurEditRegionId()
	{
		return m_ulCurEditRegionId;
	}

	//���õ�ǰ����id
	void SetCurEditRegionId(unsigned long ulRegionId)
	{
		m_ulCurEditRegionId = ulRegionId;
	}

	// �õ���ǰ���ڱ༭�ĵ��id
	int GetCurEditPointId()
	{
		return m_ulCurEditPointId;
	}
	//���õ�ǰ��id
	void SetCurEditPointId(unsigned long ulPointId)
	{
		m_ulCurEditPointId = ulPointId;
	}

	// �õ���ǰ���ڱ༭���ߵ�id
	int GetCurEditLineId()
	{
		return m_ulCurEditLineId;
	}//

	//���õ�ǰ��id
	void SetCurEditLineId(unsigned long ulLineId)
	{
		m_ulCurEditLineId = ulLineId;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// �������
	//

	// ���һ���µ�����
	unsigned long AddNewRegion();

	// ͨ��ָ����id�� ɾ��һ������
	unsigned long DelRegion(unsigned long ulId);

	// ���������������.
	void ClearRegionData();

	// ����ָ����id�� �õ�����
	CUnreachRegion* GetRegionById(unsigned long ulId);

	// ѡ��һ������
	void SelRegion(unsigned long ulId, bool bIsChangeCamera = true);

	// ��ѡ��һ������.
	void UnSelRegion(unsigned long ulId);

	// ����һ����������
	void HideRegionArea(unsigned long ulRegionId, bool bHide);

	// ��ǰ�������Ƿ�Ϸ�
	bool IsRegionValidate(unsigned long ulRegionId);

	// ���´�����ʾ���������
	void ReshapeArea(unsigned long ulRegionId);

	UNREACHREGIONMAP* GetUnreachRegionMap() { return &m_UnreachRegionMap; }

	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// �����
	//

	// ��ǰ����֮ǰ�ĵ��λ��.
	Ogre::Vector3 m_preDoPointPos;

	// ��ǰ����֮ǰ�ĵ��λ��.
	Ogre::Vector3 m_curPointPos;

	// �ڵ�ǰ�༭�����������һ���µĵ�
	unsigned long AddNewPoint(Ogre::Vector3 position);

	// ָ������id�� ָ�����id�� ָ��λ�ã� ���һ��С��.   
	void AddNewPoint(unsigned long ulRegionId, unsigned long ulPointId, Ogre::Vector3 position);

	// ָ������id�� ָ�����id�� ָ��λ�ã� ɾ��һ��С��. 
	void DelPoint(unsigned long ulRegionId, unsigned long ulPointId);

	// ������ 
	bool EditPoint(const Ogre::Vector2& pt);

	// ѡ��һ����.
	void SelPoint(unsigned long ulPointId);

	//void DelPoint(unsigned long ulPointId);

	// �ı䵱ǰ�༭�ĵ��λ��, �������������
	void ChangeCurEditPointPos(const Ogre::Vector2& pt);

	// �ı�ָ��������ָ���ĵ��λ��
	void ChangeCurEditPointPos(unsigned long ulRegionId, unsigned long ulPointId, Ogre::Vector3 position);

	// ��¼��ǰ����ǰһ�ε��λ��.
	void RecordPreDoPointPos();

public:

	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// �����Ĳ���
	//

	// ǰһ������������.

	//int m_iPreOperatorType;

	// ��ʼ������༭��Ҫ��һЩ����
	void InitRegionEdit();

	// ���������༭������С��Ĳ���
	void SetOldHitPointMaterial();

	// ��������༭�ĳ����ڵ�
	void CreateSceneNode();

	// ��������༭�����ڵ�
	void HideScene(bool bHide);

	// ���ݵ��εĸ߶ȸı�༭������
	void SetRegionHeight();

	//
	bool m_bShowDeepBuf;

	// �ı�û����Ȼ������Ĳ���
	void ChangeNoDeepMaterial();

public:

	// �������λ�ò���.
	void SetCamera(float fx, float fy);

};



class CRegionEditAction : public Action
{
public:
	static CRegionEditAction* create();

public:
	CRegionEditAction();
	const std::string& getName(void) const;


protected:


	// ���������麯��.
	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

	// ��ǰ����active����.
	void _onActive(bool active);

	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

protected:
	RegionEditManager*	m_Handle;

	// ǰһ������������.

	int m_iPreOperatorType;

};


#endif