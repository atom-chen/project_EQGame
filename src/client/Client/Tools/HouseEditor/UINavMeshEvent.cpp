#include "HouseEditorPCH.h"

#include "UINavMeshEvent.h"
#include "CommFuncGcs.h"
#include "GraphicsSystem.h"
#include "SceneManager.h"
#include "TerrainData.h"
#include "MfcInputEventMgr.h"
#include "SystemGcs.h"
#include "resource.h"
#include "PathSearchDlg.h"


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
TestNavMeshPathEvent::TestNavMeshPathEvent() 
: MfcInputEvent(epNavMesh)
, mLining(false)
, mPathObj(NULL)
, mPicking(false)
, mLineObjNode(NULL)
, mLineObj(NULL)
{
}
//-----------------------------------------------------------------------
TestNavMeshPathEvent::~TestNavMeshPathEvent()
{
	lineEnd();

	clearPath();
}
//-----------------------------------------------------------------------
TestNavMeshPathEvent * TestNavMeshPathEvent::create()
{
	return new TestNavMeshPathEvent();
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		MfcInputEventMgr::getSingleton().delEvent<TestNavMeshPathEvent>();
		processed = true;
	}
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	lineStart(point);
	processed = true;
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::LButtonUp( UINT nFlags, CPoint point, bool & processed )
{
	if ( !lineEnd() )
		return;

	exce(true);
	processed = true;
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	lineMove(point);
	processed = true;
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::lineStart(CPoint pt)
{
	if ( mLining )
		return;

	TerrainData* terr = EQSceneManager::getSingleton().getTerrainData();
	if ( terr->isEmpty() )
		return;
	
	Ogre::Ray ray = getRay( pt );
	Ogre::Vector3 position;
	if ( terr->getIntersects(ray, position, NULL, true) )
	{
		position.y += 10;
		mStartLinePos = position;
		mLining = true;

		mPicking = true;
	}

}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::_lineDestroy(bool withNode )
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
bool TestNavMeshPathEvent::lineEnd()
{

	if ( mPicking )
	{
		mPicking = false;
		return false;
	}


	_lineDestroy();

	mLining = false;
	return true;
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::lineMove(CPoint pt)
{
	if ( !mLining )
		return ;

	TerrainData* terr = EQSceneManager::getSingleton().getTerrainData();
	if ( terr->isEmpty() )
		return;

	Ogre::Ray ray = getRay( pt );
	Ogre::Vector3 position;
	if ( terr->getIntersects(ray, position, NULL, true) )
	{
		position.y += 10;
		mEndLinePos = position;
		lineDraw();
	}
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::lineDraw()
{
	_lineDestroy(false);

	Ogre::ColourValue color(0,1,0);

	assert(mLineObj==NULL);
	mLineObj = SystemGcs::getSingleton().getSceneManager()->createManualObject("-TestNavMeshPathEvent-Line-");

	mLineObj->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

	mLineObj->colour(color);
	mLineObj->position(mStartLinePos);
	mLineObj->position(mEndLinePos);

	mLineObj->end();

	//
	if ( !mLineObjNode )
		mLineObjNode = SystemGcs::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode();

	mLineObjNode->attachObject(mLineObj);
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::clearPath()
{
	mPath.clear();
	_destroyManualObject( mPathObj );
	mPathObj = NULL;
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::searchPath()
{

	SearchSystem::NavSearchSystem& nss = g_CPathSearchDlg->mNss;

	SearchSystem::Vector2f a(mStartLinePos.x, mStartLinePos.z);
	SearchSystem::Vector2f b(mEndLinePos.x, mEndLinePos.z);

	if ( nss.findPath(a, b) )
	{
		nss.getPath(mPath);
	}


}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::exce( bool bAdded )
{

	clearPath();
	searchPath();
	drawPath();

}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::drawPath()
{
	if ( mPath.empty() )
		return;

	Ogre::SceneManager * mgr = GraphicsSystem::getSingleton().getSceneMgr();
	TerrainData* terr = EQSceneManager::getSingleton().getTerrainData();

	mPathObj = mgr->createManualObject( "--TestNavMeshPathObj--" );

	mPathObj->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP );
	mPathObj->colour( Ogre::ColourValue::Green );


	for ( size_t i = 0; i < mPath.size(); ++i )
	{

		Ogre::Vector3 pos(mPath[i].x, terr->getHeightAt(mPath[i].x, mPath[i].y), mPath[i].y);
		// ¼Ó¸ßÏÔÊ¾
		pos.y += 10;
		mPathObj->position( pos );
	}

	mPathObj->end();
	mgr->getRootSceneNode()->createChildSceneNode()->attachObject( mPathObj );
}
//-----------------------------------------------------------------------
void TestNavMeshPathEvent::_destroyManualObject( Ogre::ManualObject * obj )
{
	if ( !obj )
		return;

	Ogre::SceneNode * node = obj->getParentSceneNode();
	Ogre::SceneManager * mgr = node->getCreator();

	node->detachObject( obj );

	mgr->destroyManualObject( obj );
	mgr->destroySceneNode( node );
}
