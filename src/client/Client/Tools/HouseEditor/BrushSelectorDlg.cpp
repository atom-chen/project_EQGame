// BrushSelectorDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "BrushSelectorDlg.h"

#include "RootManager.h"
#include "TerrainPaintInfoContainer.h"
#include "PaintAction.h"
#include "AABBBox.h"
#include "MfcInputEventMgr.h"
#include "RaiseHeightAction.h"
#include "LowerHeightAction.h"
#include "SmoothHeightAction.h"
#include "SimplePaintAction.h"
#include "AutoTexPaintAction.h"
#include "MainFrm.h"

CBrushSelectorDlg* g_CBrushSelectorDlg = NULL;

// CBrushSelectorDlg dialog

IMPLEMENT_DYNAMIC(CBrushSelectorDlg, CDialog)

CBrushSelectorDlg::CBrushSelectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushSelectorDlg::IDD, pParent),
	mPreviewImageWidth(64),
	mPreviewImageHeight(64),
	mTerrainModifyChanging(false),
	mBrushesTreeChanging(false)
{
	g_CBrushSelectorDlg = this;

	mCurrentFileName = "";

	mCurrentOrientationType = 0;

	mPaintInOneGrid = false;
	mIsRandomOrientation = false;

	initImage();
}

void CBrushSelectorDlg::initImage()
{
	// ��ʼ��image
	mCurrentPreviewImage.CreateBitmap(mPreviewImageWidth, mPreviewImageHeight, 1, 32, NULL);


	//�ڰ�ͼ
	BYTE* lpBits = new BYTE[mPreviewImageHeight*mPreviewImageWidth*4];
	int c = 0;
	for ( int i=0; i<mPreviewImageHeight; ++i )
	{
		for ( int j=0; j<mPreviewImageWidth; ++j )
		{
			lpBits[c+3] = 255;						//a
			lpBits[c+2] = getBlackWhiteValue(i,j);	//r
			lpBits[c+1] = getBlackWhiteValue(i,j);	//g
			lpBits[c]	= getBlackWhiteValue(i,j);	//b
			c += 4;
		}
	}
	mBlackWhitePreviewImage.CreateBitmap(mPreviewImageWidth, mPreviewImageHeight, 1, 32, lpBits);
	delete lpBits;
}

CBrushSelectorDlg::~CBrushSelectorDlg()
{
	g_CBrushSelectorDlg = NULL;
}

void CBrushSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BS_PIXMAP_LIST, mBrushesTree);
	DDX_Control(pDX, IDC_BS_FLIP_HORIZONTAL, mFlipHorizontal);
	DDX_Control(pDX, IDC_BS_FLIP_VERTICAL, mFlipVertical);
	DDX_Control(pDX, IDC_BS_ROTATE_RIGHT_ANGLE, mRotateRightAngle);
	DDX_Control(pDX, IDC_BS_RMIRROR_DIAGONAL, mMirrorDiagonal);
	DDX_Control(pDX, IDC_BS_RANDOM, mRandom);
	DDX_Control(pDX, IDC_BS_ENABLE_PIXMAP_PREVIEW, mEnablePreview);
	DDX_Control(pDX, IDC_BS_PIXMAP_PREVIEW, mBrushesPreview);
	DDX_Control(pDX, IDC_BS_TERRAIN_MODIFY, mTerrainModifyCtl);
	DDX_Control(pDX, IDC_BS_TERRAIN_VALUE, mTerrainValueCtl);
	DDX_Control(pDX, IDC_BS_AFFECT_RANGE, mAffectRangeCtl);
	DDX_Control(pDX, IDC_BS_EXP_VALUE, mExpValueCtl);
	DDX_Control(pDX, IDC_BS_PAINT_ONE_GRID, mPaintOneGridCtl);
}


BEGIN_MESSAGE_MAP(CBrushSelectorDlg, CDialog)
	ON_BN_CLICKED(IDC_BS_RELOAD_PIXMAP_LIST, &CBrushSelectorDlg::OnBnClickedBsReloadPixmapList)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BS_PIXMAP_LIST, &CBrushSelectorDlg::OnTvnSelchangedBsPixmapList)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BS_FLIP_HORIZONTAL, &CBrushSelectorDlg::OnBnClickedBsFlipHorizontal)
	ON_BN_CLICKED(IDC_BS_FLIP_VERTICAL, &CBrushSelectorDlg::OnBnClickedBsFlipVertical)
	ON_BN_CLICKED(IDC_BS_ROTATE_RIGHT_ANGLE, &CBrushSelectorDlg::OnBnClickedBsRotateRightAngle)
	ON_BN_CLICKED(IDC_BS_RMIRROR_DIAGONAL, &CBrushSelectorDlg::OnBnClickedBsRmirrorDiagonal)
	ON_BN_CLICKED(IDC_BS_RANDOM, &CBrushSelectorDlg::OnBnClickedBsRandom)
	ON_BN_CLICKED(IDC_BS_ENABLE_PIXMAP_PREVIEW, &CBrushSelectorDlg::OnBnClickedBsEnablePixmapPreview)
	ON_CBN_SELCHANGE(IDC_BS_TERRAIN_MODIFY, &CBrushSelectorDlg::OnCbnSelchangeBsTerrainModify)
	ON_EN_CHANGE(IDC_BS_TERRAIN_VALUE, &CBrushSelectorDlg::OnEnChangeBsTerrainValue)
	ON_EN_CHANGE(IDC_BS_AFFECT_RANGE, &CBrushSelectorDlg::OnEnChangeBsAffectRange)
	ON_EN_CHANGE(IDC_BS_EXP_VALUE, &CBrushSelectorDlg::OnEnChangeBsExpValue)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BS_PAINT_ONE_GRID, &CBrushSelectorDlg::OnBnClickedBsPaintOneGrid)
