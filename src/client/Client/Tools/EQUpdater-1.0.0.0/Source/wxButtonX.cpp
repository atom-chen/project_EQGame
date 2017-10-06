/////////////////////////////////////////////////////////////////////////////
// Name:        wxButtonX.cpp
// Purpose:     
// Author:      xj
// Modified by: 
// Created:     15/02/2011 09:25:37
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

#include "wxButtonX.h"

////@begin XPM images
////@end XPM images

/*
* wxStaticTextX event table definition
*/

BEGIN_EVENT_TABLE( wxButtonX, wxWindow )

////@begin wxStaticTextX event table entries
EVT_PAINT( wxButtonX::OnPaint )
EVT_ERASE_BACKGROUND( wxButtonX::OnEraseBackground )
EVT_ENTER_WINDOW( wxButtonX::OnMouseEnter )
EVT_LEAVE_WINDOW( wxButtonX::OnMouseLeave )
EVT_LEFT_DOWN( wxButtonX::OnLeftMouseDown )
EVT_LEFT_UP( wxButtonX::OnLeftMouseUp )
////@end wxStaticTextX event table entries

END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS( wxButtonX, wxWindow )


wxButtonX::~wxButtonX(void)
{
}

bool wxButtonX::Create( wxWindow *parent, wxWindowID id, const wxPoint &pos /*= wxDefaultPosition*/, const wxSize &size /*= wxDefaultSize*/, long style /*= 0*/ )
{
	wxWindow::Create(parent, id, pos, size, style);

	return true;
}

void wxButtonX::Init()
{
	m_bmpNormal = NULL;
	m_bmpHover = NULL;
	m_bmpPushed = NULL;
	m_bmpCur = NULL;

	m_btnState = BTN_STATE_NORMAL;

}

void wxButtonX::OnPaint( wxPaintEvent& event )
{
	if(m_bmpCur)
	{
		wxPaintDC dc(this);
		dc.DrawBitmap(*m_bmpCur, 0, 0);
	}

	event.Skip();
}

void wxButtonX::OnEraseBackground( wxEraseEvent& event )
{

}

void wxButtonX::OnMouseEnter( wxMouseEvent& event )
{
	m_bmpCur = m_bmpHover;
	this->Refresh(false);

}

void wxButtonX::OnMouseLeave( wxMouseEvent& event )
{
	m_bmpCur = m_bmpNormal;
	this->Refresh(false);

}

void wxButtonX::OnLeftMouseDown( wxMouseEvent& event )
{
	m_bmpCur = m_bmpPushed;
	this->Refresh(false);

}

void wxButtonX::OnLeftMouseUp( wxMouseEvent& event )
{
	m_bmpCur = m_bmpNormal;
	this->Refresh(false);

}