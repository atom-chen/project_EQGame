// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include "../../Client/EQClient/EQClient/resource.h"

class FlashDlg : public CAxDialogImpl<FlashDlg>
{
public:
    enum { IDD = IDD_DISPLAYFLASH_DIALOG };

    BEGIN_MSG_MAP(FlashDlg)
        //CHAIN_MSG_MAP(CAxWindowlessHost<CMainDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        //MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)	
        COMMAND_ID_HANDLER(IDOK, OnOK)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void PlayFlash(std::string flashUrlAddress);
};
