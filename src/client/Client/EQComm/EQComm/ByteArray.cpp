/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.cpp
	created:	D.W 2009-9-23 19:00
	purpose:	字节流类 （网络协议序列化）
==============================================================================*/
#include "EQCommPCH.h"
#include "ByteArray.h"

#include "EQLog.h"

const int MAXMSGSIZE    = 1024*1024;                    // 单个网络消息最大长度，（超过极易导致物理服务器收发队列阻塞）

char writer[MAXMSGSIZE] = "\0";

ByteArray::ByteArray()
{
	m_pContent = new char[MAXMSGSIZE];
	m_nTotalSize = int(MAXMSGSIZE);
	m_nRdptr = 0;
	m_nWrPtr = 0;
}

ByteArray::~ByteArray()
{
    delete[] m_pContent;
}

void ByteArray::print()
{
	unsigned short *p = (unsigned short*)(rd_ptr() + 2);
	printf("ByteArray:%d\n", p[0]);
}

char* ByteArray::base() const
{
	return m_pContent;
}

char* ByteArray::end(void) const
{
	return m_pContent + m_nTotalSize;
}

char* ByteArray::rd_ptr(void) const
{
	return m_pContent + m_nRdptr;
}

void ByteArray::rd_ptr(int n)
{
	m_nRdptr +=n;
}

char* ByteArray::wr_ptr(void) const
{
	return base() + m_nWrPtr;
}

void ByteArray::wr_ptr(int n)
{

	m_nWrPtr += n;
}

int ByteArray::space()
{
	return m_nTotalSize - m_nWrPtr;
}

int ByteArray::length() const
{
	return m_nWrPtr - m_nRdptr;
}

int   ByteArray::copy(const char *buf, int n)
{
	int len = this->space();

	if(len < n)
		return -1;

	memcpy(this->wr_ptr(), buf, n);
	this->wr_ptr(n);
	return 0;

}

INT16 ByteArray::read_int16()
{
	INT16 w = 0;
	Read(&w);

	//sprintf(writer,"[%hd]",w);
	//LOGDBG(writer);
	
	return w;
}

UINT16 ByteArray::read_uint16(bool log)
{
	UINT16 w = 0;
	Read(&w);

	if (log==true)
	{
		//sprintf(writer,"[%hu]",w);
		//LOGDBG(writer);
	}

	return w;
}

//UINT16 ByteArray::readonly_uint16()
//{
//	UINT16 w = 0;
//	return Read(&w,true);
//}

INT32 ByteArray::read_int()
{
	INT32 w = 0;
	Read(&w);

	//sprintf(writer,"[%d]",w);
	//LOGDBG(writer);

	return w;
}

UINT32 ByteArray::read_uint()
{
	UINT32 w = 0;
	Read(&w);

	//sprintf(writer,"[%u]",w);
	//LOGDBG(writer);

	return w;
}

INT64 ByteArray::read_int64()
{
	INT64 w = 0;
	Read(&w);

	//sprintf(writer,"[%lld]",w);
	//LOGDBG(writer);

	return w;
}

UINT64 ByteArray::read_uint64()
{
	UINT64 w = 0;
	Read(&w);

	//sprintf(writer,"[%llu]",w);
	//LOGDBG(writer);

	return w;
}

SHORT ByteArray::read_short()
{
	SHORT w = 0;
	Read(&w);

	//sprintf(writer,"[%hd]",w);
	//LOGDBG(writer);

	return w;
}

USHORT ByteArray::read_ushort()
{
	USHORT w = 0;
	Read(&w);

	//sprintf(writer,"[%hu]",w);
	//LOGDBG(writer);

	return w;
}

LONG ByteArray::read_long()
{
	LONG w = 0;
	Read(&w);

	//sprintf(writer,"[%ld]",w);
	//LOGDBG(writer);

	return w;
}

ULONG ByteArray::read_ULONG()
{
	ULONG w = 0;
	Read(&w);

	//sprintf(writer,"[%lu]",w);
	//LOGDBG(writer);

	return w;
}

CHAR ByteArray::read_char()
{
	CHAR w = 0;
	Read(&w);

	//sprintf(writer,"[%c]",w);
	//LOGDBG(writer);

	return w;
}

