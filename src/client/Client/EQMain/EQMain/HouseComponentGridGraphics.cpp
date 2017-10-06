#include "EQMainPCH.h"

#include "HouseComponentGridGraphics.h"
#include "CommFuncGcs.h"
#include "SystemGcs.h"
#include "HouseSceneDef.h"
#include "HouseTable.h"


/** ����һ���з�����пշ��� 
@param sceneMgr ����������
@param radius ���εİ뾶
@param thickness ���εĺ��
@param colour ��ɫ, ���Դ�͸��ͨ��
@param group (0,0)��Ӧԭ��
*/
static Ogre::ManualObject* CreateDirQuadGroup(Ogre::SceneManager *sceneMgr, 
									 float radius, 
									 float thickness, 
									 const Ogre::ColourValue& colour,
									 const HouseCompPropRow::GridPosArray & group )
{
	assert( radius > 0 && thickness > 0 && radius > thickness );

	static int num = 0;
	++num;

	std::string name = "--DirQuadGroup--[auto]--" + Ogre::StringConverter::toString(num) + "--";

	std::string matName;
	if (colour.a == 1.0f)
		matName = "SystemMat/Comm-2";
	else
		matName = "SystemMat/Comm-1";

	Ogre::ManualObject* manual = sceneMgr->createManualObject(name);


	for ( HouseCompPropRow::GridPosArray::const_iterator it = group.begin(); 
		it != group.end();
		++it )
	{
		const int x = it->x * HOUSE_GRID_SIZE;
		const int z = it->y * HOUSE_GRID_SIZE;
		manual->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

		manual->position(-radius + 1 + x,			0, -radius + 1 + z );			//0
		manual->colour(colour);

		manual->position(-radius + 1 + x,			0, radius - thickness + z );	//1
		manual->colour(colour);
		
		manual->position(-radius + 1 + x,			0, radius - 1 + z );			//2
		manual->colour(colour);
		
		manual->position(radius - 1 + x,			0, radius - 1 + z );			//3
		manual->colour(colour);
		
		manual->position(radius - 2*thickness + x,	0, radius - thickness + z );	//4
		manual->colour(colour);

		manual->position(radius - 1 + x,			0, -radius + thickness + z );	//5
		manual->colour(colour);

		manual->position(radius - 1 + x,			0, -radius + 1 + z );			//6
		manual->colour(colour);

		manual->position(-radius + thickness + x,	0, -radius + 1 + z );			//7
		manual->colour(colour);

		manual->position(-radius + thickness + x,	0, -radius + thickness + z );	//8
		manual->colour(colour);

		manual->position(-radius + thickness + x,	0, radius - thickness + z );	//9
		manual->colour(colour);

		manual->position(radius	- 1 + x,			0, radius - thickness + z );	//10
		manual->colour(colour);

		manual->position(radius - 2*thickness + x,	0, -radius + thickness + z );	//11
		manual->colour(colour);

		manual->position(radius - 1 + x,			0, 0 + z );						//12 �����ζ���
		manual->colour(colour);

		manual->position(radius - 3 + x,			0, 0 + z );						//13 �����ζ���
		manual->colour(colour);


		// --- �����߿� ---//
		manual->quad( 0, 7, 9, 1 );
		manual->quad( 2, 1, 10, 3 );
		manual->quad( 6, 5, 8, 7 );

		// --- ��������, ʵ������������ ---//
		// ����
		manual->quad( 12, 13, 11, 5 );
		// ����
		manual->quad( 10, 4, 13, 12 );

		manual->end();
	}

	return manual;
}
//-----------------------------------------------------------------------
static bool _createGrid( bool bShow, const Ogre::ColourValue & color, Ogre::SceneNode * parentNode,
					  Ogre::SceneNode * & sn, Ogre::ManualObject * & ent, const HouseCompPropRow::GridPosArray & group )
{
	bool bCreate = false;

	if ( !parentNode )
		return bCreate;

	Ogre::SceneManager * mgr = SystemGcs::getSingleton().getSceneManager();
	if ( bShow && !group.empty() )
	{
		if ( !sn )
		{
			sn = parentNode->createChildSceneNode();
			ent = CreateDirQuadGroup( mgr, HOUSE_GRID_HALF_SIZE, 10, color, group );
			ent->setQueryFlags( 0 );
			sn->attachObject( ent );		
			bCreate = true;
		}
	}
	else
	{
		if ( sn )
		{
			sn->detachObject( ent );
			SystemGcs::getSingleton().getSceneManager()->destroyManualObject( ent );
			SystemGcs::getSingleton().getSceneManager()->destroySceneNode( sn );
			sn = NULL;
			ent = NULL;
		}
	}

	return bCreate;
}
//-----------------------------------------------------------------------
HouseComponentGridGraphics::~HouseComponentGridGraphics()
{
	detach();
}
//-----------------------------------------------------------------------
HouseComponentGridGraphics::HouseComponentGridGraphics()
	: mMainNode( NULL )
	, mHoldGridNode( NULL )
	, mRedHoldGridNode( NULL )
	, mStopGridNode( NULL )
	, mLayGridNode( NULL )
	, mHoldGridEntity( NULL )
	, mRedHoldGridEntity( NULL )
	, mStopGridEntity( NULL )
	, mLayGridEntity( NULL )
	, mShowOffsetHeight( false )
{
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::detach()
{
	hideHoldGrid();
	showOffsetHeight( false );
	showStopGrid( false );
	showLayGrid( false );

	mMainNode = NULL;
	mHoldGridNode = NULL;
	mRedHoldGridNode = NULL;
	mStopGridNode = NULL;
	mLayGridNode = NULL; 
	mHoldGridEntity = NULL;
	mRedHoldGridEntity = NULL;
	mStopGridEntity = NULL;
	mLayGridEntity = NULL; 
	mShowOffsetHeight = false;
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::attach( Ogre::SceneNode * mainNode, const HouseCompPropRow * r )
{
	if ( mainNode == mMainNode && r == mHouseCompPropRow )
		return;

	detach();

	mHouseCompPropRow = r;
	mMainNode = mainNode;
	if ( !mHouseCompPropRow )
		EQ_EXCEPT("mHouseCompPropRow����Ϊ��", __FUNCTION__);

	if ( !mMainNode )
		EQ_EXCEPT("mMainNode����Ϊ��", __FUNCTION__);

}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::hideHoldGrid()
{
	if ( mRedHoldGridNode )
	{
		mRedHoldGridNode->detachObject( mRedHoldGridEntity );
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject( mRedHoldGridEntity );
		SystemGcs::getSingleton().getSceneManager()->destroySceneNode( mRedHoldGridNode );
		mRedHoldGridNode = NULL;
		mRedHoldGridEntity = NULL;
	}
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showHoldGrid( bool bShow )
{
	if ( bShow )
		showGreenHoldGrid();
	else
		hideHoldGrid();
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showHoldGrid( bool bShow, Ogre::ColourValue col )
{
	if ( !mHouseCompPropRow )
		return;

	if ( col != mHoldGridColor )
		hideHoldGrid();

	if ( _createGrid( bShow, col, mMainNode, mRedHoldGridNode, mRedHoldGridEntity, mHouseCompPropRow->HoldGrid ) )
	{
		mRedHoldGridNode->translate( 0, 0.1, 0 );
	}

	mHoldGridColor = col;
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showRedHoldGrid()
{
	showHoldGrid( true, Ogre::ColourValue( 1, 0, 0, 1 ) );
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showGreenHoldGrid()
{
	showHoldGrid( true, Ogre::ColourValue( 0, 1, 0, 1 ) );
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showYellowHoldGrid()
{
	showHoldGrid( true, Ogre::ColourValue( 1, 1, 0, 1 ) );
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showStopGrid( bool bShow )
{
	if ( !mHouseCompPropRow )
		return;

	if ( _createGrid( bShow, Ogre::ColourValue( 1, 0, 0, 1 ), mMainNode, mStopGridNode, mStopGridEntity, mHouseCompPropRow->StopGrid ) )	
	{
		mStopGridNode->translate( 0, 0.2, 0 );
	}
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showOffsetHeight( bool bShow )
{
	if ( mShowOffsetHeight == bShow )
		return;

	mShowOffsetHeight = bShow;
	if ( mLayGridNode )
	{
		showLayGrid( false );
		showLayGrid( true );
	}
}
//-----------------------------------------------------------------------
void HouseComponentGridGraphics::showLayGrid( bool bShow )
{
	if ( !mHouseCompPropRow )
		return;

	if ( _createGrid( bShow, Ogre::ColourValue( 0, 0, 1, 1 ), mMainNode, mLayGridNode, mLayGridEntity, mHouseCompPropRow->LayGrid ) )
	{
		float y = 0.5 + mHouseCompPropRow->SelfHeight;	
		if ( mShowOffsetHeight )
		{
			y += mHouseCompPropRow->OffsetHeight;
		}
		
		mLayGridNode->translate( 0, y, 0 );
	}
}
//-----------------------------------------------------------------------
