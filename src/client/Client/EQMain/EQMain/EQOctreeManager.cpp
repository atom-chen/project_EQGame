
#include "EQOctreeManager.h"

//#include <OgreOctreeSceneQuery.h>
#include "EQOctreeNode.h"


using namespace Ogre;


enum Intersection
{
	OUTSIDE=0,
	INSIDE=1,
	INTERSECT=2
};
int EQOctreeManager::intersect_call = 0;

Intersection intersect( const Ray &one, const Ogre::AxisAlignedBox &two )
{
	EQOctreeManager::intersect_call++;
	// Null box?
	if (two.isNull()) return OUTSIDE;
	// Infinite box?
	if (two.isInfinite()) return INTERSECT;

	bool inside = true;
	const Vector3& twoMin = two.getMinimum();
	const Vector3& twoMax = two.getMaximum();
	Vector3 origin = one.getOrigin();
	Vector3 dir = one.getDirection();

	Vector3 maxT(-1, -1, -1);

	int i = 0;
	for(i=0; i<3; i++ )
	{
		if( origin[i] < twoMin[i] )
		{
			inside = false;
			if( dir[i] > 0 )
			{
				maxT[i] = (twoMin[i] - origin[i])/ dir[i];
			}
		}
		else if( origin[i] > twoMax[i] )
		{
			inside = false;
			if( dir[i] < 0 )
			{
				maxT[i] = (twoMax[i] - origin[i]) / dir[i];
			}
		}
	}

	if( inside )
	{
		return INTERSECT;
	}
	int whichPlane = 0;
	if( maxT[1] > maxT[whichPlane])
		whichPlane = 1;
	if( maxT[2] > maxT[whichPlane])
		whichPlane = 2;

	if( ((int)maxT[whichPlane]) & 0x80000000 )
	{
		return OUTSIDE;
	}
	for(i=0; i<3; i++ )
	{
		if( i!= whichPlane )
		{
			float f = origin[i] + maxT[whichPlane] * dir[i];
			if ( f < (twoMin[i] - 0.00001f) ||
				f > (twoMax[i] +0.00001f ) )
			{
				return OUTSIDE;
			}
		}
	}

	return INTERSECT;

}


/** Checks how the second box intersects with the first.
*/
Intersection intersect( const PlaneBoundedVolume &one, const Ogre::AxisAlignedBox &two )
{
	EQOctreeManager::intersect_call++;
	// Null box?
	if (two.isNull()) return OUTSIDE;
	// Infinite box?
	if (two.isInfinite()) return INTERSECT;

	// Get centre of the box
	Vector3 centre = two.getCenter();
	// Get the half-size of the box
	Vector3 halfSize = two.getHalfSize();

	// For each plane, see if all points are on the negative side
	// If so, object is not visible.
	// If one or more are, it's partial.
	// If all aren't, full
	bool all_inside = true;
	PlaneList::const_iterator i, iend;
	iend = one.planes.end();
	for (i = one.planes.begin(); i != iend; ++i)
	{
		const Plane& plane = *i;

		Plane::Side side = plane.getSide(centre, halfSize);
		if(side == one.outside)
			return OUTSIDE;
		if(side == Plane::BOTH_SIDE)
			all_inside = false; 
	}

	if ( all_inside )
		return INSIDE;
	else
		return INTERSECT;

}


/** Checks how the second box intersects with the first.
*/
Intersection intersect( const Ogre::AxisAlignedBox &one, const Ogre::AxisAlignedBox &two )
{
	EQOctreeManager::intersect_call++;
	// Null box?
	if (one.isNull() || two.isNull()) return OUTSIDE;
	if (one.isInfinite()) return INSIDE;
	if (two.isInfinite()) return INTERSECT;


	const Vector3& insideMin = two.getMinimum();
	const Vector3& insideMax = two.getMaximum();

	const Vector3& outsideMin = one.getMinimum();
	const Vector3& outsideMax = one.getMaximum();

	if (    insideMax.x < outsideMin.x ||
		insideMax.y < outsideMin.y ||
		insideMax.z < outsideMin.z ||
		insideMin.x > outsideMax.x ||
		insideMin.y > outsideMax.y ||
		insideMin.z > outsideMax.z )
	{
		return OUTSIDE;
	}

	bool full = ( insideMin.x > outsideMin.x &&
		insideMin.y > outsideMin.y &&
		insideMin.z > outsideMin.z &&
		insideMax.x < outsideMax.x &&
		insideMax.y < outsideMax.y &&
		insideMax.z < outsideMax.z );

	if ( full )
		return INSIDE;
	else
		return INTERSECT;

}

