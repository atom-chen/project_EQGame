/******************************************************************************
* 作者: xj
* 时间: 2010-11-24
* 描述: 文件管理接口
******************************************************************************/

#pragma once

#ifndef __FileManager_H
#define __FileManager_H

class FileManager
{
public:
	/*
	说明：将源文件移至目标位置
	参数
	  target：目标相对位置，可能是一个目录或者包
	  file：  源文件相对位置
	*/
	virtual bool moveTo(const char *target, const char *file) = 0;

	/*
	说明：将源文件批量移至目标位置，对于目标类型为包的可以提高效率
	参数
	  target：目标相对位置，可能是一个目录或者包
	  files： 源文件相对位置列表
	  count： 源文件列表个数
	*/
	virtual bool moveToBatch(const char *target, const char **files, int count) = 0;

public:
	FileManager(void) {}
	virtual ~FileManager(void) {}
};

#endif