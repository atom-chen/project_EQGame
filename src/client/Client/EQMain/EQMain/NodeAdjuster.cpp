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
			// ľ����״
			_setSubEntityVisible( true, false, false , false );
		}
		break;
	case OP_MOVE_NODE:
		{
			// ��ͷ��״
			_setSubEntityVisible( true, true, false , false );
		}
		break;
	case OP_ROTATE_NODE:
		{
			// ľ��+ԲȦ��״
			_setSubEntityVisible( true, false, true , false );
		}
		break;
	case OP_SCALE_NODE:
		{
			// ľ��+������״
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
	
	// �������߲�ѯ
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

	// ɾ�����߲�ѯ
	SystemGcs::getSingleton().getSceneManager()->destroyQuery( raySceneQuery );
*/	
	this->resetMaterial();

	mSelectedEntity = dynamic_cast<Ogre::Entity *>( pPick );
	if ( mSelectedEntity==mxEntity || mSelectedEntity==myEntity || mSelectedEntity==mzEntity )
	{
		// ѡ���� ������ʾ
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

		// ��������ϵ��һ��������ת��y��
		arrowOffset.y = -arrowOffset.y;
		arrowOffset.normalise();
		mouseOffset.normalise();

		// ��������
		float opLen = Ogre::Vector2( xt, yt ).length() / 1.7f;
		const float degree90 = 3.14159f / 2;


		if ( OP_ROTATE_NODE == mOpMode ) // �������ת����
		{
			//��ͷ��ת90��
			std::swap( arrowOffset.x, arrowOffset.y );
			arrowOffset.x = -arrowOffset.x;
		}

		// �ж����������ļн�: С��90�ȱ�ʾ������ͬ������90�ȱ�ʾ�����෴
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
	// ����������
	Ogre::SceneManager * mgr = SystemGcs::getSingleton().getSceneManager();

	mXYZNode = mgr->getRootSceneNode()->createChildSceneNode();
	
	
	mxEntity = mgr->createEntity( "ModelAdjuster_XEntity", mXYZMesh );
	if ( !mxEntity )
		EQ_EXCEPT( "����x��ʵ��ʧ��", "NodeAdjuster::NodeAdjuster" );

	if ( mxEntity->getNumSubEntities() != 4 )
		EQ_EXCEPT( "x����ʵ����Ŀ����ȷ", "NodeAdjuster::NodeAdjuster" );

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

	// �������Ⱦ
	using namespace Ogre;
	mxEntity->setRenderQueueGroup( RENDER_QUEUE_SKIES_LATE );
	myEntity->setRenderQueueGroup( RENDER_QUEUE_SKIES_LATE );
	mzEntity->setRenderQueueGroup( RENDER_QUEUE_SKIES_LATE );

	// �����������ͬһ�����У��������߲�ѯ
	setQueryFlags( mQueryFlags );
	
	// ��ʼ�����������ɫ
	resetMaterial();

	// ��Ϊѡ��ģʽ
	setMode( mOpMode );		


	setVisible( mVisible );
}
//-----------------------------------------------------------------------
void NodeAdjuster::pickObject( int x, int y, Ogre::Camera * cam )
{		
	// ȡ����������
	int iWidth = cam->getViewport()->getActualWidth();
	int iHeight = cam->getViewport()->getActualHeight();

	Ogre::Vector2 kViewportSize(iWidth, iHeight);

	Ogre::Vector2 kCurMousePos(x, y);	

	Ogre::Vector2 kResult = kCurMousePos / kViewportSize;

	const Ogre::Ray kRay = cam->getCameraToViewportRay(kResult.x, kResult.y);


	// ���߲�ѯ
	Ogre::RaySceneQuery * raySceneQuery = cam->getSceneManager()->createRayQuery( kRay );
	
	// ����ѯxyz��
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


	// ���ûѡ��xyz��
	if ( !_pickObject( kRay ) )
		attach( pPickNode, cam );	

	autoScaleXYZ();
}
//-----------------------------------------------------------------------

