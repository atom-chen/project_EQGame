
#ifndef __ItemFilter_H
#define __ItemFilter_H

#include <string>
#include <list>

typedef struct tagITEMFILTERINFOA
{
	//0:abc.abc, 1:*.abc, 2:dir
	int Type;
	std::string Filter;
} ITEMFILTERINFOA, *LPITEMFILTERINFOA;

typedef struct tagITEMFILTERINFOW
{
	//0:abc.abc, 1:*.abc, 2:dir
	int Type;
	wxString Filter;
} ITEMFILTERINFOW, *LPITEMFILTERINFOW;

typedef std::list<ITEMFILTERINFOA> ITEMFILTERLISTA;
typedef std::list<ITEMFILTERINFOW> ITEMFILTERLISTW;

class ItemFilter
{
protected:
	ITEMFILTERLISTA m_itemFilterA;
	ITEMFILTERLISTW m_itemFilterW;

	FILE *m_pFilter;

protected:
	bool StartLoadFilter();
	//-1:makert
	bool LoadFilterNext(int &type, std::string &filter);
	void EndLoadFilter();

public:
	void LoadFilterA();
	void LoadFilterW();
	void Dispose();
	bool FilterFileA(std::string &file);
	bool FilterFileA(char *file);
	bool FilterFileW(wxString &file);
	bool FilterFileW(wxChar *file);
	bool FilterDirA(std::string &dir);
	bool FilterDirA(char *dir);
	bool FilterDirW(wxString &dir);
	bool FilterDirW(wxChar *dir);

public:
	ItemFilter(void);
	~ItemFilter(void);
};

#endif