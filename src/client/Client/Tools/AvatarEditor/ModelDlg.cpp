// ModelDlg.cpp : implementation file
//

#include "AvatarEditorPCH.h"
#include "MyApp.h"
#include "ModelDlg.h"
#include "GraphicsTable.h"
#include "AvatarTable.h"
#include "EQMain.h"

#include "MyMsgBox.h"

#include "MainFrm.h"
#include "HouseTable.h"
#include "HouseSceneComponent.h"
#include "HSCManager.h"
#include "NodeAdjuster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CModelDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CModelDlg::OnTvnSelchangedTree1)
	//ON_NOTIFY(NM_SETFOCUS, IDC_TREE1, &CModelDlg::OnNMSetfocusMoTree)
	ON_EN_CHANGE(IDC_EDIT1, &CModelDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()

static std::string SimpleXmlNode( const std::string & name, const std::string & val )
{
	std::string s;
	s += "<" + name + ">";
	s += val;
	s += "</" + name + ">";

	return s;
}

static std::string GridArrayToString( const HouseCompPropRow::GridPosArray & v )
{
	std::string s;

	for ( HouseCompPropRow::GridPosArray::const_iterator it = v.begin();
		it != v.end();
		++it )
	{
		s += "(" + Ogre::StringConverter::toString( it->x ) + "," + Ogre::StringConverter::toString( it->y ) + ")" + " ";
	}

	return s;
}

static HouseCompPropRow::GridPosArray StringToGridArray( const std::string & s )
{
	HouseCompPropRow::GridPosArray v;

	std::vector<std::string> strs;
	strs = Ogre::StringUtil::split( s, "(), " );
	for ( uint i = 0; i + 1 < strs.size(); i += 2 )
	{
		int x = Ogre::StringConverter::parseInt( strs[i] );

		int y = Ogre::StringConverter::parseInt( strs[i+1] );

		v.push_back( HouseCompPropRow::GridPos( x, y ) );
	}
	return v;
}


static std::string Vector3ToString( const Ogre::Vector3 & v )
{
	return FormatStr( "%.2f, %.2f, %.2f", v.x, v.y, v.z );
}

static Ogre::Vector3 StringToVector3( const std::string & s, Ogre::Vector3 v3 )
{
	std::vector<std::string> strs;
	strs = Ogre::StringUtil::split( s, " ," );
	if ( strs.size() == 3 )
	{
		v3.x = Ogre::StringConverter::parseReal(strs[0]);
		v3.y = Ogre::StringConverter::parseReal(strs[1]);
		v3.z = Ogre::StringConverter::parseReal(strs[2]);
	}

	return v3;
}

static std::string QuaternionToString( const Ogre::Quaternion & q )
{
	return FormatStr( "%.2f, %.2f, %.2f, %.2f", q.x, q.y, q.z, q.w );
}

static Ogre::Quaternion StringToQuaternion( const std::string & s )
{
	Ogre::Quaternion q(Ogre::Quaternion::IDENTITY);
	std::vector<std::string> strs;
	strs = Ogre::StringUtil::split( s, " ," );
	if ( strs.size() == 4 )
	{
		q.x = Ogre::StringConverter::parseReal(strs[0]);
		q.y = Ogre::StringConverter::parseReal(strs[1]);
		q.z = Ogre::StringConverter::parseReal(strs[2]);
		q.w = Ogre::StringConverter::parseReal(strs[3]);
	}

	return q;
}


CModelDlg::CModelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelDlg::IDD, pParent)
	, mShowHoldGrid( true )
	, mShowStopGrid( true )
	, mShowLayGrid( true )
	, mShowModel( true )
	, mShowOffsetHeight( false )
	//, mShowXYZ( true )
{
	mLightType = Ogre::Light::LT_SPOTLIGHT;
	mLightDiffuseColour = Ogre::ColourValue(1, 1, 1);
	mLightSpecularColour = Ogre::ColourValue(1, 1, 1);
	mLightAttenuation = Ogre::Vector4(200, 1, 0, 0);
	mLightSpotlightRange = Ogre::Vector3(30, 90, 1);
	mLightPos = Ogre::Vector3(0, 100, 0);
	mLightDirection = Ogre::Vector3(0, -1, 0);
	mDrawHelper = true;
	mLight = NULL;
	mLightBbs = NULL;
	mLightBb = NULL;
	mManualobject = NULL;
	mOpenConsult = false;

	mPassLight = NULL;
	mPassBbs = NULL;
	mPassBb = NULL;
	mPassManualobject = NULL;
	mPassAttenuation = Ogre::Vector4(50, 1, 0, 0);
	mPassPos = Ogre::Vector3(0, 50, 0);
	mPassVirtualCol = Ogre::ColourValue(1, 1, 1);
	mPassVirtualAtten = 0;
}

CModelDlg::~CModelDlg()
{
	
}

void CModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndModelTree);	
	DDX_Control(pDX, IDC_EDIT1, m_input);
}



void CModelDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);	

	if ( m_wndModelTree.m_hWnd )
	{
		CRect rc;
		GetClientRect( rc );
		// 预留空位，供摆放按钮
		rc.top += 30;
		m_wndModelTree.MoveWindow( rc );
	}
}

void CModelDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	uint id =  pNMTreeView->itemNew.lParam;




	switch ( static_cast<CMainFrame *>(::AfxGetMainWnd())->getPage() )
	{
	case 2:
		{
			const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( id );
			if ( r )
			{
				mModelDisplayRow = *r;
				updateModelData();
			}
		}
		break;
	case 3:
		GraphicsSystem::getSingleton().displayItem( id );
		break;
	case 4:
		{
			if ( id > 0 )
			{
				GraphicsSystem::getSingleton().getNodeAdjuster()->detach();
				const HouseCompRow * compRow = HouseTableManager::getSingleton().getHouseCompRow( id );
				if ( compRow )
				{					
					const HouseCompPropRow * propRow = HouseTableManager::getSingleton().getHouseCompPropRow( compRow->PropID );
					if ( propRow )
					{
						mHouseCompRow = *compRow;
						mHouseCompPropRow = *propRow;
						updateHouseData();
					}
				}				
			}
		}
		break;
	default:
		break;
	}
}

void CModelDlg::clearProperty()
{
	CPropertyGrid & prop = static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndPropertyDlg;
	// 清空旧属性
	for ( PropSectionMap::const_iterator it = mPropSections.begin();
		it != mPropSections.end();
		++it )
	{
		prop.RemoveSection( it->first );

		for ( PropItemMap::const_iterator p = it->second.begin();
			p != it->second.end();
			++p )
		{
			prop.RemoveItem( p->second );
		}
	}

	mPropSections.clear();	
}

void CModelDlg::showModelProperty( const ModelDisplayRow * r )
{
	// 显示属性数值
	if ( r )
	{		
		CPropertyGrid & prop = static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndPropertyDlg;
		HITEM hSec = mPropSections.begin()->first;
		prop.SetItemValue( mPropSections[hSec]["ID"], static_cast<int>(r->id) );
		prop.SetItemValue( mPropSections[hSec]["Mesh"], r->mesh );
		prop.SetItemValue( mPropSections[hSec]["Mat"], r->mat );
		prop.SetItemValue( mPropSections[hSec]["ShowMark"], static_cast<int>(r->showMark) );
		prop.SetItemValue( mPropSections[hSec]["ShowPri"], static_cast<int>(r->showPri) );
	}
}