END_MESSAGE_MAP()


void CBrushSelectorDlg::ReloadTextureList()
{
	CatalogMap catalogMap;

	mBrushesTree.DeleteAllItems();

	::HTREEITEM rootHI = mBrushesTree.InsertItem( "��ˢ�б�" );

	// ���½��������ļ�
	RootManager::getSingleton().reloadPaintInfo();

	const TerrainPaintInfoContainer *paintInfoContainer = RootManager::getSingleton().getTerrainPaintInfoContainer();

	const TextureInfoMap &textureInfoMap = paintInfoContainer->getTextureInfoMap();

	TextureInfoMap::const_iterator it = textureInfoMap.begin();

	// ������ˢ����
	while ( it != textureInfoMap.end() )
	{
		const TextureInfos &textureInfos = it->second;

		// �����û�ˢ�µ���������
		for ( size_t i=0; i<textureInfos.size(); ++i )
		{
			// ȡ�������Ĵ����������
			Ogre::String ownerTexName = textureInfos[i].ownerTextureName;

			// ��������������а�����/��˵��������һ���ļ����е�
			size_t pos = ownerTexName.find_last_of('/');

			// ���ļ�����
			if (pos != Ogre::String::npos)
			{
				// ��ȥ�������ļ�����ʣ��·����
				ownerTexName.erase(pos+1);
				// ���ϻ�ˢ������
				ownerTexName.append(textureInfos[i].brushName);
				mBrushesTree.InsertItem( textureInfos[i].textureName.c_str(), GetParentId(ownerTexName, catalogMap)  );
			}
			else
			{
				Ogre::String brushName = textureInfos[i].brushName;
				// ������ڸ�Ŀ¼�£���ֱ���û�ˢ��������Ϊ·����
				mBrushesTree.InsertItem( textureInfos[i].textureName.c_str(), GetParentId(brushName, catalogMap) );
			}
		}
		++it;
	}

}

  
HTREEITEM CBrushSelectorDlg::GetParentId( Ogre::String &catalog, CatalogMap &map )
{
	// �����������·��Ϊ�գ�˵��������������ԴĿ¼�ĸ�Ŀ¼��
	if ( catalog == "" )
		// ���ԣ����Ľڵ�Ӧ�ñ����ڸ�Ŀ¼��
		return mBrushesTree.GetRootItem();
	else
	{
		// ��������ͼ��������һ��Ŀ¼������еĻ���˵�����Ŀ¼�������Ѿ����ã���������Ϊ��ͬ��Ŀ¼�µ�����
		// ��������ˣ����ͷ������Ŀ¼
		CatalogMap::iterator i = map.find(catalog);

		if ( i != map.end() )
			return i->second;
		// ���û�У��͵õݹ������ң�������
		else
		{
			Ogre::String path, baseName;
			Ogre::StringUtil::splitFilename(catalog, baseName, path);
			if (!path.empty() && path[path.length()-1] == '/')
				path.erase(path.length() - 1);

			HTREEITEM id = mBrushesTree.InsertItem( baseName.c_str(), GetParentId(path, map) );

			map.insert( CatalogMap::value_type( catalog, id ) );

			return id;
		}
	}
}

