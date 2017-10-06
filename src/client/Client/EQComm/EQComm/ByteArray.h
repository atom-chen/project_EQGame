/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	字节流类 （网络协议序列化）
==============================================================================*/
#pragma once

#include <vector>
#include <string>
#include "BaseType.h"
using namespace std;



class ByteArray
{
public:
	ByteArray();
	virtual ~ByteArray();

	void print();
	void reuse();
public:
	INT16          read_int16();                      // 从字节流读取16位整形
	UINT16         read_uint16(bool log=true);                     // 从字节流读取16位无符号整形
	//UINT16         readonly_uint16(); //只读，不移指针位置
	
	INT32          read_int();                        // 从字节流读取32位整形
	UINT32         read_uint();                       // 从字节流读取32位整形

	INT64          read_int64();                      // 从字节流读取64位整形
	UINT64         read_uint64();                     // 从字节流读取64位无符号整形
	
	SHORT          read_short();                      // 从字节流读取短整形
	USHORT         read_ushort();                     // 从字节流读取无符号短整形

	LONG           read_long();                       // 从字节流读取长整形
	ULONG          read_ULONG();                      // 从字节流读取无符号长整形

	CHAR           read_char();                       // 从字节流读取字符
	UCHAR          read_uchar();                      // 从字节流读取无符号字符整数

	BOOL           read_bool();                       // 从字节流读取Bool值

	FLOAT          read_float();                      // 从字节流读取浮点数
	DOUBLE         read_double();                     // 从字节流读取双精度数

	BYTE*          read_string(BYTE* info, UINT len); // 从字节流读取字符串
	std::string    read_string();                     // 从字节流读取字符串

	void 		   write_int(INT32);                  // 向字节流写32位整形
	void 		   write_uint(UINT32);                // 向字节流写32位无符号整形

	void 		   write_int16(INT16);                // 向字节流写16位整形
	void 		   write_uint16(UINT16,bool log=true);              // 向字节流写16位无符号整形

	void 		   write_int64(INT64);                // 向字节流写64位整形
	void 		   write_uint64(UINT64);                // 向字节流写64位无符号整形
	
	void 		   write_short(SHORT);                // 向字节流写短整
	void 		   write_ushort(USHORT);              // 向字节流写无符号短整

	void 		   write_long(LONG);                  // 向字节流写长整
	void 		   write_ULONG(ULONG);                // 向字节流写无符号长整

	void 		   write_char(CHAR);                  // 向字节流写字符型
	void 		   write_uchar(UCHAR);                // 向字节流写无符号字符型整数

	void 		   write_bool(BOOL);                  // 向字节流写布尔型

	void 		   write_float(FLOAT);                // 向字节流写浮点数
	void 		   write_double(DOUBLE);              // 向字节流写双精度数

	void 		   write_string(const char* str);     // 向字节流写字符串
	void 		   write_string(const std::string& str); // 向字节流写字符串

public:
	int 		   length() const;                    //字节流当前长度
    
private:
	char*          base() const;                      //字节流内部实现
	char*          end(void) const;
	char*          rd_ptr(void) const;
	char*          wr_ptr(void) const;
	void           rd_ptr(int n);
	void           wr_ptr(int n);
	int            space();
	int            copy(const char *buf, int n);
	
private:
	template<typename T> friend ByteArray& operator>>(ByteArray& msg, T& v)     //运算符重载
	{
		msg.Read(&v);
		return msg;
	}

	friend ByteArray& operator>>(ByteArray& msg, std::string& str)
	{
		str = msg.read_string();
		return msg;
	}

	template<typename T> friend ByteArray& operator<<(ByteArray& msg, const T& v)
	{
		msg.Write(v);
		return msg;
	}

	friend ByteArray& operator<<(ByteArray& msg, const std::string& str)
	{
		msg.write_string(str);
		return msg;
	}

	template<typename T>
	T& Read(T* pVar,bool readonly=false)
	{
		*pVar = *((T*)rd_ptr());
		if(!readonly)//添加只读，不移指针支持，为了调试，Mike
			rd_ptr(sizeof(T));
		return *pVar;
	}

	template<typename T>
	void Write(const T& value)
    {
		T* pV = (T*)wr_ptr();
    	*pV = value;
		wr_ptr(sizeof(T));
	}

public:
	char* m_pContent;                               //字节流内容指针
	int   m_nTotalSize;                             //字节流允许大小
	int   m_nRdptr;                                 //读取位置
	int   m_nWrPtr;                                 //写入位置
};