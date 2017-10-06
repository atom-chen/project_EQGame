/******************************************************************************
* 作者: Y3
* 时间: 2009-12-15
* 描述: 图形相关的表格
******************************************************************************/
#ifndef GraphicsTable_H
#define GraphicsTable_H

#include "EQMainPrerequisites.h"

/** 模型表现
*/
struct ModelDisplayRow
{
	ModelDisplayRow()
		: id( 0 )
		, showMark( 0 )
		, showPri( 0 )
	{
	}

	uint			id;
	std::string 	mesh;		// 模型文件
	std::string 	mat;		// 材质
	uint			showMark;	// 显示掩码
	uchar			showPri;	// 显示优先级
	uint			particle;	// 粒子
};
typedef stdext::hash_map<uint, ModelDisplayRow>		ModelDisplayTable; 


/** 主动光
*/
struct ActiveLightRow
{
	uint			ID;
	std::string		Name;
	uchar			LightType;		// 灯光类型
	Ogre::ColourValue	DiffCol;	// diffuse的颜色
	Ogre::ColourValue	SpecCol;	// Specular的颜色
	float			Attenuation[4];	// 衰减系数
	Ogre::Vector3	Pos;			// 位移
	Ogre::Vector3	Dir;			// 方向
	float			SpotRange[3];	// 聚光灯的范围
	uchar			Pierce;			// 穿透属性
	uint			PierceRange;	// 穿透半径
	std::string		BbsTexture;		// 灯光光晕纹理
	std::vector<Ogre::Vector3>	BbsPos; // 灯光光晕偏移
	uint			BbsType;		// 公告板类型
};
typedef std::map<uint, ActiveLightRow>				ActiveLightTable;


/** 穿透光
*/
struct PassLightRow
{
	uint			ID;
	std::string		Name;
	bool			Indoor;		// 只影响室内或者室外
	float			DayAtten[4];	// 白天衰减系数
	float			NightAtten[4];	// 晚上衰减系数
	Ogre::Vector3	Pos;		// 正方向位移
};
typedef std::map<uint, PassLightRow>				PassLightTable;


/** 灯光表
*/
struct LightRow
{
	uint			ID;
	std::string		Name;
	bool			Visible;			// Light1,2是否默认打开
	const ActiveLightRow*	Light1;		// 主动光
	const ActiveLightRow*	Light2;
	const PassLightRow*		PassLi1;	// 穿透光
	const PassLightRow*		PassLi2;
};
typedef std::map<uint, LightRow>					LightTable;

/** 动作融合表
*/
typedef std::map<std::string, float>				AnimationBlendTable;

/** 粒子系统表
*/
struct ParticleRow
{
	uint			 id;
	std::string		 name;
	std::string		 templateName;
	uint			 life;
	int				 delay;
	Ogre::Vector3	 pos;
	Ogre::Quaternion rot;
	Ogre::Vector3	 scale;
	std::string		 bone;
	uint			 trans;
};
typedef std::map<uint, ParticleRow>					ParticleTable;


/** 场景物件的模板表, 从house editor移过来
*/
struct SceneEntityObjRow
{
	uint			id;
	std::string		mesh;
	std::string		material;
	uint			light;
	std::string		cutLevel;
	uint			particle;
	uint			loop;
};

typedef std::vector<SceneEntityObjRow> SceneEntityObjTable;
typedef std::map<int, SceneEntityObjRow> SceneEntityObjMapTable;



class GraphicsTableManager : public Ogre::Singleton<GraphicsTableManager>
{
public:
	const ModelDisplayRow*		getModelDisplayRow( uint id ) const; 
	const ActiveLightRow*		getActiveLightRow(uint id) const;
	const PassLightRow*			getPassLightRow(uint id) const;
	const LightRow*				getLightRow(uint id) const;
	float						getAnimationBlend(std::string& key);
	const ParticleRow*			getParticleRow(uint id) const;
	const SceneEntityObjRow*	getSceneEntityObjRow(uint id) const;

	const ModelDisplayTable&	getModelDisplayTable() const { return mModelDisplayTable; }
	const ActiveLightTable&		getActiveLightTable() const { return mActiveLightTable; }
	const PassLightTable&		getPassLightTable() const { return mPassLightTable; }
	const LightTable&			getLightTable() const { return mLightTable; }
	SceneEntityObjTable&		getSceneEntityObjTable() {return mSceneEntityObjTable;}
	SceneEntityObjMapTable&		getSceneEntityObjMapTable() {return mSceneEntityObjMapTable;}

	void load();

private:
	void loadModelDisplayTable();
	void loadActiveLightTable();
	void loadPassLightTable();
	void loadLightTabel();
	void loadAnimationBlendTable();
	void loadParticleTable();
	void loadSceneEntityObjTable();

private:	
	ModelDisplayTable	mModelDisplayTable; 
	ActiveLightTable	mActiveLightTable;
	PassLightTable		mPassLightTable;
	LightTable			mLightTable;
	AnimationBlendTable	mAnimationBlendTable;
	ParticleTable		mParticleTable;
	SceneEntityObjTable	mSceneEntityObjTable;
	SceneEntityObjMapTable mSceneEntityObjMapTable;
};



#endif
