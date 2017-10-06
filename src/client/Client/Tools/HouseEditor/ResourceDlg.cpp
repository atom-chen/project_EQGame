// SceneDlg.cpp : implementation file
//
#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "ResourceDlg.h"
#include "PropertyGrid/PropertyGrid.h"
#include "MainFrm.h"
#include "HouseScene.h"
#include "SceneManager.h"
#include "HouseSceneFloor.h"
#include "commfunc.h"
#include "MfcInputEventMgr.h"
#include "EQLog.h"
#include "HouseSceneComponent.h"
#include "HSCManager.h"
#include "GraphicsTable.h"
#include "UIFitmentEvent.h"
#include "AmbientMgr.h"
#include "ZoneLightMgr.h"
#include "SceneInfo.h"
#include "SceneEntityObject.h"

static std::string GridArrayToString( const std::vector<HouseSceneGrid*> & v )
{
	std::string s;
	int x = 0;
	int z = 0;

	for ( std::vector<HouseSceneGrid*>::const_iterator it = v.begin();
		it != v.end();
		++it )
	{
		(*it)->getGridNo( x, z );
		s += "(" + Ogre::StringConverter::toString( x ) + "," + Ogre::StringConverter::toString( z ) + ")" + " ";
	}

	return s;
}

static std::string Vector3ToString( const Ogre::Vector3 & v )
{
	return FormatStr( "%.1f, %.1f, %.1f", v.x, v.y, v.z );
}

static Ogre::Vector3 StringToVector3( const std::string & s )
{
	std::vector<std::string> strs;
	strs = Ogre::StringUtil::split( s, " ," );

	Ogre::Vector3 v3;
	if ( strs.size() == 3 )
	{
		v3.x = Ogre::StringConverter::parseReal(strs[0]);
		v3.y = Ogre::StringConverter::parseReal(strs[1]);
		v3.z = Ogre::StringConverter::parseReal(strs[2]);
	}

	return v3;
}

static std::string Vector4ToString( const Ogre::Vector4 & v )
{
	return FormatStr( "%.1f, %.4f, %.4f, %.4f", v.x, v.y, v.z, v.w );
}

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

static COLORREF OgreColToColRef( const Ogre::ColourValue col)
{
	return int(col.b*255)*0x10000 + int(col.g*255)*0x100 + int(col.r*255);
}

static Ogre::ColourValue ColRefToOgreCol(COLORREF colref)
{
	return Ogre::ColourValue(colref%0x100/255.0f, colref%0x10000/0x100/255.0f, colref/0x10000/255.0f);
}

CResourceDlg * g_ResourceDlg = NULL;
// CResourceDlg dialog
BEGIN_MESSAGE_MAP(CResourceDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CResourceDlg::OnTvnSelchangedTree1)
END_MESSAGE_MAP()


CResourceDlg::CResourceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResourceDlg::IDD, pParent)
{
	g_ResourceDlg = this;
	mLightBbs[0] = NULL;
	mLightBbs[1] = NULL;
	mLightBb[0] = NULL;
	mLightBb[1] = NULL;
	mManualobject[0] = NULL;
	mManualobject[1] = NULL;
	mDrawHelp[0] = true;
	mDrawHelp[1] = true;
}

CResourceDlg::~CResourceDlg()
{

}

void CResourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndResTree);
}



void CResourceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);	

	if ( m_wndResTree.m_hWnd )
	{
		CRect rc;
		GetClientRect( rc );
		m_wndResTree.MoveWindow( rc );		
	}
}


void CResourceDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	
	uint id =  pNMTreeView->itemNew.lParam;

	const int k = getTreeItemLayer( pNMTreeView->itemNew.hItem );

	switch ( k )
	{
	case 2: // floor
		{
			HTREEITEM hParent = m_wndResTree.GetParentItem(pNMTreeView->itemNew.hItem);
			if ( m_wndResTree.GetItemText(hParent) == "Floors" )
				showFloorProperty( reinterpret_cast<HouseSceneFloor *>(id) );
			else if ( m_wndResTree.GetItemText(hParent) == "Components" )
				selectComponent( reinterpret_cast<HouseSceneComponent *>(id) );
		}
		break;
	case 4: // grids and components
		{
			HTREEITEM hParent = m_wndResTree.GetParentItem(pNMTreeView->itemNew.hItem);
			if ( m_wndResTree.GetItemText(hParent) == "Grids" )
				selectGrid( reinterpret_cast<HouseSceneGrid *>(id) );
		}
		break;
	default:
		break;
	}	
	
}

