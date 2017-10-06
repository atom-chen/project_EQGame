/******************************************************************************
* 作者: Y3
* 时间: 2009-10-21
* 描述: Ogre的一些通用函数包装
******************************************************************************/
#ifndef CommFuncGcs_H
#define CommFuncGcs_H

#include "EQMainPrerequisites.h"

#include "ModelGcs.h"


/** 加载模型时的监听
@remarks processSkeletonName()会悠闲在模型的同一个目录搜索骨骼文件, 这样确保骨骼
	不必要一定放在根目录
*/
class EQMeshSerializerListener : public Ogre::MeshSerializerListener
{
public:
	void processMaterialName(Ogre::Mesh *mesh, std::string *name);
	void processSkeletonName(Ogre::Mesh *mesh, std::string *name);
};

extern EQMeshSerializerListener g_EQMeshSerializerListener;


/** 为骨骼创建一个空模型
@remarks
	mesh的名字与骨骼同名. 如果不设置boundingBox则自行计算骨骼影响的bounding box
*/
extern Ogre::MeshPtr loadNullMeshForSkeleton(const Ogre::SkeletonPtr& skeleton,
											 Ogre::AxisAlignedBox boundingBox = Ogre::AxisAlignedBox::BOX_NULL);

/** 空的ManualResourceLoader. 防止ogre的警告
*/
class NullManualResourceLoader : public Ogre::ManualResourceLoader
{
public:
	void loadResource(Ogre::Resource* resource) {}
};

extern NullManualResourceLoader g_NullManualResourceLoader;


/** 简单创建ModelGcs规则
*/
class SimpleCreateModelRule : public ModelGcs::CreateModelRuleGcs
{
public:
	SimpleCreateModelRule( const std::string& meshName, const std::string& matName = "", uint particle = 0 );

	virtual void create( ModelGcs * pModel );

private:
	//禁止默认构造函数
	SimpleCreateModelRule();

private:
	std::string mMeshName;
	std::string mMatName;
	uint		mParticle;
};


/** 创建网格函数
@remark 此函数用来创建网格，并返回一个ManualObject的对象供调用者使用
@param manager 场景管理器
@param strUniqueName 唯一名称
@param kSize 网格大小
@param kGridNum 半径网格数
@param kGapNum 画线间隔数
@param LineColour 网格颜色
@param CoreLineColour 中心线颜色
@param GapLineColour 间隔线颜色
*/
extern Ogre::ManualObject* CreateManualGrid(Ogre::SceneManager *mManager,
											Ogre::String mGridName,	
											Ogre::Real mSize,
											Ogre::Real mGridNum,
											int mGapNum = 0,                       
											Ogre::ColourValue mLineColour = Ogre::ColourValue::White,
											Ogre::ColourValue mCoreLineColour = Ogre::ColourValue::Red,
											Ogre::ColourValue mGapLineColour = Ogre::ColourValue::Green);

/** 创建矩形
@param sceneMgr 场景管理器
@param x 边长. 
@param z 边长. 由于是xz平面, 所以是z
@param colour 颜色, 可以带透明通道
*/
extern Ogre::ManualObject* CreateManualQuad(Ogre::SceneManager *sceneMgr, float x, float z, const Ogre::ColourValue& colour );

extern Ogre::ManualObject* CreateManualQuad( Ogre::SceneManager * sceneMgr, 
											const Ogre::Vector3 & a, 
											const Ogre::Vector3 & b, 
											const Ogre::Vector3 & c, 
											const Ogre::Vector3 & d, 
											const Ogre::ColourValue & colour );


/** 创建中空方形
@param sceneMgr 场景管理器
@param radius 方形的半径
@param thickness 方形的厚度
@param colour 颜色, 可以带透明通道
*/
extern Ogre::ManualObject* CreateHollowQuad(Ogre::SceneManager *sceneMgr, float radius, float thickness, const Ogre::ColourValue& colour );



/** 从字符串中提取颜色
#remarks 字符串的格式为 #RGB[A] . A透明,可以省略. 数值为16进制,范围为00-FF. 举例如 #FF00CCFF
*/
extern Ogre::ColourValue extractColorFromString( const std::string& str );

/** 创建显示数字的矩形(不支持负数)
@param sceneMgr 场景管理器
@param x 边长. 
@param z 边长. 由于是xz平面, 所以是z
@param number 数字
*/
extern Ogre::ManualObject* CreateNumberQuad(Ogre::SceneManager *sceneMgr, float x, float z, Ogre::uint number);

/** 创建球体
@param sceneMgr 场景管理器
@param r 半径
@param nRings 纵向切片
@param nSegments 横向切片
*/
extern Ogre::ManualObject* createSphere(Ogre::SceneManager *sceneMgr, const float r, const int nRings = 16, const int nSegments = 16);

/** 创建球体Mesh
@param sceneMgr 场景管理器
@param r 半径
@param nRings 纵向切片
@param nSegments 横向切片
*/
extern Ogre::MeshPtr createSphereMesh(Ogre::SceneManager *sceneMgr, const float r, const int nRings = 16, const int nSegments = 16);

/** 创建纯色自发光的材质
*/
extern Ogre::MaterialPtr createPureColourMaterial(const Ogre::ColourValue& colour);

/** 创建颜色材质
*/
extern Ogre::MaterialPtr createColourMaterial(const Ogre::ColourValue& colour);

/** 创建颜色材质
*/
extern Ogre::MaterialPtr createColourMaterial(const Ogre::ColourValue& colour, const Ogre::ColourValue& specular, Real shininess);

/** 创建投影材质
*/
extern Ogre::MaterialPtr createProjectiveMaterial(const std::string& materialTemplateName, const Ogre::Frustum* frustum);

/** 创建线段
*/
extern Ogre::ManualObject* createLine(Ogre::SceneManager *sceneMgr, const std::vector<Ogre::Vector3>& ptList, const Ogre::ColourValue& colour, Ogre::RenderOperation::OperationType ro = Ogre::RenderOperation::OT_LINE_STRIP);

#endif