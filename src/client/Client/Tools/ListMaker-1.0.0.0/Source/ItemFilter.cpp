#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ItemFilter.h"
#include <string.h>
#include "SFuns.h"

ItemFilter::ItemFilter(void)
{
	m_pFilter = NULL;

}

ItemFilter::~ItemFilter(void)
{
	
}

bool ItemFilter::StartLoadFilter()
{
	m_pFilter = fopen("ListMaker.filter", "r");
	if(NULL == m_pFilter)
		return false;

	return true;

}

bool ItemFilter::LoadFilterNext( int &type, std::string &filter )
{
	char buf[MAX_PATH + 1];

	if(NULL == m_pFilter)
		return false;

	buf[0] = 0;

	if(NULL == fgets(buf, MAX_PATH, m_pFilter))
		return false;

	std::string line = buf;

	SFuns::Trim(line);

	if(line.size() == 0)
		return false;

	//删除换行
	if(line[line.size() - 1] == '\n')
		line[line.size() - 1] = 0;

	//注释
	if(line[0] == '#')
	{
		type = -1;
		return true;
	}
	
	//某一类
	if(line[0] == '*')
	{
		type = 1;
		line.erase(0, 1);
	}//目录
	else if(line[0] == '?')
	{
		type = 2;
		line.erase(0, 1);
	}//特殊文件
	else
	{
		type = 0;
	}

	filter = line;

	return true;

}

void ItemFilter::EndLoadFilter()
{
	if(NULL == m_pFilter)
		return;

	fclose(m_pFilter);
	m_pFilter = NULL;

}

void ItemFilter::LoadFilterA()
{
	if(false == StartLoadFilter())
		return;

	m_itemFilterA.clear();

	int type;
	std::string filter;

	while(LoadFilterNext(type, filter))
	{
		if(-1 == type)
			continue;

		ITEMFILTERINFOA finfo = {0};
		finfo.Type = type;
		finfo.Filter = filter;
		m_itemFilterA.push_back(finfo);
	}

	EndLoadFilter();

}

void ItemFilter::LoadFilterW()
{
	if(false == StartLoadFilter())
		return;

	m_itemFilterW.clear();

	int type;
	std::string filter;

	while(LoadFilterNext(type, filter))
	{
		if(-1 == type)
			continue;

		ITEMFILTERINFOW finfo = {0};
		finfo.Type = type;
		finfo.Filter = wxString(SFuns::S2WS(filter));
		m_itemFilterW.push_back(finfo);
	}

	EndLoadFilter();

}

void ItemFilter::Dispose()
{
	m_itemFilterA.clear();
	m_itemFilterW.clear();
}

bool ItemFilter::FilterFileA( std::string &file )
{
	for(ITEMFILTERLISTA::iterator itr = m_itemFilterA.begin(); itr != m_itemFilterA.end(); ++itr)
	{
		std::string filter = itr->Filter;

		switch(itr->Type)
		{
		case 0:
			if(0 == stricmp(filter.c_str(), file.c_str()))
				return true;
			break;
		case 1:
			if(filter.length() > file.length())
				break;

			int p = file.find_last_of('.');
			std::string fext = "";

			if(-1 != p)
				fext = file.substr(p);

			if(0 == stricmp(filter.c_str(), fext.c_str()))
				return true;
			break;
		}
	}

	return false;

}

bool ItemFilter::FilterFileA(char *file)
{
	std::string ffile = file;

	return FilterFileA(ffile);

}

bool ItemFilter::FilterFileW( wxString &file )
{
	for(ITEMFILTERLISTW::iterator itr = m_itemFilterW.begin(); itr != m_itemFilterW.end(); ++itr)
	{
		wxString filter = itr->Filter;

		switch(itr->Type)
		{
		case 0:
			if(0 == filter.CmpNoCase(file))
				return true;
			break;
		case 1:
			if(filter.Length() > file.Length())
				break;

			wxString fext = file.Right(filter.Length());

			if(0 == filter.CmpNoCase(fext))
				return true;
			break;
		}
	}

	return false;

}

bool ItemFilter::FilterFileW( wxChar *file )
{
	wxString ffile = file;

	return FilterFileW(ffile);

}

bool ItemFilter::FilterDirA( std::string &dir )
{
	for(ITEMFILTERLISTA::iterator itr = m_itemFilterA.begin(); itr != m_itemFilterA.end(); ++itr)
	{
		std::string filter = itr->Filter;

		switch(itr->Type)
		{
		case 2:
			if(0 == stricmp(filter.c_str(), dir.c_str()))
				return true;
			break;
		}
	}

	return false;

}

bool ItemFilter::FilterDirA( char *dir )
{
	std::string fdir = dir;

	return FilterDirA(fdir);

}

bool ItemFilter::FilterDirW( wxString &dir )
{
	for(ITEMFILTERLISTW::iterator itr = m_itemFilterW.begin(); itr != m_itemFilterW.end(); ++itr)
	{
		wxString filter = itr->Filter;

		switch(itr->Type)
		{
		case 2:
			if(0 == filter.CmpNoCase(dir))
				return true;
			break;
		}
	}

	return false;

}

bool ItemFilter::FilterDirW( wxChar *dir )
{
	wxString fdir = dir;

	return FilterDirW(fdir);

}