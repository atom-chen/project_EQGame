/******************************************************************************
* ����: xj
* ʱ��: 2010-11-24
* ����: Ŀ¼�ļ�������
******************************************************************************/

#pragma once

#ifndef __DirFileManager_H
#define __DirFileManager_H

#include "FileManager.h"

class DirFileManager : public FileManager
{
public:
	virtual bool moveTo(const char *target, const char *file);
	virtual bool moveToBatch(const char *target, const char **files, int count);

public:
	DirFileManager(void);
	~DirFileManager(void);
};

#endif