void CBrushSelectorDlg::buildPreviewBitmap( const TextureInfo& texInfo )
{
	const Ogre::uchar BytePerPixel = 8;

	// ��ȡԭʼimage
	Ogre::Image *oriImage = RootManager::getSingleton().getPreviewImage(texInfo.ownerTextureName);
	// Դ������Ĵ�С
	size_t oriImageHeight = oriImage->getHeight();
	size_t oriImageWidth = oriImage->getWidth();

	Ogre::uchar *oriImageData = oriImage->getData();
	// ��ѡ����Ĵ�С
	size_t newImageWidth = texInfo.width*TexTileSize;
	size_t newImageHeight = texInfo.height*TexTileSize;

	// ����һ���㹻��Ŀռ��������½���image������
	size_t newImagegetRowSpan = newImageWidth*oriImage->getBPP()/BytePerPixel;  // �½���image���п���λΪ�ֽڣ�
	Ogre::uchar *newImageData = OGRE_ALLOC_T(Ogre::uchar, newImageHeight*newImagegetRowSpan, Ogre::MEMCATEGORY_GENERAL);

	Ogre::uchar *newImageDataPointer = newImageData;
	// ��֪��ʼ���ص�
	size_t startPoint = ( oriImageWidth * texInfo.topCorner + texInfo.leftCorner )
		* TexTileSize * oriImage->getBPP()/BytePerPixel;

	Ogre::uchar *oriImagedataPointer = oriImageData + startPoint;

	// ����ѡ�������������ȡ������������һ���µ�image
	for ( Ogre::uint i=0; i<newImageHeight; ++i )
	{
		memcpy(newImageDataPointer, oriImagedataPointer, newImagegetRowSpan);
		newImageDataPointer += newImagegetRowSpan;
		oriImagedataPointer += oriImage->getRowSpan();
	}

	Ogre::Image newImage;
	newImage.loadDynamicImage(newImageData,newImageWidth,newImageHeight,1,oriImage->getFormat(),true);


	// �����ѡ�������64*64������resize
	if ( texInfo.width > 1 || texInfo.height > 1 )
		newImage.resize(mPreviewImageWidth, mPreviewImageHeight);



	BYTE* lpBits = new BYTE[mPreviewImageHeight*mPreviewImageWidth*4];

	// �����alpha��Ҫ��ڰ�ͼ���л��
	if ( newImage.getHasAlpha() )
	{
		uchar *tempPtr = newImage.getData();
		int c = 0;
		for ( int i=0; i<mPreviewImageHeight; ++i )
		{
			for ( int j=0; j<mPreviewImageWidth; ++j )
			{
				// ȡ��alphaֵ
				float alphaValue = (float)tempPtr[3] / 255.0f;
				uchar ground = getBlackWhiteValue(i,j);

				// ���������alpha��Ϻ����ɫֵ
				lpBits[c+3] = 255;			//a
				lpBits[c+2] = tempPtr[2] * alphaValue + ground*(1.0f - alphaValue);
				lpBits[c+1] = tempPtr[1] * alphaValue + ground*(1.0f - alphaValue);
				lpBits[c]	= tempPtr[0] * alphaValue + ground*(1.0f - alphaValue);
				tempPtr += 4;
				c += 4;
			}
		}
	}
	else
	{
		uchar *tempPtr = newImage.getData();
		int c = 0;
		for ( int i=0; i<mPreviewImageHeight; ++i )
		{
			for ( int j=0; j<mPreviewImageWidth; ++j )
			{
				lpBits[c+3] = 255;			//a
				lpBits[c+2] = tempPtr[2];	//r
				lpBits[c+1] = tempPtr[1];	//g
				lpBits[c]	= tempPtr[0];	//b
				tempPtr += 3;
				c += 4;
			}
		}
	}


	mCurrentPreviewImage.SetBitmapBits( mPreviewImageHeight*mPreviewImageWidth*4, lpBits );
	HBITMAP hBitmap = HBITMAP(mCurrentPreviewImage);
	mBrushesPreview.SetBitmap(hBitmap);

	//ɾ����ʱ�ڴ�
	delete lpBits;

}

uchar CBrushSelectorDlg::getBlackWhiteValue(int w, int h)
{
	const int size = 8;
	bool wm = ((w/size)%2 == 0);
	bool hm = ((h/size)%2 != 0);
	if ( wm == hm )
		return 255;
	else
		return 128;
}


const Action* CBrushSelectorDlg::OnStartAutoTexAction( const HTREEITEM& itemId, const std::string& brushName )
{
	Action* action = RootManager::getSingleton().setActiveAction("AutoTexPaintAction", false);

	// ��ȡ�������
	std::string groupName = mBrushesTree.GetItemText(itemId);

	// ���õ�ǰ�������
	action->setParameter( "%GroupName", brushName.c_str() );

	// ��֮ǰ��·������ȥ
	HTREEITEM parentId = mBrushesTree.GetParentItem(itemId);
	HTREEITEM rootId = mBrushesTree.GetRootItem();

	while ( parentId != rootId )
	{
		std::string parentText = mBrushesTree.GetItemText(parentId);
		parentText += '/';
		groupName = parentText + groupName;

		parentId = mBrushesTree.GetParentItem(parentId);   
	}

	groupName += "/";

	// ��ȡ��Ŀ¼�µĵ�һ���ڵ�
	HTREEITEM childId = mBrushesTree.GetChildItem(itemId);

	while ( childId )
	{
		// ����ÿ���ڵ㣬������������
		std::string texName = mBrushesTree.GetItemText(childId);

		childId = mBrushesTree.GetNextSiblingItem(childId);

		if ( texName.find('|') == std::string::npos )
			continue;

		action->setParameter( "%TextureName", texName.c_str() );        
	}

	// ����InputFinish��ʾ���������������
	action->setParameter( "%TextureName", "InputFinish" );

	// �ж������Ƿ���ȷ�ؽ����˳�ʼ��
	if ( action->getParameter("%IsInit") == "Yes" )
	{
		action->setActive(true);
		return action;
	}
	else
	{
		// ����simplepaintAction
		action = RootManager::getSingleton().setActiveAction("SimplePaintAction", false);

		action->setParameter( "%TextureName", "InputBegin" );

		// ��ȡ��Ŀ¼�µĵ�һ���ڵ�
		HTREEITEM childId = mBrushesTree.GetChildItem(itemId);

		while ( childId )
		{
			// ����ÿ���ڵ㣬������������
			std::string texName = mBrushesTree.GetItemText(childId);

			childId = mBrushesTree.GetNextSiblingItem(childId);

			if ( texName.find('|') == std::string::npos )
				continue;

			const TextureInfos &currentPaintInfos = 
				RootManager::getSingleton().getTextureInfos(mCurrentFileName.c_str());

			const TextureInfo &currentPaintInfo =
				RootManager::getSingleton().getTextureInfo(mCurrentFileName.c_str(), texName.c_str());

			Ogre::Image *previewImage = RootManager::getSingleton().getPreviewImage(currentPaintInfo.ownerTextureName);

			// ���������Ϣ�ַ���
			Ogre::String texInfoString;
			texInfoString += currentPaintInfo.ownerTextureName;
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.height);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.width);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.leftCorner);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.topCorner);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(previewImage->getHeight());
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(previewImage->getWidth());
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.rotateType);

			action->setParameter( "%TextureName", texInfoString.c_str() );  

			// �����и����󣿣�����Ϊû�����ø������Ƿ�Ϊ��ͨ�����С�����������һ��������һ����������ô
			// �����ܻ�ֻ���ŵ�һ��
		}

		if ( action->getParameter("%IsInit") == "Yes" )
		{
			action->setActive(true);
			return action;
		}
		else
		{
			// Ϊ�ܳɹ���ʼ�����Ȱ�active action��ΪNULL
			RootManager::getSingleton().setActiveAction("");
			return NULL;
		}
	}
}

