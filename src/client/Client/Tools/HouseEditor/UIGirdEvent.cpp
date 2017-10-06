#include "HouseEditorPCH.h"

#include "UIGirdEvent.h"
#include "SceneManager.h"
#include "HouseGridDlg.h"
#include "EQMain.h"
#include "MfcInputEventMgr.h"
#include "SystemGcs.h"
#include "MainFrm.h"
#include "ResourceDlg.h"
#include "GraphicsSystem.h"
#include "HSCManager.h"
#include "HouseScene.h"

//-----------------------------------------------------------------------
static Ogre::Ray getRay( CPoint point )
{
	Ogre::Camera * cam = GraphicsSystem::getSingleton().getCamera();

	int w = cam->getViewport()->getActualWidth();
	int h = cam->getViewport()->getActualHeight();

	Ogre::Ray ray = cam->getCameraToViewportRay((float)point.x/w, (float)point.y/h);

	return ray;
}
//-----------------------------------------------------------------------
CreateGridEvent::CreateGridEvent() 
	: MfcInputEvent( epCreateGrid )
{
}
//-----------------------------------------------------------------------
CreateGridEvent::~CreateGridEvent()
{
	if ( g_HouseGridDlg->m_wndCreateGrid.GetCheck() != BST_UNCHECKED )
		g_HouseGridDlg->m_wndCreateGrid.SetCheck(BST_UNCHECKED);
}
//-----------------------------------------------------------------------
CreateGridEvent * CreateGridEvent::create()
{
	return new CreateGridEvent();
}
//-----------------------------------------------------------------------
bool CreateGridEvent::exec(CPoint point)
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	assert(sce); //上层确保这个存在

	// 取得当前楼层
	HouseSceneFloor * houseFloor = sce->getWalkViewFloor();
	assert(houseFloor); //上层确保这个存在

	if ( GetAsyncKeyState(VK_LBUTTON) & 0x8000 )
	{
		int x;
		int z;
		sce->getXZ( x, z, getRay( point ) );
		HouseSceneGrid * grid = houseFloor->getGrid( x, z );


		const int num = sce->getFloorAssistGridNum();
		//只能在指定范围生成
		if ( x < num && x >= -num	
			&& z < num && z >= -num && 
			!grid )
		{
			grid = houseFloor->createGrid(x, z, g_HouseGridDlg->getGridTypeID());
			//grid->calcGridDisplayMode(g_BottomCommDlg->getGridDisplayMode());
			grid->getCreator()->notifyGridChanged();

			g_ResourceDlg->OnAddGrid( houseFloor, grid );
		}

		if ( grid )
			g_ResourceDlg->selectGrid( grid );

		return true;		
	}
	else if ( GetAsyncKeyState(VK_RBUTTON) & 0x8000 )
	{
		HouseSceneGrid * grid;
		Ogre::Vector3 cp;
		sce->rayGrid( getRay(point), grid, cp);

		if ( grid && grid->getCreator() == houseFloor )
		{
			if ( grid->hasWallFace() )
			{
				g_MainFrame->showError( "请先删除墙体阻挡!" ); 
			}
			else if ( grid->hasComponents() )
			{
				g_MainFrame->showError( "请先删除格子上的物体!" ); 
			}
			else //删除
			{
				g_ResourceDlg->OnRemoveGrid( houseFloor, grid );
				houseFloor->destroyGrid( grid->getGridX(), grid->getGridZ() );
				houseFloor->notifyGridChanged();
			}
		}

		return true;
	}

	return false;
}
//-----------------------------------------------------------------------
void CreateGridEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	exec(point);
	processed = true;
}
//-----------------------------------------------------------------------
void CreateGridEvent::RButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	exec(point);
	processed = true;
}
//-----------------------------------------------------------------------
void CreateGridEvent::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	exec(point);
	processed = true;
}
//-----------------------------------------------------------------------
void CreateGridEvent::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		MfcInputEventMgr::getSingleton().delEvent<CreateGridEvent>();
		processed = true;
	}
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
GridLineEvent::GridLineEvent( int pri, bool bMoreOp ) 
	: MfcInputEvent(pri)
	, mLining(false)
	, mLineObjNode(NULL)
	, mLineObj(NULL)
	, mStartGrid(NULL)
	, mEndGrid(NULL)
	, mPicking(false)
	, mMoreOp( bMoreOp )
{
}
//-----------------------------------------------------------------------
GridLineEvent::~GridLineEvent()
{
	lineEnd();
}
//-----------------------------------------------------------------------
void GridLineEvent::lineStart(CPoint pt)
{
	if ( mLining )
		return;

	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	
	HouseSceneGrid * grid;
	Ogre::Vector3 cp;

	sce->rayGrid( getRay( pt ), grid, cp );

	if ( grid )
	{
		mStartLinePos = cp;
		mStartGrid = grid;
		mLining = true;

		
		if ( mMoreOp )
			mPicking = true;
	}
}
//-----------------------------------------------------------------------
void GridLineEvent::lineMove(CPoint pt)
{
	if ( !mLining )
		return ;

	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	
	HouseSceneGrid * grid;
	Ogre::Vector3 cp;

	sce->rayGrid( getRay( pt ), grid, cp );

	if ( grid )
	{	
		mEndLinePos = cp;
		mEndGrid = grid;
		lineDraw();
	}
}
//-----------------------------------------------------------------------
void GridLineEvent::lineDraw()
{
	_lineDestroy(false);

	Ogre::ColourValue color(1,0,0);

	assert(mLineObj==NULL);
	mLineObj = SystemGcs::getSingleton().getSceneManager()->createManualObject("-WallGridEvent-Line-");

	mLineObj->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

	mLineObj->colour(color);
	mLineObj->position(mStartLinePos);
	mLineObj->position(mEndLinePos);

	mLineObj->index(0);
	mLineObj->index(1);

	mLineObj->end();

	//
	if ( !mLineObjNode )
		mLineObjNode = SystemGcs::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode();

	mLineObjNode->attachObject(mLineObj);
}
//-----------------------------------------------------------------------
void GridLineEvent::_lineDestroy(bool withNode )
{
	if ( mLineObj )
	{
		mLineObj->detatchFromParent();
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mLineObj);
		mLineObj = NULL;
	}

	if ( withNode && mLineObjNode )
	{
		mLineObjNode->removeAndDestroyAllChildren();
		mLineObjNode->getCreator()->destroySceneNode(mLineObjNode);
		mLineObjNode = NULL;
	}
}
//-----------------------------------------------------------------------
bool GridLineEvent::lineEnd()
{
	if ( mMoreOp )
	{
		if ( mPicking )
		{
			mPicking = false;
			return false;
		}
	}

	_lineDestroy();

	mLining = false;
	return true;
}
//-----------------------------------------------------------------------
void GridLineEvent::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		MfcInputEventMgr::getSingleton().delEvent<GridLineEvent>();
		processed = true;
	}
}
//-----------------------------------------------------------------------
void GridLineEvent::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	lineMove(point);
	processed = true;
}
//-----------------------------------------------------------------------
void GridLineEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	lineStart(point);
	processed = true;
}
//-----------------------------------------------------------------------
void GridLineEvent::LButtonUp( UINT nFlags, CPoint point, bool & processed )
{
	if ( !lineEnd() )
		return;

	exce(true);
	processed = true;
}
//-----------------------------------------------------------------------
void GridLineEvent::RButtonDown(UINT nFlags, CPoint point, bool & processed )
{
	lineStart(point);
	processed = true;
}
//-----------------------------------------------------------------------
void GridLineEvent::RButtonUp( UINT nFlags, CPoint point, bool & processed )
{
	if ( !lineEnd() )
		return;

	exce(false);
	processed = true;
}
//-----------------------------------------------------------------------



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
WallGridEvent::WallGridEvent( ) 
	: GridLineEvent(epWallGrid, false)
{
}
//-----------------------------------------------------------------------
WallGridEvent::~WallGridEvent()
{
	if ( g_HouseGridDlg->m_wndWallGrid.GetCheck() != BST_UNCHECKED )
		g_HouseGridDlg->m_wndWallGrid.SetCheck(BST_UNCHECKED);
}
//-----------------------------------------------------------------------
WallGridEvent * WallGridEvent::create()
{
	return new WallGridEvent();
}
//-----------------------------------------------------------------------
void WallGridEvent::exce( bool bAdded )
{
	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
	HouseSceneGrid * a = mStartGrid;
	HouseSceneGrid * b = mEndGrid;

	if ( a && b )
	{
		if ( !a->hasComponents() && !b->hasComponents() )
		{
			int face;
			if ( a->isNeighborGrid(b, face) )
			{
				a->setWallFace(face, bAdded);
				a->getCreator()->notifyGridChanged();

				if ( bAdded )
				{
					g_ResourceDlg->OnAddWall( a, b );
				}
				else
				{
					g_ResourceDlg->OnRemoveWall( a, b );
				}
			}
		}
		else
		{
			g_MainFrame->showError( "请先删除格子上的物体!" ); 
		}

		g_ResourceDlg->selectGrid(a);
	}
}



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
BrowseGridEvent::BrowseGridEvent() 
	: MfcInputEvent(epBrowseGrid)
{
	g_ResourceDlg->clearProperty();
	GraphicsSystem::getSingleton().showFocusGrid( true );
}
//-----------------------------------------------------------------------
BrowseGridEvent::~BrowseGridEvent()
{
	GraphicsSystem::getSingleton().showFocusGrid( false );
	g_MainFrame->UnCheckedToolBarItem( ID_BUTTON_GRID );
	g_ResourceDlg->clearProperty();
}
//-----------------------------------------------------------------------
BrowseGridEvent * BrowseGridEvent::create()
{
	return new BrowseGridEvent();
}
//-----------------------------------------------------------------------
void BrowseGridEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	exec( point );
	processed = true;
}
//-----------------------------------------------------------------------
bool BrowseGridEvent::exec(CPoint point)
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	assert(sce); //上层确保这个存在

	HouseSceneGrid * grid;
	Ogre::Vector3 cp;
	sce->rayGrid( getRay(point), grid, cp);

	if ( grid )
		g_ResourceDlg->selectGrid( grid );


	return true;
}
//-----------------------------------------------------------------------




