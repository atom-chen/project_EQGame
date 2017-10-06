/******************************************************************************
* ����: Y3
* ʱ��: 2009-12-10
* ����: Mfc�����¼�����
******************************************************************************/
#ifndef MfcInputEvent_H
#define MfcInputEvent_H


/** �����¼�����
*/
class MfcInputEvent
{
public:
	/** ���з��������¼������ȼ������ڲ鿴������
	*/
	enum EventPriority
	{
		epCameraFly			= 80,	// ���
		epSetRoomID			= 50,	// ���ø��ӵķ���ID
		epTestAutoFindPath	= 50,	// �����Զ�Ѱ·
		epCreateGrid		= 50,	// ��ɾ����
		epWallGrid			= 50,	// ��ɾǽ
		epLinkGrid			= 50,	// ��ɾ��ͨ��Ϣ
		epBrowseGrid		= 50,	// �鿴����
		epBrowseComponent	= 50,	// �鿴���
		epEditFitment		= 50,	// ��װ�ڷű༭
		epTDAction			= 50,	// ����td���¼����ȼ�����
		epSceneObject		= 50,	// ���������ص��¼����ȼ�����
		epNavMesh			= 50,	// ��������
		epOusideCut			= 50,	// ��������޳�
	};
public:
	MfcInputEvent( int priority  ) : mPriority(priority) {}
	virtual ~MfcInputEvent() {}

	/// ��ȡ���ȼ�
	int getPriority() const { return mPriority; }

	/// �Ƿ���TD���¼�
	virtual bool isTDAction() { return false; }

public: //�¼����
	/** �����������¼�
	@param processed �������֮ǰ�Ƿ��Ѿ��������
	@return ����δ�������
	*/
	virtual void LButtonDown( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void LButtonUp( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void RButtonDown( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void RButtonUp( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void MButtonDown( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void MButtonUp( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed ) { }

	virtual void MouseWheel( UINT nFlags, short zDelta, CPoint pt, bool & processed ) { }

	virtual void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed ) { }

	virtual void KeyUp( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed ) {  }
private:
	/// ���ȼ� ��ʼ�����Ͳ������޸�
	int		mPriority;
};


#endif
