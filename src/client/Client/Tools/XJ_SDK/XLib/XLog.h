#pragma once

#ifndef __XLog_H
#define __XLog_H

class XLog
{
public:
	//一次最大只能输出8k大小
	static void Trace(const char *format, ...);
	//一次最大只能写入8k大小
	static void Write(const char *format, ...);
	static void SetLogFile(const char *file);
	static void SetLogFileMaxSize(long size);

public:
	XLog(void);
	~XLog(void);
};

#endif