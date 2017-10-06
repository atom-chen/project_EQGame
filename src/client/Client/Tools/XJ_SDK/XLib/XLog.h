#pragma once

#ifndef __XLog_H
#define __XLog_H

class XLog
{
public:
	//һ�����ֻ�����8k��С
	static void Trace(const char *format, ...);
	//һ�����ֻ��д��8k��С
	static void Write(const char *format, ...);
	static void SetLogFile(const char *file);
	static void SetLogFileMaxSize(long size);

public:
	XLog(void);
	~XLog(void);
};

#endif