/*******************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: LOGIN����
********************************************************************/
#ifndef Login_H
#define Login_H

#include "EQLogin.h"

class Login :
	public EQLogin
{
public:
	Login(void) {}
	virtual ~Login(void) {}

	// �����¼
	virtual BOOL LoginRequest();
	// ��ʾ���볡��
	virtual BOOL ReqEnterGame();
	// ���ص�¼���
	virtual void LoginReply();
	// ���ؽ�ɫ�б�
	virtual void RolesReply();

};
#endif