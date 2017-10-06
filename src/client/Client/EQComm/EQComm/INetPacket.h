/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	网络协议类型 
	！（网络协议代码由自动协议生成工具生成，代码禁止手动修改）
==============================================================================*/
#pragma once

#include "ByteArray.h"

class INetPacket
{
public:
	virtual void build(ByteArray &byteArray){};
};