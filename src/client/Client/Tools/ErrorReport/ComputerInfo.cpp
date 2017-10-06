#include "StdAfx.h"
#include "ComputerInfo.h"
#include <fstream>

class Reg
{
public:
	~Reg()
	{
		if (mKey)
			RegCloseKey(mKey);
	}
public:
	explicit Reg(const std::string & sPath)
		: mKey(NULL)
	{
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,
									sPath.c_str(),
									0, 
									KEY_QUERY_VALUE, 
									&mKey);
	}
private:
	Reg(const Reg &);
	void operator = (const Reg &);
public:
	std::string QueryValue(const std::string & sKey)
	{
		if (mKey)
		{
			char buf[1024] = {0};
			DWORD bufSize = 1024;
			::LSTATUS lRet = RegQueryValueEx(mKey, sKey.c_str(), NULL, NULL,
									reinterpret_cast<LPBYTE>(buf), &bufSize);
			
			if (lRet == ERROR_SUCCESS && bufSize > 0)
			{
				return std::string(buf);
			}
		}

		return std::string();
	}
private:
	::HKEY mKey;
};

class WSAStarter
{
public:
	~WSAStarter()
	{
		if (mStarted)
			WSACleanup();
	}
public:
	WSAStarter()
		: mStarted(false)
	{
		WSADATA data;
		::ZeroMemory(&data, sizeof(data));
		
		mStarted = (WSAStartup(MAKEWORD(1, 1), &data) == 0);
	}
public:
	bool IsStarted() const
	{
		return mStarted;
	}
private:
	bool mStarted;
};

// 取本机注册表的某个值
static std::string GetRegValue(const std::string & sPath, const std::string & sKey)
{
	Reg r(sPath);
	return r.QueryValue(sKey);	
}

// 格式化字串
std::string FormatStr(const char * str, ...)
{
    char buf[1024];
    va_list args;

    va_start(args, str);
	vsprintf_s(buf, str, args);
    va_end(args);

	return buf;
}

std::string ComputerInfo::GetComputerInfo()
{
	std::string result;

	// 电脑名称
	result += "  电脑名称: " + ComputerInfo::GetMyComputerName();
	result += "\r\n";

	// IP
	result += "  IP: " + ComputerInfo::GetMyIP();
	result += "\r\n";

	// windows 版本		
	result += ComputerInfo::GetWinVersionInfo();
	result += "\r\n";

	// 内存
	result += ComputerInfo::GetMemoryInfo();
	result += "\r\n";

	// cpu
	result += ComputerInfo::GetCPUInfo();
	result += "\r\n";

	// 显卡
	result += ComputerInfo::GetDisplayCardInfo();
	result += "\r\n";

	// DirectX
	result += ComputerInfo::GetDirectXVersion();
	result += "\r\n";

	return result;
}

void ComputerInfo::SaveToFile(const std::string & fileName)
{
	std::ofstream f(fileName.c_str());
	std::string sInfo = GetComputerInfo();

	f.write(sInfo.c_str(), sInfo.size());
}


// 取操作系统版本信息
std::string ComputerInfo::GetWinVersionInfo()
{
	std::string sWinType;

	::OSVERSIONINFO verInfo;
	::ZeroMemory(&verInfo, sizeof(verInfo));
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	::GetVersionEx(&verInfo);

	if (verInfo.dwMajorVersion >= 5)
	{
		if (verInfo.dwMinorVersion == 1)
		{
			sWinType = "WindowsXP";
		}
		else if (verInfo.dwMinorVersion == 2)
		{
			sWinType = "Windows Server 2003 family";
		}
		else
		{
			sWinType = "Windows2000";
		}
	}
	else //	WIndows 95, 98, Me, or NT 40
	{
		if (verInfo.dwMajorVersion > 3)
		{			
			if (verInfo.dwPlatformId = VER_PLATFORM_WIN32_NT)
			{
				sWinType = "WindowsNT40";
			}
			else
			{				
				DWORD Win32BuildNumber = (verInfo.dwBuildNumber & 0xFFFF);
				if (verInfo.dwMinorVersion >= 90)
				{
					sWinType = "WindowsMe";
				}
				else
				{
					if (verInfo.dwMinorVersion >= 10)
					{
						if (Win32BuildNumber >= 2222)
						{
							sWinType = "Windows98SE";
						}
						else
						{
							sWinType = "Windows98";
						}
					}
					else
					{ 
						if (Win32BuildNumber >= 1000)
						{
							sWinType = "Windows95OSR2";
						}
						else
						{
							sWinType = "Windows95";
						}
					}
				}
			}
		}
		else
		{
			sWinType = "Unknown";
		}
	}

	sWinType = "  版本：" + sWinType + " " + verInfo.szCSDVersion;
	return sWinType;
}

