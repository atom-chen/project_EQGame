/******************************************************************************
* 作者: Y3
* 时间: 2011-3-15
* 描述: 
******************************************************************************/
#ifndef EQOCTREEMANAGER_H
#define EQOCTREEMANAGER_H

#include "Ogre/OgreSphere.h"
#include "Ogre/OgrePlaneBoundedVolume.h"
#include "Ogre/OgreSceneQuery.h"

#include <list>
#include <algorithm>

#include "EQOctree.h"




class EQOctreeNode;



typedef std::list < Ogre::WireBoundingBox * > BoxList;
typedef std::list < unsigned long > ColorList;
//typedef std::list < EQOctreeNode * > SceneNodeList;


/** Specialized SceneManager that divides the geometry into an octree in order to facilitate spatial queries.
@remarks
*/

class EQOctreeManager
{
public:
	static int intersect_call;
	/** Standard Constructor.  Initializes the octree to -10000,-10000,-10000 to 10000,10000,10000 with a depth of 8. */
	EQOctreeManager();
	/** Standard Constructor */
	EQOctreeManager( Ogre::AxisAlignedBox &box, int max_depth );
	/** Standard destructor */
	~EQOctreeManager();

	/** Initializes the manager to the given box and depth.
	*/
	void init( Ogre::AxisAlignedBox &box, int d );

	/** Creates a specialized EQOctreeNode */
	virtual EQOctreeNode * createOctreeNodeImpl ( void );
	/** Creates a specialized EQOctreeNode */
	virtual EQOctreeNode * createOctreeNodeImpl ( const std::string &name );
	/** Creates a specialized OctreeCamera */
	//virtual Camera * createCamera( const std::string &name );

	/** Deletes a scene node */
	virtual void destroyOctreeNode( const std::string &name );



	//这个函数很重要, 需要在图变更的时候去更新他, 在ogre里面是每帧更新, 我们看需求更新
	/** Does nothing more */
	virtual void _updateSceneGraph();


	/** Checks the given EQOctreeNode, and determines if it needs to be moved
	* to a different octant.
	*/
	void _updateOctreeNode( EQOctreeNode * );
	/** Removes the given octree node */
	void _removeOctreeNode( EQOctreeNode * );
	/** Adds the EQOctree Node, starting at the given octree, and recursing at max to the specified depth.
	*/
	void _addOctreeNode( EQOctreeNode *, EQOctree *octree, int depth = 0 );

	/** Recurses the octree, adding any nodes intersecting with the box into the given list.
	It ignores the exclude scene node.
	*/
	void findNodesIn( const Ogre::AxisAlignedBox &box, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude = 0 );

	/** Recurses the octree, adding any nodes intersecting with the sphere into the given list.
	It ignores the exclude scene node.
	*/
	void findNodesIn( const Ogre::Sphere &sphere, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude = 0 );

	/** Recurses the octree, adding any nodes intersecting with the volume into the given list.
	It ignores the exclude scene node.
	*/
	void findNodesIn( const Ogre::PlaneBoundedVolume &volume, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude=0 );

	/** Recurses the octree, adding any nodes intersecting with the ray into the given list.
	It ignores the exclude scene node.
	*/
	void findNodesIn( const Ogre::Ray &ray, std::list < EQOctreeNode * > &list, EQOctreeNode *exclude=0 );

	/** Sets the box visibility flag */
	void setShowBoxes( bool b )
	{
		mShowBoxes = b;
	};



	/** Resizes the octree to the given size */
	void resize( const Ogre::AxisAlignedBox &box );


	/** Overridden from SceneManager */
	void clearScene(void);


public:
	void cutNodes(const Ogre::Vector3& center);
	void setRadius(float r) {mRadius = r;}

	void clearSceneBase();
	EQOctreeNode* createOctreeNode(void);
	EQOctreeNode* createOctreeNode(const std::string& name);
	EQOctreeNode* getOctreeNode(const std::string& name) const;
	bool hasOctreeNode(const std::string& name) const;
	EQOctreeNode* EQOctreeManager::getRootNode(void);

protected:
	EQOctreeNode* mOctreeRoot;

	typedef std::map<std::string, EQOctreeNode*> OctreeNodeList;
	OctreeNodeList	mOctreeNodes;

	std::vector<EQOctreeNode*> mLastNotifyNodes;
	float mRadius;

protected:



	//NodeList mVisible;														//没用到

	/// The root octree
	EQOctree *mOctree;

	/// List of boxes to be rendered
	BoxList mBoxes;

	/// Number of rendered objs
	int mNumObjects;

	/// Max depth for the tree
	int mMaxDepth;
	/// Size of the octree
	Ogre::AxisAlignedBox mBox;

	/// Boxes visibility flag
	bool mShowBoxes;


	//bool mLoose;																//没用到

	//Ogre::Real mCorners[ 24 ];												//没用到
	static unsigned long mColors[ 8 ];
	static unsigned short mIndexes[ 24 ];

	Ogre::Matrix4 mScaleFactor;

};



#endif