/** Checks how the box intersects with the sphere.
*/
Intersection intersect( const Sphere &one, const Ogre::AxisAlignedBox &two )
{
	EQOctreeManager::intersect_call++;
	// Null box?
	if (two.isNull()) return OUTSIDE;
	if (two.isInfinite()) return INTERSECT;

	float sradius = one.getRadius();

	sradius *= sradius;

	Vector3 scenter = one.getCenter();

	const Vector3& twoMin = two.getMinimum();
	const Vector3& twoMax = two.getMaximum();

	float s, d = 0;

	Vector3 mndistance = ( twoMin - scenter );
	Vector3 mxdistance = ( twoMax - scenter );

	if ( mndistance.squaredLength() < sradius &&
		mxdistance.squaredLength() < sradius )
	{
		return INSIDE;
	}

	//find the square of the distance
	//from the sphere to the box
	for ( int i = 0 ; i < 3 ; i++ )
	{
		if ( scenter[ i ] < twoMin[ i ] )
		{
			s = scenter[ i ] - twoMin[ i ];
			d += s * s;
		}

		else if ( scenter[ i ] > twoMax[ i ] )
		{
			s = scenter[ i ] - twoMax[ i ];
			d += s * s;
		}

	}

	bool partial = ( d <= sradius );

	if ( !partial )
	{
		return OUTSIDE;
	}

	else
	{
		return INTERSECT;
	}


}

unsigned long white = 0xFFFFFFFF;

unsigned short EQOctreeManager::mIndexes[ 24 ] = {0, 1, 1, 2, 2, 3, 3, 0,       //back
0, 6, 6, 5, 5, 1,             //left
3, 7, 7, 4, 4, 2,             //right
6, 7, 5, 4 };          //front
unsigned long EQOctreeManager::mColors[ 8 ] = {white, white, white, white, white, white, white, white };

EQOctreeManager::EQOctreeManager() : mRadius(1000), mOctreeRoot(NULL)
{
	AxisAlignedBox b( -10000, -10000, -10000, 10000, 10000, 10000 );
	int depth = 8; 
	mOctree = 0;
	init( b, depth );
}

EQOctreeManager::EQOctreeManager( AxisAlignedBox &box, int max_depth ) : mRadius(1000), mOctreeRoot(NULL)
{
	mOctree = 0;
	init( box, max_depth );
}

void EQOctreeManager::init( AxisAlignedBox &box, int depth )
{

	mLastNotifyNodes.clear();

	if ( mOctree != 0 )
		delete mOctree;

	mOctree = new EQOctree( 0 );

	mMaxDepth = depth;
	mBox = box;

	mOctree -> mBox = box;

	Vector3 min = box.getMinimum();

	Vector3 max = box.getMaximum();

	mOctree -> mHalfSize = ( max - min ) / 2;


	mShowBoxes = false;

	mNumObjects = 0;

	Vector3 v( 1.5, 1.5, 1.5 );

	mScaleFactor.setScale( v );

}

EQOctreeManager::~EQOctreeManager()
{
	clearScene();


	//
	if ( mOctree )
	{
		delete mOctree;
		mOctree = 0;
	}

	OGRE_DELETE mOctreeRoot;
}

void EQOctreeManager::destroyOctreeNode( const std::string &name )
{
	EQOctreeNode * on = getOctreeNode( name );

	if ( on != 0 )
		_removeOctreeNode( on );


	//////
	OctreeNodeList::iterator i = mOctreeNodes.find(name);

	if (i == mOctreeNodes.end())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "EQOctreeNode '" + name + "' not found.",
			"EQOctreeManager::destroySceneNode");
	}

	// detach from parent (don't do this in destructor since bulk destruction
	// behaves differently)
	Node* parentNode = i->second->getParent();
	if (parentNode)
	{
		parentNode->removeChild(i->second);
	}
	OGRE_DELETE i->second;
	mOctreeNodes.erase(i);
}

EQOctreeNode * EQOctreeManager::createOctreeNodeImpl( void )
{
	return OGRE_NEW EQOctreeNode( this );
}

EQOctreeNode * EQOctreeManager::createOctreeNodeImpl( const std::string &name )
{
	return OGRE_NEW EQOctreeNode( this, name );
}