UCHAR ByteArray::read_uchar()
{
	UCHAR w = 0;
	Read(&w);

	//sprintf(writer,"[%c]",w);
	//LOGDBG(writer);

	return w;
}

BOOL ByteArray::read_bool()
{
	BOOL w = 0;
	Read(&w);

	//sprintf(writer,"[%c]",w);
	//LOGDBG(writer);

	return w;
}

FLOAT ByteArray::read_float()
{
	FLOAT w = 0;
	Read(&w);

	//sprintf(writer,"[%f]",w);
	//LOGDBG(writer);

	return w;
}

DOUBLE ByteArray::read_double()
{
	DOUBLE w = 0;
	Read(&w);

	//sprintf(writer,"[%lf]",w);
	//LOGDBG(writer);

	return w;
}

BYTE* ByteArray::read_string(BYTE* info, UINT len)
{
	BYTE* result = (BYTE*)memcpy(info, rd_ptr(), len);
	rd_ptr(len);

	//sprintf(writer,"[%s]",result);
	//LOGDBG(writer);

	return result;
}

std::string ByteArray::read_string()
{
	UINT16 len = read_uint16(false);
	std::string strValue(rd_ptr(), len);
	rd_ptr(len);

	//sprintf(writer,"[%s]",strValue.c_str());
	//LOGDBG(writer);

	return strValue;
}

 
void ByteArray::write_int(INT32 value)
{
	Write(value);

	//sprintf(writer,"[%d]",value);
	//LOGDBG(writer);
}

void ByteArray::write_uint(UINT32 value)
{
	Write(value);

	//sprintf(writer,"[%u]",value);
	//LOGDBG(writer);
}

void ByteArray::write_int16(INT16 value)
{
	Write(value);

	//sprintf(writer,"[%hd]",value);
	//LOGDBG(writer);
}

void ByteArray::write_uint16(UINT16 value,bool log)
{
	Write(value);

	if(log == true)
	{
		//sprintf(writer,"[%hu]",value);
		//LOGDBG(writer);
	}
}

void ByteArray::write_int64(INT64 value)
{
	Write(value);

	//sprintf(writer,"[%lld]",value);
	//LOGDBG(writer);
}

void ByteArray::write_uint64(UINT64 value)
{
	Write(value);

	//sprintf(writer,"[%llu]",value);
	//LOGDBG(writer);
}

void ByteArray::write_short(SHORT value)
{
	Write(value);

	//sprintf(writer,"[%hd]",value);
	//LOGDBG(writer);
}

void ByteArray::write_ushort(USHORT value)
{
	Write(value);

	//sprintf(writer,"[%hu]",value);
	//LOGDBG(writer);
}

void ByteArray::write_long(LONG value)
{
	Write(value);

	//sprintf(writer,"[%ld]",value);
	//LOGDBG(writer);
}

void ByteArray::write_ULONG(ULONG value)
{
	Write(value);

	//sprintf(writer,"[%lu]",value);
	//LOGDBG(writer);
}

void ByteArray::write_char(CHAR value)
{
	Write(value);

	//sprintf(writer,"[%c]",value);
	//LOGDBG(writer);
}

void ByteArray::write_uchar(UCHAR value)
{
	Write(value);

	//sprintf(writer,"[%c]",value);
	//LOGDBG(writer);
}

void ByteArray::write_bool(BOOL value)
{
	Write(value);

	//sprintf(writer,"[%c]",value);
	//LOGDBG(writer);
}

void ByteArray::write_float(FLOAT value)
{
	Write(value);

	//sprintf(writer,"[%f]",value);
	//LOGDBG(writer);
}

void ByteArray::write_double(DOUBLE value)
{
	Write(value);

	//sprintf(writer,"[%lf]",value);
	//LOGDBG(writer);
}

void  ByteArray::write_string(const char* str)
{
	write_string(std::string(str));

	//sprintf(writer,"[%s]",str);
	//LOGDBG(writer);
}

void  ByteArray::write_string(const std::string& str)
{
	const int STRING_HDR_LEN = 2;
	write_uint16(static_cast<UINT16>(str.length()),false);
	copy(str.data(), str.length());

	//sprintf(writer,"[%s]",str.c_str());
	//LOGDBG(writer);
}

void ByteArray::reuse()
{
	m_nRdptr = 0; m_nWrPtr = 0; m_nTotalSize = int(MAXMSGSIZE);
}