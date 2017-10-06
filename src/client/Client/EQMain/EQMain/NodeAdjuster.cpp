#include "EQMainPCH.h"

#include "EQMain.h"

#include "SystemGcs.h"
#include "NodeAdjuster.h"
#include "CollisionTools.h"




static Ogre::Vector2 ScreenPos( Ogre::Camera * cam, const Ogre::Vector3 & pos ) 
{
	Ogre::Vector2 result;

	Ogre::Vector3 eyeSpacePos = cam->getViewMatrix(true) * pos;
	if ( eyeSpacePos.z < 0) 
	{
		Ogre::Vector3 screenSpacePos = cam->getProjectionMatrix() * eyeSpacePos;
		result.x = screenSpacePos.x;
		result.y = screenSpacePos.y;
	} 
	else 
	{
		result.x = (-eyeSpacePos.x > 0) ? -1 : 1;
		result.y = (-eyeSpacePos.y > 0) ? -1 : 1;
	}
	return result;
}


//-----------------------------------------------------------------------
NodeAdjuster::~NodeAdjuster()
{
	if ( !mXYZNode )
		return;

	detach();
	Ogre::SceneManager * mgr = SystemGcs::getSingleton().getSceneManager();
	mxNode->detachObject( mxEntity );
	myNode->detachObject( myEntity );
	mzNode->detachObject( mzEntity );

	
	mgr->destroyEntity( mxEntity );
	mgr->destroyEntity( myEntity );
	mgr->destroyEntity( mzEntity );

	mgr->destroySceneNode( mxNode );
	mgr->destroySceneNode( myNode );
	mgr->destroySceneNode( mzNode );

	mgr->destroySceneNode( mXYZNode );
}
//-----------------------------------------------------------------------
NodeAdjuster::NodeAdjuster( const std::string & arrowMesh )
	: mOpMode( OP_SELECT_NODE )
	, mQueryFlags( DEFAULT_QUERY_FLAGS )
	, mOpNode( NULL )
	, mxEntity( NULL )
	, myEntity( NULL )
	, mzEntity( NULL )
	, mSelectedEntity( NULL )
	, mxNode( NULL )
	, myNode( NULL )
	, mzNode( NULL )
	, mXYZNode( NULL )
	, mCamera( NULL )
	, mVisible( false )
	, mXYZMesh( arrowMesh )
{
}
//-----------------------------------------------------------------------
void NodeAdjuster::attach( Ogre::SceneNode * node, Ogre::Camera * cam )
{
	if ( !mXYZNode )
		_init();

	if ( node != mOpNode )
	{
		detach();
		if ( !node || !cam )
			return;

		mCamera = cam;
		mOpNode = node;

		if ( node->getParentSceneNode() )
		{
			node->getParentSceneNode()->addChild( mXYZNode );
		}
		
		mXYZNode->setPosition( mOpNode->getPosition() );

		setVisible( true );
	}
}
//-----------------------------------------------------------------------
bool NodeAdjuster::isVisible() const
{
	return mVisible;
}
//-----------------------------------------------------------------------
void NodeAdjuster::setVisible( bool val )
{
	mVisible = val;
	if ( !mXYZNode )
		return;

	mXYZNode->setVisible( val );
}
//-----------------------------------------------------------------------
void NodeAdjuster::detach()
{
	if ( !mXYZNode )
		return;

	if ( mXYZNode->getParentSceneNode() )
	{
		mXYZNode->getParentSceneNode()->removeChild( mXYZNode );
	}

	setVisible( false );
	mOpNode			= NULL;
	mSelectedEntity = NULL;		
	mCamera			= NULL;
}
//-----------------------------------------------------------------------
void NodeAdjuster::setQueryFlags( uint val )
{	
	mQueryFlags = val;

	if ( !mXYZNode )
		return;

	mxEntity->setQueryFlags( val );
	myEntity->setQueryFlags( val );
	mzEntity->setQueryFlags( val );
}
//-----------------------------------------------------------------------
uint NodeAdjuster::getQueryFlags() const
{
	return mQueryFlags;
}
//-----------------------------------------------------------------------
void NodeAdjuster::resetMaterial()
{
	if ( !mXYZNode )
		return;

	mxEntity->setMaterialName("mtSCBRED");
	myEntity->setMaterialName("mtSCBGREEN");
	mzEntity->setMaterialName("mtSCBBLUE");
}
//-----------------------------------------------------------------------
void NodeAdjuster::_setSubEntityVisible( bool b0, bool b1, bool b2, bool b3 )
{
	_setSubEntityVisible( mxEntity, b0, b1, b2 , b3 );
	_setSubEntityVisible( myEntity, b0, b1, b2 , b3 );
	_setSubEntityVisible( mzEntity, b0, b1, b2 , b3 );
}
//-----------------------------------------------------------------------
void NodeAdjuster::_setSubEntityVisible( Ogre::Entity * ent, bool b0, bool b1, bool b2, bool b3 )
{
	ent->getSubEntity( 0 )->setVisible( b0 );	
	ent->getSubEntity( 1 )->setVisible( b1 );	
	ent->getSubEntity( 2 )->setVisible( b2 );	
	ent->getSubEntity( 3 )->setVisible( b3 );	
}
//-----------------------------------------------------------------------
void NodeAdjuster::setMode( NodeOperationType mode ) 
{
	mOpMode = mode;
	if ( !mXYZNode )
		return;

	switch ( mode )
	{
	case OP_SELECT_NODE:
		{				
			// 木棒形状
			_setSubEntityVisible( true, false, false , false );
		}
		break;
	case OP_MOVE_NODE:
		{
			// 箭头形状
			_setSubEntityVisible( true, true, false , false );
		}
		break;
	case OP_ROTATE_NODE:
		{
			// 木棒+圆圈形状
			_setSubEntityVisible( true, false, true , false );
		}
		break;
	case OP_SCALE_NODE:
		{
			// 木棒+方块形状
			_setSubEntityVisible( true, false, false , true );
		}
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------
/**
*/
bool NodeAdjuster::_pickObject( const Ogre::Ray & kRay )
{
	if ( !mOpNode )
		return false;

	Ogre::Vector3 result;
	Ogre::MovableObject* pPick;
	float closest_distance;
	MOC::CollisionTools::getSingleton().raycast(kRay, result, pPick, closest_distance);
/*
	Ogre::SceneManager * mgr = SystemGcs::getSingleton().getSceneManager();
	
	// 创建射线查询
	Ogre::RaySceneQuery * raySceneQuery = SystemGcs::getSingleton().getSceneManager()->createRayQuery( kRay );
	raySceneQuery->setQueryMask( this->getQueryFlags() );
	Ogre::RaySceneQueryResult r = raySceneQuery->execute();

	Ogre::MovableObject * pPick = NULL;
	for ( Ogre::RaySceneQueryResult::iterator it = r.begin(); 
		it != r.end();
		++it )
	{
		if ( NULL == it->movable )
			continue;
	
		if ( !it->movable->isVisible() )
			continue;
		
		if ( mxEntity == it->movable || myEntity == it->movable || mzEntity == it->movable )
		{
			pPick = it->movable;
			break;
		}	
	}

	// 删除射线查询
	SystemGcs::getSingleton().getSceneManager()->destroyQuery( raySceneQuery );
*/	
	this->resetMaterial();

	mSelectedEntity = dynamic_cast<Ogre::Entity *>( pPick );
	if ( mSelectedEntity==mxEntity || mSelectedEntity==myEntity || mSelectedEntity==mzEntity )
	{
		// 选中轴 高亮显示
		mSelectedEntity->setMaterialName( "mtSCBYELLOWL" );

		return true;
	}	

	return false;

}	
//-----------------------------------------------------------------------
void NodeAdjuster::autoScaleXYZ()
{
	if ( !mOpNode )
		return;
	const float baseDis = 400.0f;
	const float dis = mCamera->getPosition().distance( mOpNode->getPosition() );
	float k = 12.0f / baseDis * dis;
	mxNode->setScale( k, k, 2 * k );
	myNode->setScale( k, k, 2 * k );
	mzNode->setScale( k, k, 2 * k );
}
//-----------------------------------------------------------------------
void NodeAdjuster::onMouseDrag( float xt, float yt )
{
	if ( !mOpNode )
		return;

	autoScaleXYZ();

	const float baseDis = 400.0f;
	const float dis = mCamera->getPosition().distance( mOpNode->getPosition() );

	xt = xt * 770 / baseDis * dis;
	yt = yt * 519 / baseDis * dis;
	
	Ogre::Vector2 mouseOffset( xt, yt );


	if ( !mSelectedEntity )
	{
		if ( OP_MOVE_NODE == mOpMode  )
			mOpNode->translate( xt/2.0f, -yt/2.0f, 0, Ogre::Node::TS_WORLD );
	}
	else
	{
		Ogre::Vector2 p0 = ScreenPos( mCamera, mXYZNode->_getDerivedPosition() );
		Ogre::Vector2 p1 = ScreenPos( mCamera, mSelectedEntity->getParentSceneNode()->_getDerivedPosition() );
		Ogre::Vector2 arrowOffset( p1 - p0 );

		// 由于坐标系不一样，这里转化y轴
		arrowOffset.y = -arrowOffset.y;
		arrowOffset.normalise();
		mouseOffset.normalise();

		// 鼠标操作量
		float opLen = Ogre::Vector2( xt, yt ).length() / 1.7f;
		const float degree90 = 3.14159f / 2;


		if ( OP_ROTATE_NODE == mOpMode ) // 如果是旋转操作
		{
			//箭头旋转90度
			std::swap( arrowOffset.x, arrowOffset.y );
			arrowOffset.x = -arrowOffset.x;
		}

		// 判断两个向量的夹角: 小于90度表示方向相同，大于90度表示方向相反
		if ( degree90 < acos( arrowOffset.dotProduct(mouseOffset) ) )
		{
			opLen = -opLen;
		}

		switch ( mOpMode )
		{
		case OP_MOVE_NODE:
			{
				if ( mSelectedEntity == mxEntity )
					mOpNode->translate( opLen, 0, 0 );		
				else if ( mSelectedEntity == myEntity )
					mOpNode->translate( 0, opLen, 0 );		
				else if ( mSelectedEntity == mzEntity )
					mOpNode->translate( 0, 0, opLen );		
			}
			break;
		case OP_ROTATE_NODE:
			{
				if ( mSelectedEntity == mxEntity )
					mOpNode->pitch( Ogre::Degree( opLen ), Ogre::Node::TS_PARENT );
				else if ( mSelectedEntity == myEntity )
					mOpNode->yaw( Ogre::Degree( opLen ), Ogre::Node::TS_PARENT );
				else if ( mSelectedEntity == mzEntity )
					mOpNode->roll( Ogre::Degree( opLen ), Ogre::Node::TS_PARENT );
			}
			break;
		case OP_SCALE_NODE:
			{
				float j = 1.02f;
				if ( opLen < 0.0 )
					j = 0.99;

				mOpNode->scale( j, j, j );				
			}
			break;
		default:
			return;
			break;
		}
	}
	mXYZNode->setPosition( mOpNode->getPosition() );
	//mXYZNode->setOrientation( mOpNode->getOrientation() );
}
//-----------------------------------------------------------------------
void NodeAdjuster::_init()
{
	// 创建三根轴
	Ogre::SceneManager * mgr = SystemGcs::getSingleton().getSceneManager();

	mXYZNode = mgr->getRootSceneNode()->createChildSceneNode();
	
	
	mxEntity = mgr->createEntity( "ModelAdjuster_XEntity", mXYZMesh );
	if ( !mxEntity )
		EQ_EXCEPT( "创建x轴实体失败", "NodeAdjuster::NodeAdjuster" );

	if ( mxEntity->getNumSubEntities() != 4 )
		EQ_EXCEPT( "x轴子实体数目不正确", "NodeAdjuster::NodeAdjuster" );

	myEntity = mgr->createEntity( "ModelAdjuster_YEntity", mXYZMesh );
	mzEntity = mgr->createEntity( "ModelAdjuster_ZEntity", mXYZMesh );
	
	mxNode = mXYZNode->createChildSceneNode();
	myNode = mXYZNode->createChildSceneNode();
	mzNode = mXYZNode->createChildSceneNode();

	mxNode->attachObject( mxEntity );
	myNode->attachObject( myEntity );
	mzNode->attachObject( mzEntity );

	mxNode->yaw( Ogre::Degree( -90 ) );
	mxNode->translate( 3, 0, 0 );

	myNode->pitch( Ogre::Degree( 90 ) );
	myNode->translate( 0, 3, 0 );

	mzNode->yaw( Ogre::Degree( 180 ) );
	mzNode->translate( 0, 0, 3 );

	mxNode->scale( 12, 12, 24 );
	myNode->scale( 12, 12, 24 );
	mzNode->scale( 12, 12, 24 );

	// 在外层渲染
	using namespace Ogre;
	mxEntity->setRenderQueueGroup( RENDER_QUEUE_SKIES_LATE );
	myEntity->setRenderQueueGroup( RENDER_QUEUE_SKIES_LATE );
	mzEntity->setRenderQueueGroup( RENDER_QUEUE_SKIES_LATE );

	// 将三根轴放在同一个组中，便于射线查询
	setQueryFlags( mQueryFlags );
	
	// 初始化三根轴的颜色
	resetMaterial();

	// 设为选择模式
	setMode( mOpMode );		


	setVisible( mVisible );
}
//-----------------------------------------------------------------------
void NodeAdjuster::pickObject( int x, int y, Ogre::Camera * cam )
{		
	// 取得鼠标点射线
	int iWidth = cam->getViewport()->getActualWidth();
	int iHeight = cam->getViewport()->getActualHeight();

	Ogre::Vector2 kViewportSize(iWidth, iHeight);

	Ogre::Vector2 kCurMousePos(x, y);	

	Ogre::Vector2 kResult = kCurMousePos / kViewportSize;

	const Ogre::Ray kRay = cam->getCameraToViewportRay(kResult.x, kResult.y);


	// 射线查询
	Ogre::RaySceneQuery * raySceneQuery = cam->getSceneManager()->createRayQuery( kRay );
	
	// 不查询xyz轴
	raySceneQuery->setQueryMask( ~getQueryFlags() );

	Ogre::RaySceneQueryResult r = raySceneQuery->execute();	

	Ogre::SceneNode * pPickNode = NULL;
	for ( Ogre::RaySceneQueryResult::iterator it = r.begin(); 
		it != r.end();
		++it )
	{
		if ( NULL == it->movable )
			continue;
	
		if ( !it->movable->isVisible() )
			continue;
		
		pPickNode = it->movable->getParentSceneNode();
		break;
	}
	cam->getSceneManager()->destroyQuery( raySceneQuery );


	// 如果没选中xyz轴
	if ( !_pickObject( kRay ) )
		attach( pPickNode, cam );	

	autoScaleXYZ();
}
//-----------------------------------------------------------------------

