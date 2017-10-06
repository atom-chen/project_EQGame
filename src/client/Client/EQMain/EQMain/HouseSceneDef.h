/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-27
* ����: ���ݶ��岿��
******************************************************************************/
#ifndef HouseSceneDef_H
#define HouseSceneDef_H

#include "EQMainPrerequisites.h"

const int HOUSE_DEF_FLOOR_HEGHT		= 280;					//¥��Ĭ�ϸ߶�(����)

const int HOUSE_GRID_SIZE			= 80;					//���ݸ��ӵı߳�(����)
const int HOUSE_GRID_HALF_SIZE		= 40;

const int HOUSE_BLOCKED_GRID_TYPE	= 144;					//�赲���� 
const int HOUSE_VISIBLE_GRID_TYPE	= 127;					//�ɼ����� 

const int WALL_FACE_COUNT			= 4;					//�����м���ǽ
const int WALL_FACE_X				= 0;					//Xǽ
const int WALL_FACE_NX				= 2;					//NXǽ
const int WALL_FACE_Z				= 3;					//Zǽ
const int WALL_FACE_NZ				= 1;					//NZǽ

/// ���ݵĹ۲�ģʽ
enum HouseWatchMode
{
	HW_MODE_HIDE	= 0,		// ����, û���ֹ۲�ģʽ. ������Ҫ����
	HW_MODE_1		= 1,		// �۲�ģʽ1, ��ǽ
	HW_MODE_2		= 2,		// �۲�ģʽ2, ��ǽ
	HW_MODE_3		= 3,		// �۲�ģʽ3, ȫǽ
	HW_MODE_4		= 4,		// �۲�ģʽ4, �ݶ�
};

/// ���ݲ������۲�ģʽ
enum CompBeWatchMode
{
	CW_MODE_ROOF	= 0,		// ��ʾ���ݶ�
	CW_MODE_INWALL	= 1,		// ��ʾ����ǽ
	CW_MODE_OUTWALL	= 2,		// ��ʾ����ǽ
	CW_MODE_COMM	= 3,		// ��ͨ���,ȫ���۲�.
	CW_MODE_LIMIT	= 4,		// ����
};

/// ������ʾ��ʽ
enum GridDisplayMode
{
	GD_MODE_HIDE	= 0,		// ����ʾ, ������, û�����ַ�ʽ
	GD_MODE_GAME	= 1,		// ��Ϸ�зǼ�װģʽҲ�������ַ�ʽ. ��ȻĿǰ��Ч����GD_MODE_HIDEһ��
	GD_MODE_FITMENT	= 2,		// ֻ��ʾ���ض����Ե�����߿�, ����Ϸ��װģʽΪ���ַ�ʽ
	GD_MODE_STYLE	= 3,		// ֻ���ڱ༭�� ��ʾ����ı߿������
	GD_MODE_CONNECTEDNESS	= 4,// ֻ���ڱ༭�� ��ʾ����ı߿�����ͺͷ���ź���ͨ��
};



/// ����ϵͳ��һЩ��������ֵ, ������Щֵ, ����������ʾ��ʾ
enum HouseReturnValue
{
	HRV_FAIL		= -1,		// ʧ��
	HRV_OK			= 0,		// �ɹ�

	//�ڷ��������
	HRV_LAYED		= 1,		// �Ѿ��ڷ�
	HRV_NO_GRID_LAY	= 2,		// û���������
	HRV_WALL_STOP	= 3,		// ��ǽ���赲
	HRV_NEAR_WALL	= 4,		// ���뿿ǽ
	HRV_NO_LAY_SPACE= 5,		// û�аڷſռ�
	HRV_NO_AUTO_LAYWALL	= 6,	// �����Զ���ǽ

	//��ȡ���
	HRV_NO_LAYED	= 20,		// û�ڷ�
	HRV_NOTAKE_FRAME= 21,		// ��ܲ���������ȡ
	HRV_NOTAKE		= 21,		// �޷���ȡ

	//�ڷ��������
	HRV_HAS_COMP_LAY= 40,		// ������ڷ�������
	HRV_NO_MATCH_LAYTYPE = 41,	// �������Բ���
};



#endif