BOOL CResourceDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if ( WM_PG_ITEMCHANGED == message ) 
	{
		COLORREF colValue;
		string sValue;
		bool bValue;
		int nValue;
		double dValue;

		for(PropSectionMap::iterator it = mAmbientSections.begin(); it != mAmbientSections.end(); it++)
		{
			CPropertyGrid & prop = g_MainFrame->getAmbientDlg();
			const HITEM hItem = wParam;
			if ( hItem == it->second["DayNight"] )
			{
				prop.GetItemValue( hItem, bValue );

				AmbientMgr::getSingleton().changeDayNight(bValue);
			}
			else if ( hItem == it->second["ChangeTime"] )
			{
				prop.GetItemValue( hItem, nValue );

				AmbientMgr::getSingleton().mChangetime = nValue;
			}
			else if ( hItem == it->second["AutoTurnOnOff"] )
			{
				prop.GetItemValue( hItem, bValue );

				AmbientMgr::getSingleton().setAutoTurnOnOffLight(bValue);
			}
			else if ( hItem == it->second["DayAmbient"] )
			{
				prop.GetItemValue( hItem, colValue );

				AmbientMgr::getSingleton().mDayAmbient = ColRefToOgreCol(colValue);

				if(AmbientMgr::getSingleton().getDayNight())
				{
					AmbientMgr::getSingleton().setAmbientLight(ColRefToOgreCol(colValue));
				}
			}
			else if ( hItem == it->second["DayDiffuse"] )
			{
				prop.GetItemValue( hItem, colValue );

				AmbientMgr::getSingleton().mDayDiffuse = ColRefToOgreCol(colValue);

				if(AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::Light* light = AmbientMgr::getSingleton().getMainLight();
					light->setDiffuseColour(ColRefToOgreCol(colValue));
					AmbientMgr::getSingleton().notifyZoneLightDirty(0);
				}
			}
			else if ( hItem == it->second["DaySpecular"] )
			{
				prop.GetItemValue( hItem, colValue );

				AmbientMgr::getSingleton().mDaySpecular = ColRefToOgreCol(colValue);

				if(AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::Light* light = AmbientMgr::getSingleton().getMainLight();
					light->setSpecularColour(ColRefToOgreCol(colValue));
					AmbientMgr::getSingleton().notifyZoneLightDirty(0);
				}
			}
			else if ( hItem == it->second["DayMainDir"] )
			{
				prop.GetItemValue( hItem, sValue );

				AmbientMgr::getSingleton().mDayMainDir = StringToVector3(sValue);
				
				if(AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::Light* light = AmbientMgr::getSingleton().getMainLight();
					light->setDirection(StringToVector3(sValue));
				}
			}
			else if ( hItem == it->second["DayFogColor"] )
			{
				prop.GetItemValue( hItem, colValue );
				Ogre::ColourValue col = ColRefToOgreCol(colValue);

				AmbientMgr::getSingleton().mDayFogColor = col;

				if(AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::FogMode mode = GraphicsSystem::getSingleton().getSceneMgr()->getFogMode();
					Ogre::Real exp = GraphicsSystem::getSingleton().getSceneMgr()->getFogDensity();
					Ogre::Real start = GraphicsSystem::getSingleton().getSceneMgr()->getFogStart();
					Ogre::Real end = GraphicsSystem::getSingleton().getSceneMgr()->getFogEnd();

					GraphicsSystem::getSingleton().getSceneMgr()->setFog(mode, col, exp, start, end);
				}
			}
			else if ( hItem == it->second["DayFogParam"] )
			{
				prop.GetItemValue( hItem, sValue );
				Ogre::Vector4 fog = StringToVector4(sValue);

				AmbientMgr::getSingleton().mDayFogParam = fog;

				if(AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::ColourValue col = GraphicsSystem::getSingleton().getSceneMgr()->getFogColour();
					GraphicsSystem::getSingleton().getSceneMgr()->setFog(Ogre::FogMode(int(fog.x)), col, fog.w, fog.y, fog.z);
				}
			}
			else if ( hItem == it->second["DaySkyBox"] )
			{
				prop.GetItemValue( hItem, sValue );

				AmbientMgr::getSingleton().mDaySkybox = sValue;

				if(AmbientMgr::getSingleton().getDayNight())
				{
					if(sValue.empty())
						GraphicsSystem::getSingleton().getSceneMgr()->setSkyBox(false, "", 100);
					else
					{
						GraphicsSystem::getSingleton().getSceneMgr()->setSkyBox(true, sValue, 100);
						SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();
						if(scene)
							scene->changeSkybox(sValue);
					}
				}
			}
			else if ( hItem == it->second["NightAmbient"] )
			{
				prop.GetItemValue( hItem, colValue );

				AmbientMgr::getSingleton().mNightAmbient = ColRefToOgreCol(colValue);

				if(!AmbientMgr::getSingleton().getDayNight())
				{
					AmbientMgr::getSingleton().setAmbientLight(ColRefToOgreCol(colValue));
				}
			}
			else if ( hItem == it->second["NightDiffuse"] )
			{
				prop.GetItemValue( hItem, colValue );

				AmbientMgr::getSingleton().mNightDiffuse = ColRefToOgreCol(colValue);

				if(!AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::Light* light = AmbientMgr::getSingleton().getMainLight();
					light->setDiffuseColour(ColRefToOgreCol(colValue));
					AmbientMgr::getSingleton().notifyZoneLightDirty(0);
				}
			}
			else if ( hItem == it->second["NightSpecular"] )
			{
				prop.GetItemValue( hItem, colValue );

				AmbientMgr::getSingleton().mNightSpecular = ColRefToOgreCol(colValue);

				if(!AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::Light* light = AmbientMgr::getSingleton().getMainLight();
					light->setSpecularColour(ColRefToOgreCol(colValue));
					AmbientMgr::getSingleton().notifyZoneLightDirty(0);
				}
			}
			else if ( hItem == it->second["NightMainDir"] )
			{
				prop.GetItemValue( hItem, sValue );

				AmbientMgr::getSingleton().mNightMainDir = StringToVector3(sValue);

				if(!AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::Light* light = AmbientMgr::getSingleton().getMainLight();
					light->setDirection(StringToVector3(sValue));
				}
			}
			else if ( hItem == it->second["NightFogColor"] )
			{
				prop.GetItemValue( hItem, colValue );
				Ogre::ColourValue col = ColRefToOgreCol(colValue);

				AmbientMgr::getSingleton().mNightFogColor = col;

				if(!AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::FogMode mode = GraphicsSystem::getSingleton().getSceneMgr()->getFogMode();
					Ogre::Real exp = GraphicsSystem::getSingleton().getSceneMgr()->getFogDensity();
					Ogre::Real start = GraphicsSystem::getSingleton().getSceneMgr()->getFogStart();
					Ogre::Real end = GraphicsSystem::getSingleton().getSceneMgr()->getFogEnd();

					GraphicsSystem::getSingleton().getSceneMgr()->setFog(mode, col, exp, start, end);
				}
			}
			else if ( hItem == it->second["NightFogParam"] )
			{
				prop.GetItemValue( hItem, sValue );
				Ogre::Vector4 fog = StringToVector4(sValue);

				AmbientMgr::getSingleton().mNightFogParam = fog;

				if(!AmbientMgr::getSingleton().getDayNight())
				{
					Ogre::ColourValue col = GraphicsSystem::getSingleton().getSceneMgr()->getFogColour();
					GraphicsSystem::getSingleton().getSceneMgr()->setFog(Ogre::FogMode(int(fog.x)), col, fog.w, fog.y, fog.z);
				}
			}
			else if ( hItem == it->second["NightSkyBox"] )
			{
				prop.GetItemValue( hItem, sValue );

				AmbientMgr::getSingleton().mNightSkybox = sValue;

				if(!AmbientMgr::getSingleton().getDayNight())
				{
					if(sValue.empty())
						GraphicsSystem::getSingleton().getSceneMgr()->setSkyBox(false, "", 100);
					else
					{
						GraphicsSystem::getSingleton().getSceneMgr()->setSkyBox(true, sValue, 100);
						SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();
						if(scene)
							scene->changeSkybox(sValue);
					}
				}
			}
			else if ( hItem == it->second["Contrast"] )
			{
				prop.GetItemValue( hItem, dValue );

				AmbientMgr::getSingleton().mContrast = dValue;

				GraphicsSystem::getSingleton().setContact(dValue);
			}
		}

		for(PropSectionMap::iterator it = mPostProcessSections.begin(); it != mPostProcessSections.end(); it++)
		{
			CPropertyGrid & prop = g_MainFrame->getPostProcessDlg();
			const HITEM hItem = wParam;
			
			Ogre::Viewport* vp = GraphicsSystem::getSingleton().getViewport();
			if ( hItem == it->second["Gaussian Blur"] )
			{
				prop.GetItemValue( hItem, bValue );

				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Gaussian Blur", bValue);
			}
// 			else if ( hItem == it->second["Contrast"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Contrast", bValue);
// 			}
// 			else if ( hItem == it->second["Bloom"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Bloom", bValue);
// 			}
// 			else if ( hItem == it->second["Glass"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Glass", bValue);
// 			}
// 			else if ( hItem == it->second["Old TV"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Old TV", bValue);
// 			}
// 			else if ( hItem == it->second["B&W"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "B&W", bValue);
// 			}
// 			else if ( hItem == it->second["Embossed"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Embossed", bValue);
// 			}
// 			else if ( hItem == it->second["Sharpen Edges"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Sharpen Edges", bValue);
// 			}
// 			else if ( hItem == it->second["Invert"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Invert", bValue);
// 			}
// 			else if ( hItem == it->second["Posterize"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Posterize", bValue);
// 			}
// 			else if ( hItem == it->second["Laplace"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Laplace", bValue);
// 			}
// 			else if ( hItem == it->second["Tiling"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Tiling", bValue);
// 			}
// 			else if ( hItem == it->second["Old Movie"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Old Movie", bValue);
// 			}
// 			else if ( hItem == it->second["HDR"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "HDR", bValue);
// 			}
// 			else if ( hItem == it->second["Radial Blur"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Radial Blur", bValue);
// 			}
// 			else if ( hItem == it->second["ASCII"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "ASCII", bValue);
// 			}
// 			else if ( hItem == it->second["Halftone"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Halftone", bValue);
// 			}
// 			else if ( hItem == it->second["Night Vision"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Night Vision", bValue);
// 			}
// 			else if ( hItem == it->second["Dither"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Dither", bValue);
// 			}
// 			else if ( hItem == it->second["Motion Blur"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Motion Blur", bValue);
// 			}
// 			else if ( hItem == it->second["Heat Vision"] )
// 			{
// 				prop.GetItemValue( hItem, bValue );
// 
// 				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Heat Vision", bValue);
// 			}
		}

		uint lightno=0;
		for(PropSectionMap::iterator it = mLightSections.begin(); it != mLightSections.end(); it++)
		{
			assert(mLightPo);
			assert(mLightSections.size() <= 2);

			CPropertyGrid & prop = g_MainFrame->getLightDlg();
			const HITEM hItem = wParam;

			ZoneLight* zl=NULL;
			if (mLightPo->getClassName() == "HouseSceneComponent")
			{
				HouseSceneComponent* comp = dynamic_cast<HouseSceneComponent*>(mLightPo);

				if(lightno >= comp->getLight().size())
					continue;

				zl = comp->getLight()[lightno];
			}
			else if(mLightPo->getClassName() == "SceneEntityObject")
			{
				SceneEntityObject* obj = dynamic_cast<SceneEntityObject*>(mLightPo);

				if(lightno >= obj->getLight().size())
					continue;

				zl = obj->getLight()[lightno];
			}
			if(!zl)
				continue;

			Ogre::Light* l = zl->getLight();
			if(!l)
				continue;

			if( it->second.find("LightType")!=it->second.end() && hItem == it->second["LightType"] )
			{
				prop.GetItemValue( hItem, nValue );

				if(nValue == 0)
				{
					l->setType( Ogre::Light::LT_POINT);

					prop.SetItemEditable(it->second["Dir"], false);
					prop.SetItemEditable(it->second["SpotRange"], false);
					prop.SetItemEditable(it->second["Pierce"], true);
					prop.SetItemEditable(it->second["PierceRange"], true);
				}
				else
				{
					l->setType( Ogre::Light::LT_SPOTLIGHT);

					prop.SetItemEditable(it->second["Dir"], true);
					prop.SetItemEditable(it->second["SpotRange"], true);
					prop.SetItemEditable(it->second["Pierce"], false);
					prop.SetItemEditable(it->second["PierceRange"], false);
				}
			}
			else if ( it->second.find("DiffCol")!=it->second.end() && hItem == it->second["DiffCol"] )
			{
				prop.GetItemValue( hItem, colValue );

				l->setDiffuseColour(ColRefToOgreCol(colValue));
			}
			else if ( it->second.find("SpecCol")!=it->second.end() && hItem == it->second["SpecCol"] )
			{
				prop.GetItemValue( hItem, colValue );

				l->setSpecularColour(ColRefToOgreCol(colValue));
			}
			else if ( it->second.find("Attenuation")!=it->second.end() && hItem == it->second["Attenuation"] )
			{
				prop.GetItemValue( hItem, sValue );

				Ogre::Vector4 atten = StringToVector4(sValue);
				l->setAttenuation(atten.x, atten.y, atten.z, atten.w);
			}
			else if ( it->second.find("Pos")!=it->second.end() && hItem == it->second["Pos"] )
			{
				prop.GetItemValue( hItem, sValue );

				zl->setPos(StringToVector3(sValue));
			}
			else if (it->second.find("Dir")!=it->second.end() &&  hItem == it->second["Dir"] )
			{
				prop.GetItemValue( hItem, sValue );

				l->setDirection(StringToVector3(sValue));
			}
			else if ( it->second.find("SpotRange")!=it->second.end() && hItem == it->second["SpotRange"] )
			{
				prop.GetItemValue( hItem, sValue );

				if(l->getType() == Ogre::Light::LT_SPOTLIGHT)
				{
					Ogre::Vector3 range = StringToVector3(sValue);
					l->setSpotlightRange(Ogre::Degree(range.x), Ogre::Degree(range.y), range.z);
				}
			}
			else if ( it->second.find("Pierce")!=it->second.end() && hItem == it->second["Pierce"] )
			{
				prop.GetItemValue( hItem, nValue );

				zl->setPierceType((ZoneLight::PierceType)nValue);
			}
			else if ( it->second.find("PierceRange")!=it->second.end() && hItem == it->second["PierceRange"] )
			{
				prop.GetItemValue( hItem, nValue );

				zl->setPierceRange(nValue);
			}
			else if ( it->second.find("Visible")!=it->second.end() && hItem == it->second["Visible"] )
			{
				prop.GetItemValue( hItem, bValue );

				l->setVisible(bValue);
			}
			else if ( it->second.find("Indoor")!=it->second.end() && hItem == it->second["Indoor"] )
			{
				bool bValue;
				prop.GetItemValue( hItem, bValue );

				zl->setPassLightIndoor(bValue);
			}
			else if ( it->second.find("P_DayAtten")!=it->second.end() && hItem == it->second["P_DayAtten"] )
			{
				prop.GetItemValue( hItem, sValue );

				Ogre::Vector4 atten = StringToVector4(sValue);
				zl->setAttenuation(true, atten.x, atten.y, atten.z, atten.w);
			}
			else if ( it->second.find("P_NightAtten")!=it->second.end() && hItem == it->second["P_NightAtten"] )
			{
				prop.GetItemValue( hItem, sValue );

				Ogre::Vector4 atten = StringToVector4(sValue);
				zl->setAttenuation(false, atten.x, atten.y, atten.z, atten.w);
			}
			else if ( it->second.find("P_Pos")!=it->second.end() && hItem == it->second["P_Pos"] )
			{
				prop.GetItemValue( hItem, sValue );

				zl->setPos(StringToVector3(sValue));
			}
			else if ( hItem == it->second["DrawHelper"] )
			{
				prop.GetItemValue( hItem, bValue );

				mDrawHelp[lightno] = bValue;
			}
			else if ( hItem == it->second["Adjuster"] )
			{
				prop.GetItemValue( hItem, bValue );

				if(bValue)
					g_MainFrame->getLightAdjust().open(mLightSections, &prop, zl->getZoneLightType()==ZoneLight::ZLT_ACTIVE, lightno, AmbientMgr::getSingleton().getDayNight());
				else
					g_MainFrame->getLightAdjust().ShowWindow(FALSE);
			}

			if(zl)
				ZoneLightMgr::getSingleton().changeLightToZone(ZoneLightMgr::getSingleton().getZoneIDByLight(l), zl);

			resetLightHelp(lightno);
			if(mDrawHelp[lightno])
				drawLightHelper(l, lightno);
			GraphicsSystem::getSingleton().render();

			lightno++;
		}
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CResourceDlg::showSelectGrid()
{
	HTREEITEM hItem = m_wndResTree.GetSelectedItem();
	HTREEITEM hParent = m_wndResTree.GetParentItem(hItem);
	if ( m_wndResTree.GetItemText(hParent) == "Grids" )
		selectGrid( reinterpret_cast<HouseSceneGrid *>( m_wndResTree.GetItemData(hItem) ) );
}

void CResourceDlg::showSelectComponent()
{
	HTREEITEM hItem = m_wndResTree.GetSelectedItem();
	HTREEITEM hParent = m_wndResTree.GetParentItem(hItem);
	if ( m_wndResTree.GetItemText(hParent) == "Components" )
		selectComponent( reinterpret_cast<HouseSceneComponent *>( m_wndResTree.GetItemData(hItem) ) );
}

void CResourceDlg::clearProperty()
{
	CPropertyGrid & prop = g_MainFrame->getPropertyDlg();
	// 清空旧属性
	for ( PropSectionMap::const_iterator it = mPropSections.begin();
		it != mPropSections.end();
		++it )
	{
		for ( PropItemMap::const_iterator p = it->second.begin();
			p != it->second.end();
			++p )
		{
			prop.RemoveItem( p->second );
		}
		prop.RemoveSection( it->first );
	}

	mPropSections.clear();	
	prop.Invalidate();
}

void CResourceDlg::showCompProperty( const HouseSceneComponent * comp )
{
	assert( comp );

	clearProperty();

	CPropertyGrid & prop = g_MainFrame->getPropertyDlg();
	::HSECTION hSec = prop.AddSection( "物件属性" );
	
	mPropSections[hSec]["PropID"] = prop.AddIntegerItem( hSec, "PropID", comp->getHouseCompPropRow()->PropID, "", true );	
	mPropSections[hSec]["UID"] = prop.AddIntegerItem( hSec, "UID", comp->getUid(), "", true );	
	mPropSections[hSec]["Name"] = prop.AddStringItem( hSec, "Name", comp->getHouseCompPropRow()->Name, true );	
	mPropSections[hSec]["Frame"] = prop.AddBoolItem( hSec, "IsFrame", comp->isFromFrame(), true );	

	mPropSections[hSec]["HoldGrids"] = prop.AddStringItem( hSec, "HoldGrids", GridArrayToString( comp->getHoldGrids() ), true );	

	mPropSections[hSec]["AcceptLay"] = prop.AddStringItem( hSec, "AcceptLay", FormatStr( "%X", comp->getHouseCompPropRow()->AcceptLay ), true );	
	mPropSections[hSec]["LayType"] = prop.AddStringItem( hSec, "LayType", FormatStr( "%X", comp->getHouseCompPropRow()->LayType ), true );	
	mPropSections[hSec]["CanRep"] = prop.AddBoolItem( hSec, "CanRep", comp->getHouseCompPropRow()->CanRep, true );	
	mPropSections[hSec]["HoldType"] = prop.AddIntegerItem( hSec, "HoldType", comp->getHouseCompPropRow()->HoldType, "", true );	

	if ( comp->isFromFrame() )
	{
		mPropSections[hSec]["Mesh"] = prop.AddStringItem( hSec, "Mesh", comp->getFrmMesh(), true );	
		mPropSections[hSec]["Pos"] = prop.AddStringItem( hSec, "Pos", Ogre::StringConverter::toString(comp->getFrmPos()), true );	
		mPropSections[hSec]["Rot"] = prop.AddStringItem( hSec, "Rot", Ogre::StringConverter::toString(comp->getFrmRot()), true );	
		mPropSections[hSec]["Scale"] = prop.AddStringItem( hSec, "Scale", Ogre::StringConverter::toString(comp->getFrmScale()), true );	
	}
	else
	{
		const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( comp->getHouseCompRow()->DisID );
		if ( r )
		{			
			mPropSections[hSec]["Mesh"] = prop.AddStringItem( hSec, "Mesh", r->mesh, true );	
		}
		mPropSections[hSec]["ID"] = prop.AddIntegerItem( hSec, "ID", comp->getHouseCompRow()->ID, "", true );	
		mPropSections[hSec]["Pos"] = prop.AddStringItem( hSec, "Pos", Ogre::StringConverter::toString( comp->getModel()->getMainNode()->getPosition() ), true );	
		mPropSections[hSec]["Rot"] = prop.AddStringItem( hSec, "Rot", Ogre::StringConverter::toString( comp->getModel()->getMainNode()->getOrientation() ), true );	
		mPropSections[hSec]["Scale"] = prop.AddStringItem( hSec, "Scale", Ogre::StringConverter::toString( comp->getModel()->getMainNode()->getScale() ), true );	
	}
}


void CResourceDlg::showFloorProperty( const HouseSceneFloor * floor )
{
	assert( floor );

	clearProperty();

	CPropertyGrid & prop = g_MainFrame->getPropertyDlg();
	::HSECTION hSec = prop.AddSection( "楼层属性" );
	mPropSections[hSec]["NO"] = prop.AddIntegerItem( hSec, "NO", floor->getFloorNO(), "", true );	
	mPropSections[hSec]["UpFloorNO"] = prop.AddIntegerItem( hSec, "上一层", floor->getUpFloor() ? floor->getUpFloor()->getFloorNO() : 0, "", true );	
	mPropSections[hSec]["DownFloorNO"] = prop.AddIntegerItem( hSec, "下一层", floor->getDownFloor() ? floor->getDownFloor()->getFloorNO() : 0, "", true );	
	mPropSections[hSec]["Height"] = prop.AddIntegerItem( hSec, "高度", floor->getFloorHeight(), "", true );		
}


void CResourceDlg::selectGrid( HouseSceneGrid * grid )
{
	for ( HTREEITEM hItem = m_wndResTree.GetChildItem( m_wndResTree.GetRootItem() );	
		NULL != hItem;
		hItem = m_wndResTree.GetNextSiblingItem(hItem) )
	{					
		HouseSceneFloor * p = reinterpret_cast<HouseSceneFloor *>( m_wndResTree.GetItemData( hItem ) );
		if ( p == grid->getCreator() )
		{			
			for ( HTREEITEM hGridList = m_wndResTree.GetChildItem( hItem );	
				NULL != hGridList;
				hGridList = m_wndResTree.GetNextSiblingItem(hGridList) )
			{
				if ( m_wndResTree.GetItemText(hGridList) == "Grids" )
				{
					for ( HTREEITEM hGrid = m_wndResTree.GetChildItem( hGridList );	
						NULL != hGrid;
						hGrid = m_wndResTree.GetNextSiblingItem( hGrid) )
					{
						if ( grid == reinterpret_cast<HouseSceneGrid *>(m_wndResTree.GetItemData( hGrid )) )
						{							
							m_wndResTree.SelectItem( hGrid );
							showGridProperty( grid );
					
							GraphicsSystem::getSingleton().setFocusGrid( grid->getSceneNode() );
							return;
						}				
					}					
				}
			}
			return;
		}		
	}	
}

void CResourceDlg::selectComponent( HouseSceneComponent * comp )
{
	assert( comp );

	for ( HTREEITEM hItem = m_wndResTree.GetRootItem();	
		NULL != hItem;
		hItem = m_wndResTree.GetNextSiblingItem(hItem) )
	{					
		if ( m_wndResTree.GetItemText(hItem) == "Components" )
		{			
			for ( HTREEITEM hComp = m_wndResTree.GetChildItem( hItem );	
				NULL != hComp;
				hComp = m_wndResTree.GetNextSiblingItem(hComp) )
			{
				if ( comp == reinterpret_cast<HouseSceneComponent *>(m_wndResTree.GetItemData( hComp )) )
				{							
					m_wndResTree.SelectItem( hComp );

					BrowseComponentEvent * e = ::MfcInputEventMgr::getSingleton().getEvent<BrowseComponentEvent>();
					if ( e )
					{
						showCompProperty( comp );
						e->setBrowseComponent( comp );
					}

					return;
				}				
			}
			return;
		}		
	}	

}


void CResourceDlg::showGridProperty( const HouseSceneGrid * grid )
{
	assert( grid );		

	clearProperty();

	int x = 0;
	int z = 0;
	grid->getGridNo( x, z );
	
	CPropertyGrid & prop = g_MainFrame->getPropertyDlg();
	::HSECTION hSec = prop.AddSection( "格子属性" );
	mPropSections[hSec]["Type"] = prop.AddIntegerItem( hSec, "类型", grid->getHouseGridTypeRow()->TypeID, "", true );	
	mPropSections[hSec]["TypeName"] = prop.AddStringItem( hSec, "类型名", grid->getHouseGridTypeRow()->Name, true );	
	mPropSections[hSec]["AcceptLay"] = prop.AddStringItem( hSec, "AcceptLay", FormatStr( "%X", grid->getHouseGridTypeRow()->AcceptLay ), true );	
	mPropSections[hSec]["X"] = prop.AddIntegerItem( hSec, "X", x, "", true );	
	mPropSections[hSec]["Z"] = prop.AddIntegerItem( hSec, "Z", z, "", true );	

	mPropSections[hSec]["Blocked"] = prop.AddBoolItem( hSec, "Blocked", grid->isBlocked(), true );	
	hSec = prop.AddSection( "墙" );
	mPropSections[hSec]["Left"] = prop.AddStringItem( hSec, "NX", grid->getWallFace(WALL_FACE_NX) ? "墙" : "", true );	
	mPropSections[hSec]["Right"] = prop.AddStringItem( hSec, " X", grid->getWallFace(WALL_FACE_X) ? "墙" : "", true );	
	mPropSections[hSec]["Up"] = prop.AddStringItem( hSec, "NZ", grid->getWallFace(WALL_FACE_NZ) ? "墙" : "", true );	
	mPropSections[hSec]["Down"] = prop.AddStringItem( hSec, " Z", grid->getWallFace(WALL_FACE_Z) ? "墙" : "", true );	
	hSec = prop.AddSection( "房间" );
	mPropSections[hSec]["Room"] = prop.AddIntegerItem( hSec, "Room", grid->getRoomID(), "", true );	
	mPropSections[hSec]["InDoor"] = prop.AddBoolItem( hSec, "InDoor", grid->getHouseGridTypeRow()->InDoor, true );
}

void CResourceDlg::notifySceneCreate()
{
	resetContent();
	loadTree();
}

void CResourceDlg::notifySceneDestroy()
{
	resetContent();
}


void CResourceDlg::loadTree()
{
	const HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();

	// --- 节点 --- //
	m_wndResTree.DeleteAllItems();


	if ( sce )
	{
		// 楼层列表
		::HTREEITEM hFloorList = m_wndResTree.InsertItem( "Floors" );
		for ( std::map<int, HouseSceneFloor *>::const_iterator it = sce->getFloors().begin();
			it != sce->getFloors().end();
			++it )
		{
			HTREEITEM hFloor = m_wndResTree.InsertItem( Ogre::StringConverter::toString( it->first ).c_str(), hFloorList );
			// 记录指针
			m_wndResTree.SetItemData( hFloor, reinterpret_cast<uint>(it->second) );	


			::HTREEITEM hGridList = m_wndResTree.InsertItem( "Grids", hFloor );
			const HouseSceneFloor * pFloor = it->second;
			// 网格列表
			for ( HouseSceneFloor::GridMap::const_iterator itFloor = pFloor->getGridMap().begin();
				itFloor != pFloor->getGridMap().end();
				++itFloor )
			{
				std::string sGrid;

				sGrid = FormatStr( "(%d,%d)", itFloor->first.x, itFloor->first.y );
				HTREEITEM hGrid = m_wndResTree.InsertItem( sGrid.c_str(), hGridList );
				// 记录指针
				m_wndResTree.SetItemData( hGrid, reinterpret_cast<uint>(itFloor->second) );	

			}
			m_wndResTree.SortChildren( hGridList );	
			m_wndResTree.Expand( hFloor, TVE_EXPAND );		
		}
		m_wndResTree.SortChildren( hFloorList );
		m_wndResTree.Expand( hFloorList, TVE_EXPAND );	

		// 房屋部件列表
		::HTREEITEM hCompList = m_wndResTree.InsertItem( "Components" );
		for ( HSCManager::HouseSceneComponentMap::const_iterator it = HSCManager::getSingleton().getHouseSceneComponentMap().begin();
			it != HSCManager::getSingleton().getHouseSceneComponentMap().end();
			++it )
		{
			std::string compDesc = Ogre::StringConverter::toString( it->first ) + ":" + it->second->getHouseCompPropRow()->Name;
			HTREEITEM hComp = m_wndResTree.InsertItem( compDesc.c_str(), hCompList );
			m_wndResTree.SetItemData( hComp, reinterpret_cast<uint>(it->second));
		}
		m_wndResTree.SortChildren( hCompList );
		m_wndResTree.Expand( hCompList, TVE_EXPAND );	

	}

}

void CResourceDlg::OnAddWall( const HouseSceneGrid * a, const HouseSceneGrid * b )
{
}

void CResourceDlg::OnRemoveWall( const HouseSceneGrid * a, const HouseSceneGrid * b )
{
}

void CResourceDlg::OnAddComponent( const HouseSceneComponent * comp )
{
	for ( HTREEITEM hItem = m_wndResTree.GetRootItem();	
		NULL != hItem;
		hItem = m_wndResTree.GetNextSiblingItem(hItem) )
	{		
		if ( m_wndResTree.GetItemText( hItem ) == "Components" )
		{			
			std::string compDesc = FormatStr( "%d:%s(%s)", comp->getUid(), 
															comp->getHouseCompRow()->Name.c_str(), 
															comp->getHouseCompPropRow()->Name.c_str() );
			HTREEITEM hComp = m_wndResTree.InsertItem( compDesc.c_str(), hItem );
			m_wndResTree.SetItemData( hComp, reinterpret_cast<uint>(comp));
			return;
		}
	}	
}

void CResourceDlg::OnRemoveComponent( const HouseSceneComponent * comp )
{
	for ( HTREEITEM hItem = m_wndResTree.GetRootItem();	
		NULL != hItem;
		hItem = m_wndResTree.GetNextSiblingItem(hItem) )
	{		
		if ( m_wndResTree.GetItemText( hItem ) == "Components" )
		{			
			for ( HTREEITEM hComp = m_wndResTree.GetChildItem(hItem);
				NULL != hComp;
				hComp = m_wndResTree.GetNextSiblingItem(hComp) )
			{
				if ( comp == reinterpret_cast<HouseSceneComponent *>(m_wndResTree.GetItemData( hComp)) )
				{
					m_wndResTree.DeleteItem( hComp );
					m_wndResTree.SelectItem( hItem );
					clearProperty();
					return;
				}				
			}
			return;
		}
	}	
}


void CResourceDlg::OnAddGrid( const HouseSceneFloor * houseFloor, const HouseSceneGrid * grid )
{	
	for ( HTREEITEM hItem = m_wndResTree.GetChildItem( m_wndResTree.GetRootItem() );	
		NULL != hItem;
		hItem = m_wndResTree.GetNextSiblingItem(hItem) )
	{					
		HouseSceneFloor * p = reinterpret_cast<HouseSceneFloor *>( m_wndResTree.GetItemData( hItem ) );
		if ( p == houseFloor )
		{			
			for ( HTREEITEM hGridList = m_wndResTree.GetChildItem( hItem );	
				NULL != hGridList;
				hGridList = m_wndResTree.GetNextSiblingItem(hGridList) )
			{
				if ( m_wndResTree.GetItemText(hGridList) == "Grids" )
				{
					int x = 0;
					int z = 0;
					grid->getGridNo( x, z );
					std::string s = FormatStr( "(%d,%d)", x, z );

					HTREEITEM hChild = m_wndResTree.InsertItem( s.c_str(), hGridList );
					m_wndResTree.SetItemData( hChild, reinterpret_cast<uint>(grid) );
					return;
				}
			}
			return;
		}		
	}
}

void CResourceDlg::OnRemoveGrid( const HouseSceneFloor * houseFloor, const HouseSceneGrid * grid )
{
	for ( HTREEITEM hItem = m_wndResTree.GetChildItem( m_wndResTree.GetRootItem() );	
		NULL != hItem;
		hItem = m_wndResTree.GetNextSiblingItem(hItem) )
	{					
		HouseSceneFloor * p = reinterpret_cast<HouseSceneFloor *>( m_wndResTree.GetItemData( hItem ) );
		if ( p == houseFloor )
		{			
			for ( HTREEITEM hGridList = m_wndResTree.GetChildItem( hItem );	
				NULL != hGridList;
				hGridList = m_wndResTree.GetNextSiblingItem(hGridList) )
			{
				if ( m_wndResTree.GetItemText(hGridList) == "Grids" )
				{
					for ( HTREEITEM hGrid = m_wndResTree.GetChildItem( hGridList );	
						NULL != hGrid;
						hGrid = m_wndResTree.GetNextSiblingItem( hGrid) )
					{
						if ( grid == reinterpret_cast<HouseSceneGrid *>(m_wndResTree.GetItemData( hGrid)) )
						{
							m_wndResTree.DeleteItem( hGrid );
							m_wndResTree.SelectItem( hGridList );
							clearProperty();
							return;
						}				
					}					
				}
			}
			return;
		}		
	}
}


int CResourceDlg::getTreeItemLayer( HTREEITEM hItem ) const
{
	int n = 0;
	for ( ; NULL != hItem; hItem = m_wndResTree.GetParentItem( hItem ) )
	{
		++n;
	}
	return n;
}

void CResourceDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CResourceDlg::resetContent()
{
	clearProperty();
	m_wndResTree.DeleteAllItems();
}

void CResourceDlg::showAmbientSetting()
{
	CPropertyGrid & prop = g_MainFrame->getAmbientDlg();
	for ( PropSectionMap::const_iterator it = mAmbientSections.begin();
		it != mAmbientSections.end();
		++it )
	{
		for ( PropItemMap::const_iterator p = it->second.begin();
			p != it->second.end();
			++p )
		{
			prop.RemoveItem( p->second );
		}
		prop.RemoveSection( it->first );
	}
	mAmbientSections.clear();	
	prop.Invalidate();

	Ogre::ColourValue col;
	Ogre::Vector3 dir;
	::HSECTION hSec = prop.AddSection( "环境光" );
	mAmbientSections[hSec]["DayNight"]		= prop.AddBoolItem( hSec, "是否白天", AmbientMgr::getSingleton().mDayNight, true );
	mAmbientSections[hSec]["ChangeTime"]	= prop.AddIntegerItem( hSec, "昼夜切换时间", AmbientMgr::getSingleton().mChangetime, "", true );
	mAmbientSections[hSec]["AutoTurnOnOff"]	= prop.AddBoolItem( hSec, "自动开关灯", AmbientMgr::getSingleton().mAutoTurnOnOff, true );
	mAmbientSections[hSec]["DayAmbient"]	= prop.AddColorItem( hSec, "白天环境光", OgreColToColRef(AmbientMgr::getSingleton().mDayAmbient), true );
	mAmbientSections[hSec]["DayDiffuse"]	= prop.AddColorItem( hSec, "白天太阳光", OgreColToColRef(AmbientMgr::getSingleton().mDayDiffuse), true );
	mAmbientSections[hSec]["DaySpecular"]	= prop.AddColorItem( hSec, "白天高光", OgreColToColRef(AmbientMgr::getSingleton().mDaySpecular), true );
	mAmbientSections[hSec]["DayMainDir"]	= prop.AddStringItem( hSec, "白天光方向", Vector3ToString(AmbientMgr::getSingleton().mDayMainDir), true );
	mAmbientSections[hSec]["DayFogColor"]	= prop.AddColorItem( hSec, "白天雾颜色", OgreColToColRef(AmbientMgr::getSingleton().mDayFogColor), true );
	mAmbientSections[hSec]["DayFogParam"]	= prop.AddStringItem( hSec, "白天雾参数", Vector4ToString(AmbientMgr::getSingleton().mDayFogParam), true );
	mAmbientSections[hSec]["DaySkyBox"]		= prop.AddStringItem( hSec, "白天天空盒", AmbientMgr::getSingleton().mDaySkybox, true);
	mAmbientSections[hSec]["NightAmbient"]	= prop.AddColorItem( hSec, "晚上环境光", OgreColToColRef(AmbientMgr::getSingleton().mNightAmbient), true );
	mAmbientSections[hSec]["NightDiffuse"]	= prop.AddColorItem( hSec, "晚上太阳光", OgreColToColRef(AmbientMgr::getSingleton().mNightDiffuse), true );
	mAmbientSections[hSec]["NightSpecular"]	= prop.AddColorItem( hSec, "晚上高光", OgreColToColRef(AmbientMgr::getSingleton().mNightSpecular), true );
	mAmbientSections[hSec]["NightMainDir"]	= prop.AddStringItem( hSec, "晚上光方向", Vector3ToString(AmbientMgr::getSingleton().mNightMainDir), true );
	mAmbientSections[hSec]["NightFogColor"]	= prop.AddColorItem( hSec, "晚上雾颜色", OgreColToColRef(AmbientMgr::getSingleton().mNightFogColor), true );
	mAmbientSections[hSec]["NightFogParam"]	= prop.AddStringItem( hSec, "晚上雾参数", Vector4ToString(AmbientMgr::getSingleton().mNightFogParam), true );
	mAmbientSections[hSec]["NightSkyBox"]	= prop.AddStringItem( hSec, "晚上天空盒", AmbientMgr::getSingleton().mNightSkybox, true);
	mAmbientSections[hSec]["Contrast"]		= prop.AddDoubleItem( hSec, "对比度", AmbientMgr::getSingleton().mContrast, "", true );	
}

void CResourceDlg::showPostProcessSetting()
{
	CPropertyGrid & prop = g_MainFrame->getPostProcessDlg();
	for ( PropSectionMap::const_iterator it = mPostProcessSections.begin();
		it != mPostProcessSections.end();
		++it )
	{
		for ( PropItemMap::const_iterator p = it->second.begin();
			p != it->second.end();
			++p )
		{
			prop.RemoveItem( p->second );
		}
		prop.RemoveSection( it->first );
	}
	mPostProcessSections.clear();	
	prop.Invalidate();

	::HSECTION hSec = prop.AddSection( "特效" );

	mPostProcessSections[hSec]["Gaussian Blur"]	= prop.AddBoolItem( hSec, "模糊", false, true );
//	mPostProcessSections[hSec]["Contrast"]		= prop.AddBoolItem( hSec, "对比度", false, true );
// 	mPostProcessSections[hSec]["Bloom"]			= prop.AddBoolItem( hSec, "全屏柔光", false, true );
// 	mPostProcessSections[hSec]["Glass"]			= prop.AddBoolItem( hSec, "玻璃", false, true );
// 	mPostProcessSections[hSec]["Old TV"]		= prop.AddBoolItem( hSec, "旧电视", false, true );
// 	mPostProcessSections[hSec]["B&W"]			= prop.AddBoolItem( hSec, "黑白", false, true );
// 	mPostProcessSections[hSec]["Embossed"]		= prop.AddBoolItem( hSec, "浮雕", false, true );
// 	mPostProcessSections[hSec]["Sharpen Edges"]	= prop.AddBoolItem( hSec, "边缘锐化", false, true );
// 	mPostProcessSections[hSec]["Invert"]		= prop.AddBoolItem( hSec, "反色", false, true );
// 	mPostProcessSections[hSec]["Posterize"]		= prop.AddBoolItem( hSec, "色调分离", false, true );
// 	mPostProcessSections[hSec]["Laplace"]		= prop.AddBoolItem( hSec, "拉普拉斯", false, true );
// 	mPostProcessSections[hSec]["Tiling"]		= prop.AddBoolItem( hSec, "贴砖", false, true );
// 	mPostProcessSections[hSec]["Old Movie"]		= prop.AddBoolItem( hSec, "旧电影", false, true );
// 	mPostProcessSections[hSec]["HDR"]			= prop.AddBoolItem( hSec, "全屏泛光", false, true );
// 	mPostProcessSections[hSec]["Radial Blur"]	= prop.AddBoolItem( hSec, "放射", false, true );
// 	mPostProcessSections[hSec]["ASCII"]			= prop.AddBoolItem( hSec, "ASCII", false, true );
// 	mPostProcessSections[hSec]["Halftone"]		= prop.AddBoolItem( hSec, "半色", false, true );
// 	mPostProcessSections[hSec]["Night Vision"]	= prop.AddBoolItem( hSec, "夜视", false, true );
// 	mPostProcessSections[hSec]["Dither"]		= prop.AddBoolItem( hSec, "扰动", false, true );
// 	mPostProcessSections[hSec]["Motion Blur"]	= prop.AddBoolItem( hSec, "运动模糊", false, true );
// 	mPostProcessSections[hSec]["Heat Vision"]	= prop.AddBoolItem( hSec, "热浪", false, true );
}

void CResourceDlg::showLightProperty(PickObject* po)
{
	mLightPo = po;

	CPropertyGrid & prop = g_MainFrame->getLightDlg();
	for ( PropSectionMap::const_iterator it = mLightSections.begin();
		it != mLightSections.end();
		++it )
	{
		for ( PropItemMap::const_iterator p = it->second.begin();
			p != it->second.end();
			++p )
		{
			prop.RemoveItem( p->second );
		}
		prop.RemoveSection( it->first );
	}
	mLightSections.clear();	
	prop.Invalidate();
	resetLightHelp(0);
	resetLightHelp(1);

	uint lightid;
	HouseSceneComponent* comp=NULL;
	SceneEntityObject* obj=NULL;
	if (po->getClassName() == "HouseSceneComponent")
	{
		comp = dynamic_cast<HouseSceneComponent*>(po);
		lightid = comp->getHouseCompPropRow()->LightID;
	}
	else if(po->getClassName() == "SceneEntityObject")
	{
		obj = dynamic_cast<SceneEntityObject*>(po);
		lightid = obj->getLightId();
	}

	const LightRow* lr = GraphicsTableManager::getSingleton().getLightRow(lightid);
	if(!lr)
		return;

	if(lr->Light1)
	{
		const ActiveLightRow* alr = lr->Light1;
		ZoneLight* zl = comp? comp->getLight()[0] : obj->getLight()[0];
		Ogre::Light* l = zl->getLight();

		std::string str;
		vector<Ogre::String> vstr;
		::HSECTION hActive1 = prop.AddSection( "主动光1" );
		mLightSections[hActive1]["ID"]			= prop.AddIntegerItem( hActive1, "ID", alr->ID, "", false );
		mLightSections[hActive1]["Name"]		= prop.AddStringItem( hActive1, "名字", alr->Name.c_str(), false );
		vstr.clear(); vstr.push_back("点光源"); vstr.push_back("聚光灯");
		mLightSections[hActive1]["LightType"]	= prop.AddComboItem( hActive1, "灯光类型", vstr, l->getType()?1:0, true );
		mLightSections[hActive1]["DiffCol"]		= prop.AddColorItem( hActive1, "漫反射颜色", OgreColToColRef(l->getDiffuseColour()), true );
		mLightSections[hActive1]["SpecCol"]		= prop.AddColorItem( hActive1, "镜面反射颜色", OgreColToColRef(l->getSpecularColour()), true );
		str = FormatStr("%.1f, %.2f, %.4f %.6f", l->getAttenuationRange(), l->getAttenuationConstant(), l->getAttenuationLinear(), l->getAttenuationQuadric());
		mLightSections[hActive1]["Attenuation"]	= prop.AddStringItem( hActive1, "衰减系数", str.c_str(), true );
		mLightSections[hActive1]["Pos"]			= prop.AddStringItem( hActive1, "坐标", Vector3ToString(zl->getPos()).c_str(), true );
		mLightSections[hActive1]["Dir"]		= prop.AddStringItem( hActive1, "方向", Vector3ToString(l->getDirection()).c_str(), l->getType()?true:false );
		str = FormatStr("%.1f, %.1f, %.1f", Ogre::Math::RadiansToDegrees(l->getSpotlightInnerAngle().valueRadians()), Ogre::Math::RadiansToDegrees(l->getSpotlightOuterAngle().valueRadians()), l->getSpotlightFalloff());
		mLightSections[hActive1]["SpotRange"]	= prop.AddStringItem( hActive1, "聚光范围", str.c_str(), l->getType()?true:false );
		vstr.clear(); vstr.push_back("无穿透"); vstr.push_back("穿透室外"); vstr.push_back("穿透室内");
		mLightSections[hActive1]["Pierce"]		= prop.AddComboItem( hActive1, "穿透类型", vstr, zl->getPierceType(), l->getType()?false:true );
		mLightSections[hActive1]["PierceRange"]	= prop.AddIntegerItem( hActive1, "穿透距离", zl->getPierceRange(), "", l->getType()?false:true );
		mLightSections[hActive1]["DrawHelper"]	= prop.AddBoolItem( hActive1, "绘制辅助线", mDrawHelp[0], true );
		mLightSections[hActive1]["Visible"]		= prop.AddBoolItem( hActive1, "开启灯光", l->getVisible(), true );
		mLightSections[hActive1]["Adjuster"]	= prop.AddBoolItem( hActive1, "开启调节助手", false, true );

		drawLightHelper(l, 0);
	}

	if(lr->Light2)
	{
		const ActiveLightRow* alr = lr->Light2;
		ZoneLight* zl = comp? comp->getLight()[1] : obj->getLight()[1];
		Ogre::Light* l = zl->getLight();

		std::string str;
		vector<Ogre::String> vstr;
		::HSECTION hActive2 = prop.AddSection( "主动光2" );
		mLightSections[hActive2]["ID"]			= prop.AddIntegerItem( hActive2, "ID", alr->ID, "", false );
		mLightSections[hActive2]["Name"]		= prop.AddStringItem( hActive2, "名字", alr->Name.c_str(), false );
		vstr.clear(); vstr.push_back("点光源"); vstr.push_back("聚光灯");
		mLightSections[hActive2]["LightType"]	= prop.AddComboItem( hActive2, "灯光类型", vstr, l->getType()?1:0, true );
		mLightSections[hActive2]["DiffCol"]		= prop.AddColorItem( hActive2, "漫反射颜色", OgreColToColRef(l->getDiffuseColour()), true );
		mLightSections[hActive2]["SpecCol"]		= prop.AddColorItem( hActive2, "镜面反射颜色", OgreColToColRef(l->getSpecularColour()), true );
		str = FormatStr("%.1f, %.2f, %.4f %.6f", l->getAttenuationRange(), l->getAttenuationConstant(), l->getAttenuationLinear(), l->getAttenuationQuadric());
		mLightSections[hActive2]["Attenuation"]	= prop.AddStringItem( hActive2, "衰减系数", str.c_str(), true );
		mLightSections[hActive2]["Pos"]			= prop.AddStringItem( hActive2, "坐标", Vector3ToString(zl->getPos()).c_str(), true );
		mLightSections[hActive2]["Dir"]		= prop.AddStringItem( hActive2, "方向", Vector3ToString(l->getDirection()).c_str(), l->getType()?true:false );
		str = FormatStr("%.1f, %.1f, %.1f", Ogre::Math::RadiansToDegrees(l->getSpotlightInnerAngle().valueRadians()), Ogre::Math::RadiansToDegrees(l->getSpotlightOuterAngle().valueRadians()), l->getSpotlightFalloff());
		mLightSections[hActive2]["SpotRange"]	= prop.AddStringItem( hActive2, "聚光范围", str.c_str(), l->getType()?true:false );
		vstr.clear(); vstr.push_back("无穿透"); vstr.push_back("穿透室外"); vstr.push_back("穿透室内");
		mLightSections[hActive2]["Pierce"]		= prop.AddComboItem( hActive2, "穿透类型", vstr, zl->getPierceType(), l->getType()?false:true );
		mLightSections[hActive2]["PierceRange"]	= prop.AddIntegerItem( hActive2, "穿透距离", zl->getPierceRange(), "", l->getType()?false:true );
		mLightSections[hActive2]["DrawHelper"]	= prop.AddBoolItem( hActive2, "绘制辅助线", mDrawHelp[1], true );
		mLightSections[hActive2]["Visible"]		= prop.AddBoolItem( hActive2, "开启灯光", l->getVisible(), true );
		mLightSections[hActive2]["Adjuster"]	= prop.AddBoolItem( hActive2, "开启调节助手", false, true );

		drawLightHelper(l, 1);
	}

	if(lr->PassLi1)
	{
		const PassLightRow* plr = lr->PassLi1;
		ZoneLight* zl = comp->getLight()[0];

		std::string str;
		::HSECTION hPass1 = prop.AddSection( "穿透光1" );
		mLightSections[hPass1]["ID"]			= prop.AddIntegerItem( hPass1, "ID", plr->ID, "", false );
		mLightSections[hPass1]["Name"]			= prop.AddStringItem( hPass1, "名字", plr->Name, false );
		mLightSections[hPass1]["Indoor"]		= prop.AddBoolItem( hPass1, "是否室内光", zl->getPassLightIndoor(), true );
		str = FormatStr("%.1f, %.2f, %.4f %.6f", zl->getAttenuation0(true), zl->getAttenuation1(true), zl->getAttenuation2(true), zl->getAttenuation3(true));
		mLightSections[hPass1]["P_DayAtten"]	= prop.AddStringItem( hPass1, "白天衰减", str.c_str(), true );
		str = FormatStr("%.1f, %.2f, %.4f %.6f", zl->getAttenuation0(false), zl->getAttenuation1(false), zl->getAttenuation2(false), zl->getAttenuation3(false));
		mLightSections[hPass1]["P_NightAtten"]	= prop.AddStringItem( hPass1, "晚上衰减", str.c_str(), true );
		mLightSections[hPass1]["P_Pos"]			= prop.AddStringItem( hPass1, "坐标", Vector3ToString(zl->getPos()).c_str(), true );
		mLightSections[hPass1]["DrawHelper"]	= prop.AddBoolItem( hPass1, "绘制辅助线", mDrawHelp[0], true );
		mLightSections[hPass1]["Adjuster"]		= prop.AddBoolItem( hPass1, "开启调节助手", false, true );

		drawLightHelper(zl->getLight(), 0);
	}

	if(lr->PassLi2)
	{
		const PassLightRow* plr = lr->PassLi2;
		ZoneLight* zl = comp->getLight()[1];

		std::string str;
		::HSECTION hPass2 = prop.AddSection( "穿透光2" );
		mLightSections[hPass2]["ID"]			= prop.AddIntegerItem( hPass2, "ID", plr->ID, "", false );
		mLightSections[hPass2]["Name"]			= prop.AddStringItem( hPass2, "名字", plr->Name, false );
		mLightSections[hPass2]["Indoor"]		= prop.AddBoolItem( hPass2, "是否室内光", zl->getPassLightIndoor(), true );
		str = FormatStr("%.1f, %.2f, %.4f %.6f", zl->getAttenuation0(true), zl->getAttenuation1(true), zl->getAttenuation2(true), zl->getAttenuation3(true));
		mLightSections[hPass2]["P_DayAtten"]	= prop.AddStringItem( hPass2, "白天衰减", str.c_str(), true );
		str = FormatStr("%.1f, %.2f, %.4f %.6f", zl->getAttenuation0(false), zl->getAttenuation1(false), zl->getAttenuation2(false), zl->getAttenuation3(false));
		mLightSections[hPass2]["P_NightAtten"]	= prop.AddStringItem( hPass2, "晚上衰减", str.c_str(), true );
		mLightSections[hPass2]["P_Pos"]			= prop.AddStringItem( hPass2, "坐标", Vector3ToString(zl->getPos()).c_str(), true );
		mLightSections[hPass2]["DrawHelper"]	= prop.AddBoolItem( hPass2, "绘制辅助线", mDrawHelp[1], true );
		mLightSections[hPass2]["Adjuster"]		= prop.AddBoolItem( hPass2, "开启调节助手", false, true );

		drawLightHelper(zl->getLight(), 1);
	}
}

void CResourceDlg::drawCircle(Ogre::ManualObject* manual, Ogre::Real radius, Ogre::Real zOffset)
{
	for (unsigned int i = 0; i <= 32; i++)
	{
		Ogre::Real x = Ogre::Math::Cos(Ogre::Radian((Ogre::Math::TWO_PI*i)/32)) * radius;
		Ogre::Real y = Ogre::Math::Sin(Ogre::Radian((Ogre::Math::TWO_PI*i)/32)) * radius;
		manual->position(x, y, zOffset);
	}
}

void CResourceDlg::drawSphere(Ogre::ManualObject* manual, const float r, const int nRings, const int nSegments)
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

void CResourceDlg::drawLightHelper(Ogre::Light* light, int n)
{
	if(!light || !light->getVisible())
		return;

	static int index=0;

	//辅助线创建个自己的node
	mHelpNode[n] = light->getParentSceneNode()->createChildSceneNode();

	//贴个公告板便于查看灯光位置
	mLightBbs[n] = GraphicsSystem::getSingleton().getSceneMgr()->createBillboardSet(light->getName()+"bbs"+Ogre::StringConverter::toString(index));
	mLightBbs[n]->setMaterialName("LightBbs");
	mLightBb[n] = mLightBbs[n]->createBillboard(0,0,0,Ogre::ColourValue(1,1,1));
	mHelpNode[n]->attachObject(mLightBbs[n]);

	//绘制灯光辅助线
	mManualobject[n] =  GraphicsSystem::getSingleton().getSceneMgr()->createManualObject(light->getName()+"manual"+Ogre::StringConverter::toString(index++));
	mHelpNode[n]->attachObject(mManualobject[n]);

	float offset = 0.65f;
	float range = light->getAttenuationRange();

	if(light->getType() == Ogre::Light::LT_POINT)
	{
		drawSphere(mManualobject[n], range, 24, 24);
	}
	else if(light->getType() == Ogre::Light::LT_SPOTLIGHT)
	{
		mHelpNode[n]->setDirection(light->getDirection(), Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);

		mManualobject[n]->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real firstInnerRadius = Ogre::Math::Tan(light->getSpotlightInnerAngle()/2) * offset;
		drawCircle(mManualobject[n], firstInnerRadius, offset);
		mManualobject[n]->end();

		mManualobject[n]->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real secondInnerRadius = Ogre::Math::Tan(light->getSpotlightInnerAngle()/2) * range;
		drawCircle(mManualobject[n], secondInnerRadius, range);
		mManualobject[n]->end();

		mManualobject[n]->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real firstOuterRadius = Ogre::Math::Tan(light->getSpotlightOuterAngle()/2) * offset;
		drawCircle(mManualobject[n], firstOuterRadius, offset);
		mManualobject[n]->end();

		mManualobject[n]->begin("LightHelper", Ogre::RenderOperation::OT_LINE_STRIP);
		Ogre::Real secondOuterRadius = Ogre::Math::Tan(light->getSpotlightOuterAngle()/2) * range;
		drawCircle(mManualobject[n], secondOuterRadius, range);
		mManualobject[n]->end();

		mManualobject[n]->begin("LightHelper", Ogre::RenderOperation::OT_LINE_LIST);
		mManualobject[n]->position(firstInnerRadius, 0, offset);
		mManualobject[n]->position(secondInnerRadius, 0, range);
		mManualobject[n]->position(0, firstInnerRadius, offset);
		mManualobject[n]->position(0, secondInnerRadius, range);
		mManualobject[n]->position(0, -firstInnerRadius, offset);
		mManualobject[n]->position(0, -secondInnerRadius, range);
		mManualobject[n]->position(-firstInnerRadius, 0, offset);
		mManualobject[n]->position(-secondInnerRadius, 0, range);
		mManualobject[n]->position(firstOuterRadius, 0, offset);
		mManualobject[n]->position(secondOuterRadius, 0, range);
		mManualobject[n]->position(0, firstOuterRadius, offset);
		mManualobject[n]->position(0, secondOuterRadius, range);
		mManualobject[n]->position(0, -firstOuterRadius, offset);
		mManualobject[n]->position(0, -secondOuterRadius, range);
		mManualobject[n]->position(-firstOuterRadius, 0, offset);
		mManualobject[n]->position(-secondOuterRadius, 0, range);
		mManualobject[n]->end();
	}
}

void CResourceDlg::resetLightHelp(int n)
{
	if(mLightBbs[n])
	{
		mLightBbs[n]->removeBillboard(mLightBb[n]);
		mLightBb[n] = NULL;
		GraphicsSystem::getSingleton().getSceneMgr()->destroyBillboardSet(mLightBbs[n]);
		mLightBbs[n] = NULL;
	}

	if(mManualobject[n])
	{
		GraphicsSystem::getSingleton().getSceneMgr()->destroyManualObject(mManualobject[n]);
		mManualobject[n] = NULL;
		//mHelpNode[n]->getParentSceneNode()->removeAndDestroyChild(mHelpNode[n]->getName());
	}
}