// 内存信息
std::string ComputerInfo::GetMemoryInfo()
{
	std::string result;
	::MEMORYSTATUS memStatus;
	::ZeroMemory(&memStatus, sizeof(memStatus));
	memStatus.dwLength = sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&memStatus); 
	const int iUnit = 1024 * 1024;
	result += FormatStr("  物理总内存：%dM 可用：%dM",  
						memStatus.dwTotalPhys / iUnit,  
						memStatus.dwAvailPhys / iUnit);
	result += "\r\n";
	result += FormatStr("  虚拟总内存：%dM 可用：%dM", 
						memStatus.dwTotalVirtual / iUnit, 
						memStatus.dwAvailVirtual / iUnit);
	return result;
}

int ComputerInfo::GetCPUCount()
{
	SYSTEM_INFO sysInfo;
	::ZeroMemory(&sysInfo, sizeof(sysInfo));
	//获取CPU信息
	GetSystemInfo(&sysInfo);
	//处理器的个数
	return sysInfo.dwNumberOfProcessors;
}

// 取IP
std::string ComputerInfo::GetMyIP()
{
	std::string sIp;

	WSAStarter wsa;
	if (!wsa.IsStarted())
		return "unknow ip";

	const int bufSize = 1024;
	char buf[bufSize] = {0};
	DWORD n = bufSize;
	::gethostname(buf, bufSize);
	hostent * ph = ::gethostbyname(buf);
	in_addr * addr = reinterpret_cast<in_addr *>(ph->h_addr); 
    sIp = inet_ntoa(addr[0]);
	
	return sIp;
}

// 取电脑名称
std::string ComputerInfo::GetMyComputerName()
{
	const int bufSize = 1024;
	char buf[bufSize] = {0};
	DWORD n = bufSize;
	::GetComputerName(buf, &n);

	return std::string(buf, buf + n);
}

// 取cpu信息
std::string ComputerInfo::GetCPUInfo()
{
	std::string result;

	int n = GetCPUCount();
	for (int i = 0; i < n; ++i)
	{
		std::string sPath = FormatStr("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", i);
		std::string s = GetRegValue(sPath, "ProcessorNameString");
		result += FormatStr("  cpu %d: %s", i, s.c_str());
		result += "\r\n";
	}
	
	return result;
}


// 取显卡信息
std::string ComputerInfo::GetDisplayCardInfo()
{
	std::string result;

	::DISPLAY_DEVICE displayDevice;
	::ZeroMemory(&displayDevice, sizeof(displayDevice));

	displayDevice.cb = sizeof(displayDevice);
	DWORD dwFlags = 0;
	
	for(DWORD i = 0; EnumDisplayDevices(NULL, i, &displayDevice , dwFlags); ++i)
	{
		std::string sDev = displayDevice.DeviceName;
		if (sDev == "\\\\.\\Display1" || 
			sDev == "\\\\.\\DISPLAY1")
		{
			result += FormatStr("  显卡%d：%s", i, displayDevice.DeviceString);
		}
	}
	return result;
}


std::string ComputerInfo::GetDirectXVersion() 
{ 
	std::string result = "  DirectX ";
	const std::string sVer = GetRegValue("SOFTWARE\\Microsoft\\DirectX", "Version");
	// 取版本号
	// 格式: 4.09.00.0904, 第一个.后面为版本号09
	size_t iStart = sVer.find('.', 0);
	if (0 <= iStart && iStart < sVer.size())
	{
		++iStart;
		size_t iEnd = sVer.find('.', iStart);
		if (iStart < iEnd && iEnd < sVer.size())
		{
			std::string sNum(sVer.begin() + iStart, sVer.begin() + iEnd);
			for (; sNum.size() > 0 && (*sNum.begin() == '0'); )
			{
				sNum.erase(sNum.begin());
			}

			result += sNum;
		}		
	}

	result += FormatStr("(%s)", sVer.c_str());
    return result;
} 

