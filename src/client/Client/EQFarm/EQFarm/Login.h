/*******************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: LOGIN管理
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

	// 请求登录
	virtual BOOL LoginRequest();
	// 请示进入场景
	virtual BOOL ReqEnterGame();
	// 返回登录结果
	virtual void LoginReply();
	// 返回角色列表
	virtual void RolesReply();

};
#endif