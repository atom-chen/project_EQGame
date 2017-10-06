#pragma once

#include <string>

class ComputerInfo
{
private:
	static int GetCPUCount();
public:
	static void ComputerInfo::SaveToFile(const std::string & fileName);
	// ȡ����������Ϣ
	static std::string ComputerInfo::GetComputerInfo();
	// ȡ�汾��Ϣ
	static std::string ComputerInfo::GetWinVersionInfo();
	// ȡ�ڴ���Ϣ
	static std::string ComputerInfo::GetMemoryInfo();
	// ȡCPU��Ϣ
	static std::string ComputerInfo::GetCPUInfo();
	// ȡ�Կ���Ϣ
	static std::string ComputerInfo::GetDisplayCardInfo();
	// ȡDirectX��Ϣ
	static std::string ComputerInfo::GetDirectXVersion();
	// ȡ��������
	static std::string ComputerInfo::GetMyComputerName();
	// ȡIP
	static std::string ComputerInfo::GetMyIP();
};

std::string FormatStr(const char * str, ...);