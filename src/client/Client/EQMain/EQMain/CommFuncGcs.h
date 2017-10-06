/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-21
* ����: Ogre��һЩͨ�ú�����װ
******************************************************************************/
#ifndef CommFuncGcs_H
#define CommFuncGcs_H

#include "EQMainPrerequisites.h"

#include "ModelGcs.h"


/** ����ģ��ʱ�ļ���
@remarks processSkeletonName()��������ģ�͵�ͬһ��Ŀ¼���������ļ�, ����ȷ������
	����Ҫһ�����ڸ�Ŀ¼
*/
class EQMeshSerializerListener : public Ogre::MeshSerializerListener
{
public:
	void processMaterialName(Ogre::Mesh *mesh, std::string *name);
	void processSkeletonName(Ogre::Mesh *mesh, std::string *name);
};

extern EQMeshSerializerListener g_EQMeshSerializerListener;


/** Ϊ��������һ����ģ��
@remarks
	mesh�����������ͬ��. ���������boundingBox�����м������Ӱ���bounding box
*/
extern Ogre::MeshPtr loadNullMeshForSkeleton(const Ogre::SkeletonPtr& skeleton,
											 Ogre::AxisAlignedBox boundingBox = Ogre::AxisAlignedBox::BOX_NULL);

/** �յ�ManualResourceLoader. ��ֹogre�ľ���
*/
class NullManualResourceLoader : public Ogre::ManualResourceLoader
{
public:
	void loadResource(Ogre::Resource* resource) {}
};

extern NullManualResourceLoader g_NullManualResourceLoader;


/** �򵥴���ModelGcs����
*/
class SimpleCreateModelRule : public ModelGcs::CreateModelRuleGcs
{
public:
	SimpleCreateModelRule( const std::string& meshName, const std::string& matName = "", uint particle = 0 );

	virtual void create( ModelGcs * pModel );

private:
	//��ֹĬ�Ϲ��캯��
	SimpleCreateModelRule();

private:
	std::string mMeshName;
	std::string mMatName;
	uint		mParticle;
};


/** ����������
@remark �˺��������������񣬲�����һ��ManualObject�Ķ��󹩵�����ʹ��
@param manager ����������
@param strUniqueName Ψһ����
@param kSize �����С
@param kGridNum �뾶������
@param kGapNum ���߼����
@param LineColour ������ɫ
@param CoreLineColour ��������ɫ
@param GapLineColour �������ɫ
*/
extern Ogre::ManualObject* CreateManualGrid(Ogre::SceneManager *mManager,
											Ogre::String mGridName,	
											Ogre::Real mSize,
											Ogre::Real mGridNum,
											int mGapNum = 0,                       
											Ogre::ColourValue mLineColour = Ogre::ColourValue::White,
											Ogre::ColourValue mCoreLineColour = Ogre::ColourValue::Red,
											Ogre::ColourValue mGapLineColour = Ogre::ColourValue::Green);

/** ��������
@param sceneMgr ����������
@param x �߳�. 
@param z �߳�. ������xzƽ��, ������z
@param colour ��ɫ, ���Դ�͸��ͨ��
*/
extern Ogre::ManualObject* CreateManualQuad(Ogre::SceneManager *sceneMgr, float x, float z, const Ogre::ColourValue& colour );

extern Ogre::ManualObject* CreateManualQuad( Ogre::SceneManager * sceneMgr, 
											const Ogre::Vector3 & a, 
											const Ogre::Vector3 & b, 
											const Ogre::Vector3 & c, 
											const Ogre::Vector3 & d, 
											const Ogre::ColourValue & colour );


/** �����пշ���
@param sceneMgr ����������
@param radius ���εİ뾶
@param thickness ���εĺ��
@param colour ��ɫ, ���Դ�͸��ͨ��
*/
extern Ogre::ManualObject* CreateHollowQuad(Ogre::SceneManager *sceneMgr, float radius, float thickness, const Ogre::ColourValue& colour );



/** ���ַ�������ȡ��ɫ
#remarks �ַ����ĸ�ʽΪ #RGB[A] . A͸��,����ʡ��. ��ֵΪ16����,��ΧΪ00-FF. ������ #FF00CCFF
*/
extern Ogre::ColourValue extractColorFromString( const std::string& str );

/** ������ʾ���ֵľ���(��֧�ָ���)
@param sceneMgr ����������
@param x �߳�. 
@param z �߳�. ������xzƽ��, ������z
@param number ����
*/
extern Ogre::ManualObject* CreateNumberQuad(Ogre::SceneManager *sceneMgr, float x, float z, Ogre::uint number);

/** ��������
@param sceneMgr ����������
@param r �뾶
@param nRings ������Ƭ
@param nSegments ������Ƭ
*/
extern Ogre::ManualObject* createSphere(Ogre::SceneManager *sceneMgr, const float r, const int nRings = 16, const int nSegments = 16);

/** ��������Mesh
@param sceneMgr ����������
@param r �뾶
@param nRings ������Ƭ
@param nSegments ������Ƭ
*/
extern Ogre::MeshPtr createSphereMesh(Ogre::SceneManager *sceneMgr, const float r, const int nRings = 16, const int nSegments = 16);

/** ������ɫ�Է���Ĳ���
*/
extern Ogre::MaterialPtr createPureColourMaterial(const Ogre::ColourValue& colour);

/** ������ɫ����
*/
extern Ogre::MaterialPtr createColourMaterial(const Ogre::ColourValue& colour);

/** ������ɫ����
*/
extern Ogre::MaterialPtr createColourMaterial(const Ogre::ColourValue& colour, const Ogre::ColourValue& specular, Real shininess);

/** ����ͶӰ����
*/
extern Ogre::MaterialPtr createProjectiveMaterial(const std::string& materialTemplateName, const Ogre::Frustum* frustum);

/** �����߶�
*/
extern Ogre::ManualObject* createLine(Ogre::SceneManager *sceneMgr, const std::vector<Ogre::Vector3>& ptList, const Ogre::ColourValue& colour, Ogre::RenderOperation::OperationType ro = Ogre::RenderOperation::OT_LINE_STRIP);

#endif