void CBrushSelectorDlg::unselectTerrainModifyCtl()
{
	if ( !mTerrainModifyChanging && !g_MainFrameClosing )
	{
		mTerrainModifyCtl.SetCurSel(0);

		mTerrainValueCtl.ShowWindow(SW_HIDE);
		mAffectRangeCtl.ShowWindow(SW_HIDE);
		mExpValueCtl.ShowWindow(SW_HIDE);
	}
}

void CBrushSelectorDlg::unselectBrushesTree()
{
	if ( !mBrushesTreeChanging && !g_MainFrameClosing )
		mBrushesTree.SelectItem(NULL);

}

// CBrushSelectorDlg message handlers

void CBrushSelectorDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

//BOOL CBrushSelectorDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	return CDialog::Create(lpszTemplateName, pParentWnd);
//}

void CBrushSelectorDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

BOOL CBrushSelectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	//���ز����ļ�
	ReloadTextureList();

	mEnablePreview.SetCheck(1);

	// ��ʼ��ʱ����ѡ��ˢ�ķ���ѡ��
	mFlipHorizontal.EnableWindow(FALSE);
	mFlipVertical.EnableWindow(FALSE);
	mRotateRightAngle.EnableWindow(FALSE);
	mMirrorDiagonal.EnableWindow(FALSE);
	mRandom.EnableWindow(FALSE);
	mEnablePreview.EnableWindow(FALSE);

	CRect rc;
	mBrushesPreview.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.right = rc.left + 64;
	rc.bottom = rc.top + 64;
	mBrushesPreview.MoveWindow(rc);

	// ����ͼ�úڰ�ͼ
	mBrushesPreview.SetBitmap(HBITMAP(mBlackWhitePreviewImage));


	//===================================

	{ //����
		mTerrainModifyCtl.ResetContent();
		mTerrainModifyCtl.InsertString(0, "(��)");
		mTerrainModifyCtl.InsertString(1, "�������θ߶�");
		mTerrainModifyCtl.InsertString(2, "���͵��θ߶�");
		mTerrainModifyCtl.InsertString(3, "ƽ�����θ߶�");
		mTerrainModifyCtl.InsertString(4, "ƽ�����θ߶�");
		mTerrainModifyCtl.InsertString(5, "���õ��θ߶�");
		mTerrainModifyCtl.InsertString(6, "ɾ��͸������ͼ");
		mTerrainModifyCtl.InsertString(7, "��ת�Խ���");
		mTerrainModifyCtl.SetCurSel(0);

		mAffectRangeCtl.SetWindowText("2");
		mExpValueCtl.SetWindowText("0.5");

		mTerrainValueCtl.ShowWindow(SW_HIDE);
		mAffectRangeCtl.ShowWindow(SW_HIDE);
		mExpValueCtl.ShowWindow(SW_HIDE);

		mAffectRangeTT.Create(this,TTS_ALWAYSTIP);
		mAffectRangeTT.Activate(TRUE);
		mAffectRangeTT.AddTool(&mAffectRangeCtl,"���õ��λ�ˢ�Ķ���Ӱ�췶Χ(0~9)");

		mExpValueTT.Create(this,TTS_ALWAYSTIP);
		mExpValueTT.Activate(TRUE);
		mExpValueTT.AddTool(&mExpValueCtl,"���õ��λ�ˢ���ⷶΧ��Expֵ(��ΧԽԶ,Ӱ��ԽС,0~10)");
		
		mTerrainValueTT.Create(this,TTS_ALWAYSTIP);
		mTerrainValueTT.Activate(TRUE);
		mTerrainValueTT.AddTool(&mTerrainValueCtl,"���õ��εĸ߶�,�����Ǹ��߶�");
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBrushSelectorDlg::OnBnClickedBsReloadPixmapList()
{
	// TODO: Add your control notification handler code here
	RootManager::getSingleton().clearActiveAction();

	AutoTexPaintAction::clearBrushMap();
	ReloadTextureList();
}

class ValueReset
{
public:
	ValueReset(bool& v) : val(v)
	{
		val = true;
	}

	~ValueReset()
	{
		val = false;
	}

	bool& val;
};

void CBrushSelectorDlg::OnTvnSelchangedBsPixmapList(NMHDR *pNMHDR, LRESULT *pResult)
{
	ValueReset val(mBrushesTreeChanging);

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;



	// �����Ǹĳɶ����ļ�����޸�
	//  mPaintInOneGrid = false;
	// ���浱ǰѡ�е�����Ĵ�С
	int selectedTexXSize = 0;
	int selectedTexZSize = 0;

	// ����ˢ��ѡ��ʱ��Ҫ�����ж�
	HTREEITEM itemId = mBrushesTree.GetSelectedItem();
	if ( itemId == NULL )
		return;

	HTREEITEM rootId = mBrushesTree.GetRootItem();

	// ���ѡ�и����ͷ���
	if ( itemId == rootId )
	{
		// ѡ�и�Ŀ¼��ʱ������ѡ����
		mFlipHorizontal.EnableWindow(FALSE);
		mFlipVertical.EnableWindow(FALSE);
		mRotateRightAngle.EnableWindow(FALSE);
		mMirrorDiagonal.EnableWindow(FALSE);
		mRandom.EnableWindow(FALSE);
		mEnablePreview.EnableWindow(FALSE);

		// ����ͼ�úڰ�ͼ
		mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);

		return;
	}

	{
		mCurrentOrientationType = 0;

		// ���ѡ��ĸ���root��˵���û�ˢ�Ƿ��ڸ�Ŀ¼�µģ�Ҳ�п����Ǹ�Ŀ¼�µ���Ŀ¼
		if ( mBrushesTree.GetParentItem(itemId) == rootId )
		{
			mCurrentFileName = mBrushesTree.GetItemText(itemId);
		}
		else
		{
			// ���´����Ƿ��ڸ�Ŀ¼�µĻ�ˢ

			HTREEITEM parentId = mBrushesTree.GetParentItem(itemId);
			mCurrentFileName = mBrushesTree.GetItemText(itemId);

			// �Դ�ȡ����Ŀ¼��Ŀ¼������ϳ����·�������������ļ���
			while ( parentId != rootId )
			{
				std::string parentText = mBrushesTree.GetItemText(parentId);
				parentText += '/';
				mCurrentFileName = parentText + mCurrentFileName;

				parentId = mBrushesTree.GetParentItem(parentId);
			}		
		}	

		// �����������.��˵�����ļ���������Ŀ¼��
		if ( mCurrentFileName.find_first_of('|') != std::string::npos )
		{
			mFlipHorizontal.EnableWindow(TRUE);
			mFlipVertical.EnableWindow(TRUE);
			mRotateRightAngle.EnableWindow(TRUE);
			mMirrorDiagonal.EnableWindow(TRUE);
			mRandom.EnableWindow(TRUE);
			mEnablePreview.EnableWindow(TRUE);

			Ogre::String currentFileName(mCurrentFileName.c_str());

			size_t pos = currentFileName.find_last_of('/');

			// ȡ��brush����
			Ogre::String brushName = currentFileName.substr(0,pos);
			Ogre::String textureName = currentFileName.substr(pos+1);

			const TextureInfos &currentPaintInfos = 
				RootManager::getSingleton().getTextureInfos(brushName);

			const TextureInfo &currentPaintInfo = RootManager::getSingleton().getTextureInfo(brushName, textureName);

			Ogre::Image *previewImage = RootManager::getSingleton().getPreviewImage(currentPaintInfo.ownerTextureName);


			// ���������Ϣ�ַ���
			std::string texInfoString;
			texInfoString += currentPaintInfo.ownerTextureName;
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.height);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.width);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.leftCorner);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.topCorner);
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(previewImage->getHeight());
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(previewImage->getWidth());
			texInfoString += "|";
			texInfoString += Ogre::StringConverter::toString(currentPaintInfo.rotateType);

			Action* action = RootManager::getSingleton().setActiveAction("SimplePaintAction", false);
			action->setParameter( "%TextureName", "InputBegin" );
			action->setParameter( "%TextureName", texInfoString.c_str() );
			action->setParameter( "%OrientationType", Ogre::StringConverter::toString(mCurrentOrientationType) );

			if ( (currentPaintInfo.width > 1 || currentPaintInfo.height > 1) && !mPaintInOneGrid )
			{               
				action->setParameter( "%TexNormalSize", "false" );
				mFlipHorizontal.EnableWindow(FALSE);
				mFlipVertical.EnableWindow(FALSE);
				mRotateRightAngle.EnableWindow(FALSE);
				mMirrorDiagonal.EnableWindow(FALSE);
				mRandom.EnableWindow(FALSE);
			}
			else
			{
				action->setParameter("%TexNormalSize","true");
			}

			action->setActive(true);
			//GetSceneManipulator()->setActiveAction(action);

			buildPreviewBitmap(currentPaintInfo);

			if ( mEnablePreview.GetCheck() != BST_CHECKED )
			{
				// �������ʾ����ͼ������ʾĬ�ϵĺڰ�ͼ
				mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
			}
		}
		else // �����������.��˵�������ļ�������Ŀ¼��
		{
			mFlipHorizontal.EnableWindow(FALSE);
			mFlipVertical.EnableWindow(FALSE);
			mRotateRightAngle.EnableWindow(FALSE);
			mMirrorDiagonal.EnableWindow(FALSE);
			mRandom.EnableWindow(FALSE);
			mEnablePreview.EnableWindow(FALSE);

			// ������Ŀ¼����������жϣ������뵽͸�������б�

			const Action* action = OnStartAutoTexAction(itemId, mCurrentFileName);

			if ( action != 0 )
			{
				// �Ӹ��������ʵ�������������һ������Ϊ����ͼ
				// ��ȡ�������������ŵ�mPreviewImageWidth*mPreviewImageHeight����������ͼ
				//  Ogre::String tempTexName = action->getParameter( "%TextureName" );

				const TextureInfos &currentPaintInfos = 
					RootManager::getSingleton().getTextureInfos(mCurrentFileName.c_str());

				//  const WX::TextureInfo &currentPaintInfo = currentPaintInfos[0];

				// ����preview image
				buildPreviewBitmap(currentPaintInfos[0]);                
			}
			else
			{
				mEnablePreview.EnableWindow(FALSE);
				mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
			}

			if ( mEnablePreview.GetCheck() != BST_CHECKED )
			{
				// �������ʾ����ͼ������ʾĬ�ϵĺڰ�ͼ
				mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
			}
		}
	}


	// �����е�checkbox�ظ�δѡ���״̬
	mFlipHorizontal.SetCheck(0);
	mFlipVertical.SetCheck(0);
	mRotateRightAngle.SetCheck(0);
	mMirrorDiagonal.SetCheck(0);
	mRandom.SetCheck(0);

}

void CBrushSelectorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

}

void CBrushSelectorDlg::OnBnClickedBsFlipHorizontal()
{
	// TODO: Add your control notification handler code here
	OnCheck(IDC_BS_FLIP_HORIZONTAL);
}

void CBrushSelectorDlg::OnBnClickedBsFlipVertical()
{
	// TODO: Add your control notification handler code here
	OnCheck(IDC_BS_FLIP_VERTICAL);
}

void CBrushSelectorDlg::OnBnClickedBsRotateRightAngle()
{
	// TODO: Add your control notification handler code here
	OnCheck(IDC_BS_ROTATE_RIGHT_ANGLE);
}

void CBrushSelectorDlg::OnBnClickedBsRmirrorDiagonal()
{
	// TODO: Add your control notification handler code here
	OnCheck(IDC_BS_RMIRROR_DIAGONAL);
}

void CBrushSelectorDlg::OnBnClickedBsRandom()
{
	// TODO: Add your control notification handler code here
	OnCheck(IDC_BS_RANDOM);
}

void CBrushSelectorDlg::OnBnClickedBsEnablePixmapPreview()
{
	// TODO: Add your control notification handler code here
	OnCheck(IDC_BS_ENABLE_PIXMAP_PREVIEW);
}


void CBrushSelectorDlg::OnCheck(int cid)
{
	if ( cid == IDC_BS_RANDOM )
	{
		// ��ֹ������ť
		if ( mRandom.GetCheck() == BST_CHECKED )
		{
			mIsRandomOrientation = true;

			mFlipHorizontal.EnableWindow(FALSE);
			mFlipVertical.EnableWindow(FALSE);
			mRotateRightAngle.EnableWindow(FALSE);
			mMirrorDiagonal.EnableWindow(FALSE);
		}
		else
		{
			mIsRandomOrientation = false;

			mFlipHorizontal.EnableWindow(TRUE);
			mFlipVertical.EnableWindow(TRUE);
			mRotateRightAngle.EnableWindow(TRUE);
			mMirrorDiagonal.EnableWindow(TRUE);
		}
	}
	else
	{
		mCurrentOrientationType = 0;

		// ������checkbox״̬���ж�
		if ( mFlipHorizontal.GetCheck() == BST_CHECKED )
		{
			mCurrentOrientationType |= 1;
		}

		if ( mFlipVertical.GetCheck() == BST_CHECKED )
		{
			mCurrentOrientationType |= 2;
		}

		if ( mRotateRightAngle.GetCheck() == BST_CHECKED )
		{
			mCurrentOrientationType |= 4;
		}

		if ( mMirrorDiagonal.GetCheck() == BST_CHECKED )
		{
			mCurrentOrientationType |= 8;
		}
	}

	//
	Action* action = MfcInputEventMgr::getSingleton().getEvent<SimplePaintAction>();
	if ( action )
	{
		action->setParameter( "%UseRandomOrientation", mIsRandomOrientation ? "true" : "false" );
		action->setParameter( "%OrientationType", Ogre::StringConverter::toString(mCurrentOrientationType) );

		action->setActive(true);
	}


	//Ԥ��
	if ( mEnablePreview.GetCheck() == BST_CHECKED )
	{
		// ѡ�а�ťʱ������ͼҲҪ��Ӧ�仯
		if ( cid == IDC_BS_FLIP_HORIZONTAL )
		{
			BmpMirror(mCurrentPreviewImage);
			mBrushesPreview.SetBitmap(mCurrentPreviewImage);
		}

		if ( cid == IDC_BS_FLIP_VERTICAL )
		{
			BmpMirror(mCurrentPreviewImage, false);
			mBrushesPreview.SetBitmap(mCurrentPreviewImage);
		}

		if ( cid == IDC_BS_ROTATE_RIGHT_ANGLE )
		{
			if ( mRotateRightAngle.GetCheck() == BST_CHECKED )
			{
				BmpRotate90(mCurrentPreviewImage);	//�����в��
				mBrushesPreview.SetBitmap(mCurrentPreviewImage);
			}
			else
			{
				BmpRotate90(mCurrentPreviewImage, false);
				mBrushesPreview.SetBitmap(mCurrentPreviewImage);
			}
		}
	}
	else
	{
		// �������ʾ����ͼ������ʾĬ�ϵĺڰ�ͼ
		mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
	}

	// Ԥ��ͼ�Ŀ���
	if ( cid == IDC_BS_ENABLE_PIXMAP_PREVIEW )
	{
		if ( mEnablePreview.GetCheck() == BST_CHECKED )
		{
			// ����û��򿪿��أ�����ǰ��OnPixmapListSelected�м���õ�Ԥ��ͼ����ʾ
			// ��Ҳ��Ϊʲôǰ��OnPixmapListSelected��������������Ƿ�򿪶�Ҫ����Ԥ��ͼ��ԭ��
			// ��Ȼ�Ļ�������򿪿��غ���ʾ�����ϴμ����Ԥ��ͼ�������ǵ�ǰ��ѡ�����Ԥ��ͼ
			mBrushesPreview.SetBitmap(mCurrentPreviewImage);
		}
		else
		{
			mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
		}
	}
}

