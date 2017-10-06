#include "DirFileManager.h"

#include "XLog.h"
#include "XFuns.h"
#include <string>

DirFileManager::DirFileManager(void)
{
}

DirFileManager::~DirFileManager(void)
{
}

bool DirFileManager::moveTo( const char *target, const char *file )
{
	std::string strFName, dstFile, srcFile = file;
	int p = srcFile.find_last_of('/');
	if(-1 != p)
		strFName = srcFile.substr(p + 1);
	else
		strFName = srcFile;

	dstFile.append(target);
	dstFile.append(strFName);

	if(false == XFuns::CheckDir(dstFile.c_str()))
		return false;

	if(XFuns::CheckFileExists(dstFile.c_str()))
	{
		if(0 != remove(dstFile.c_str()))
		{
			XLog::Write("Error remove [%s]", dstFile.c_str());
			return false;
		}
	}

	if(0 != rename(file, dstFile.c_str()))
	{
		XLog::Write("Error rename [%s] to [%s]", file, dstFile.c_str());
		return false;
	}

	return true;
}

bool DirFileManager::moveToBatch( const char *target, const char **files, int count )
{
	for(int i = 0; i < count; i ++)
	{
		if(false == moveTo(target, files[i]))
			return false;
	}

	return true;
}