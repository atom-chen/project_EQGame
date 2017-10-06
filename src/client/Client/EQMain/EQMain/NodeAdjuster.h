/******************************************************************************
* 作者: Hongjx
* 时间: 2009-12-09
* 描述: 节点调整类(处理选择，移动，旋转，缩放等)
******************************************************************************/
#ifndef NodeAdjuster_H
#define NodeAdjuster_H

#include "EQMainPrerequisites.h"




enum NodeOperationType
{
	OP_SELECT_NODE = 1, // 选择
	OP_MOVE_NODE,		// 移动
	OP_ROTATE_NODE,		// 旋转
	OP_SCALE_NODE,		// 缩放
};



class NodeAdjuster
{
public:
	enum { DEFAULT_QUERY_FLAGS = 32 };
public:
	virtual ~NodeAdjuster();
	
	NodeAdjuster( const std::string & arrowMesh = "scbArrowEx.mesh" );

public:
	/** 绑到某个节点上
	*/
	void attach( Ogre::SceneNode * node, Ogre::Camera * cam );
	
	/** 
	*/
	void detach();
	

	/** 重设三根轴材质
	*/
	void resetMaterial();
	
	/** 设置操作模式( 选择，移动，旋转，缩放 )
	@param mode 请参考 NodeOperationType
	*/
	void setMode( NodeOperationType mode );

	/** 自动调整三根轴大小
	*/
	void autoScaleXYZ();


	void pickObject( int x, int y,  Ogre::Camera * cam );
	
	/** 鼠标移动 
	@param xt x轴偏移量/screen width
	@param yt y轴偏移量/screen height
	*/
	void onMouseDrag( float xt, float yt );

	void setQueryFlags( uint val );
	uint getQueryFlags() const;

protected:
	bool isVisible() const;
	void setVisible( bool val );
protected:
	void _setSubEntityVisible( bool b0, bool b1, bool b2, bool b3 );
	/** 设置各个子实体的可见性，以便组合出各种形状
	*/
	void _setSubEntityVisible( Ogre::Entity * ent, bool b0, bool b1, bool b2, bool b3 );	

	void _init();

	/** 挑选三根轴
	*/
	bool _pickObject( const Ogre::Ray & kRay );
protected:
	bool				mVisible;
	NodeOperationType	mOpMode; // 操作模式 (选择，移动，旋转，缩放)
	uint				mQueryFlags;
	
	Ogre::Camera *		mCamera;
	Ogre::SceneNode *	mOpNode; // 被操作的节点

	Ogre::Entity *		mxEntity;
	Ogre::Entity *		myEntity;
	Ogre::Entity *		mzEntity;
	Ogre::Entity *		mSelectedEntity;
	Ogre::SceneNode *	mxNode;
	Ogre::SceneNode *	myNode;
	Ogre::SceneNode *	mzNode;
	Ogre::SceneNode *	mXYZNode;
	std::string			mXYZMesh;
};





#endif