void CBrushSelectorDlg::BmpRotate90( CBitmap& bitmap, bool clockwise )
{
	int size = mPreviewImageWidth*mPreviewImageHeight*4;
	BYTE* lpBits = new BYTE[size];
	BYTE* lpBits2 = new BYTE[size];
	bitmap.GetBitmapBits(size, lpBits);

	int ic = mPreviewImageWidth-1;
	int jc = mPreviewImageHeight-1;
	for ( int i = 0; i < mPreviewImageWidth; ++i )
	{
		for ( int j = 0; j < mPreviewImageHeight; ++j )
		{
			int ti = i*2 - ic;
			int tj = j*2 - jc;
			if ( clockwise )
				Int2AABBBox::rotateQuarter(ti, tj, 1, true);
			else
				Int2AABBBox::rotateQuarter(ti, tj, 1);
			ti += ic; ti >>= 1;
			tj += jc; tj >>= 1;

			assert(ti<mPreviewImageWidth && ti>=0);
			assert(tj<mPreviewImageHeight && tj>=0);

			int c = (i * mPreviewImageWidth + j)*4;
			int tc = (ti * mPreviewImageWidth + tj)*4;
			lpBits2[tc]		 = lpBits[c];
			lpBits2[tc+1]	 = lpBits[c+1];
			lpBits2[tc+2]	 = lpBits[c+2];
			lpBits2[tc+3]	 = lpBits[c+3];
		}
	}

	bitmap.SetBitmapBits(size, lpBits2);

	delete lpBits2;
	delete lpBits;

}

