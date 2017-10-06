#pragma once

#ifndef __wxButton_H
#define __wxButton_H

class wxButtonX : public wxWindow
{
	DECLARE_DYNAMIC_CLASS( wxButtonX )
	DECLARE_EVENT_TABLE()
public:
	wxButtonX(void) { Init(); }
	wxButtonX(wxWindow *parent, wxWindowID id,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = 0)
	{
		Init();
		Create(parent, id, pos, size, style);
	}
	~wxButtonX(void);

	bool Create(wxWindow *parent, wxWindowID id,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = 0);
	void Init();
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground( wxEraseEvent& event );
	void OnMouseEnter(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	void OnLeftMouseDown(wxMouseEvent& event);
	void OnLeftMouseUp(wxMouseEvent& event);

	void SetNormalBmp(wxBitmap *bmp) { m_bmpNormal = bmp; m_bmpCur = bmp; }
	void SetHoverBmp(wxBitmap *bmp) { m_bmpHover = bmp; }
	void SetPushedBmp(wxBitmap *bmp) { m_bmpPushed = bmp; }

	enum BUTTON_STATE
	{
		BTN_STATE_NORMAL,
		BTN_STATE_HOVER,
		BTN_STATE_PUSHED
	};

protected:
	wxBitmap *m_bmpNormal, *m_bmpHover, *m_bmpPushed, *m_bmpCur;
	BUTTON_STATE m_btnState;
};

#endif
