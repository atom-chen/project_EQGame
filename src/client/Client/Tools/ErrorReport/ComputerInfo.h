#pragma once

#include <string>

class ComputerInfo
{
private:
	static int GetCPUCount();
public:
	static void ComputerInfo::SaveToFile(const std::string & fileName);
	// 取电脑配置信息
	static std::string ComputerInfo::GetComputerInfo();
	// 取版本信息
	static std::string ComputerInfo::GetWinVersionInfo();
	// 取内存信息
	static std::string ComputerInfo::GetMemoryInfo();
	// 取CPU信息
	static std::string ComputerInfo::GetCPUInfo();
	// 取显卡信息
	static std::string ComputerInfo::GetDisplayCardInfo();
	// 取DirectX信息
	static std::string ComputerInfo::GetDirectXVersion();
	// 取电脑名称
	static std::string ComputerInfo::GetMyComputerName();
	// 取IP
	static std::string ComputerInfo::GetMyIP();
};

std::string FormatStr(const char * str, ...);