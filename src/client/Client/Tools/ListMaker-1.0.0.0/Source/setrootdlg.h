/////////////////////////////////////////////////////////////////////////////
// Name:        setrootdlg.h
// Purpose:     
// Author:      xj
// Modified by: 
// Created:     19/11/2010 10:36:01
// RCS-ID:      
// Copyright:   XJ-1
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 19/11/2010 10:36:01

#ifndef _SETROOTDLG_H_
#define _SETROOTDLG_H_


/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_SETROOTDLG 10030
#define ID_TXT_DIR 10035
#define ID_BTN_DIR 10036
#define ID_BTN_OK 10037
#define ID_BTN_CANCEL 10038
#define SYMBOL_SETROOTDLG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_SETROOTDLG_TITLE wxGetTranslation(wxString() + (wxChar) 0x8BBE + (wxChar) 0x7F6E + (wxChar) 0x6839)
#define SYMBOL_SETROOTDLG_IDNAME ID_SETROOTDLG
#define SYMBOL_SETROOTDLG_SIZE wxSize(400, 300)
#define SYMBOL_SETROOTDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * SetRootDlg class declaration
 */

class SetRootDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( SetRootDlg )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    SetRootDlg();
    SetRootDlg( wxWindow* parent, wxWindowID id = SYMBOL_SETROOTDLG_IDNAME, const wxString& caption = SYMBOL_SETROOTDLG_TITLE, const wxPoint& pos = SYMBOL_SETROOTDLG_POSITION, const wxSize& size = SYMBOL_SETROOTDLG_SIZE, long style = SYMBOL_SETROOTDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SETROOTDLG_IDNAME, const wxString& caption = SYMBOL_SETROOTDLG_TITLE, const wxPoint& pos = SYMBOL_SETROOTDLG_POSITION, const wxSize& size = SYMBOL_SETROOTDLG_SIZE, long style = SYMBOL_SETROOTDLG_STYLE );

    /// Destructor
    ~SetRootDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin SetRootDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_DIR
    void OnBtnDirClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_OK
    void OnBtnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_CANCEL
    void OnBtnCancelClick( wxCommandEvent& event );

////@end SetRootDlg event handler declarations

////@begin SetRootDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SetRootDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SetRootDlg member variables
    wxTextCtrl* m_txtDir;
////@end SetRootDlg member variables
};

#endif
    // _SETROOTDLG_H_