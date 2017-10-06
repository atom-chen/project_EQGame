/******************************************************************************
* 作者: Y3
* 时间: 2011-3-15
* 描述: 
******************************************************************************/
#ifndef EQOCTREE_H
#define EQOCTREE_H

#include <Ogre/OgreAxisAlignedBox.h>
#include <Ogre/OgreWireBoundingBox.h>

#include <list>



class EQOctreeNode;

typedef std::list < EQOctreeNode * > NodeList;


/** Octree datastructure for managing scene nodes.
@remarks
This is a loose octree implementation, meaning that each
octant child of the octree actually overlaps it's siblings by a factor
of .5.  This guarantees that any thing that is half the size of the parent will
fit completely into a child, with no splitting necessary.
*/

class EQOctree
{
public:
	EQOctree( EQOctree * p );
	~EQOctree();

	/** Adds an Octree scene node to this octree level.
	@remarks
	This is called by the OctreeSceneManager after
	it has determined the correct Octree to insert the node into.
	*/
	void _addNode( EQOctreeNode * );

	/** Removes an Octree scene node to this octree level.
	*/
	void _removeNode( EQOctreeNode * );

	/** Returns the number of scene nodes attached to this octree
	*/
	int numNodes()
	{
		return mNumNodes;
	};

	/** The bounding box of the octree
	@remarks
	This is used for octant index determination and rendering, but not culling
	*/
	Ogre::AxisAlignedBox mBox;
	Ogre::WireBoundingBox* mWireBoundingBox;

	/** Creates the wire frame bounding box for this octant
	*/
	Ogre::WireBoundingBox* getWireBoundingBox();

	/** Vector containing the dimensions of this octree / 2
	*/
	Ogre::Vector3 mHalfSize;

	/** 3D array of children of this octree.
	@remarks
	Children are dynamically created as needed when nodes are inserted in the Octree.
	If, later, all the nodes are removed from the child, it is still kept around.
	*/
	EQOctree * mChildren[ 2 ][ 2 ][ 2 ];

	/** Determines if this octree is twice as big as the given box.
	@remarks
	This method is used by the OctreeSceneManager to determine if the given
	box will fit into a child of this octree.
	*/
	bool _isTwiceSize( const Ogre::AxisAlignedBox &box ) const;

	/**  Returns the appropriate indexes for the child of this octree into which the box will fit.
	@remarks
	This is used by the OctreeSceneManager to determine which child to traverse next when
	finding the appropriate octree to insert the box.  Since it is a loose octree, only the
	center of the box is checked to determine the octant.
	*/
	void _getChildIndexes( const Ogre::AxisAlignedBox &, int *x, int *y, int *z ) const;

	/** Creates the AxisAlignedBox used for culling this octree.
	@remarks
	Since it's a loose octree, the culling bounds can be different than the actual bounds of the octree.
	*/
	void _getCullBounds( Ogre::AxisAlignedBox * ) const;


	/** Public list of SceneNodes attached to this particular octree
	*/
	NodeList mNodes;

protected:

	/** Increments the overall node count of this octree and all its parents
	*/
	inline void _ref()
	{
		mNumNodes++;

		if ( mParent != 0 ) mParent -> _ref();
	};

	/** Decrements the overall node count of this octree and all its parents
	*/
	inline void _unref()
	{
		mNumNodes--;

		if ( mParent != 0 ) mParent -> _unref();
	};

	///number of SceneNodes in this octree and all its children.
	int mNumNodes;

	///parent octree
	EQOctree * mParent;

};



#endif


