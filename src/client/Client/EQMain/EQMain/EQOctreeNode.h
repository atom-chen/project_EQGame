/******************************************************************************
* 作者: Y3
* 时间: 2011-3-15
* 描述: 
******************************************************************************/
#ifndef EQOCTREENODE_H
#define EQOCTREENODE_H

#include <Ogre/OgreNode.h>
#include <Ogre/OgreAxisAlignedBox.h>
#include "EQOctreeSpirit.h"


class EQOctreeManager;
class EQOctree;



/** Specialized SceneNode that is customized for working within an EQOctree. Each node
* maintains its own bounding box, rather than merging it with all the children.
*
*/

class EQOctreeNode : public Ogre::Node
{
public:
	/** Standard constructor */
	EQOctreeNode( EQOctreeManager* creator );
	/** Standard constructor */
	EQOctreeNode( EQOctreeManager* creator, const std::string& name );
	/** Standard destructor */
	~EQOctreeNode();

	/** Overridden from Node to remove any reference to octants */
	Node * removeChild( unsigned short index );

	/** Overridden from Node to remove any reference to octants */
	Node * removeChild( const std::string & name );

	/** Overridden from Node to remove any reference to octants */
	Node * removeChild( Node* child);

	/** Overridden from Node to remove any reference to octants */
	void removeAllChildren(void);

	/** Returns the EQOctree in which this OctreeNode resides
	*/
	EQOctree * getOctant()
	{
		return mOctant;
	};

	/** Sets the EQOctree in which this OctreeNode resides
	*/
	void setOctant( EQOctree *o )
	{
		mOctant = o;
	};

	/** Determines if the center of this node is within the given box
	*/
	bool _isIn( Ogre::AxisAlignedBox &box );

	/** Adds all the attached scenenodes to the render queue
	*/
	//virtual void _addToRenderQueue( Camera* cam, RenderQueue * q, bool onlyShadowCasters, 
	//	VisibleObjectsBoundsInfo* visibleBounds);

	/** Sets up the LegacyRenderOperation for rendering this scene node as geometry.
	@remarks
	This will render the scenenode as a bounding box.
	*/
	//virtual void getRenderOperation( Ogre::RenderOperation& op );


public:

	void _update(bool updateChildren, bool parentHasChanged);
	
	const Ogre::AxisAlignedBox& _getWorldAABB(void) const { return mWorldAABB;}

	void attach(EQOctreeSpirit* spi);
	void unattatch();
	EQOctreeSpirit* getAttatch() {return mSpirit;}

protected:

	
	/** Internal method for creating a new child node - must be overridden per subclass. */
	virtual Node* createChildImpl(void);

	/** Internal method for creating a new child node - must be overridden per subclass. */
	virtual Node* createChildImpl(const std::string& name);




	EQOctreeManager* mCreator;
	Ogre::AxisAlignedBox mWorldAABB;
	EQOctreeSpirit* mSpirit;

protected:
	/** Internal method for updating the bounds for this OctreeNode.
	@remarks
	This method determines the bounds solely from the attached objects, not
	any children. If the node has changed its bounds, it is removed from its
	current octree, and reinserted into the tree.
	*/
	void _updateBounds( void );

	void _removeNodeAndChildren( );

	///local bounding box
	//Ogre::AxisAlignedBox mLocalAABB;					//没有用

	///EQOctree this node is attached to.
	EQOctree *mOctant;

	///preallocated corners for rendering
	Ogre::Real mCorners[ 24 ];



};




#endif