void CBrushSelectorDlg::BmpMirror( CBitmap& bitmap, bool horizontally )
{
	int size = mPreviewImageWidth*mPreviewImageHeight*4;
	BYTE* lpBits = new BYTE[size];
	bitmap.GetBitmapBits(size, lpBits);

	if ( horizontally )
	{
		for ( int i = 0; i < mPreviewImageWidth; ++i )
		{
			for ( int j = 0; j < mPreviewImageHeight/2; ++j )
			{
				int c = (i * mPreviewImageWidth + j)*4;
				int bc = (i * mPreviewImageWidth + mPreviewImageHeight-j-1)*4;
				BYTE tmp0 = lpBits[c];
				BYTE tmp1 = lpBits[c+1];
				BYTE tmp2 = lpBits[c+2];
				BYTE tmp3 = lpBits[c+3];
				lpBits[c]	= lpBits[bc];
				lpBits[c+1] = lpBits[bc+1];
				lpBits[c+2]	= lpBits[bc+2];
				lpBits[c+3] = lpBits[bc+3];
				lpBits[bc]	= tmp0;
				lpBits[bc+1]= tmp1;
				lpBits[bc+2]= tmp2;
				lpBits[bc+3]= tmp3;
			}
		}
	}
	else
	{
		for ( int i = 0; i < mPreviewImageWidth/2; ++i )
		{
			for ( int j = 0; j < mPreviewImageHeight; ++j )
			{
				int c = (i * mPreviewImageWidth + j)*4;
				int bc = ((mPreviewImageWidth-i-1) * mPreviewImageWidth + j)*4;
				BYTE tmp0 = lpBits[c];
				BYTE tmp1 = lpBits[c+1];
				BYTE tmp2 = lpBits[c+2];
				BYTE tmp3 = lpBits[c+3];
				lpBits[c]	= lpBits[bc];
				lpBits[c+1] = lpBits[bc+1];
				lpBits[c+2]	= lpBits[bc+2];
				lpBits[c+3] = lpBits[bc+3];
				lpBits[bc]	= tmp0;
				lpBits[bc+1]= tmp1;
				lpBits[bc+2]= tmp2;
				lpBits[bc+3]= tmp3;
			}
		}
	}

	bitmap.SetBitmapBits(size, lpBits);

	delete lpBits;
}


