#include "EQMainPCH.h"

#include "ModelManagerGcs.h"

//-----------------------------------------------------------------------
template<> ModelManagerGcs* Ogre::Singleton<ModelManagerGcs>::ms_Singleton = 0;
//-----------------------------------------------------------------------
ModelManagerGcs::ModelManagerGcs() :
mUpdating(false),
mAutoModelNum(0)
{
}
//-----------------------------------------------------------------------
ModelManagerGcs::~ModelManagerGcs()
{
	// 删除模型
	for ( ModelMap::iterator i = mModelMap.begin();
		i != mModelMap.end(); ++i )
	{
		delete i->second;
	}
	mModelMap.clear();

}
//-----------------------------------------------------------------------
ModelGcs* ModelManagerGcs::createModel(ModelGcs::CreateModelRuleGcs* rule, const std::string& name)
{
	std::string newName;

	if ( name == "" )
	{
		newName = "AutoName-ModelGcs-" + Ogre::StringConverter::toString(mAutoModelNum++);
	}
	else
	{
		if ( hasModel(name) )
		{
			EQ_EXCEPT( "Model with name '" + name + "' already exists.",
				"ModelManagerGcs::createModel" );
		}
		newName = name;
	}

	assert( !hasModel(newName) );

	ModelGcs* model = new ModelGcs(newName);

	rule->create(model);
	

	if ( mUpdating )
		mDelayAttatchModels.insert(ModelMap::value_type(newName,model));
	else
		mModelMap.insert(ModelMap::value_type(newName,model));

	return model;
}
//-----------------------------------------------------------------------
void ModelManagerGcs::destroyModel(const std::string& name)
{
	ModelMap::iterator it = mModelMap.find(name);
	if ( it != mModelMap.end() )
	{
		delete it->second;
		it->second = NULL;

		if ( mUpdating )
			mDelayRemoveModels.insert(ModelMap::value_type(name,NULL));
		else
			mModelMap.erase(it);
	}
}
//-----------------------------------------------------------------------
void ModelManagerGcs::destroyModel(ModelGcs* model)
{
	destroyModel(model->getName());
}
//-----------------------------------------------------------------------
bool ModelManagerGcs::hasModel(const std::string& name)
{
	if ( mUpdating )
	{
		if ( mDelayRemoveModels.find(name) != mDelayRemoveModels.end() )
			return false;
		if ( mDelayAttatchModels.find(name) != mDelayAttatchModels.end() )
			return true;
		ModelMap::iterator it = mModelMap.find(name);
		return it != mModelMap.end();
	}
	else
	{
		ModelMap::iterator it = mModelMap.find(name);
		return it != mModelMap.end();
	}
}

//-----------------------------------------------------------------------
ModelGcs* ModelManagerGcs::getModel(const std::string& name)
{
	ModelMap::iterator it = mModelMap.find(name);
	if(it !=mModelMap.end())
		return it->second;
	else
		return NULL;
}
//-----------------------------------------------------------------------
void ModelManagerGcs::updateAllModels( float delta )
{
	mUpdating = true;

	//更新
	for ( ModelMap::iterator it = mModelMap.begin(); it != mModelMap.end(); ++it )
	{
		if ( it->second )
			it->second->_update(delta);
	}

	//删除在update过程中删除的东西
	for ( ModelMap::iterator it = mDelayRemoveModels.begin(); it != mDelayRemoveModels.end(); ++it )
	{
		mModelMap.erase(it->first);
	}
	mDelayRemoveModels.clear();

	//添加在update过程中产生的东西
	for ( ModelMap::iterator it = mDelayAttatchModels.begin(); it != mDelayAttatchModels.end(); ++it )
	{
		mModelMap.insert(ModelMap::value_type(it->first,it->second));
	}
	mDelayAttatchModels.clear();

	mUpdating = false;
}






