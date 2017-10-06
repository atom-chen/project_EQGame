// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#include "EQCommPCH.h"

//#include <strsafe.h>
#include "FlashDlg.h"
#include "flash10l.tlh"

CComModule _Module;

LRESULT FlashDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	this->GetDlgItem(IDOK).BringWindowToTop();
    // Initialize Flash Player (Shockwave .swf)
	return TRUE;
}

LRESULT FlashDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add validation code 
	DestroyWindow();
    return 0;
}


void FlashDlg::PlayFlash(std::string flashUrlAddress)
{
	this->GetDlgItem(IDOK).BringWindowToTop();

	CComPtr<IShockwaveFlash> flashPtr;
	CAxWindow wndFlashPlayer = this->GetDlgItem(IDC_SHOCKWAVEFLASH);
	HRESULT hr = wndFlashPlayer.QueryControl(&flashPtr);

	if(SUCCEEDED(hr)) 
	{
		hr = flashPtr->put_Movie(CComBSTR(flashUrlAddress.c_str()));
		if(SUCCEEDED(hr)) 
			flashPtr->Play();
	}
}