void EQOctreeManager::_updateOctreeNode( EQOctreeNode * onode )
{
	const AxisAlignedBox& box = onode -> _getWorldAABB();

	if ( box.isNull() )
		return ;

	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

	if ( onode -> getOctant() == 0 )
	{
		//if outside the octree, force into the root node.
		if ( ! onode -> _isIn( mOctree -> mBox ) )
			mOctree->_addNode( onode );
		else
			_addOctreeNode( onode, mOctree );
		return ;
	}

	if ( ! onode -> _isIn( onode -> getOctant() -> mBox ) )
	{
		_removeOctreeNode( onode );

		//if outside the octree, force into the root node.
		if ( ! onode -> _isIn( mOctree -> mBox ) )
			mOctree->_addNode( onode );
		else
			_addOctreeNode( onode, mOctree );
	}
}

/** Only removes the node from the octree.  It leaves the octree, even if it's empty.
*/
void EQOctreeManager::_removeOctreeNode( EQOctreeNode * n )
{
	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

	EQOctree * oct = n -> getOctant();

	if ( oct )
	{
		oct -> _removeNode( n );
	}

	n->setOctant(0);
}

void EQOctreeManager::_addOctreeNode( EQOctreeNode * n, EQOctree *octant, int depth )
{

	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

	const AxisAlignedBox& bx = n -> _getWorldAABB();


	//if the octree is twice as big as the scene node,
	//we will add it to a child.
	if ( ( depth < mMaxDepth ) && octant -> _isTwiceSize( bx ) )
	{
		int x, y, z;
		octant -> _getChildIndexes( bx, &x, &y, &z );

		if ( octant -> mChildren[ x ][ y ][ z ] == 0 )
		{
			octant -> mChildren[ x ][ y ][ z ] = new EQOctree( octant );
			const Vector3& octantMin = octant -> mBox.getMinimum();
			const Vector3& octantMax = octant -> mBox.getMaximum();
			Vector3 min, max;

			if ( x == 0 )
			{
				min.x = octantMin.x;
				max.x = ( octantMin.x + octantMax.x ) / 2;
			}

			else
			{
				min.x = ( octantMin.x + octantMax.x ) / 2;
				max.x = octantMax.x;
			}

			if ( y == 0 )
			{
				min.y = octantMin.y;
				max.y = ( octantMin.y + octantMax.y ) / 2;
			}

			else
			{
				min.y = ( octantMin.y + octantMax.y ) / 2;
				max.y = octantMax.y;
			}

			if ( z == 0 )
			{
				min.z = octantMin.z;
				max.z = ( octantMin.z + octantMax.z ) / 2;
			}

			else
			{
				min.z = ( octantMin.z + octantMax.z ) / 2;
				max.z = octantMax.z;
			}

			octant -> mChildren[ x ][ y ][ z ] -> mBox.setExtents( min, max );
			octant -> mChildren[ x ][ y ][ z ] -> mHalfSize = ( max - min ) / 2;
		}

		_addOctreeNode( n, octant -> mChildren[ x ][ y ][ z ], ++depth );

	}

	else
	{
		octant -> _addNode( n );
	}
}

void EQOctreeManager::_updateSceneGraph()
{
	getRootNode()->_update(true, false);
}

