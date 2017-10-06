/******************************************************************************
* ����: xj
* ʱ��: 2010-11-24
* ����: ���ļ�������
******************************************************************************/

#pragma once

#ifndef __PackFileManager_H
#define __PackFileManager_H

#include "FileManager.h"

class PackFileManager : public FileManager
{
public:
	virtual bool moveTo(const char *target, const char *file);
	virtual bool moveToBatch(const char *target, const char **files, int count);

public:
	PackFileManager(void);
	~PackFileManager(void);
};

#endif