/////////////////////////////////////////////////////////////////////////////
// Name:        listmakerapp.h
// Purpose:     
// Author:      xj
// Modified by: 
// Created:     16/11/2010 14:19:22
// RCS-ID:      
// Copyright:   XJ-1
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _LISTMAKERAPP_H_
#define _LISTMAKERAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "listmakerframe.h"
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
 * ListMakerApp class declaration
 */

class ListMakerApp: public wxApp
{    
    DECLARE_CLASS( ListMakerApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    ListMakerApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin ListMakerApp event handler declarations

////@end ListMakerApp event handler declarations

////@begin ListMakerApp member function declarations

////@end ListMakerApp member function declarations

////@begin ListMakerApp member variables
////@end ListMakerApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(ListMakerApp)
////@end declare app

#endif
    // _LISTMAKERAPP_H_
