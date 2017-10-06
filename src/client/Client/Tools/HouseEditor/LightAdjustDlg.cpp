// LightAdjustDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "LightAdjustDlg.h"
#include "CommFunc.h"
#include "MainFrm.h"

static Ogre::Vector4 StringToVector4( const std::string & s )
{
	std::vector<std::string> strs;
	strs = Ogre::StringUtil::split( s, " ," );

	Ogre::Vector4 v4;
	if ( strs.size() == 4 )
	{
		v4.x = Ogre::StringConverter::parseReal(strs[0]);
		v4.y = Ogre::StringConverter::parseReal(strs[1]);
		v4.z = Ogre::StringConverter::parseReal(strs[2]);
		v4.w = Ogre::StringConverter::parseReal(strs[3]);
	}

	return v4;
}

static std::string Vector4ToString( const Ogre::Vector4 & v )
{
	return FormatStr( "%.1f, %.2f, %.4f, %.6f", v.x, v.y, v.z, v.w );
}

void CLightAdjustDlg::open(CResourceDlg::PropSectionMap& section, CPropertyGrid* prop, bool AoP, int index, bool daynight)
{
	mProp = prop;
	ShowWindow(TRUE);

	CResourceDlg::PropSectionMap::iterator it = section.begin();
	assert(index==0 || index==1);
	if(index != 0)
		it++;

	if(AoP)
		mItem = it->second["Attenuation"];
	else if(daynight)
		mItem = it->second["P_DayAtten"];
	else
		mItem = it->second["P_NightAtten"];

	std::string sValue;

	mProp->GetItemValue( mItem, sValue );
	Ogre::Vector4 atten = StringToVector4(sValue);

	int i;
	CString str;

	i = atten.x;
	str.Format("%d", i);
	mEditRangeP.SetWindowText("1");
	mEditRangeB.SetWindowText("0");
	mEditRangeE.SetWindowText(str);
	mScrollRange.SetScrollRange(0, i);
	mScrollRange.SetScrollPos(i);

	i = atten.y/0.01;
	str.Format("%d", i);
	mEditConstP.SetWindowText("0.01");
	mEditConstB.SetWindowText("0");
	mEditConstE.SetWindowText(str);
	mScrollConst.SetScrollRange(0, i);
	mScrollConst.SetScrollPos(i);

	i = atten.z/0.0001;
	str.Format("%d", i);
	mEditLineP.SetWindowText("0.0001");
	mEditLineB.SetWindowText("0");
	mEditLineE.SetWindowText(str);
	mScrollLine.SetScrollRange(0, i);
	mScrollLine.SetScrollPos(i);

	i = atten.w/0.000001;
	str.Format("%d", i);
	mEditQuadP.SetWindowText("0.000001");
	mEditQuadB.SetWindowText("0");
	mEditQuadE.SetWindowText(str);
	mScrollQuad.SetScrollRange(0, i);
	mScrollQuad.SetScrollPos(i);
}

void CLightAdjustDlg::onValueChange()
{
	Ogre::Vector4 v4;
	CString str;
	float p;

	mEditRangeP.GetWindowText(str);
	sscanf(str, "%f", &p);
	v4.x = p * mScrollRange.GetScrollPos();

	mEditConstP.GetWindowText(str);
	sscanf(str, "%f", &p);
	v4.y = p * mScrollConst.GetScrollPos();

	mEditLineP.GetWindowText(str);
	sscanf(str, "%f", &p);
	v4.z = p * mScrollLine.GetScrollPos();

	mEditQuadP.GetWindowText(str);
	sscanf(str, "%f", &p);
	v4.w = p * mScrollQuad.GetScrollPos();

	std::string sValue = Vector4ToString(v4);
	mProp->SetItemValue( mItem, sValue );

	::SendMessage(g_MainFrame->getResourceDlg().m_hWnd, WM_PG_ITEMCHANGED, mItem, 0);
}

void CLightAdjustDlg::onRangeChange()
{
	CString str;
	int b,e;

	mEditRangeB.GetWindowText(str);
	sscanf(str, "%d", &b);
	mEditRangeE.GetWindowText(str);
	sscanf(str, "%d", &e);
	mScrollRange.SetScrollRange(b, e);

	mEditConstB.GetWindowText(str);
	sscanf(str, "%d", &b);
	mEditConstE.GetWindowText(str);
	sscanf(str, "%d", &e);
	mScrollConst.SetScrollRange(b, e);

	mEditLineB.GetWindowText(str);
	sscanf(str, "%d", &b);
	mEditLineE.GetWindowText(str);
	sscanf(str, "%d", &e);
	mScrollLine.SetScrollRange(b, e);

	mEditQuadB.GetWindowText(str);
	sscanf(str, "%d", &b);
	mEditQuadE.GetWindowText(str);
	sscanf(str, "%d", &e);
	mScrollQuad.SetScrollRange(b, e);
}

// CLightAdjustDlg dialog

IMPLEMENT_DYNAMIC(CLightAdjustDlg, CDialog)

CLightAdjustDlg::CLightAdjustDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightAdjustDlg::IDD, pParent)
{

}

CLightAdjustDlg::~CLightAdjustDlg()
{
}

void CLightAdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RANGE_P, mEditRangeP);
	DDX_Control(pDX, IDC_EDIT_RANGE_B, mEditRangeB);
	DDX_Control(pDX, IDC_EDIT_RANGE_E, mEditRangeE);
	DDX_Control(pDX, IDC_SCROLLBAR_RANGE, mScrollRange);
	DDX_Control(pDX, IDC_EDIT_CONST_P, mEditConstP);
	DDX_Control(pDX, IDC_EDIT_CONST_B, mEditConstB);
	DDX_Control(pDX, IDC_EDIT_CONST_E, mEditConstE);
	DDX_Control(pDX, IDC_SCROLLBAR_CONST, mScrollConst);
	DDX_Control(pDX, IDC_EDIT_LINE_P, mEditLineP);
	DDX_Control(pDX, IDC_EDIT_LINE_B, mEditLineB);
	DDX_Control(pDX, IDC_EDIT_LINE_E, mEditLineE);
	DDX_Control(pDX, IDC_SCROLLBAR_LINE, mScrollLine);
	DDX_Control(pDX, IDC_EDIT_QUAD_P, mEditQuadP);
	DDX_Control(pDX, IDC_EDIT_QUAD_B, mEditQuadB);
	DDX_Control(pDX, IDC_EDIT_QUAD_E, mEditQuadE);
	DDX_Control(pDX, IDC_SCROLLBAR_QUAD, mScrollQuad);
}

BEGIN_MESSAGE_MAP(CLightAdjustDlg, CDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_RANGE_B, &CLightAdjustDlg::OnEnChangeEditRangeB)
	ON_EN_CHANGE(IDC_EDIT_RANGE_E, &CLightAdjustDlg::OnEnChangeEditRangeE)
	ON_EN_CHANGE(IDC_EDIT_RANGE_P, &CLightAdjustDlg::OnEnChangeEditRangeP)
	ON_EN_CHANGE(IDC_EDIT_CONST_P, &CLightAdjustDlg::OnEnChangeEditConstP)
	ON_EN_CHANGE(IDC_EDIT_CONST_B, &CLightAdjustDlg::OnEnChangeEditConstB)
	ON_EN_CHANGE(IDC_EDIT_CONST_E, &CLightAdjustDlg::OnEnChangeEditConstE)
	ON_EN_CHANGE(IDC_EDIT_LINE_P, &CLightAdjustDlg::OnEnChangeEditLineP)
	ON_EN_CHANGE(IDC_EDIT_LINE_B, &CLightAdjustDlg::OnEnChangeEditLineB)
	ON_EN_CHANGE(IDC_EDIT_LINE_E, &CLightAdjustDlg::OnEnChangeEditLineE)
	ON_EN_CHANGE(IDC_EDIT_QUAD_P, &CLightAdjustDlg::OnEnChangeEditQuadP)
	ON_EN_CHANGE(IDC_EDIT_QUAD_B, &CLightAdjustDlg::OnEnChangeEditQuadB)
	ON_EN_CHANGE(IDC_EDIT_QUAD_E, &CLightAdjustDlg::OnEnChangeEditQuadE)
END_MESSAGE_MAP()

// CLightAdjustDlg message handlers

void CLightAdjustDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar==&mScrollRange || pScrollBar==&mScrollConst || 
			pScrollBar==&mScrollLine || pScrollBar==&mScrollQuad )
	{
		int minpos;
		int maxpos;
		pScrollBar->GetScrollRange(&minpos, &maxpos); 
		maxpos = pScrollBar->GetScrollLimit();

		// Get the current position of scroll box.
		int curpos = pScrollBar->GetScrollPos();

		// Determine the new position of scroll box.
		switch (nSBCode)
		{
		case SB_LEFT:      // Scroll to far left.
			curpos = minpos;
			break;

		case SB_RIGHT:      // Scroll to far right.
			curpos = maxpos;
			break;

		case SB_ENDSCROLL:   // End scroll.
			break;

		case SB_LINELEFT:      // Scroll left.
			if (curpos > minpos)
				curpos--;
			break;

		case SB_LINERIGHT:   // Scroll right.
			if (curpos < maxpos)
				curpos++;
			break;

		case SB_PAGELEFT:    // Scroll one page left.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);

				if (curpos > minpos)
					curpos = max(minpos, curpos - (int) info.nPage);
			}
			break;

		case SB_PAGERIGHT:      // Scroll one page right.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);

				if (curpos < maxpos)
					curpos = min(maxpos, curpos + (int) info.nPage);
			}
			break;

		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
			curpos = nPos;      // of the scroll box at the end of the drag operation.
			break;

		case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
			curpos = nPos;     // position that the scroll box has been dragged to.
			break;
		}

		pScrollBar->SetScrollPos(curpos);
		onValueChange();
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLightAdjustDlg::OnEnChangeEditRangeP()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onValueChange();
}

void CLightAdjustDlg::OnEnChangeEditRangeB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}

void CLightAdjustDlg::OnEnChangeEditRangeE()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}

void CLightAdjustDlg::OnEnChangeEditConstP()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onValueChange();
}

void CLightAdjustDlg::OnEnChangeEditConstB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}

void CLightAdjustDlg::OnEnChangeEditConstE()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}

void CLightAdjustDlg::OnEnChangeEditLineP()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onValueChange();
}

void CLightAdjustDlg::OnEnChangeEditLineB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}

void CLightAdjustDlg::OnEnChangeEditLineE()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}

void CLightAdjustDlg::OnEnChangeEditQuadP()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onValueChange();
}

void CLightAdjustDlg::OnEnChangeEditQuadB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}

void CLightAdjustDlg::OnEnChangeEditQuadE()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	onRangeChange();
}
