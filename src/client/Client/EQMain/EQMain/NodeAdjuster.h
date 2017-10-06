/******************************************************************************
* ����: Hongjx
* ʱ��: 2009-12-09
* ����: �ڵ������(����ѡ���ƶ�����ת�����ŵ�)
******************************************************************************/
#ifndef NodeAdjuster_H
#define NodeAdjuster_H

#include "EQMainPrerequisites.h"




enum NodeOperationType
{
	OP_SELECT_NODE = 1, // ѡ��
	OP_MOVE_NODE,		// �ƶ�
	OP_ROTATE_NODE,		// ��ת
	OP_SCALE_NODE,		// ����
};



class NodeAdjuster
{
public:
	enum { DEFAULT_QUERY_FLAGS = 32 };
public:
	virtual ~NodeAdjuster();
	
	NodeAdjuster( const std::string & arrowMesh = "scbArrowEx.mesh" );

public:
	/** ��ĳ���ڵ���
	*/
	void attach( Ogre::SceneNode * node, Ogre::Camera * cam );
	
	/** 
	*/
	void detach();
	

	/** �������������
	*/
	void resetMaterial();
	
	/** ���ò���ģʽ( ѡ���ƶ�����ת������ )
	@param mode ��ο� NodeOperationType
	*/
	void setMode( NodeOperationType mode );

	/** �Զ������������С
	*/
	void autoScaleXYZ();


	void pickObject( int x, int y,  Ogre::Camera * cam );
	
	/** ����ƶ� 
	@param xt x��ƫ����/screen width
	@param yt y��ƫ����/screen height
	*/
	void onMouseDrag( float xt, float yt );

	void setQueryFlags( uint val );
	uint getQueryFlags() const;

protected:
	bool isVisible() const;
	void setVisible( bool val );
protected:
	void _setSubEntityVisible( bool b0, bool b1, bool b2, bool b3 );
	/** ���ø�����ʵ��Ŀɼ��ԣ��Ա���ϳ�������״
	*/
	void _setSubEntityVisible( Ogre::Entity * ent, bool b0, bool b1, bool b2, bool b3 );	

	void _init();

	/** ��ѡ������
	*/
	bool _pickObject( const Ogre::Ray & kRay );
protected:
	bool				mVisible;
	NodeOperationType	mOpMode; // ����ģʽ (ѡ���ƶ�����ת������)
	uint				mQueryFlags;
	
	Ogre::Camera *		mCamera;
	Ogre::SceneNode *	mOpNode; // �������Ľڵ�

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