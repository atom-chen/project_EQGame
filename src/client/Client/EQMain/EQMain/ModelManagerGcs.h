/******************************************************************************
* 作者: Y3
* 时间: 2009-10-28
* 描述: 物件模型管理器
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

	/** 通过规则创建模型实例
	@param rule 规则
	@param neme 名字, 必须唯一, 如果为空,则由自动给定一个名字
	*/
	ModelGcs* createModel(ModelGcs::CreateModelRuleGcs* rule, const std::string& name = "");

	/// 销毁模型实例, 会连挂接的模型实例一起销毁, 所以一次可能销毁多个.
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
