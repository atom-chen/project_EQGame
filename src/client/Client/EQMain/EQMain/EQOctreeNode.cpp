
#include <Ogre/OgreRoot.h>
#include "EQOctreeNode.h"
#include "EQOctreeManager.h"

using namespace Ogre;


EQOctreeNode::EQOctreeNode( EQOctreeManager* creator ) : mCreator(creator), mSpirit(NULL)
{
	mOctant = 0;
	mWorldAABB.setNull();
}

EQOctreeNode::EQOctreeNode( EQOctreeManager* creator, const std::string& name ) : Node( name ), mCreator(creator), mSpirit(NULL)
{
	mOctant = 0;
	mWorldAABB.setNull();
}

EQOctreeNode::~EQOctreeNode()
{
	unattatch();
}

void EQOctreeNode::_removeNodeAndChildren( )
{
	static_cast< EQOctreeManager * > ( mCreator ) -> _removeOctreeNode( this ); 
	//remove all the children nodes as well from the octree.
	ChildNodeMap::iterator it = mChildren.begin();
	while( it != mChildren.end() )
	{
		static_cast<EQOctreeNode *>( it->second ) -> _removeNodeAndChildren();
		++it;
	}
}
Node * EQOctreeNode::removeChild( unsigned short index )
{
	EQOctreeNode *on = static_cast<EQOctreeNode* >( Node::removeChild( index ) );
	on -> _removeNodeAndChildren(); 
	return on; 
}
Node * EQOctreeNode::removeChild( Node* child )
{
	EQOctreeNode *on = static_cast<EQOctreeNode* >( Node::removeChild( child ) );
	on -> _removeNodeAndChildren(); 
	return on; 
}
void EQOctreeNode::removeAllChildren()
{
	ChildNodeMap::iterator i, iend;
	iend = mChildren.end();
	for (i = mChildren.begin(); i != iend; ++i)
	{
		EQOctreeNode* on = static_cast<EQOctreeNode*>(i->second);
		on->setParent(0);
		on->_removeNodeAndChildren();
	}
	mChildren.clear();
	mChildrenToUpdate.clear();

}

Node * EQOctreeNode::removeChild( const std::string & name )
{
	EQOctreeNode *on = static_cast< EQOctreeNode * >( Node::removeChild(  name ) );
	on -> _removeNodeAndChildren( ); 
	return on; 
}

//same as SceneNode, only it doesn't care about children...
void EQOctreeNode::_updateBounds( void )
{
	mWorldAABB.setNull();

	if ( mSpirit )
		mWorldAABB = mSpirit->_getWorldAABB();

	//update the OctreeSceneManager that things might have moved.
	// if it hasn't been added to the octree, add it, and if has moved
	// enough to leave it's current node, we'll update it.
	if ( ! mWorldAABB.isNull() )
	{
		 mCreator -> _updateOctreeNode( this );
	}

}


bool EQOctreeNode::_isIn( AxisAlignedBox &box )
{
	// Always fail if not in the scene graph or box is null
	if ( box.isNull()) return false;

	// Always succeed if AABB is infinite
	if (box.isInfinite())
		return true;

	Vector3 center = mWorldAABB.getMaximum().midPoint( mWorldAABB.getMinimum() );

	Vector3 bmin = box.getMinimum();
	Vector3 bmax = box.getMaximum();

	bool centre = ( bmax > center && bmin < center );
	if (!centre)
		return false;

	// Even if covering the centre line, need to make sure this BB is not large
	// enough to require being moved up into parent. When added, bboxes would
	// end up in parent due to cascade but when updating need to deal with
	// bbox growing too large for this child
	Vector3 octreeSize = bmax - bmin;
	Vector3 nodeSize = mWorldAABB.getMaximum() - mWorldAABB.getMinimum();
	return nodeSize < octreeSize;

}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void EQOctreeNode::_update(bool updateChildren, bool parentHasChanged)
{
	Node::_update(updateChildren, parentHasChanged);
	_updateBounds();
}

Node* EQOctreeNode::createChildImpl(void)
{
	return mCreator->createOctreeNode();
}

/** Internal method for creating a new child node - must be overridden per subclass. */
Node* EQOctreeNode::createChildImpl(const std::string& name)
{
	return mCreator->createOctreeNode(name);
}

void EQOctreeNode::attach(EQOctreeSpirit* spi)
{
	if ( mSpirit )
		mSpirit->mAtt = NULL;

	mSpirit = spi;
	mSpirit->mAtt = this;
}

void EQOctreeNode::unattatch()
{
	if ( mSpirit )
	{
		mSpirit->mAtt = NULL;
		mSpirit = NULL;
	}
}


