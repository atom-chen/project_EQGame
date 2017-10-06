/******************************************************************************
* 作者: xj
* 时间: 2010-11-24
* 描述: 包文件管理类
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