/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-9-3
* ����: ��̬�ؼ��������
******************************************************************************/
#ifndef DynamicCtrlVector_H
#define DynamicCtrlVector_H

#include "EQMainPrerequisites.h"
#include "CEGUI.h"


/*************************************************************************
*	��̬�ؼ����������
*************************************************************************/
class DynamicCtrlVector
{
public:
	//	�ؼ����뷽ʽ
	enum AlignType
	{
		AT_LEFT,	//	�����
		AT_CENTER,	//	���ж���
		AT_RIGHT,	//	�Ҷ���
		RIGHT_SIDE	//	�ؼ���ǰһ���ؼ��ұ߱�־
	};

	// ��������ʾ��ʽ
	enum ScrollbarShowType
	{
		SST_SHOW,	// �߶ȹ̶���������ʾ
		SST_HIDE,	// �߶Ȳ��̶�������ʾ���߶ȸ��������Զ�����
		SST_AUTO	// �߶ȹ̶������������Զ���ʾ������
	};

	//	�ؼ�����ṹ��
	struct ctrlObject
	{
		CEGUI::Window *ctrl;	//	�ؼ�����
		float x;				//	x����
		float y;				//	y����
		float hor;				//	�ؼ�ˮƽǰ����϶
		float ver;				//	�ؼ���ֱǰ����϶
		AlignType alignType;	//	�ؼ���ˮƽ���뷽ʽ
	};

public:
	/**
	*	���캯��
	*/
	DynamicCtrlVector();

	/**
	*	��������
	*/
	~DynamicCtrlVector();

	/**
	*	��	�ܣ���ʼ����̬�ؼ��������
	*	��	����parent:	������
	*	����ֵ��void
	*/
	void initialise(CEGUI::Window* parent);

	/**
	*	��	�ܣ���ʼ����̬�ؼ��������
	*	��	����parentName:	����������
	*	����ֵ��void
	*/
	void initialise(std::string parentName);

	/**
	*	��	�ܣ���ӿؼ�
	*	��	����ctrl:	�ؼ�
	*			x:	�ؼ�������е�x����
	*			y:	�ؼ�������е�y����
	*	����ֵ��void
	*/
	void addControl(CEGUI::Window* ctrl, float x, float y);

	/**
	*	��	�ܣ���ӿؼ�
	*	��	����ctrlName:	�ؼ�����
	*			x:	�ؼ�������е�x����
	*			y:	�ؼ�������е�y����
	*	����ֵ��void
	*/
	void addControl(std::string ctrlName, float x, float y);

	/**
	*	��	�ܣ���ӿؼ�
	*	��	����ctrl:	�ؼ�
	*	����ֵ��void
	*/
	void addControl(CEGUI::Window* ctrl);

	/**
	*	��	�ܣ���ӿؼ�
	*	��	����ctrlName:	�ؼ�����
	*	����ֵ��void
	*/
	void addControl(std::string ctrlName);

	/**
	*	��	�ܣ����ù�������ʾ��ʽ
	*	��	����showType:	��������ʾ��ʽ��SST_SHOW��SST_HIDE��SST_AUTO��
	*	����ֵ��void
	*/
	void setScrollbarVisible(std::string showType);

	/**
	*	��	�ܣ����ù�����λ��
	*	��	����pos:	λ��
	*	����ֵ��void
	*/
	void setScrollbarPosition(float pos);

	/**
	*	��	�ܣ����ÿؼ��������ؼ��ͱ߿��ˮƽ����ֱ��϶
	*	��	����val:	��϶
	*	����ֵ��void
	*/
	void setInterval(float val);

	/**
	*	��	�ܣ����ÿؼ���ˮƽǰ����϶
	*	��	����hor:	��϶
	*	����ֵ��void
	*/
	void setHorInterval(float hor);

	/**
	*	��	�ܣ����ÿؼ��Ĵ�ֱǰ����϶
	*	��	����ver:	��϶
	*	����ֵ��void
	*/
	void setVerInterval(float ver);

	/**
	*	��	�ܣ����ÿؼ��Ķ��뷽ʽ
	*	��	����alignType:	���뷽ʽ��AT_LEFT������룻AT_CENTER�����ж��룻AT_RIGHT���Ҷ��룩	
	*	����ֵ��0�����óɹ�����-1������ʧ�ܣ�
	*/
	int setAlign(std::string alignType);

	/**
	*	��	�ܣ����ÿؼ���ǰһ���ؼ����ұ�
	*	��	����void	
	*	����ֵ��void
	*/
	void setAtRight(void);

	/**
	*	��	�ܣ������崹ֱ�������Ŀ��
	*	��	����void	
	*	����ֵ��float
	*/
	float getScrollbarWidth(void);

	/**
	*	��	�ܣ��Ƴ���̬�ؼ��������������пؼ�
	*	��	����void
	*	����ֵ��void
	*/
	void removeAllControl(void);

	/**
	*	��	�ܣ��ؼ��Ű棨�����ã�
	*	��	����void
	*	����ֵ��void
	*/
	void controlTypeset(void);

protected:
	float mScreenWidth;			//	��Ļ���

	float mScreenHeight;		//	��Ļ�߶�

	std::vector<ctrlObject> mCtrlVector;	//	�ؼ�����

	CEGUI::Window *mRoot;		//	������

	CEGUI::ScrollablePane *mPane;	//	���

	ScrollbarShowType mScrollbarShowType;	// ��������ʾ��ʽ

	float mInterval;			//	��϶

	float mHorInterval;			//	�ؼ�ˮƽǰ����϶

	float mVerInterval;			//	�ؼ���ֱǰ����϶

	AlignType mAlignType;		//	�ؼ�λ��

};	//	end of class DynamicCtrlVector

/**
*	��	�ܣ�������̬�ؼ�����������󣨱Ƚχ������,Ϊ���ṩ��Lua���ã�
*	��	����void
*	����ֵ����̬�ؼ������������
*/
DynamicCtrlVector createDynamicCtrlVector(void);


#endif	//	end of guard DynamicCtrlVector_H
