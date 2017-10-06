#pragma once

#ifndef __XMD5Calc_H
#define __XMD5Calc_H

#include <windows.h>
#include <wincrypt.h>

#define MD5_CODE_LEN 16

class XMD5Calc
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
	XMD5Calc(void);
	~XMD5Calc(void);
};

#endif