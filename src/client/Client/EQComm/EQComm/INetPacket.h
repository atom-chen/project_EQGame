/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	����Э������ 
	��������Э��������Զ�Э�����ɹ������ɣ������ֹ�ֶ��޸ģ�
==============================================================================*/
#pragma once

#include "ByteArray.h"

class INetPacket
{
public:
	virtual void build(ByteArray &byteArray){};
};