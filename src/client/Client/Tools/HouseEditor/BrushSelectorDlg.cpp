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
	// 初始化image
	mCurrentPreviewImage.CreateBitmap(mPreviewImageWidth, mPreviewImageHeight, 1, 32, NULL);


	//黑白图
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

	::HTREEITEM rootHI = mBrushesTree.InsertItem( "画刷列表" );

	// 重新解析定义文件
	RootManager::getSingleton().reloadPaintInfo();

	const TerrainPaintInfoContainer *paintInfoContainer = RootManager::getSingleton().getTerrainPaintInfoContainer();

	const TextureInfoMap &textureInfoMap = paintInfoContainer->getTextureInfoMap();

	TextureInfoMap::const_iterator it = textureInfoMap.begin();

	// 遍历画刷数组
	while ( it != textureInfoMap.end() )
	{
		const TextureInfos &textureInfos = it->second;

		// 遍历该画刷下的所有纹理
		for ( size_t i=0; i<textureInfos.size(); ++i )
		{
			// 取出所属的大纹理的名称
			Ogre::String ownerTexName = textureInfos[i].ownerTextureName;

			// 如果该纹理名称中包含了/，说明它是在一个文件夹中的
			size_t pos = ownerTexName.find_last_of('/');

			// 在文件夹中
			if (pos != Ogre::String::npos)
			{
				// 先去除纹理文件名，剩下路径名
				ownerTexName.erase(pos+1);
				// 加上画刷的名称
				ownerTexName.append(textureInfos[i].brushName);
				mBrushesTree.InsertItem( textureInfos[i].textureName.c_str(), GetParentId(ownerTexName, catalogMap)  );
			}
			else
			{
				Ogre::String brushName = textureInfos[i].brushName;
				// 如果是在根目录下，就直接用画刷名称来作为路径名
				mBrushesTree.InsertItem( textureInfos[i].textureName.c_str(), GetParentId(brushName, catalogMap) );
			}
		}
		++it;
	}

}

  
HTREEITEM CBrushSelectorDlg::GetParentId( Ogre::String &catalog, CatalogMap &map )
{
	// 如果送入的相对路径为空，说明这个纹理放在资源目录的根目录下
	if ( catalog == "" )
		// 所以，它的节点应该被建在根目录下
		return mBrushesTree.GetRootItem();
	else
	{
		// 否则，先试图找它的上一级目录，如果有的话（说明这个目录在树中已经建好，可能是因为有同个目录下的纹理
		// 被处理过了），就返回这个目录
		CatalogMap::iterator i = map.find(catalog);

		if ( i != map.end() )
			return i->second;
		// 如果没有，就得递归往上找，并建树
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

	// 读取原始image
	Ogre::Image *oriImage = RootManager::getSingleton().getPreviewImage(texInfo.ownerTextureName);
	// 源大纹理的大小
	size_t oriImageHeight = oriImage->getHeight();
	size_t oriImageWidth = oriImage->getWidth();

	Ogre::uchar *oriImageData = oriImage->getData();
	// 所选纹理的大小
	size_t newImageWidth = texInfo.width*TexTileSize;
	size_t newImageHeight = texInfo.height*TexTileSize;

	// 分配一个足够大的空间来保存新建的image的数据
	size_t newImagegetRowSpan = newImageWidth*oriImage->getBPP()/BytePerPixel;  // 新建的image的行宽（单位为字节）
	Ogre::uchar *newImageData = OGRE_ALLOC_T(Ogre::uchar, newImageHeight*newImagegetRowSpan, Ogre::MEMCATEGORY_GENERAL);

	Ogre::uchar *newImageDataPointer = newImageData;
	// 得知起始像素点
	size_t startPoint = ( oriImageWidth * texInfo.topCorner + texInfo.leftCorner )
		* TexTileSize * oriImage->getBPP()/BytePerPixel;

	Ogre::uchar *oriImagedataPointer = oriImageData + startPoint;

	// 把所选的纹理的数据提取出来，并创建一个新的image
	for ( Ogre::uint i=0; i<newImageHeight; ++i )
	{
		memcpy(newImageDataPointer, oriImagedataPointer, newImagegetRowSpan);
		newImageDataPointer += newImagegetRowSpan;
		oriImagedataPointer += oriImage->getRowSpan();
	}

	Ogre::Image newImage;
	newImage.loadDynamicImage(newImageData,newImageWidth,newImageHeight,1,oriImage->getFormat(),true);


	// 如果所选纹理大于64*64，就先resize
	if ( texInfo.width > 1 || texInfo.height > 1 )
		newImage.resize(mPreviewImageWidth, mPreviewImageHeight);



	BYTE* lpBits = new BYTE[mPreviewImageHeight*mPreviewImageWidth*4];

	// 如果有alpha，要与黑白图进行混合
	if ( newImage.getHasAlpha() )
	{
		uchar *tempPtr = newImage.getData();
		int c = 0;
		for ( int i=0; i<mPreviewImageHeight; ++i )
		{
			for ( int j=0; j<mPreviewImageWidth; ++j )
			{
				// 取出alpha值
				float alphaValue = (float)tempPtr[3] / 255.0f;
				uchar ground = getBlackWhiteValue(i,j);

				// 计算出经过alpha混合后的颜色值
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

	//删除临时内存
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

	// 获取组的名称
	std::string groupName = mBrushesTree.GetItemText(itemId);

	// 设置当前组的名称
	action->setParameter( "%GroupName", brushName.c_str() );

	// 把之前的路径加上去
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

	// 获取该目录下的第一个节点
	HTREEITEM childId = mBrushesTree.GetChildItem(itemId);

	while ( childId )
	{
		// 遍历每个节点，设置纹理名称
		std::string texName = mBrushesTree.GetItemText(childId);

		childId = mBrushesTree.GetNextSiblingItem(childId);

		if ( texName.find('|') == std::string::npos )
			continue;

		action->setParameter( "%TextureName", texName.c_str() );        
	}

	// 输入InputFinish表示纹理名称输入完毕
	action->setParameter( "%TextureName", "InputFinish" );

	// 判断纹理是否正确地进行了初始化
	if ( action->getParameter("%IsInit") == "Yes" )
	{
		action->setActive(true);
		return action;
	}
	else
	{
		// 启用simplepaintAction
		action = RootManager::getSingleton().setActiveAction("SimplePaintAction", false);

		action->setParameter( "%TextureName", "InputBegin" );

		// 获取该目录下的第一个节点
		HTREEITEM childId = mBrushesTree.GetChildItem(itemId);

		while ( childId )
		{
			// 遍历每个节点，设置纹理名称
			std::string texName = mBrushesTree.GetItemText(childId);

			childId = mBrushesTree.GetNextSiblingItem(childId);

			if ( texName.find('|') == std::string::npos )
				continue;

			const TextureInfos &currentPaintInfos = 
				RootManager::getSingleton().getTextureInfos(mCurrentFileName.c_str());

			const TextureInfo &currentPaintInfo =
				RootManager::getSingleton().getTextureInfo(mCurrentFileName.c_str(), texName.c_str());

			Ogre::Image *previewImage = RootManager::getSingleton().getPreviewImage(currentPaintInfo.ownerTextureName);

			// 组成纹理信息字符串
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

			// 这里有个错误？？，因为没有设置该纹理是否为普通纹理大小，所以如果这一个组中有一个大纹理，那么
			// 它可能会只缩放到一格
		}

		if ( action->getParameter("%IsInit") == "Yes" )
		{
			action->setActive(true);
			return action;
		}
		else
		{
			// 为能成功初始化，先把active action设为NULL
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

	//加载材质文件
	ReloadTextureList();

	mEnablePreview.SetCheck(1);

	// 初始化时不能选择画刷的方向选项
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

	// 缩略图用黑白图
	mBrushesPreview.SetBitmap(HBITMAP(mBlackWhitePreviewImage));


	//===================================

	{ //地形
		mTerrainModifyCtl.ResetContent();
		mTerrainModifyCtl.InsertString(0, "(无)");
		mTerrainModifyCtl.InsertString(1, "提升地形高度");
		mTerrainModifyCtl.InsertString(2, "降低地形高度");
		mTerrainModifyCtl.InsertString(3, "平滑地形高度");
		mTerrainModifyCtl.InsertString(4, "平整地形高度");
		mTerrainModifyCtl.InsertString(5, "设置地形高度");
		mTerrainModifyCtl.InsertString(6, "删除透明层贴图");
		mTerrainModifyCtl.InsertString(7, "反转对角线");
		mTerrainModifyCtl.SetCurSel(0);

		mAffectRangeCtl.SetWindowText("2");
		mExpValueCtl.SetWindowText("0.5");

		mTerrainValueCtl.ShowWindow(SW_HIDE);
		mAffectRangeCtl.ShowWindow(SW_HIDE);
		mExpValueCtl.ShowWindow(SW_HIDE);

		mAffectRangeTT.Create(this,TTS_ALWAYSTIP);
		mAffectRangeTT.Activate(TRUE);
		mAffectRangeTT.AddTool(&mAffectRangeCtl,"设置地形画刷的额外影响范围(0~9)");

		mExpValueTT.Create(this,TTS_ALWAYSTIP);
		mExpValueTT.Activate(TRUE);
		mExpValueTT.AddTool(&mExpValueCtl,"设置地形画刷额外范围的Exp值(范围越远,影响越小,0~10)");
		
		mTerrainValueTT.Create(this,TTS_ALWAYSTIP);
		mTerrainValueTT.Activate(TRUE);
		mTerrainValueTT.AddTool(&mTerrainValueCtl,"设置地形的高度,可以是负高度");
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



	// 以下是改成定义文件后的修改
	//  mPaintInOneGrid = false;
	// 保存当前选中的纹理的大小
	int selectedTexXSize = 0;
	int selectedTexZSize = 0;

	// 当画刷被选中时，要进行判断
	HTREEITEM itemId = mBrushesTree.GetSelectedItem();
	if ( itemId == NULL )
		return;

	HTREEITEM rootId = mBrushesTree.GetRootItem();

	// 如果选中根，就返回
	if ( itemId == rootId )
	{
		// 选中根目录名时，所有选项都变灰
		mFlipHorizontal.EnableWindow(FALSE);
		mFlipVertical.EnableWindow(FALSE);
		mRotateRightAngle.EnableWindow(FALSE);
		mMirrorDiagonal.EnableWindow(FALSE);
		mRandom.EnableWindow(FALSE);
		mEnablePreview.EnableWindow(FALSE);

		// 缩略图用黑白图
		mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);

		return;
	}

	{
		mCurrentOrientationType = 0;

		// 如果选项的父是root，说明该画刷是放在根目录下的，也有可能是根目录下的子目录
		if ( mBrushesTree.GetParentItem(itemId) == rootId )
		{
			mCurrentFileName = mBrushesTree.GetItemText(itemId);
		}
		else
		{
			// 以下处理不是放在根目录下的画刷

			HTREEITEM parentId = mBrushesTree.GetParentItem(itemId);
			mCurrentFileName = mBrushesTree.GetItemText(itemId);

			// 以此取出父目录的目录名，组合成相对路径名，并加上文件名
			while ( parentId != rootId )
			{
				std::string parentText = mBrushesTree.GetItemText(parentId);
				parentText += '/';
				mCurrentFileName = parentText + mCurrentFileName;

				parentId = mBrushesTree.GetParentItem(parentId);
			}		
		}	

		// 如果名字中有.，说明是文件名，不是目录名
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

			// 取出brush名称
			Ogre::String brushName = currentFileName.substr(0,pos);
			Ogre::String textureName = currentFileName.substr(pos+1);

			const TextureInfos &currentPaintInfos = 
				RootManager::getSingleton().getTextureInfos(brushName);

			const TextureInfo &currentPaintInfo = RootManager::getSingleton().getTextureInfo(brushName, textureName);

			Ogre::Image *previewImage = RootManager::getSingleton().getPreviewImage(currentPaintInfo.ownerTextureName);


			// 组成纹理信息字符串
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
				// 如果不显示缩略图，就显示默认的黑白图
				mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
			}
		}
		else // 如果名字中有.，说明不是文件名，是目录名
		{
			mFlipHorizontal.EnableWindow(FALSE);
			mFlipVertical.EnableWindow(FALSE);
			mRotateRightAngle.EnableWindow(FALSE);
			mMirrorDiagonal.EnableWindow(FALSE);
			mRandom.EnableWindow(FALSE);
			mEnablePreview.EnableWindow(FALSE);

			// 把整个目录的纹理进行判断，并加入到透明纹理列表

			const Action* action = OnStartAutoTexAction(itemId, mCurrentFileName);

			if ( action != 0 )
			{
				// 从该组纹理的实心纹理中随机用一个来做为缩略图
				// 读取这张纹理，并缩放到mPreviewImageWidth*mPreviewImageHeight，用于缩略图
				//  Ogre::String tempTexName = action->getParameter( "%TextureName" );

				const TextureInfos &currentPaintInfos = 
					RootManager::getSingleton().getTextureInfos(mCurrentFileName.c_str());

				//  const WX::TextureInfo &currentPaintInfo = currentPaintInfos[0];

				// 构建preview image
				buildPreviewBitmap(currentPaintInfos[0]);                
			}
			else
			{
				mEnablePreview.EnableWindow(FALSE);
				mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
			}

			if ( mEnablePreview.GetCheck() != BST_CHECKED )
			{
				// 如果不显示缩略图，就显示默认的黑白图
				mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
			}
		}
	}


	// 让所有的checkbox回复未选择的状态
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
		// 禁止其他按钮
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

		// 以下是checkbox状态的判断
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


	//预览
	if ( mEnablePreview.GetCheck() == BST_CHECKED )
	{
		// 选中按钮时，缩略图也要相应变化
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
				BmpRotate90(mCurrentPreviewImage);	//这里有差别
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
		// 如果不显示缩略图，就显示默认的黑白图
		mBrushesPreview.SetBitmap(mBlackWhitePreviewImage);
	}

	// 预览图的开关
	if ( cid == IDC_BS_ENABLE_PIXMAP_PREVIEW )
	{
		if ( mEnablePreview.GetCheck() == BST_CHECKED )
		{
			// 如果用户打开开关，就用前面OnPixmapListSelected中计算好的预览图来显示
			// 这也是为什么前面OnPixmapListSelected中无论这个开关是否打开都要计算预览图的原因
			// 不然的话，这里打开开关后显示的是上次计算的预览图，而不是当前所选纹理的预览图
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
		//取消操作 
		RootManager::getSingleton().clearActiveAction();
	}
	else if ( sel == 1 )
	{
		//升高地形
		RootManager::getSingleton().setActiveAction("RaiseHeightAction");
		mAffectRangeCtl.ShowWindow(SW_SHOW);
		mExpValueCtl.ShowWindow(SW_SHOW);
	}
	else if ( sel == 2 )
	{
		//降低地形
		RootManager::getSingleton().setActiveAction("LowerHeightAction");
		mAffectRangeCtl.ShowWindow(SW_SHOW);
		mExpValueCtl.ShowWindow(SW_SHOW);
	}
	else if ( sel == 3 )
	{
		//平滑地形高度
		RootManager::getSingleton().setActiveAction("SmoothHeightAction");
	}
	else if ( sel == 4 )
	{
		//平整地形高度
		RootManager::getSingleton().setActiveAction("LevelOffHeightAction");
	}
	else if ( sel == 5 )
	{
		//设置地形高度
		RootManager::getSingleton().setActiveAction("SetTerrainHeightAction");
		mTerrainValueCtl.ShowWindow(SW_SHOW);
		OnEnChangeBsTerrainValue();
	}
	else if ( sel == 6 )
	{
		//删除透明层贴图
		RootManager::getSingleton().setActiveAction("TerrainLayerOneEraserAction");
	}
	else if ( sel == 7 )
	{
		//反转对角线
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