// --- non template versions
void _findNodes( const AxisAlignedBox &t, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude, bool full, EQOctree *octant )
{

	if ( !full )
	{
		AxisAlignedBox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersect( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		EQOctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersect( t, on -> _getWorldAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	EQOctree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}

void _findNodes( const Sphere &t, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude, bool full, EQOctree *octant )
{

	if ( !full )
	{
		AxisAlignedBox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersect( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		EQOctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersect( t, on -> _getWorldAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	EQOctree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}


void _findNodes( const PlaneBoundedVolume &t, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude, bool full, EQOctree *octant )
{

	if ( !full )
	{
		AxisAlignedBox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersect( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		EQOctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersect( t, on -> _getWorldAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	EQOctree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}

void _findNodes( const Ray &t, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude, bool full, EQOctree *octant )
{

	if ( !full )
	{
		AxisAlignedBox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersect( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		EQOctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersect( t, on -> _getWorldAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	EQOctree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}

void EQOctreeManager::findNodesIn( const AxisAlignedBox &box, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude )
{
	_findNodes( box, list, exclude, false, mOctree );
}

void EQOctreeManager::findNodesIn( const Sphere &sphere, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude )
{
	_findNodes( sphere, list, exclude, false, mOctree );
}

void EQOctreeManager::findNodesIn( const PlaneBoundedVolume &volume, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude )
{
	_findNodes( volume, list, exclude, false, mOctree );
}

void EQOctreeManager::findNodesIn( const Ray &r, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude )
{
	_findNodes( r, list, exclude, false, mOctree );
}

void EQOctreeManager::resize( const AxisAlignedBox &box )
{
	std::list < EQOctreeNode * > nodes;
	std::list < EQOctreeNode * > ::iterator it;

	_findNodes( mOctree->mBox, nodes, 0, true, mOctree );

	delete mOctree;

	mOctree = new EQOctree( 0 );
	mOctree->mBox = box;

	const Vector3 min = box.getMinimum();
	const Vector3 max = box.getMaximum();
	mOctree->mHalfSize = ( max - min ) * 0.5f;

	it = nodes.begin();

	while ( it != nodes.end() )
	{
		EQOctreeNode * on = ( *it );
		on -> setOctant( 0 );
		_updateOctreeNode( on );
		++it;
	}

}

void EQOctreeManager::clearScene(void)
{
	clearSceneBase();
	init(mBox, mMaxDepth);

}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void EQOctreeManager::clearSceneBase()
{
	getRootNode()->removeAllChildren();

	for (OctreeNodeList::iterator i = mOctreeNodes.begin();
		i != mOctreeNodes.end(); ++i)
	{
		OGRE_DELETE i->second;
	}
	mOctreeNodes.clear();

	mLastNotifyNodes.clear();
}
//-----------------------------------------------------------------------
EQOctreeNode* EQOctreeManager::createOctreeNode(void)
{
	EQOctreeNode* sn = createOctreeNodeImpl();
	assert(mOctreeNodes.find(sn->getName()) == mOctreeNodes.end());
	mOctreeNodes[sn->getName()] = sn;
	return sn;
}
//-----------------------------------------------------------------------
EQOctreeNode* EQOctreeManager::createOctreeNode(const std::string& name)
{
	// Check name not used
	if (mOctreeNodes.find(name) != mOctreeNodes.end())
	{
		OGRE_EXCEPT(
			Ogre::Exception::ERR_DUPLICATE_ITEM,
			"A scene node with the name " + name + " already exists",
			"EQOctreeManager::createOctreeNode" );
	}

	EQOctreeNode* sn = createOctreeNodeImpl(name);
	mOctreeNodes[sn->getName()] = sn;
	return sn;
}
//-----------------------------------------------------------------------
EQOctreeNode* EQOctreeManager::getOctreeNode(const std::string& name) const
{
	OctreeNodeList::const_iterator i = mOctreeNodes.find(name);

	if (i == mOctreeNodes.end())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "EQOctreeNode '" + name + "' not found.",
			"EQOctreeManager::getOctreeNode");
	}

	return i->second;

}
//-----------------------------------------------------------------------
bool EQOctreeManager::hasOctreeNode(const std::string& name) const
{
	return (mOctreeNodes.find(name) != mOctreeNodes.end());
}
//-----------------------------------------------------------------------
EQOctreeNode* EQOctreeManager::getRootNode(void)
{
	if (!mOctreeRoot)
	{
		// Create root scene node
		mOctreeRoot = createOctreeNodeImpl("EQ/OctreeRoot");
	}

	return mOctreeRoot;
}
//-----------------------------------------------------------------------
void EQOctreeManager::cutNodes(const Ogre::Vector3& center)
{
	// 通知上一次的物件显示的物件
	for ( uint i = 0; i < mLastNotifyNodes.size(); ++i )
		mLastNotifyNodes[i]->getAttatch()->notifyLastCheck();

	Ogre::Sphere sphere(center, mRadius);

	std::list < EQOctreeNode * > list;
	findNodesIn( sphere, list, 0 );

	//清除
	mLastNotifyNodes.clear();
	mLastNotifyNodes.reserve(list.size());

	 std::list < EQOctreeNode * >::iterator it = list.begin();
	 for(; it != list.end(); ++it )
	 {
		 if ( sphere.intersects((*it)->_getWorldAABB()) )
			 mLastNotifyNodes.push_back((*it));
	 }

	// 通知这次的物件显示
	 for ( uint i = 0; i < mLastNotifyNodes.size(); ++i )
		 mLastNotifyNodes[i]->getAttatch()->notifyCheck();
}
