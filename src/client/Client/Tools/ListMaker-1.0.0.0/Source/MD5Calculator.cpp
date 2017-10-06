#include "MD5Calculator.h"

#include <stdio.h>

#define READ_FILE_BUFFER_SIZE (1024*1024)

#ifndef SAFE_FREE
#define SAFE_FREE(p) {if(p){free(p);(p)=NULL;}}
#endif

#ifndef SAFE_FCLOSE
#define SAFE_FCLOSE(p) {if(p){fclose(p);(p)=NULL;}}
#endif

MD5Calculator::MD5Calculator(void)
{
	m_hCryptProv = 0;
	m_hMd5 = 0;
}

MD5Calculator::~MD5Calculator(void)
{
}

bool MD5Calculator::StartUp()
{
	if(FALSE == CryptAcquireContext(&m_hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		if(NTE_EXISTS != GetLastError())
			return false;

		if(FALSE == CryptAcquireContext(&m_hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
			return false;
	}

	return true;
}

void MD5Calculator::CleanUp()
{
	if(m_hCryptProv)
	{
		CryptReleaseContext(m_hCryptProv, 0);
		m_hCryptProv = 0;
	}
}

bool MD5Calculator::Begin()
{
	if(m_hMd5)
		return false;

	ZeroMemory(m_bMd5Code, sizeof(m_bMd5Code));

	return CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &m_hMd5) ? true : false;
}

void MD5Calculator::End()
{
	if(m_hMd5)
	{
		CryptDestroyHash(m_hMd5);
		m_hMd5 = 0;
	}
}

bool MD5Calculator::Init()
{
	return true;
}

void MD5Calculator::Dispose()
{
	End();

	CleanUp();

}

void MD5Calculator::Release()
{
	Dispose();

	delete this;

}

bool MD5Calculator::Calculate( const BYTE *data, DWORD len )
{
	if(false == AddData(data, len))
		return false;

	return DoCalculate();
}

bool MD5Calculator::Calculate( const char *file )
{
	bool rtn = false;
	BYTE *readBuf = (BYTE *)malloc(READ_FILE_BUFFER_SIZE);
	FILE *pf = fopen(file, "rb");

	if(NULL == pf || NULL == readBuf)
		goto CALCULATE_RTN;

	while(!feof(pf))
	{
		DWORD readSize = fread(readBuf, sizeof(BYTE), READ_FILE_BUFFER_SIZE, pf);
		if(false == AddData(readBuf, readSize))
			goto CALCULATE_RTN;
	}

	if(false == DoCalculate())
		goto CALCULATE_RTN;

	rtn = true;

CALCULATE_RTN:
	SAFE_FREE(readBuf);
	SAFE_FCLOSE(pf);

	return rtn;
}

const BYTE * MD5Calculator::GetResult()
{
	return m_bMd5Code;

}

bool MD5Calculator::GetResult( char *buf, DWORD size )
{
	if(size <= (MD5_CODE_LEN*2))
		return false;

	BYTE n = 0;
	char ab[3] = {0};

	for(BYTE i = 0; i < MD5_CODE_LEN; i ++)
	{
		sprintf(ab, "%02X", m_bMd5Code[i]);

		buf[n ++] = ab[0];
		buf[n ++] = ab[1];
	}

	buf[n] = 0;

	return true;
}

bool MD5Calculator::AddData( const BYTE *data, DWORD len )
{
	return CryptHashData(m_hMd5, data, len, 0) ? true : false;
}

bool MD5Calculator::DoCalculate()
{
	DWORD dwCount = MD5_CODE_LEN;

	return CryptGetHashParam(m_hMd5, HP_HASHVAL, m_bMd5Code, &dwCount, 0) ? true : false;
}