void CModelDlg::OnNMSetfocusMoTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	static_cast<CMainFrame *>( ::AfxGetMainWnd() )->m_wnd3DView.SetFocus();
}

BOOL CModelDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// 处理消息: 属性改变
	if ( WM_PG_ITEMCHANGED == message ) 
	{
		GraphicsSystem::getSingleton().getNodeAdjuster()->detach();

		CPropertyGrid & prop = static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndPropertyDlg;

		// 处理材质更换
		if ( !mPropSections.empty() )
		{
			switch ( static_cast<CMainFrame *>(::AfxGetMainWnd())->getPage() )
			{
			case 2: // 模型表现
				{
					std::string strValue;

					const HITEM hItem = wParam;
					PropSectionMap::iterator it = mPropSections.begin();
					
					if ( hItem == it->second["Mat"] )
					{
						prop.GetItemValue( hItem, strValue );
						mModelDisplayRow.mat = strValue;
					}
					else if ( hItem == it->second["Mesh"] )
					{
						prop.GetItemValue( hItem, strValue );
						mModelDisplayRow.mat = strValue;
					}

					updateModelData();
				}
				break;
			case 4: // 房屋部件
				{
					std::string strValue;
					const HITEM hItem = wParam;
					if ( g_HouseComp )
					{
						//--- 部件类属性 ---//
						PropSectionMap::iterator it = mPropSections.begin();
						if ( hItem == it->second["HoldGrid"] )
						{
							prop.GetItemValue( hItem, strValue );
							HouseCompPropRow::GridPosArray gridArr = StringToGridArray( strValue );		
							
							if ( gridArr.empty() )
							{
								MessageBox( "HoldGrid不能为空", 0, MB_ICONERROR );
							}
							else
							{
								mHouseCompPropRow.HoldGrid = gridArr;
							}
						}
						else if ( hItem == it->second["StopGrid"] )
						{
							prop.GetItemValue( hItem, strValue );
							mHouseCompPropRow.StopGrid = StringToGridArray( strValue );
						}
						else if ( hItem == it->second["LayGrid"] )
						{
							prop.GetItemValue( hItem, strValue );
							mHouseCompPropRow.LayGrid = StringToGridArray( strValue );
						}
						else if ( hItem == it->second["SelfHeight"] )
						{
							int intValue = 0;
							prop.GetItemValue( hItem, intValue );
							mHouseCompPropRow.SelfHeight = intValue;
						}
						else if ( hItem == it->second["OffsetHeight"] )
						{
							int intValue = 0;
							prop.GetItemValue( hItem, intValue );
							mHouseCompPropRow.OffsetHeight = intValue;
						}

						//--- 部件属性 ---//
						++it;
						if ( hItem == it->second["DisID"] )
						{								
							int intValue = 0;
							prop.GetItemValue( hItem, intValue );
							if ( !GraphicsTableManager::getSingleton().getModelDisplayRow( intValue ) )
							{
								MessageBox( "DisID 填写错误", "属性错误", MB_ICONERROR );
								break;
							}
							mHouseCompRow.DisID = intValue;
						}
						else if ( hItem == it->second["Pos"] )
						{
							prop.GetItemValue( hItem, strValue );
							mHouseCompRow.Pos = StringToVector3( strValue, Ogre::Vector3::ZERO );
						}
						else if ( hItem == it->second["Rot"] )
						{
							prop.GetItemValue( hItem, strValue );
							mHouseCompRow.Rot = StringToQuaternion( strValue );
						}
						else if ( hItem == it->second["Scale"] )
						{
							prop.GetItemValue( hItem, strValue );
							mHouseCompRow.Scale = StringToVector3( strValue, Ogre::Vector3(1, 1, 1) );
						}

						//--- 其它属性 ---//
						++it;
						bool bVal = false;
						if ( hItem == it->second["ShowHoldGrid"] )
						{
							prop.GetItemValue( hItem, bVal );
							mShowHoldGrid = bVal;
						}
						else if ( hItem == it->second["ShowStopGrid"] )
						{
							prop.GetItemValue( hItem, bVal );
							mShowStopGrid = bVal;
						}
						else if ( hItem == it->second["ShowLayGrid"] )
						{
							prop.GetItemValue( hItem, bVal );
							mShowLayGrid = bVal;
						}
						else if ( hItem == it->second["ShowModel"] )
						{
							prop.GetItemValue( hItem, bVal );
							mShowModel = bVal;
						}
						//else if ( hItem == it->second["ShowOffsetHeight"] )
						//{
						//	prop.GetItemValue( hItem, bVal );
						//	mShowOffsetHeight = bVal;
						//}
						//else if ( hItem == it->second["ShowXYZ"] )
						//{
						//	prop.GetItemValue( hItem, bVal );
						//	mShowXYZ = bVal;
						//}

						//--- 主动光属性 ---//
						++it;
						int nValue;
						bool bValue;
						COLORREF colValue;
						std::string strValue;
						if( hItem == it->second["LightType"] )
						{
							prop.GetItemValue( hItem, nValue );
							if(nValue == 0)
								mLightType = Ogre::Light::LT_POINT;
							else
								mLightType = Ogre::Light::LT_SPOTLIGHT;
						}
						else if( hItem == it->second["DiffCol"] )
						{
							prop.GetItemValue( hItem, colValue );
							mLightDiffuseColour = Ogre::ColourValue(colValue%0x100/255.0f, colValue%0x10000/0x100/255.0f, colValue/0x10000/255.0f);
						}
						else if( hItem == it->second["SpecCol"] )
						{
							prop.GetItemValue( hItem, colValue );
							mLightSpecularColour = Ogre::ColourValue(colValue%0x100/255.0f, colValue%0x10000/0x100/255.0f, colValue/0x10000/255.0f);
						}
						else if( hItem == it->second["Attenuation"] )
						{
							prop.GetItemValue( hItem, strValue );
							std::vector<std::string> strs;
							strs = Ogre::StringUtil::split( strValue, " ," );
							if ( strs.size() == 4 )
							{
								mLightAttenuation.x = Ogre::StringConverter::parseReal(strs[0]);
								mLightAttenuation.y = Ogre::StringConverter::parseReal(strs[1]);
								mLightAttenuation.z = Ogre::StringConverter::parseReal(strs[2]);
								mLightAttenuation.w = Ogre::StringConverter::parseReal(strs[3]);
							}
						}
						else if( hItem == it->second["Pos"] )
						{
							prop.GetItemValue( hItem, strValue );
							std::vector<std::string> strs;
							strs = Ogre::StringUtil::split( strValue, " ," );
							if ( strs.size() == 3 )
							{
								mLightPos.x = Ogre::StringConverter::parseReal(strs[0]);
								mLightPos.y = Ogre::StringConverter::parseReal(strs[1]);
								mLightPos.z = Ogre::StringConverter::parseReal(strs[2]);
							}
						}
						else if( hItem == it->second["Dir"] )
						{
							prop.GetItemValue( hItem, strValue );
							std::vector<std::string> strs;
							strs = Ogre::StringUtil::split( strValue, " ," );
							if ( strs.size() == 3 )
							{
								mLightDirection.x = Ogre::StringConverter::parseReal(strs[0]);
								mLightDirection.y = Ogre::StringConverter::parseReal(strs[1]);
								mLightDirection.z = Ogre::StringConverter::parseReal(strs[2]);
							}
						}
						else if( hItem == it->second["SpotRange"] )
						{
							prop.GetItemValue( hItem, strValue );
							std::vector<std::string> strs;
							strs = Ogre::StringUtil::split( strValue, " ," );
							if ( strs.size() == 3 )
							{
								mLightSpotlightRange.x = Ogre::StringConverter::parseReal(strs[0]);
								mLightSpotlightRange.y = Ogre::StringConverter::parseReal(strs[1]);
								mLightSpotlightRange.z = Ogre::StringConverter::parseReal(strs[2]);
							}
						}

						//--- 穿透光属性 ---//
						++it;
						double dValue;
						if( hItem == it->second["Attenuation"] )
						{
							prop.GetItemValue( hItem, strValue );
							std::vector<std::string> strs;
							strs = Ogre::StringUtil::split( strValue, " ," );
							if ( strs.size() == 4 )
							{
								mPassAttenuation.x = Ogre::StringConverter::parseReal(strs[0]);
								mPassAttenuation.y = Ogre::StringConverter::parseReal(strs[1]);
								mPassAttenuation.z = Ogre::StringConverter::parseReal(strs[2]);
								mPassAttenuation.w = Ogre::StringConverter::parseReal(strs[3]);
							}
						}
						else if( hItem == it->second["Pos"] )
						{
							prop.GetItemValue( hItem, strValue );
							std::vector<std::string> strs;
							strs = Ogre::StringUtil::split( strValue, " ," );
							if ( strs.size() == 3 )
							{
								mPassPos.x = Ogre::StringConverter::parseReal(strs[0]);
								mPassPos.y = Ogre::StringConverter::parseReal(strs[1]);
								mPassPos.z = Ogre::StringConverter::parseReal(strs[2]);
							}
						}
						else if( hItem == it->second["VirtualCol"])
						{
							prop.GetItemValue( hItem, colValue );
							mPassVirtualCol = Ogre::ColourValue(colValue%0x100/255.0f, colValue%0x10000/0x100/255.0f, colValue/0x10000/255.0f);
						}
						else if( hItem == it->second["VirtualAtten"])
						{
							prop.GetItemValue( hItem, dValue );
							if(dValue > 1)
							{
								dValue = 1;
								prop.SetItemValue( hItem, dValue);
							}
							else if(dValue < 0)
							{
								dValue = 0;
								prop.SetItemValue( hItem, dValue);
							}

							mPassVirtualAtten = dValue;
						}

						//--- 光其他属性 ---//
						++it;
						if( hItem == it->second["Ambient"] )
						{
							prop.GetItemValue( hItem, colValue );
							GraphicsSystem::getSingleton().getSceneMgr()->setAmbientLight(
								Ogre::ColourValue(colValue%0x100/255.0f, colValue%0x10000/0x100/255.0f, colValue/0x10000/255.0f));
							GraphicsSystem::getSingleton().getMainLight()->setDiffuseColour(
								Ogre::ColourValue(colValue%0x100/255.0f, colValue%0x10000/0x100/255.0f, colValue/0x10000/255.0f));
						}
						else if( hItem == it->second["DrawHelper"] )
						{
							prop.GetItemValue( hItem, bValue );
							mDrawHelper = bValue;
						}
						else if( hItem == it->second["OpenConsult"] )
						{
							prop.GetItemValue( hItem, bValue );
							if(!bValue)
								clearConsultation();
						}
						else if( hItem == it->second["Consultations"] )
						{
							prop.GetItemValue( hItem, strValue );
							if(strValue != "无")
								addConsultation(atoi(strValue.c_str()));
						}
					}
					updateHouseData();
				}
				break;
			default:
				break;
			}
		}

	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CModelDlg::updateModelData()
{
	std::vector<const ModelDisplayRow *> rows;
	if ( !mModelDisplayRow.mesh.empty() )
	{
		rows.push_back( &mModelDisplayRow );
	}

	GraphicsSystem::getSingleton().displayItem( rows );

	showModelProperty( &mModelDisplayRow );
}

void CModelDlg::showXml()
{
	CPropertyGrid & prop = static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndPropertyDlg;

	switch ( static_cast<CMainFrame *>(::AfxGetMainWnd())->getPage() )
	{
	case 2: // 模型表现
		{
			::HTREEITEM hSelect = this->m_wndModelTree.GetSelectedItem();
			if ( !hSelect )
				return;

			uint id = m_wndModelTree.GetItemData( hSelect );

			std::string msg;
						
			if ( mPropSections.size() > 0 )
			{
				PropItemMap & myMap = mPropSections.begin()->second;
				int iVal = 0;
				std::string s;

				msg += "\n<!--模型表现表(ModelDisplay.xml)-->\n\t\t";

				prop.GetItemValue( myMap["ID"], iVal );
				msg += SimpleXmlNode( "ID", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

				prop.GetItemValue( myMap["Mesh"], s );
				msg += SimpleXmlNode( "Mesh", s ) + "\n\t\t";

				prop.GetItemValue( myMap["Mat"], s );
				msg += SimpleXmlNode( "Mat", s ) + "\n\t\t";

				prop.GetItemValue( myMap["ShowMark"], iVal );
				msg += SimpleXmlNode( "ShowMark", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

				prop.GetItemValue( myMap["ShowPri"], iVal );
				msg += SimpleXmlNode( "ShowPri", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";
			}

			CMyMsgBox::Show( msg );
		}
		break;
	case 4: // 房屋部件
		{
			std::string msg;
			
			if ( mPropSections.size() > 0 )
			{
				PropSectionMap::iterator itBegin = mPropSections.begin();
				PropItemMap & myMap = itBegin->second;
				std::string s;
				int iVal = 0;
			
				msg += "\n<!--房屋部件属性表(HouseCompProp.xml)-->\n\t\t";

				prop.GetItemValue( myMap["PropID"], iVal );
				msg += SimpleXmlNode( "PropID", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";
				const int propID = iVal;

				prop.GetItemValue( myMap["PropName"], s );
				msg += SimpleXmlNode( "Name", s ) + "\n\t\t";

				prop.GetItemValue( myMap["BeWatchMode"], iVal );
				msg += SimpleXmlNode( "BeWatchMode", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

				prop.GetItemValue( myMap["HoldGrid"], s );
				msg += SimpleXmlNode( "HoldGrid", s ) + "\n\t\t";

				prop.GetItemValue( myMap["StopGrid"], s );
				msg += SimpleXmlNode( "StopGrid", s ) + "\n\t\t";

				prop.GetItemValue( myMap["LayGrid"], s );
				msg += SimpleXmlNode( "LayGrid", s ) + "\n\t\t";

				prop.GetItemValue( myMap["SelfHeight"], iVal );
				msg += SimpleXmlNode( "SelfHeight", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

				prop.GetItemValue( myMap["OffsetHeight"], iVal );
				msg += SimpleXmlNode( "OffsetHeight", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";				

				prop.GetItemValue( myMap["AcceptLay"], s );
				msg += SimpleXmlNode( "AcceptLay", s ) + "\n\t\t";				

				prop.GetItemValue( myMap["LayType"], s );
				msg += SimpleXmlNode( "LayType", s ) + "\n\t\t";				

				bool bVal = false;
				prop.GetItemValue( myMap["CanRep"], bVal );
				s = bVal ? "1" : "0";
				msg += SimpleXmlNode( "CanRep", s ) + "\n\t\t";				

				prop.GetItemValue( myMap["HoldType"], iVal );
				msg += SimpleXmlNode( "HoldType", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";				

				++itBegin;
				PropItemMap & secMap = itBegin->second;
				msg += "  \n<!--房屋部件表(house_comp.xml)-->\n\t\t";
				prop.GetItemValue( secMap["ID"], iVal );
				msg += SimpleXmlNode( "id", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

				prop.GetItemValue( secMap["Name"], s );
				msg += SimpleXmlNode( "name", s ) + "\n\t\t";

				msg += SimpleXmlNode( "prop_id", Ogre::StringConverter::toString( propID ) ) + "\n\t\t";

				prop.GetItemValue( secMap["DisID"], iVal );
				msg += SimpleXmlNode( "dis_id", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

				prop.GetItemValue( secMap["Pos"], s );
				msg += SimpleXmlNode( "pos", s ) + "\n\t\t";

				prop.GetItemValue( secMap["Rot"], s );
				msg += SimpleXmlNode( "rot", s ) + "\n\t\t";
				
				prop.GetItemValue( secMap["Scale"], s );
				msg += SimpleXmlNode( "scale", s ) + "\n\t\t";		

				++itBegin;

				++itBegin;
				if(mHouseCompRow.ID == COMPONENT_LIGHT)
				{
					PropItemMap & lightMap = itBegin->second;
					msg += "  \n<!--主动灯光表(ActiveLight.xml)-->\n\t\t";

					prop.GetItemValue( lightMap["ID"], iVal );
					msg += SimpleXmlNode( "ID", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

					prop.GetItemValue( lightMap["Name"], s );
					msg += SimpleXmlNode( "Name", s ) + "\n\t\t";

					prop.GetItemValue( lightMap["LightType"], iVal );
					msg += SimpleXmlNode( "LightType", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t"; 

					COLORREF colValue;
					prop.GetItemValue( lightMap["DiffCol"], colValue );
					char temp[16];
					sprintf(temp, "%X%X%X", colValue%0x100, colValue%0x10000/0x100, colValue/0x10000);
					msg += SimpleXmlNode( "DiffCol", temp ) + "\n\t\t"; 

					prop.GetItemValue( lightMap["SpecCol"], colValue );
					sprintf(temp, "%X%X%X", colValue%0x100, colValue%0x10000/0x100, colValue/0x10000);
					msg += SimpleXmlNode( "SpecCol", temp) + "\n\t\t"; 

					prop.GetItemValue( lightMap["Attenuation"], s );
					msg += SimpleXmlNode( "Attenuation", s ) + "\n\t\t";

					prop.GetItemValue( lightMap["Pos"], s );
					msg += SimpleXmlNode( "Pos", s ) + "\n\t\t";

					if(mLight->getType() == Ogre::Light::LT_SPOTLIGHT)
					{
						prop.GetItemValue( lightMap["Dir"], s );
						msg += SimpleXmlNode( "Dir", s ) + "\n\t\t";

						prop.GetItemValue( lightMap["SpotRange"], s );
						msg += SimpleXmlNode( "SpotRange", s ) + "\n\t\t";
					}
					else if(mLight->getType() == Ogre::Light::LT_POINT)
					{
						prop.GetItemValue( lightMap["Pierce"], iVal );
						msg += SimpleXmlNode( "Pierce", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t"; 

						prop.GetItemValue( lightMap["PierceRange"], iVal );
						msg += SimpleXmlNode( "PierceRange", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";
					}
				}
				else if(mHouseCompRow.ID == COMPONENT_PASS)
				{
					++itBegin;
					PropItemMap & pierceMap = itBegin->second;
					msg += "  \n<!--穿透灯光表(PassLight.xml)-->\n\t\t";

					prop.GetItemValue( pierceMap["ID"], iVal );
					msg += SimpleXmlNode( "ID", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";

					prop.GetItemValue( pierceMap["Name"], s );
					msg += SimpleXmlNode( "Name", s ) + "\n\t\t";

					prop.GetItemValue( pierceMap["Indoor"], iVal );
					msg += SimpleXmlNode( "Indoor", Ogre::StringConverter::toString( iVal ) ) + "\n\t\t";
	
					prop.GetItemValue( pierceMap["Attenuation"], s );
					msg += SimpleXmlNode( "Attenuation", s ) + "\n\t\t";

					prop.GetItemValue( pierceMap["Pos"], s );
					msg += SimpleXmlNode( "Pos", s ) + "\n\t\t";
				}
			}

			CMyMsgBox::Show( msg );
		}
		break;
	default:
		break;
	}

}

void CModelDlg::loadPage( int iPage )
{
	switch ( iPage )
	{
	case 2:
	case 3:
		{
			m_wndModelTree.DeleteAllItems();
			// 模型列表 
			::HTREEITEM hModel = m_wndModelTree.InsertItem( "模型表现" );

			for ( ModelDisplayTable::const_iterator it = GraphicsTableManager::getSingleton().getModelDisplayTable().begin();
				it != GraphicsTableManager::getSingleton().getModelDisplayTable().end();
				++it )
			{			
				HTREEITEM h = m_wndModelTree.InsertItem( Ogre::StringConverter::toString( it->second.id ).c_str(), hModel );
				// 记录模型表现id
				m_wndModelTree.SetItemData( h, it->second.id );
			}

			m_wndModelTree.SortChildren( hModel );

			m_wndModelTree.Expand( hModel, TVE_EXPAND );		

			// --- 属性 --- //
			CPropertyGrid & prop = static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndPropertyDlg;
	
			clearProperty();

			// 重新加节点, 加属性
			::HSECTION hSec = prop.AddSection( "模型表现" );
			
			mPropSections[hSec]["ID"] = prop.AddIntegerItem( hSec, "ID", 0, "", false );
			mPropSections[hSec]["Mesh"] = prop.AddStringItem( hSec, "Mesh", "", true );
			mPropSections[hSec]["Mat"] = prop.AddStringItem( hSec, "Mat", "", true );
			mPropSections[hSec]["ShowMark"] = prop.AddIntegerItem( hSec, "ShowMark", 0, "", false );
			mPropSections[hSec]["ShowPri"] = prop.AddIntegerItem( hSec, "ShowPri", 0, "", false );	

			updateModelData();
		}
		break;
	case 4:
		{
			m_wndModelTree.DeleteAllItems();
			// 模型列表 
			::HTREEITEM hModel = m_wndModelTree.InsertItem( "房屋部件" );

			
			for ( HouseCompTable::const_iterator it = HouseTableManager::getSingleton().getHouseCompTable().begin();
				it != HouseTableManager::getSingleton().getHouseCompTable().end();
				++it )
			{			
				HTREEITEM h = m_wndModelTree.InsertItem( Ogre::StringConverter::toString( it->second.ID ).c_str(), hModel );
				// 记录房屋部件id
				m_wndModelTree.SetItemData( h, it->second.ID );
			}


			m_wndModelTree.SortChildren( hModel );

			m_wndModelTree.Expand( hModel, TVE_EXPAND );	

			//--- 属性相关 ---//
			CPropertyGrid & prop = static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndPropertyDlg;
	
			clearProperty();			

			// 重新加节点, 加属性
			::HSECTION hSec = prop.AddSection( "部件类属性" );

			mPropSections[hSec]["PropID"]		= prop.AddIntegerItem( hSec, "PropID", 0, "", false );
			mPropSections[hSec]["PropName"]		= prop.AddStringItem( hSec, "PropName", "", false );
			mPropSections[hSec]["BeWatchMode"]	= prop.AddIntegerItem( hSec, "BeWatchMode", 0, "", false );
			mPropSections[hSec]["HoldGrid"]		= prop.AddStringItem( hSec, "HoldGrid", "", true );
			mPropSections[hSec]["StopGrid"]		= prop.AddStringItem( hSec, "StopGrid", "", true );
			mPropSections[hSec]["LayGrid"]		= prop.AddStringItem( hSec, "LayGrid", "", true );
			mPropSections[hSec]["SelfHeight"]	= prop.AddIntegerItem( hSec, "SelfHeight", 0, "", true );
			mPropSections[hSec]["OffsetHeight"] = prop.AddIntegerItem( hSec, "OffsetHeight", 0, "", true );	
			mPropSections[hSec]["AcceptLay"]	= prop.AddStringItem( hSec, "AcceptLay", "", false );	
			mPropSections[hSec]["LayType"]		= prop.AddStringItem( hSec, "LayType", "", false );	
			mPropSections[hSec]["CanRep"]		= prop.AddBoolItem( hSec, "CanRep", false, false );	
			mPropSections[hSec]["HoldType"]		= prop.AddIntegerItem( hSec, "HoldType", 0, "", false );	

			::HSECTION hComp = prop.AddSection( "部件属性", false );

			mPropSections[hComp]["ID"]			= prop.AddIntegerItem( hComp, "ID", 0, "", false );
			mPropSections[hComp]["Name"]		= prop.AddStringItem( hComp, "Name", "", false );
			mPropSections[hComp]["DisID"]		= prop.AddIntegerItem( hComp, "DisID", 0, "", true );
			mPropSections[hComp]["Pos"]			= prop.AddStringItem( hComp, "Pos", "", true );
			mPropSections[hComp]["Rot"]			= prop.AddStringItem( hComp, "Rot", "", true );
			mPropSections[hComp]["Scale"]		= prop.AddStringItem( hComp, "Scale", "", true );

			::HSECTION hOther = prop.AddSection( "其它属性" );

			mPropSections[hOther]["ShowHoldGrid"]	= prop.AddBoolItem( hOther, "ShowHoldGrid", true, true );
			mPropSections[hOther]["ShowStopGrid"]	= prop.AddBoolItem( hOther, "ShowStopGrid", true, true );
			mPropSections[hOther]["ShowLayGrid"]	= prop.AddBoolItem( hOther, "ShowLayGrid", true, true );
			mPropSections[hOther]["ShowModel"]		= prop.AddBoolItem( hOther, "ShowModel", true, true );
			//mPropSections[hOther]["ShowOffsetHeight"]	= prop.AddBoolItem( hOther, "ShowOffsetHeight", true, true );
			//mPropSections[hOther]["ShowXYZ"]	= prop.AddBoolItem( hOther, "ShowXYZ", true, true );
			

			::HSECTION hLight = prop.AddSection( "主动光属性" );

			mPropSections[hLight]["ID"]				= prop.AddIntegerItem( hLight, "ID", 0, "", true );
			mPropSections[hLight]["Name"]			= prop.AddStringItem( hLight, "Name", "", true );
			vector<Ogre::String> vstr;
			vstr.push_back("POINT");
			vstr.push_back("SPOTLIGHT");
			mPropSections[hLight]["LightType"]		= prop.AddComboItem( hLight, "LightType", vstr, 1, true );
			mPropSections[hLight]["DiffCol"]		= prop.AddColorItem( hLight, "DiffCol", COLORREF(0x00FFFFFF), true );
			mPropSections[hLight]["SpecCol"]		= prop.AddColorItem( hLight, "SpecCol", COLORREF(0x00FFFFFF), true );
			mPropSections[hLight]["Attenuation"]	= prop.AddStringItem( hLight, "Attenuation", "100 1 0 0", true );
			mPropSections[hLight]["Pos"]			= prop.AddStringItem( hLight, "Pos", "", true );
			mPropSections[hLight]["Dir"]			= prop.AddStringItem( hLight, "Dir", "0 -1 0", true );
			mPropSections[hLight]["SpotRange"]		= prop.AddStringItem( hLight, "SpotRange", "30 90 1", true );
			vstr.clear();
			vstr.push_back("无穿透");
			vstr.push_back("穿透室外");
			vstr.push_back("穿透室内");
			mPropSections[hLight]["Pierce"]			= prop.AddComboItem( hLight, "Pierce", vstr, 0, true );
			mPropSections[hLight]["PierceRange"]	= prop.AddIntegerItem( hLight, "PierceRange", 0, "", true );


			::HSECTION hPierce = prop.AddSection( "穿透光属性" );

			mPropSections[hPierce]["ID"]			= prop.AddIntegerItem( hPierce, "ID", 0, "", true );
			mPropSections[hPierce]["Name"]			= prop.AddStringItem( hPierce, "Name", "", true );
			mPropSections[hPierce]["Indoor"]		= prop.AddBoolItem( hPierce, "Indoor", false, true );
			mPropSections[hPierce]["Attenuation"]	= prop.AddStringItem( hPierce, "Attenuation", "50 1 0 0", true );
			mPropSections[hPierce]["Pos"]			= prop.AddStringItem( hPierce, "Pos", "0 50 0", true );
			mPropSections[hPierce]["VirtualCol"]	= prop.AddColorItem( hPierce, "VirtualCol", COLORREF(0x00FFFFFF), true );
			mPropSections[hPierce]["VirtualAtten"]	= prop.AddDoubleItem( hPierce, "VirtualAtten", 0, "", true );


			::HSECTION hHelper = prop.AddSection( "灯光辅助选项" );

			mPropSections[hHelper]["Ambient"]		= prop.AddColorItem( hHelper, "Ambient", COLORREF(0x00999999), true );
			mPropSections[hHelper]["DrawHelper"]	= prop.AddBoolItem( hHelper, "DrawHelper", true, true );
			mPropSections[hHelper]["OpenConsult"]	= prop.AddBoolItem( hHelper, "OpenConsult", false, true);
			vstr.clear();
			vstr.push_back("无");
			for ( HouseCompTable::const_iterator it = HouseTableManager::getSingleton().getHouseCompTable().begin();
				it != HouseTableManager::getSingleton().getHouseCompTable().end();
				++it )
			{			
				vstr.push_back(Ogre::StringConverter::toString( it->second.ID ).c_str());
			}
			mPropSections[hHelper]["Consultations"]	= prop.AddComboItem( hHelper, "Consultations", vstr, 0, true );

			updateHouseData();
		}
		break;
	default:
		break;
	}
}



void CModelDlg::showHouseCompProperty()
{
	if ( mHouseCompRow.ID == 0 || mHouseCompPropRow.PropID == 0 )
		return;

	CPropertyGrid & prop = static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndPropertyDlg;

	// 部件类属性	
	PropSectionMap::iterator it = mPropSections.begin();

	HITEM hSec = it->first;
	prop.SetSectionCollapsed(hSec, false);
	prop.SetSectionEditable(hSec, true);

	prop.SetItemValue( mPropSections[hSec]["PropID"], static_cast<int>(mHouseCompPropRow.PropID) );
	prop.SetItemValue( mPropSections[hSec]["PropName"], mHouseCompPropRow.Name );
	prop.SetItemValue( mPropSections[hSec]["BeWatchMode"], static_cast<int>(mHouseCompPropRow.BeWatchMode) );

	prop.SetItemValue( mPropSections[hSec]["HoldGrid"], GridArrayToString(mHouseCompPropRow.HoldGrid) );
	prop.SetItemValue( mPropSections[hSec]["StopGrid"], GridArrayToString(mHouseCompPropRow.StopGrid) );
	prop.SetItemValue( mPropSections[hSec]["LayGrid"], GridArrayToString(mHouseCompPropRow.LayGrid) );
	prop.SetItemValue( mPropSections[hSec]["SelfHeight"], static_cast<int>(mHouseCompPropRow.SelfHeight) );
	prop.SetItemValue( mPropSections[hSec]["OffsetHeight"], static_cast<int>(mHouseCompPropRow.OffsetHeight) );

	prop.SetItemValue( mPropSections[hSec]["AcceptLay"], "0x" + FormatStr( "%X", mHouseCompPropRow.AcceptLay ) );
	prop.SetItemValue( mPropSections[hSec]["LayType"], "0x" + FormatStr( "%X", mHouseCompPropRow.LayType ) );
	prop.SetItemValue( mPropSections[hSec]["CanRep"], mHouseCompPropRow.CanRep );
	prop.SetItemValue( mPropSections[hSec]["HoldType"], mHouseCompPropRow.HoldType );


	// 部件属性
	++it;

	HITEM hComp = it->first;
	prop.SetSectionCollapsed(hComp, false);
	prop.SetSectionEditable(hComp, true);

	prop.SetItemValue( mPropSections[hComp]["ID"], static_cast<int>(mHouseCompRow.ID) );
	prop.SetItemValue( mPropSections[hComp]["Name"], mHouseCompRow.Name );
	prop.SetItemValue( mPropSections[hComp]["DisID"], static_cast<int>(mHouseCompRow.DisID) );

	prop.SetItemValue( mPropSections[hComp]["Pos"], Vector3ToString( mHouseCompRow.Pos ) );
	prop.SetItemValue( mPropSections[hComp]["Rot"], QuaternionToString( mHouseCompRow.Rot ) );
	prop.SetItemValue( mPropSections[hComp]["Scale"], Vector3ToString( mHouseCompRow.Scale ) );

	// 其它属性
	++it;
	HITEM hOther = it->first;
	prop.SetSectionCollapsed(hOther, false);
	prop.SetSectionEditable(hOther, true);

	//灯光属性
	++it;
	HITEM hLight = it->first;
	++it;
	HITEM hPierce = it->first;
	++it;
	HITEM hHelper = it->first;

	if(mHouseCompRow.ID == COMPONENT_LIGHT || mHouseCompRow.ID == COMPONENT_PASS)
	{
		prop.SetSectionEditable(hLight, true);
		prop.SetSectionCollapsed(hLight, false);

		prop.SetSectionEditable(hSec, false);
		prop.SetSectionCollapsed(hSec, true);
		prop.SetSectionEditable(hComp, false);
		prop.SetSectionCollapsed(hComp, true);
		prop.SetSectionEditable(hOther, false);
		prop.SetSectionCollapsed(hOther, true);
		prop.SetSectionEditable(hHelper, true);
		prop.SetSectionCollapsed(hHelper, false);

		bool bValue;
		prop.GetItemValue(mPropSections[hLight]["OpenConsult"], bValue);
		if(bValue)
			prop.SetItemEditable(mPropSections[hLight]["Consultations"], true);
		else
			prop.SetItemEditable(mPropSections[hLight]["Consultations"], false);

		if(mHouseCompRow.ID == COMPONENT_LIGHT)
		{
			prop.SetSectionEditable(hLight, true);
			prop.SetSectionCollapsed(hLight, false);
			prop.SetSectionEditable(hPierce, false);
			prop.SetSectionCollapsed(hPierce, true);

			int nValue;
			prop.GetItemValue(mPropSections[hLight]["LightType"], nValue);
			if(nValue == 0)
			{
				prop.SetItemEditable(mPropSections[hLight]["Dir"], false);
				prop.SetItemEditable(mPropSections[hLight]["SpotRange"], false);
				prop.SetItemEditable(mPropSections[hLight]["Pierce"], true);
				prop.SetItemEditable(mPropSections[hLight]["PierceRange"], true);
			}
			else
			{
				prop.SetItemEditable(mPropSections[hLight]["Dir"], true);
				prop.SetItemEditable(mPropSections[hLight]["SpotRange"], true);
				prop.SetItemEditable(mPropSections[hLight]["Pierce"], false);
				prop.SetItemEditable(mPropSections[hLight]["PierceRange"], false);
			}

			prop.SetItemValue(mPropSections[hLight]["Pos"], Vector3ToString(mLightPos));
		}
		else if(mHouseCompRow.ID == COMPONENT_PASS)
		{
			prop.SetSectionEditable(hLight, false);
			prop.SetSectionCollapsed(hLight, true);
			prop.SetSectionEditable(hPierce, true);
			prop.SetSectionCollapsed(hPierce, false);

			prop.SetItemValue(mPropSections[hPierce]["Pos"], Vector3ToString(mPassPos));
		}
	}
	else
	{
		prop.SetSectionEditable(hLight, false);
		prop.SetSectionCollapsed(hLight, true);
		prop.SetSectionEditable(hPierce, false);
		prop.SetSectionCollapsed(hPierce, true);
		prop.SetSectionEditable(hHelper, false);
		prop.SetSectionCollapsed(hHelper, true);
	}
}

void CModelDlg::updateHouseData()
{
	if ( mHouseCompRow.ID == 0 || mHouseCompPropRow.PropID == 0 )
		return;

	showHouseCompProperty();	
	mCompGridGraphics.detach();
	if ( g_HouseComp )
	{
		HSCManager::getSingleton().destroyHouseSceneComponent( g_HouseComp );
		g_HouseComp = NULL;
	}

	{
		g_HouseComp = HSCManager::getSingleton().createHouseSceneComponent();
		g_HouseComp->parseRes( &mHouseCompRow, &mHouseCompPropRow );
		g_HouseComp->load();	

		g_HouseComp->getModel()->setParentNode( GraphicsSystem::getSingleton().getItemNode() );

		g_HouseComp->getModel()->setVisible( mShowModel );
		//已经没有这个东西了
		//if ( mShowOffsetHeight )
		//	g_HouseComp->setPosition( Ogre::Vector3( 0, g_HouseComp->getHouseCompPropRow()->OffsetHeight, 0 ) );
		//else
			g_HouseComp->setPosition( Ogre::Vector3( 0, 0, 0 ) );
		
		mCompGridGraphics.attach( GraphicsSystem::getSingleton().getItemNode(), &mHouseCompPropRow );
		mCompGridGraphics.showLayGrid( mShowLayGrid );
		mCompGridGraphics.showHoldGrid( mShowHoldGrid );
		mCompGridGraphics.showStopGrid( mShowStopGrid );
		mCompGridGraphics.showOffsetHeight( /*mShowOffsetHeight*/false );		
	}

	resetLight();
	if(mHouseCompRow.ID == COMPONENT_LIGHT)
	{
		//创建灯光
		mLight = GraphicsSystem::getSingleton().getSceneMgr()->createLight("light");
		mLight->setType(mLightType);
		mLight->setDiffuseColour(mLightDiffuseColour);
		mLight->setSpecularColour(mLightSpecularColour);
		mLight->setAttenuation(mLightAttenuation.x, mLightAttenuation.y, mLightAttenuation.z, mLightAttenuation.w);
		if(mLightType == Ogre::Light::LT_SPOTLIGHT)
			mLight->setSpotlightRange(Ogre::Degree(mLightSpotlightRange.x), Ogre::Degree(mLightSpotlightRange.y), mLightSpotlightRange.z);

		Ogre::SceneNode* lightNode = g_HouseComp->getModel()->getMainNode()->createChildSceneNode();
		lightNode->attachObject(mLight);
		lightNode->setPosition(mLightPos);
		//只有聚光灯才转节点，只是为了绘制聚光灯帮助线条方便而已
		if(mLightType == Ogre::Light::LT_SPOTLIGHT)
			lightNode->setDirection(mLightDirection, Ogre::Node::TS_LOCAL, Ogre::Vector3::UNIT_Z);

		if(mDrawHelper)
			drawLightHelper(mLight, mLightBbs, mLightBb, mManualobject);
	}
	else if(mHouseCompRow.ID == COMPONENT_PASS)
	{
		//点光源模拟穿透光
		mPassLight = GraphicsSystem::getSingleton().getSceneMgr()->createLight("passlight");
		mPassLight->setType(Ogre::Light::LT_POINT);
		mPassLight->setDiffuseColour(mPassVirtualCol);
		//mPassLight->setAttenuation(mPassAttenuation.x, 
		//	mPassAttenuation.y/mPassVirtualAtten, mPassAttenuation.z/mPassVirtualAtten, mPassAttenuation.w/mPassVirtualAtten);

		Ogre::SceneNode* pierceNode1 = g_HouseComp->getModel()->getMainNode()->createChildSceneNode();
		pierceNode1->attachObject(mPassLight);
		pierceNode1->setPosition(mPassPos);

		if(mDrawHelper)
			drawLightHelper(mPassLight, mPassBbs, mPassBb, mPassManualobject);
	}
	else
	{
		GraphicsSystem::getSingleton().getSceneMgr()->setAmbientLight(DEFAULT_LIGHT);
		GraphicsSystem::getSingleton().getMainLight()->setDiffuseColour(DEFAULT_LIGHT);
	}
}

void CModelDlg::syncNodeInfo()
{
	if(!g_HouseComp)
		return;

	mHouseCompRow.Pos = g_HouseComp->getModel()->getMainNode()->getPosition();
	//if ( mShowOffsetHeight )
	//	mHouseCompRow.Pos.y -= g_HouseComp->getHouseCompPropRow()->OffsetHeight;

	mHouseCompRow.Rot = g_HouseComp->getModel()->getMainNode()->getOrientation();


	if( mHouseCompRow.ID == COMPONENT_LIGHT )
	{
		//主动光节点位移
		mLightPos = mLight->getParentSceneNode()->getPosition();
	}
	else if( mHouseCompRow.ID == COMPONENT_PASS )
	{
		//穿透光节点位移
		mPassPos = mPassLight->getParentNode()->getPosition();
	}
	else
	{
		//灯光不缩放
		mHouseCompRow.Scale = g_HouseComp->getModel()->getMainNode()->getScale();
	}

	showHouseCompProperty();
}


void CModelDlg::resetContent()
{
	clearProperty();
	mCompGridGraphics.detach();
	resetLight();
	GraphicsSystem::getSingleton().getSceneMgr()->setAmbientLight(DEFAULT_LIGHT);
	GraphicsSystem::getSingleton().getMainLight()->setDiffuseColour(DEFAULT_LIGHT);
	clearConsultation();
}

void CModelDlg::resetLight()
{
	if(mLight)
	{
		GraphicsSystem::getSingleton().getSceneMgr()->destroyLight(mLight);
		mLight = NULL;
	}
	if(mLightBbs)
	{
		mLightBbs->removeBillboard(mLightBb);
		mLightBb = NULL;
		GraphicsSystem::getSingleton().getSceneMgr()->destroyBillboardSet(mLightBbs);
		mLightBbs = NULL;
	}
	if(mManualobject)
	{
		GraphicsSystem::getSingleton().getSceneMgr()->destroyManualObject(mManualobject);
		mManualobject = NULL;
	}

	if(mPassLight)
	{
		GraphicsSystem::getSingleton().getSceneMgr()->destroyLight(mPassLight);
		mPassLight = NULL;
	}
	if(mPassBbs)
	{
		mPassBbs->removeBillboard(mPassBb);
		mPassBb = NULL;
		GraphicsSystem::getSingleton().getSceneMgr()->destroyBillboardSet(mPassBbs);
		mPassBbs = NULL;
	}
	if(mPassManualobject)
	{
		GraphicsSystem::getSingleton().getSceneMgr()->destroyManualObject(mPassManualobject);
		mPassManualobject = NULL;
	}
}

void CModelDlg::drawLightHelper(Ogre::Light* light, Ogre::BillboardSet*& bbs, Ogre::Billboard*& bb, Ogre::ManualObject*& manual)
{
	if(!light->getVisible())
		return;

	//贴个公告板便于查看灯光位置
	bbs = GraphicsSystem::getSingleton().getSceneMgr()->createBillboardSet(light->getName()+"bbs");
	bbs->setMaterialName("LightBbs");
	bb = bbs->createBillboard(0,0,0,Ogre::ColourValue(1,1,1));
	light->getParentSceneNode()->attachObject(bbs);

	//绘制灯光辅助线
	manual =  GraphicsSystem::getSingleton().getSceneMgr()->createManualObject(light->getName()+"manual");
	light->getParentSceneNode()->attachObject(manual);

	float offset = 0.65f;
	float range = light->getAttenuationRange();

	if(light->getType() == Ogre::Light::LT_POINT)
	{
		drawSphere(manual, range, 24, 24);
	}
	else if(light->getType() == Ogre::Light::LT_SPOTLIGHT)
	{
		manual->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real firstInnerRadius = Ogre::Math::Tan(light->getSpotlightInnerAngle()/2) * offset;
		drawCircle(manual, firstInnerRadius, offset);
		manual->end();

		manual->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real secondInnerRadius = Ogre::Math::Tan(light->getSpotlightInnerAngle()/2) * range;
		drawCircle(manual, secondInnerRadius, range);
		manual->end();

		manual->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real firstOuterRadius = Ogre::Math::Tan(light->getSpotlightOuterAngle()/2) * offset;
		drawCircle(manual, firstOuterRadius, offset);
		manual->end();

		manual->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real secondOuterRadius = Ogre::Math::Tan(light->getSpotlightOuterAngle()/2) * range;
		drawCircle(manual, secondOuterRadius, range);
		manual->end();

		manual->begin("LightHelper", Ogre::RenderOperation::OT_LINE_LIST);
		manual->position(firstInnerRadius, 0, offset);
		manual->position(secondInnerRadius, 0, range);
		manual->position(0, firstInnerRadius, offset);
		manual->position(0, secondInnerRadius, range);
		manual->position(0, -firstInnerRadius, offset);
		manual->position(0, -secondInnerRadius, range);
		manual->position(-firstInnerRadius, 0, offset);
		manual->position(-secondInnerRadius, 0, range);
		manual->position(firstOuterRadius, 0, offset);
		manual->position(secondOuterRadius, 0, range);
		manual->position(0, firstOuterRadius, offset);
		manual->position(0, secondOuterRadius, range);
		manual->position(0, -firstOuterRadius, offset);
		manual->position(0, -secondOuterRadius, range);
		manual->position(-firstOuterRadius, 0, offset);
		manual->position(-secondOuterRadius, 0, range);
		manual->end();
	}
}

void CModelDlg::drawCircle(Ogre::ManualObject* manual, Ogre::Real radius, Ogre::Real zOffset)
{
	for (unsigned int i = 0; i <= 32; i++)
	{
		Ogre::Real x = Ogre::Math::Cos(Ogre::Radian((Ogre::Math::TWO_PI*i)/32)) * radius;
		Ogre::Real y = Ogre::Math::Sin(Ogre::Radian((Ogre::Math::TWO_PI*i)/32)) * radius;
		manual->position(x, y, zOffset);
	}
}

void CModelDlg::drawSphere(Ogre::ManualObject* manual, const float r, const int nRings, const int nSegments)
{
	manual->begin("LightHelper", Ogre::RenderOperation::OT_LINE_LIST);

	float fDeltaRingAngle = (Ogre::Math::PI / nRings);
	float fDeltaSegAngle = (2 * Ogre::Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	for( int ring = 0; ring <= nRings; ring++ ) 
	{
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		for(int seg = 0; seg <= nSegments; seg++) 
		{
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			manual->position( x0, y0, z0);
			manual->normal(Ogre::Vector3(x0, y0, z0).normalisedCopy());
			manual->textureCoord((float) seg / (float) nSegments, (float) ring / (float) nRings);

			if (ring != nRings) 
			{
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex);               
				manual->index(wVerticeIndex + nSegments);
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex + 1);
				manual->index(wVerticeIndex);
				wVerticeIndex ++;
			}
		};
	}
	manual->end();

	Ogre::MeshPtr mesh = manual->convertToMesh("pointlight");
	mesh->_setBounds( Ogre::AxisAlignedBox( Ogre::Vector3(-r, -r, -r), Ogre::Vector3(r, r, r) ), false );

	mesh->_setBoundingSphereRadius(r);
	unsigned short src, dest;
	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
	{
		mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
	}
}

void CModelDlg::addConsultation(int row)
{
	const HouseCompRow* compRow = HouseTableManager::getSingleton().getHouseCompRow( row );
	if ( compRow )
	{					
		const HouseCompPropRow* propRow = HouseTableManager::getSingleton().getHouseCompPropRow( compRow->PropID );
		if ( propRow )
		{
			HouseSceneComponent* houseComp = HSCManager::getSingleton().createHouseSceneComponent();
			houseComp->parseRes( (HouseCompRow*)compRow, (HouseCompPropRow*)propRow );
			houseComp->load();	
			houseComp->getModel()->setParentNode( GraphicsSystem::getSingleton().getItemNode() );

			mConsultation.push_back(houseComp);
		}
	}
}

void CModelDlg::clearConsultation()
{
	for(int i=0; i<(int)mConsultation.size(); i++)
		HSCManager::getSingleton().destroyHouseSceneComponent(mConsultation[i]);

	mConsultation.clear();
}

void CModelDlg::OnEnChangeEdit1()
{
	CString str;
	m_input.GetWindowText(str);
	std::string input(str.GetBuffer());

	HTREEITEM root = m_wndModelTree.GetRootItem();

	std::string sid;
	if(m_wndModelTree.GetItemText(root) == "模型表现")
	{
		m_wndModelTree.DeleteAllItems();
		// 模型列表 
		HTREEITEM hModel = m_wndModelTree.InsertItem( "模型表现" );

		for ( ModelDisplayTable::const_iterator it = GraphicsTableManager::getSingleton().getModelDisplayTable().begin();
			it != GraphicsTableManager::getSingleton().getModelDisplayTable().end();
			++it )
		{
			sid = Ogre::StringConverter::toString( it->second.id );

			if(!input.empty())
			{
				if(sid.find(input) == std::string::npos)
					continue;
			}

			HTREEITEM h = m_wndModelTree.InsertItem( sid.c_str(), hModel );
			// 记录模型表现id
			m_wndModelTree.SetItemData( h, it->second.id );
		}

		m_wndModelTree.SortChildren( hModel );

		m_wndModelTree.Expand( hModel, TVE_EXPAND );
	}
	else if(m_wndModelTree.GetItemText(root) == "房屋部件")
	{
		m_wndModelTree.DeleteAllItems();
		// 模型列表 
		HTREEITEM hModel = m_wndModelTree.InsertItem( "房屋部件" );

		for ( HouseCompTable::const_iterator it = HouseTableManager::getSingleton().getHouseCompTable().begin();
			it != HouseTableManager::getSingleton().getHouseCompTable().end();
			++it )
		{			
			sid = Ogre::StringConverter::toString( it->second.ID );

			if(!input.empty())
			{
				if(sid.find(input) == std::string::npos && it->second.Name.find(input) == std::string::npos)
					continue;
			}

			HTREEITEM h = m_wndModelTree.InsertItem( sid.c_str(), hModel );
			// 记录房屋部件id
			m_wndModelTree.SetItemData( h, it->second.ID );
		}

		m_wndModelTree.SortChildren( hModel );

		m_wndModelTree.Expand( hModel, TVE_EXPAND );
	}
}
