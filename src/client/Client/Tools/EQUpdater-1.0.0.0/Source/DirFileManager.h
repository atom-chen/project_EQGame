/******************************************************************************
* 作者: xj
* 时间: 2010-11-24
* 描述: 目录文件管理类
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