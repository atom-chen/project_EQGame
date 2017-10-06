#pragma once

#ifndef __wxGaugeX_H
#define __wxGaugeX_H

class wxGaugeX : public wxWindow
{
	DECLARE_DYNAMIC_CLASS( wxStaticTextX )
	DECLARE_EVENT_TABLE()
public:
	wxGaugeX(void) { Init(); }
	wxGaugeX(wxWindow *parent, wxWindowID id,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = 0)
	{
		Init();
		Create(parent, id, pos, size, style);
	}
	~wxGaugeX(void);

	bool Create(wxWindow *parent, wxWindowID id,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = 0);
	void Init();
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground( wxEraseEvent& event );

	void SetBgBmp(wxBitmap *bmp) { m_bmpBg = bmp; }
	void SetRgBmp(wxBitmap *bmp) { m_bmpRg = bmp; }
	void SetValue(int value) { m_value = value; Refresh(false); }
	void SetRange(int range) { m_range = range; Refresh(); }
	int GetValue() { return m_value; }
	int GetRange() { return m_range; }

protected:
	wxBitmap *m_bmpBg, *m_bmpRg;
	wxString m_label;
	int m_value, m_range;
};

#endif