void CBrushSelectorDlg::OnCbnSelchangeBsTerrainModify()
{
	mTerrainModifyChanging = true;

	mTerrainValueCtl.ShowWindow(SW_HIDE);
	mAffectRangeCtl.ShowWindow(SW_HIDE);
	mExpValueCtl.ShowWindow(SW_HIDE);

	int sel = mTerrainModifyCtl.GetCurSel();

	if ( sel == 0 )
	{
		//ȡ������ 
		RootManager::getSingleton().clearActiveAction();
	}
	else if ( sel == 1 )
	{
		//���ߵ���
		RootManager::getSingleton().setActiveAction("RaiseHeightAction");
		mAffectRangeCtl.ShowWindow(SW_SHOW);
		mExpValueCtl.ShowWindow(SW_SHOW);
	}
	else if ( sel == 2 )
	{
		//���͵���
		RootManager::getSingleton().setActiveAction("LowerHeightAction");
		mAffectRangeCtl.ShowWindow(SW_SHOW);
		mExpValueCtl.ShowWindow(SW_SHOW);
	}
	else if ( sel == 3 )
	{
		//ƽ�����θ߶�
		RootManager::getSingleton().setActiveAction("SmoothHeightAction");
	}
	else if ( sel == 4 )
	{
		//ƽ�����θ߶�
		RootManager::getSingleton().setActiveAction("LevelOffHeightAction");
	}
	else if ( sel == 5 )
	{
		//���õ��θ߶�
		RootManager::getSingleton().setActiveAction("SetTerrainHeightAction");
		mTerrainValueCtl.ShowWindow(SW_SHOW);
		OnEnChangeBsTerrainValue();
	}
	else if ( sel == 6 )
	{
		//ɾ��͸������ͼ
		RootManager::getSingleton().setActiveAction("TerrainLayerOneEraserAction");
	}
	else if ( sel == 7 )
	{
		//��ת�Խ���
		RootManager::getSingleton().setActiveAction("FlipDiagonalAction");
	}

	
	mTerrainModifyChanging = false;
}


void CBrushSelectorDlg::OnEnChangeBsTerrainValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if ( !mTerrainValueCtl.IsWindowEnabled() )
		return;
	if ( !mTerrainValueCtl.IsWindowVisible() )
		return;

	Action* oa = MfcInputEventMgr::getSingleton().getEvent<Action>();
	if ( !oa )
		return;

	if ( oa->getName() != "SetTerrainHeightAction" )
		return;

	CString str;
	mTerrainValueCtl.GetWindowText(str);

	oa->setParameter("%TerrainHeight", str.GetBuffer());
}

void CBrushSelectorDlg::OnEnChangeBsAffectRange()
{
	CString str;
	mAffectRangeCtl.GetWindowText(str);

	int a = Ogre::StringConverter::parseInt(str.GetBuffer());

	bool change = false;
	if ( a < 0 )
	{
		a = 0;
		change = true;
	}

	if ( a > 9 )
	{
		a = 9;
		change = true;
	}

	if ( change )
	{
		std::string s = Ogre::StringConverter::toString(a);
		mAffectRangeCtl.SetWindowText(s.c_str());
	}

	RootManager::getSingleton().setJunctionWeightAffectRange(a);
}

void CBrushSelectorDlg::OnEnChangeBsExpValue()
{
	CString str;
	mExpValueCtl.GetWindowText(str);

	float a = Ogre::StringConverter::parseReal(str.GetBuffer());

	bool change = false;
	if ( a < 0 )
	{
		a = 0;
		change = true;
	}

	if ( a > 10 )
	{
		a = 10;
		change = true;
	}

	if ( change )
	{
		std::string s = Ogre::StringConverter::toString(a);
		mExpValueCtl.SetWindowText(s.c_str());
	}

	RootManager::getSingleton().setJunctionWeightExpValue(a);
}

BOOL CBrushSelectorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(mAffectRangeTT.GetSafeHwnd()!=NULL)
		mAffectRangeTT.RelayEvent(pMsg);

	if(mExpValueTT.GetSafeHwnd()!=NULL)
		mExpValueTT.RelayEvent(pMsg);

	if(mTerrainValueTT.GetSafeHwnd()!=NULL)
		mTerrainValueTT.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CBrushSelectorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if ( mBrushesTree.m_hWnd )
	{
		CRect rc;
		GetClientRect( rc );
		rc.top += 185;
		mBrushesTree.MoveWindow( rc );
	}
}

void CBrushSelectorDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if ( !bShow )
	{
		RootManager::getSingleton().clearActiveAction();
	}
}

void CBrushSelectorDlg::OnBnClickedBsPaintOneGrid()
{
	if ( mPaintOneGridCtl.GetCheck() == BST_CHECKED )
		mPaintInOneGrid = true;
	else
		mPaintInOneGrid = false;

	RootManager::getSingleton().clearActiveAction();
}
