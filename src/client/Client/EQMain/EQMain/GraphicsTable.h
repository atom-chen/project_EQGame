/******************************************************************************
* ����: Y3
* ʱ��: 2009-12-15
* ����: ͼ����صı��
******************************************************************************/
#ifndef GraphicsTable_H
#define GraphicsTable_H

#include "EQMainPrerequisites.h"

/** ģ�ͱ���
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
	std::string 	mesh;		// ģ���ļ�
	std::string 	mat;		// ����
	uint			showMark;	// ��ʾ����
	uchar			showPri;	// ��ʾ���ȼ�
	uint			particle;	// ����
};
typedef stdext::hash_map<uint, ModelDisplayRow>		ModelDisplayTable; 


/** ������
*/
struct ActiveLightRow
{
	uint			ID;
	std::string		Name;
	uchar			LightType;		// �ƹ�����
	Ogre::ColourValue	DiffCol;	// diffuse����ɫ
	Ogre::ColourValue	SpecCol;	// Specular����ɫ
	float			Attenuation[4];	// ˥��ϵ��
	Ogre::Vector3	Pos;			// λ��
	Ogre::Vector3	Dir;			// ����
	float			SpotRange[3];	// �۹�Ƶķ�Χ
	uchar			Pierce;			// ��͸����
	uint			PierceRange;	// ��͸�뾶
	std::string		BbsTexture;		// �ƹ��������
	std::vector<Ogre::Vector3>	BbsPos; // �ƹ����ƫ��
	uint			BbsType;		// ���������
};
typedef std::map<uint, ActiveLightRow>				ActiveLightTable;


/** ��͸��
*/
struct PassLightRow
{
	uint			ID;
	std::string		Name;
	bool			Indoor;		// ֻӰ�����ڻ�������
	float			DayAtten[4];	// ����˥��ϵ��
	float			NightAtten[4];	// ����˥��ϵ��
	Ogre::Vector3	Pos;		// ������λ��
};
typedef std::map<uint, PassLightRow>				PassLightTable;


/** �ƹ��
*/
struct LightRow
{
	uint			ID;
	std::string		Name;
	bool			Visible;			// Light1,2�Ƿ�Ĭ�ϴ�
	const ActiveLightRow*	Light1;		// ������
	const ActiveLightRow*	Light2;
	const PassLightRow*		PassLi1;	// ��͸��
	const PassLightRow*		PassLi2;
};
typedef std::map<uint, LightRow>					LightTable;

/** �����ںϱ�
*/
typedef std::map<std::string, float>				AnimationBlendTable;

/** ����ϵͳ��
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


/** ���������ģ���, ��house editor�ƹ���
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
