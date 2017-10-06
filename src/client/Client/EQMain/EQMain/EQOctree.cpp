
#include "EQOctree.h"
#include "EQOctreeNode.h"

using namespace Ogre;

/** Returns true is the box will fit in a child.
*/
bool EQOctree::_isTwiceSize( const AxisAlignedBox &box ) const
{
	// infinite boxes never fit in a child - always root node
	if (box.isInfinite())
		return false;

	Vector3 halfMBoxSize = mBox.getHalfSize();
	Vector3 boxSize = box.getSize();
	return ((boxSize.x <= halfMBoxSize.x) && (boxSize.y <= halfMBoxSize.y) && (boxSize.z <= halfMBoxSize.z));

}

/** It's assumed the the given box has already been proven to fit into
* a child.  Since it's a loose octree, only the centers need to be
* compared to find the appropriate node.
*/
void EQOctree::_getChildIndexes( const AxisAlignedBox &box, int *x, int *y, int *z ) const
{
	Vector3 max = mBox.getMaximum();
	Vector3 min = box.getMinimum();

	Vector3 center = mBox.getMaximum().midPoint( mBox.getMinimum() );

	Vector3 ncenter = box.getMaximum().midPoint( box.getMinimum() );

	if ( ncenter.x > center.x )
		* x = 1;
	else
		*x = 0;

	if ( ncenter.y > center.y )
		* y = 1;
	else
		*y = 0;

	if ( ncenter.z > center.z )
		* z = 1;
	else
		*z = 0;

}

EQOctree::EQOctree( EQOctree * parent ) 
: mWireBoundingBox(0),
mHalfSize( 0, 0, 0 )
{
	//initialize all children to null.
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			for ( int k = 0; k < 2; k++ )
			{
				mChildren[ i ][ j ][ k ] = 0;
			}
		}
	}

	mParent = parent;
	mNumNodes = 0;
}

EQOctree::~EQOctree()
{
	//initialize all children to null.
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			for ( int k = 0; k < 2; k++ )
			{
				if ( mChildren[ i ][ j ][ k ] != 0 )
					OGRE_DELETE mChildren[ i ][ j ][ k ];
			}
		}
	}

	if(mWireBoundingBox)
		OGRE_DELETE mWireBoundingBox;

	mParent = 0;
}


void EQOctree::_addNode( EQOctreeNode * n )
{
	mNodes.push_back( n );
	n -> setOctant( this );

	//update total counts.
	_ref();

}

void EQOctree::_removeNode( EQOctreeNode * n )
{
	mNodes.erase( std::find( mNodes.begin(), mNodes.end(), n ) );
	n -> setOctant( 0 );

	//update total counts.
	_unref();
}

void EQOctree::_getCullBounds( AxisAlignedBox *b ) const
{
	b -> setExtents( mBox.getMinimum() - mHalfSize, mBox.getMaximum() + mHalfSize );
}

WireBoundingBox* EQOctree::getWireBoundingBox()
{
	// Create a WireBoundingBox if needed
	if(mWireBoundingBox == 0)
		mWireBoundingBox = OGRE_NEW WireBoundingBox();

	mWireBoundingBox->setupBoundingBox(mBox);
	return mWireBoundingBox;
}
