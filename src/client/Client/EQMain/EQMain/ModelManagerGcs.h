/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-28
* ����: ���ģ�͹�����
******************************************************************************/
#ifndef ModelManagerGcs_H
#define ModelManagerGcs_H

#include "EQMainPrerequisites.h"
#include "ModelGcs.h"

class ModelManagerGcs : public Ogre::Singleton<ModelManagerGcs>
{
public:
	ModelManagerGcs();
	~ModelManagerGcs();

	/** ͨ�����򴴽�ģ��ʵ��
	@param rule ����
	@param neme ����, ����Ψһ, ���Ϊ��,�����Զ�����һ������
	*/
	ModelGcs* createModel(ModelGcs::CreateModelRuleGcs* rule, const std::string& name = "");

	/// ����ģ��ʵ��, �����ҽӵ�ģ��ʵ��һ������, ����һ�ο������ٶ��.
	void destroyModel(const std::string& name);

	void destroyModel(ModelGcs* model);

	bool hasModel(const std::string& name);

	ModelGcs* getModel(const std::string& name);

	void updateAllModels( float delta );

public:
	typedef std::map<std::string, ModelGcs*> ModelMap;

protected:
	ModelMap			mModelMap;

	ModelMap			mDelayRemoveModels;
	ModelMap			mDelayAttatchModels;

	bool				mUpdating;
	
	ulong				mAutoModelNum;
};


#endif
