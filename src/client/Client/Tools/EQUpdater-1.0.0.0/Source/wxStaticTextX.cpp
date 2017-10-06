/////////////////////////////////////////////////////////////////////////////
// Name:        wxStaticTextX.cpp
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

#include "wxStaticTextX.h"

////@begin XPM images
////@end XPM images

/*
* wxStaticTextX event table definition
*/

BEGIN_EVENT_TABLE( wxStaticTextX, wxWindow )

////@begin wxStaticTextX event table entries
EVT_PAINT( wxStaticTextX::OnPaint )
EVT_ERASE_BACKGROUND( wxStaticTextX::OnEraseBackground )
////@end wxStaticTextX event table entries

END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS( wxStaticTextX, wxWindow )

wxStaticTextX::~wxStaticTextX(void)
{
}

bool wxStaticTextX::Create( wxWindow *parent, wxWindowID id, const wxPoint &pos /*= wxDefaultPosition*/, const wxSize &size /*= wxDefaultSize*/, long style /*= 0*/ )
{
	wxWindow::Create(parent, id, pos, size, style);

	return true;
}

void wxStaticTextX::Init()
{
	m_bmpBg = NULL;
	m_align = wxALIGN_LEFT | wxALIGN_TOP;

}

void wxStaticTextX::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);

	wxFont ft(9, wxFONTFAMILY_ROMAN, wxNORMAL, wxNORMAL, false, wxT("ËÎÌו"));
	dc.SetFont(ft);
	dc.SetTextForeground(wxColour(33,132,202));//33,132,202           255,0,0
	//dc.DrawText(m_label, wxPoint(0, 0));
	dc.DrawLabel(m_label, wxRect(GetSize()), m_align);

	event.Skip();
}

void wxStaticTextX::OnEraseBackground( wxEraseEvent& event )
{
	if(NULL == m_bmpBg)
		return;

	wxClientDC *cdc = NULL;

	if(NULL == event.GetDC())
		cdc = new wxClientDC(this);

	wxClientDC *dc = cdc ? cdc : static_cast<wxClientDC*>(event.GetDC());
	wxMemoryDC mdc(*m_bmpBg);

	wxSize size = GetSize();
	wxPoint pt = GetPosition();

	dc->Blit(wxPoint(0, 0), size, (wxDC *)&mdc, pt, wxCOPY);

	if(cdc)
		delete cdc;

}
