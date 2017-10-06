/////////////////////////////////////////////////////////////////////////////
// Name:        listmakerapp.cpp
// Purpose:     
// Author:      xj
// Modified by: 
// Created:     16/11/2010 14:19:22
// RCS-ID:      
// Copyright:   XJ-1
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#define USE_STD_STRING

#include "listmakerapp.h"
#include "DBWindow.h"
#include <string>
#include "ItemFilter.h"
#include "MD5Calculator.h"
#include "SFuns.h"

////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( ListMakerApp )
////@end implement app


/*
 * ListMakerApp type definition
 */

IMPLEMENT_CLASS( ListMakerApp, wxApp )


/*
 * ListMakerApp event table definition
 */

BEGIN_EVENT_TABLE( ListMakerApp, wxApp )

////@begin ListMakerApp event table entries
////@end ListMakerApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for ListMakerApp
 */

ListMakerApp::ListMakerApp()
{
    Init();
}


/*
 * Member initialisation
 */

void ListMakerApp::Init()
{
////@begin ListMakerApp member initialisation
////@end ListMakerApp member initialisation
}

/*
 * Initialisation for ListMakerApp
 */

ItemFilter g_itemFilter;
void AddDir(std::string &parent, std::string &ppath)
{
	_WIN32_FIND_DATAA fData;
	HANDLE hSearch;
	BOOL bFinished = FALSE;

	std::string temp;
	temp.append(parent);
	temp.append("*.*");

	hSearch = FindFirstFileA(temp.c_str(), &fData);

	if(INVALID_HANDLE_VALUE == hSearch)
		bFinished = TRUE;

	MD5Calculator md5Clc;
	md5Clc.StartUp();
	char md5Code[33];

	while(FALSE == bFinished)
	{
		if(0 != strcmp(fData.cFileName, ".") && 0 != strcmp(fData.cFileName, ".."))
		{
			if(fData.dwFileAttributes &  FILE_ATTRIBUTE_DIRECTORY)
			{
				if(false == g_itemFilter.FilterDirA(fData.cFileName))
				{
					temp = parent;
					temp.append("/");
					temp.append(fData.cFileName);
					temp.append("/");

					std::string pp = ppath;
					pp.append(fData.cFileName);
					pp.append("/");

					AddDir(temp, pp);
				}
			}
			else
			{
				if(false == g_itemFilter.FilterFileA(fData.cFileName))//¹ýÂË
				{
					temp = parent;
					temp.append(fData.cFileName);

					md5Code[0] = 0;

					md5Clc.Begin();
					md5Clc.Calculate(temp.c_str());
					md5Clc.GetResult(md5Code, 33);
					md5Clc.End();

					if("" == ppath)
						temp = fData.cFileName;
					else
					{
						temp = ppath;
						temp.append(fData.cFileName);
					}

					fprintf(stderr, "  Add Item [%s].\n", temp.c_str());

					fprintf(stdout, "        <Item File=\"%s\" Code=\"%s\" />\n", temp.c_str(), md5Code);
				}
			}
		}

		if(FALSE == FindNextFileA(hSearch, &fData))
			bFinished = TRUE;
	}

	FindClose(hSearch);

	md5Clc.Dispose();

}

void AddRootDir(std::string &root)
{
	_WIN32_FIND_DATAA fData;
	HANDLE hSearch;
	BOOL bFinished = FALSE;

	std::string temp, ppath = "";
	temp.append(root);
	temp.append("/*.*");

	hSearch = FindFirstFileA(temp.c_str(), &fData);

	if(INVALID_HANDLE_VALUE == hSearch)
		bFinished = TRUE;

	g_itemFilter.LoadFilterA();

	while(FALSE == bFinished)
	{
		if(0 != strcmp(fData.cFileName, ".") && 0 != strcmp(fData.cFileName, "..") && 0 != strcmp(fData.cFileName, ".svn"))
		{
			if(fData.dwFileAttributes &  FILE_ATTRIBUTE_DIRECTORY)
			{
				if(false == g_itemFilter.FilterDirA(fData.cFileName))
				{
					fprintf(stderr, "Add DIR Group [%s].\n", fData.cFileName);

					fprintf(stdout, "    <Group Directory=\"%s/\" Type=\"GROUP_TYPE_DIR\">\n", fData.cFileName);
					temp = root;
					temp.append("/");
					temp.append(fData.cFileName);
					temp.append("/");
					AddDir(temp, ppath);
					fprintf(stdout, "    </Group>\n");
				}
			}
		}

		if(FALSE == FindNextFileA(hSearch, &fData))
			bFinished = TRUE;
	}

	FindClose(hSearch);

	g_itemFilter.Dispose();

}

bool ListMakerApp::OnInit()
{    
////@begin ListMakerApp initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif

	if(wxApp::argc == 3)
	{
		wxString param = wxApp::argv[1];

		param.LowerCase();

		if(param == wxT("-d"))
		{
			fprintf(stderr, "Start DIR list file maker.\n");

			std::string rootDir = SFuns::WChar2Ansi(argv[2]);
			fprintf(stdout, "<?xml version=\"1.0\" ?>\n");
			fprintf(stdout, "<List Directory=\"\">\n");
			AddRootDir(rootDir);
			fprintf(stdout, "</List>");

			fprintf(stderr, "End DIR list file maker.\n");

			return false;
		}
	}

	ListMakerFrame* mainWindow = new ListMakerFrame( NULL );
	mainWindow->Show(true);

	wxString helpFile = wxT("Help.mht");
	if(wxApp::argc)
	{
		wxString curDir = wxApp::argv[0];
		curDir.LowerCase();
		int p = curDir.find_last_of(wxT("\\"));
		if(-1 != p)
			curDir.Remove(p + 1);
		helpFile.insert(0, curDir);
	}
	mainWindow->SetHelpFile(helpFile);

	wxString initFile = wxT("");
	if(wxApp::argc > 1)
		initFile.Append(wxApp::argv[1]);
	initFile.LowerCase();
	if(-1 != initFile.Find(wxT(".lmp")))
		mainWindow->OpenAFile(initFile);

////@end ListMakerApp initialisation

    return true;
}


/*
 * Cleanup for ListMakerApp
 */

int ListMakerApp::OnExit()
{    
////@begin ListMakerApp cleanup
	return wxApp::OnExit();
////@end ListMakerApp cleanup
}

