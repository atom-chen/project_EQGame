/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	�ֽ����� ������Э�����л���
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
	INT16          read_int16();                      // ���ֽ�����ȡ16λ����
	UINT16         read_uint16(bool log=true);                     // ���ֽ�����ȡ16λ�޷�������
	//UINT16         readonly_uint16(); //ֻ��������ָ��λ��
	
	INT32          read_int();                        // ���ֽ�����ȡ32λ����
	UINT32         read_uint();                       // ���ֽ�����ȡ32λ����

	INT64          read_int64();                      // ���ֽ�����ȡ64λ����
	UINT64         read_uint64();                     // ���ֽ�����ȡ64λ�޷�������
	
	SHORT          read_short();                      // ���ֽ�����ȡ������
	USHORT         read_ushort();                     // ���ֽ�����ȡ�޷��Ŷ�����

	LONG           read_long();                       // ���ֽ�����ȡ������
	ULONG          read_ULONG();                      // ���ֽ�����ȡ�޷��ų�����

	CHAR           read_char();                       // ���ֽ�����ȡ�ַ�
	UCHAR          read_uchar();                      // ���ֽ�����ȡ�޷����ַ�����

	BOOL           read_bool();                       // ���ֽ�����ȡBoolֵ

	FLOAT          read_float();                      // ���ֽ�����ȡ������
	DOUBLE         read_double();                     // ���ֽ�����ȡ˫������

	BYTE*          read_string(BYTE* info, UINT len); // ���ֽ�����ȡ�ַ���
	std::string    read_string();                     // ���ֽ�����ȡ�ַ���

	void 		   write_int(INT32);                  // ���ֽ���д32λ����
	void 		   write_uint(UINT32);                // ���ֽ���д32λ�޷�������

	void 		   write_int16(INT16);                // ���ֽ���д16λ����
	void 		   write_uint16(UINT16,bool log=true);              // ���ֽ���д16λ�޷�������

	void 		   write_int64(INT64);                // ���ֽ���д64λ����
	void 		   write_uint64(UINT64);                // ���ֽ���д64λ�޷�������
	
	void 		   write_short(SHORT);                // ���ֽ���д����
	void 		   write_ushort(USHORT);              // ���ֽ���д�޷��Ŷ���

	void 		   write_long(LONG);                  // ���ֽ���д����
	void 		   write_ULONG(ULONG);                // ���ֽ���д�޷��ų���

	void 		   write_char(CHAR);                  // ���ֽ���д�ַ���
	void 		   write_uchar(UCHAR);                // ���ֽ���д�޷����ַ�������

	void 		   write_bool(BOOL);                  // ���ֽ���д������

	void 		   write_float(FLOAT);                // ���ֽ���д������
	void 		   write_double(DOUBLE);              // ���ֽ���д˫������

	void 		   write_string(const char* str);     // ���ֽ���д�ַ���
	void 		   write_string(const std::string& str); // ���ֽ���д�ַ���

public:
	int 		   length() const;                    //�ֽ�����ǰ����
    
private:
	char*          base() const;                      //�ֽ����ڲ�ʵ��
	char*          end(void) const;
	char*          rd_ptr(void) const;
	char*          wr_ptr(void) const;
	void           rd_ptr(int n);
	void           wr_ptr(int n);
	int            space();
	int            copy(const char *buf, int n);
	
private:
	template<typename T> friend ByteArray& operator>>(ByteArray& msg, T& v)     //���������
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
		if(!readonly)//���ֻ��������ָ��֧�֣�Ϊ�˵��ԣ�Mike
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
	char* m_pContent;                               //�ֽ�������ָ��
	int   m_nTotalSize;                             //�ֽ��������С
	int   m_nRdptr;                                 //��ȡλ��
	int   m_nWrPtr;                                 //д��λ��
};