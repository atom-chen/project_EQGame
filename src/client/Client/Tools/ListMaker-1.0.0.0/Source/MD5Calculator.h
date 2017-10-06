/******************************************************************************
* 作者: xj
* 时间: 2010-11-12
* 描述: 
******************************************************************************/

#pragma once

#ifndef __MD5Calculator_H
#define __MD5Calculator_H

#include <windows.h>
#include <wincrypt.h>

#define MD5_CODE_LEN 16

class MD5Calculator
{
protected:
	HCRYPTPROV m_hCryptProv;
	HCRYPTHASH m_hMd5;
	BYTE       m_bMd5Code[MD5_CODE_LEN];

public:
	virtual bool StartUp();
	virtual void CleanUp();
	virtual bool Begin();
	virtual void End();
	virtual bool Init();
	virtual void Dispose();
	virtual void Release();
	virtual bool Calculate(const BYTE *data, DWORD len);
	virtual bool Calculate(const char *file);
	virtual const BYTE *GetResult();
	virtual bool GetResult(char *buf, DWORD size);

protected:
	bool AddData(const BYTE *data, DWORD len);
	bool DoCalculate();

public:
	MD5Calculator(void);
	~MD5Calculator(void);
};

#endif