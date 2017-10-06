/////////////////////////////////////////////////////////////////////////////
// Name:        wxGaugeX.cpp
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

#include "wxGaugeX.h"

////@begin XPM images
////@end XPM images

/*
* wxStaticTextX event table definition
*/

BEGIN_EVENT_TABLE( wxGaugeX, wxWindow )

////@begin wxStaticTextX event table entries
EVT_PAINT( wxGaugeX::OnPaint )
EVT_ERASE_BACKGROUND( wxGaugeX::OnEraseBackground )
////@end wxStaticTextX event table entries

END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS( wxGaugeX, wxWindow )

wxGaugeX::~wxGaugeX(void)
{
}

bool wxGaugeX::Create( wxWindow *parent, wxWindowID id, const wxPoint &pos /*= wxDefaultPosition*/, const wxSize &size /*= wxDefaultSize*/, long style /*= 0*/ )
{
	wxWindow::Create(parent, id, pos, size, style);

	return true;
}

void wxGaugeX::Init()
{
	m_bmpBg = NULL;
	m_bmpRg = NULL;
	m_value = 0;
	m_range = 0;

}

void wxGaugeX::OnPaint( wxPaintEvent& event )
{
	if(m_bmpRg)
	{
		int curp = 0;

		wxSize size = GetSize();

		if(m_value && m_range)
			curp = m_value * size.x / m_range;

		if(curp)
		{
			wxPaintDC dc(this);
			wxMemoryDC mdc(*m_bmpRg);
			dc.Blit(wxPoint(0, 0), wxSize(curp, size.y), (wxDC *)&mdc, wxPoint(0, 0), wxCOPY, true);
		}
	}

	event.Skip();
}

void wxGaugeX::OnEraseBackground( wxEraseEvent& event )
{
	if(NULL == m_bmpBg)
		return;

	wxClientDC *cdc = NULL;

	if(NULL == event.GetDC())
		cdc = new wxClientDC(this);

	wxClientDC *dc = cdc ? cdc : static_cast<wxClientDC*>(event.GetDC());

	dc->DrawBitmap(*m_bmpBg, 0, 0, true);

	if(cdc)
		delete cdc;
}