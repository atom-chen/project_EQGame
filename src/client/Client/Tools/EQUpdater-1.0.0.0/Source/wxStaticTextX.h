#pragma once

#ifndef __wxStaticTextX_H
#define __wxStaticTextX_H

class wxStaticTextX : public wxWindow
{
	DECLARE_DYNAMIC_CLASS( wxStaticTextX )
	DECLARE_EVENT_TABLE()
public:
	wxStaticTextX(void) { Init(); }
	wxStaticTextX(wxWindow *parent, wxWindowID id,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = 0)
	{
		Init();
		Create(parent, id, pos, size, style);
	}
	~wxStaticTextX(void);

	bool Create(wxWindow *parent, wxWindowID id,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = 0);
	void Init();
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground( wxEraseEvent& event );

	void SetBgBmp(wxBitmap *bmp) { m_bmpBg = bmp; }
	void SetAlign(int align) { m_align = align; }
	virtual void SetLabel(const wxString& label) { m_label = label; Refresh(); }
	virtual wxString GetLabel() const { return m_label; }

protected:
	wxBitmap *m_bmpBg;
	wxString m_label;
	int m_align;
};

#endif