/******************************************************************************
* ����: Mike
* ʱ��: 2010-7-05
* ����: �����¼�����
******************************************************************************/
#ifndef INPUTDEF_H
#define INPUTDEF_H

/** EventPriorityFlags
	1�����з��������¼������ȼ�
	2����ʶǰ�洦������¼�
*/
#define	epfUI				(1 << 12)
#define	epfHotkey			(1 << 11)
#define	epfDragItem			(1 << 11)
#define	epfCameraFly		(1 << 10)
#define	epfFitEvent			(1 << 8)
#define	epfPick				(1 << 6)
#define	epfCursor			(1 << 4)
#define	epfMainplayer		(1 << 2)
#define	epfNone				0

/** �����¼�����
*/
class EQInputEventListener
{
public:
public:
	EQInputEventListener( int priority  ) : mPriority(priority) 
	{}
	virtual ~EQInputEventListener() {}

	/// ��ȡ���ȼ�
	int getPriority() const { return mPriority; }

public: //�¼����
	/** �����������¼�
	@param processed �������֮ǰ�Ƿ��Ѿ��������
	@return ����δ�������
	*/
	virtual void LButtonDown( UINT& flags, POINT point ) { }
	virtual void LButtonUp	( UINT& flags, POINT point ) { }
	virtual void RButtonDown( UINT& flags, POINT point ) { }
	virtual void RButtonUp	( UINT& flags, POINT point ) { }
	virtual void MButtonDown( UINT& flags, POINT point ) { }
	virtual void MButtonUp	( UINT& flags, POINT point ) { }
	virtual void MouseMove	( UINT& flags, POINT delta, POINT point ) { }
	virtual void MouseWheel	( UINT& flags, POINT point, short zDelta ){ }
	virtual void KeyDown	( UINT& flags, UINT nChar, UINT nRepCnt ) { }
	virtual void KeyUp		( UINT& flags, UINT nChar, UINT nRepCnt ) { }

	virtual void Update		(float timElapsed) { }
private:
	/// ���ȼ� ��ʼ�����Ͳ������޸�
	int		mPriority;
};


#endif