//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
CreateLinkGridEvent::CreateLinkGridEvent() 
	: GridLineEvent(epLinkGrid, true)
{
}
//-----------------------------------------------------------------------
CreateLinkGridEvent::~CreateLinkGridEvent()
{
	if ( g_HouseGridDlg->m_wndLinkGrid.GetCheck() != BST_UNCHECKED )
		g_HouseGridDlg->m_wndLinkGrid.SetCheck(BST_UNCHECKED);
}
//-----------------------------------------------------------------------
CreateLinkGridEvent * CreateLinkGridEvent::create()
{
	return new CreateLinkGridEvent();
}
//-----------------------------------------------------------------------
void CreateLinkGridEvent::exce( bool bAdded )
{
	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
	HouseSceneGrid * a = mStartGrid;
	HouseSceneGrid * b = mEndGrid;

	if ( a && b )
	{
		// 不同楼层才能建立梯子连通信息
		// 同一楼层只能在不同房间 建立门连通信息

		int face;
		bool bOk = (a->isNeighborGrid( b, face ) && a->getRoomID() != b->getRoomID() );

		bOk = bOk || ( a->getCreator() != b->getCreator() );


		if ( bOk )
		{
			if ( bAdded )
			{
				sce->addStair( a, b );
			}
			else
			{
				sce->removeStair( a, b );
			}
		}		
	}
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
EditRoomEvent::EditRoomEvent() 
	: MfcInputEvent( epSetRoomID )
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	sce->getWalkViewFloor()->showRoomIDPlane( true );
}
//-----------------------------------------------------------------------
EditRoomEvent::~EditRoomEvent()
{
	if ( g_HouseGridDlg->m_wndEditRoom.GetCheck() != BST_UNCHECKED )
		g_HouseGridDlg->m_wndEditRoom.SetCheck(BST_UNCHECKED);
}
//-----------------------------------------------------------------------
EditRoomEvent * EditRoomEvent::create()
{
	return new EditRoomEvent();
}
//-----------------------------------------------------------------------
bool EditRoomEvent::exec(CPoint point)
{
	int room = g_HouseGridDlg->getRoomID();

	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	assert(sce); //上层确保这个存在

	// 取得当前楼层
	HouseSceneFloor * houseFloor = sce->getWalkViewFloor();
	assert(houseFloor); //上层确保这个存在

	if ( GetAsyncKeyState(VK_LBUTTON) & 0x8000 )
	{
		int x;
		int z;
		sce->getXZ( x, z, getRay( point ) );
		HouseSceneGrid * grid = houseFloor->getGrid( x, z );

		const int num = sce->getFloorAssistGridNum();
		if ( x < num && x >= -num	
			&& z < num && z >= -num && 
			grid && grid->getRoomID() != room )
		{
			grid->setRoomID(room);
			grid->getCreator()->notifyRoomIDChanged();
		}

		return true;		
	}
	else if ( GetAsyncKeyState(VK_RBUTTON) & 0x8000 )
	{
		HouseSceneGrid * grid;
		Ogre::Vector3 cp;
		sce->rayGrid( getRay(point), grid, cp);

		if ( grid && grid->getCreator() == houseFloor )
		{
			grid->setRoomID(0);
			grid->getCreator()->notifyRoomIDChanged();
		}

		return true;
	}

	return false;
}
//-----------------------------------------------------------------------
void EditRoomEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	exec(point);
	processed = true;
}
//-----------------------------------------------------------------------
void EditRoomEvent::RButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	exec(point);
	processed = true;
}
//-----------------------------------------------------------------------
void EditRoomEvent::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	exec(point);
	processed = true;
}
//-----------------------------------------------------------------------
void EditRoomEvent::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		MfcInputEventMgr::getSingleton().delEvent<CreateGridEvent>();
		processed = true;
	}
}
//-----------------------------------------------------------------------



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
TestAutoFindPathEvent::TestAutoFindPathEvent() 
	: GridLineEvent(epTestAutoFindPath, true)
	, mPathObj(NULL)
{
}
//-----------------------------------------------------------------------
TestAutoFindPathEvent::~TestAutoFindPathEvent()
{
	clearPath();
}
//-----------------------------------------------------------------------
TestAutoFindPathEvent * TestAutoFindPathEvent::create()
{
	return new TestAutoFindPathEvent();
}
//-----------------------------------------------------------------------
void TestAutoFindPathEvent::exce( bool bAdded )
{
	HouseSceneGrid * a = mStartGrid;
	HouseSceneGrid * b = mEndGrid;

	if ( a && b )
	{
		clearPath();
		searchPath();
		drawPath();
	}
}
//-----------------------------------------------------------------------
void TestAutoFindPathEvent::clearPath()
{
	mPosArray.clear();
	_destroyManualObject( mPathObj );
	mPathObj = NULL;
}
//-----------------------------------------------------------------------
void TestAutoFindPathEvent::searchPath()
{
	//assert( mStartGrid );
	//assert( mEndGrid );
	//	
	//// 调用寻路函数取得路径
	//SearchSystem::Path result;
	//if(SearchSystem::SearchSystem::getSingleton().findPath( mStartGrid->getGridX(), 
	//						mStartGrid->getGridZ(), 
	//						mStartGrid->getRoomID(),
	//						mStartGrid->getCreator()->getFloorNO(),
	//						mEndGrid->getGridX(),
	//						mEndGrid->getGridZ(), 
	//						mEndGrid->getRoomID() ,
	//						mEndGrid->getCreator()->getFloorNO(),
	//						result
	//			))
	//{
	//	// 添加第一个格子
	//	int startRoomId = mStartGrid->getCreator()->getFloorNO() * 100 + mStartGrid->getRoomID();
	//	result.push_front(SearchSystem::Point(mStartGrid->getGridX(), startRoomId, mStartGrid->getGridZ()));
	//}
	//else
	//	return;

	//for (SearchSystem::Path::const_iterator it = result.begin();
	//	it != result.end();
	//	++it )
	//{
	//	const int roomID = it->_y;
	//	const int y = SearchSystem::SearchSystem::getSingleton().getFloorNum(roomID);
	//	mPosArray.push_back( TPosition( it->_x, y, it->_z) );

	//}
}
//-----------------------------------------------------------------------
void TestAutoFindPathEvent::drawPath()
{
	if ( mPosArray.empty() )
		return;

	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();

	Ogre::SceneManager * mgr = GraphicsSystem::getSingleton().getSceneMgr();

	mPathObj = mgr->createManualObject( "_TestAutoFindPathObj_" );
	
	mPathObj->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP );
	mPathObj->colour( Ogre::ColourValue::Green );
	

	for ( TPosArray::const_iterator it = mPosArray.begin();
		it != mPosArray.end();
		++it )
	{
		HouseSceneGrid * gr = sce->getGrid( it->y, it->x, it->z );
		if ( !gr )
			EQ_EXCEPT( "找不到对应格子，可能是房屋编号重复", "TestAutoFindPathEvent::drawPath()");
				
		Ogre::Vector3 pos = gr->getSceneNode()->getPosition();
		// 加高显示
		pos.y += 10;
		mPathObj->position( pos );
	}

	mPathObj->end();
	mgr->getRootSceneNode()->createChildSceneNode()->attachObject( mPathObj );
}
//-----------------------------------------------------------------------
void TestAutoFindPathEvent::_destroyManualObject( Ogre::ManualObject * obj )
{
	if ( !obj )
		return;

	Ogre::SceneNode * node = obj->getParentSceneNode();
	Ogre::SceneManager * mgr = node->getCreator();
	
	node->detachObject( obj );

	mgr->destroyManualObject( obj );
	mgr->destroySceneNode( node );
}
//-----------------------------------------------------------------------
