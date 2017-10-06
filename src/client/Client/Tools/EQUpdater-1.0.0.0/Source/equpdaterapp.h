/////////////////////////////////////////////////////////////////////////////
// Name:        equpdaterapp.h
// Purpose:     
// Author:      xj
// Modified by: 
// Created:     15/11/2010 09:25:37
// RCS-ID:      
// Copyright:   XJ-1
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _EQUPDATERAPP_H_
#define _EQUPDATERAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "wx/snglinst.h"
#include "equpdatermainwin.h"
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
////@end control identifiers

/*!
 * EQUpdaterApp class declaration
 */

class EQUpdaterApp: public wxApp
{    
    DECLARE_CLASS( EQUpdaterApp )
    DECLARE_EVENT_TABLE()

protected:
	wxSingleInstanceChecker *m_checker;

public:
    /// Constructor
    EQUpdaterApp();

    void Init();

	virtual bool Initialize(int& argc, wxChar **argv);

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin EQUpdaterApp event handler declarations

////@end EQUpdaterApp event handler declarations

////@begin EQUpdaterApp member function declarations

////@end EQUpdaterApp member function declarations

////@begin EQUpdaterApp member variables
////@end EQUpdaterApp member variables

public:
	wxChar* m_para;
	wxChar*	m_user;
	wxChar*	m_pwd;
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(EQUpdaterApp)
////@end declare app

#endif
    // _EQUPDATERAPP_H_
