#include "EQMainPCH.h"

#include "ModelGcs.h"
#include "SystemGcs.h"
#include "CommFuncGcs.h"
#include "CommFunc.h"
#include "ModelManagerGcs.h"
#include "EQLog.h"
#include "ZoneLightListener.h"
#include "EQCartoon.h"
#include "GraphicsTable.h"
#include "VideoCardCapabilities.h"
#include "ParticleMgr.h"

//-----------------------------------------------------------------------
#define MODEL_MORPH_ANIMATION_NAME			"model_morph"
#define MODEL_MORPH_ANIMATION_KEYFRAME_TIME	0.0333333
#define MODEL_DEFAULT_SKELETON_ANIMATION_NAME "auto"

ulong ModelGcs::mSkelEntNum = 0;
ulong ModelGcs::mCreateEntityAttNum = 0;
//-----------------------------------------------------------------------
ModelGcs::EntityAtt::EntityAtt() :
entity(NULL),
visible(true),
showMark(0),
showPri(0),
particleHandle(0),
markVisible(true),
isSelfIllum(false),
isStrokes(false),
isHighLight(false),
isTextureAnimation(false),
isLightMat(false)
{
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
ModelGcs::ModelGcs( const std::string& name ) :
mName(name),
mMainNode(NULL),
mExternalBoundingBox(Ogre::AxisAlignedBox::BOX_NULL),
mPickObjectProxyPt(NULL),
mSkeletonEntity(NULL),
mShareEntity(NULL),
mCurrentAnimationState(NULL),
mFadeOutAnimationState(NULL),
mFadeOutDelay(0),
mAnimationRate(1.0f),
mSelfIllumEnable(false),
mSelfIllumColor(Ogre::ColourValue::Black),
mAttachToModelGcs(NULL),
mAnimPause(false),
mVisible(true),
mNeedSysnVisible(false),
mStrokes(false),
mHighLight(false),
mTextureAnimationEnable(false),
mLightMat(false),
mParticleHandle(0)
{
}
//-----------------------------------------------------------------------
ModelGcs::~ModelGcs()
{
	std::map<uint, Particle*>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); it++)
	{
		ParticleMgr::getSingleton().removeParticle(it->second);
	}

	//mLocatorAttMap
	{
		LocatorAttMap::iterator it = mLocatorAttMap.begin();
		for ( ; it != mLocatorAttMap.end(); ++it )
		{
			SlotMap::iterator its;
			for ( its = it->second.slotMap.begin(); its != it->second.slotMap.end(); ++its )
			{
				//分离模型
				ModelGcs* mod = its->second;
				mod->setParentNode(NULL);
				mod->getMainNode()->setOrientation(Ogre::Quaternion::IDENTITY);
				mod->getMainNode()->setPosition(Ogre::Vector3::ZERO);
				mod->_notifyAttach(NULL);

				//删除模型
				ModelManagerGcs::getSingleton().destroyModel(mod);
			}
			it->second.slotMap.clear();

			//删除挂接点
			it->second.locatorNode->getParentSceneNode()->
				removeAndDestroyChild(it->second.locatorNode->getName());
		}
		mLocatorAttMap.clear();
	}

	//mEntityAttMap
	//mShareEntity
	{
		mShareEntity = NULL;

		EntityAttMap::iterator it;
		for ( it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
			_destroyEntityAtt(it->second);
		mEntityAttMap.clear();
	}

	//mSkeletonEntity
	{
		if ( mSkeletonEntity )
		{
			Ogre::SceneNode* parentNode = mSkeletonEntity->getParentSceneNode();
			assert(parentNode);
			parentNode->detachObject(mSkeletonEntity);
			SystemGcs::getSingleton().getSceneManager()->destroyEntity(mSkeletonEntity);
		}

	}

	//mMainNode
	{
		if ( mMainNode )
		{
			//if (mMainNode->getParentSceneNode())
			//{
			//	mMainNode->getParentSceneNode()->removeAndDestroyChild(mMainNode->getName());
			//}
			//else
			//{
				mMainNode->removeAndDestroyAllChildren();
				mMainNode->getCreator()->destroySceneNode(mMainNode);
			//}
			mMainNode = NULL;
		}
	}
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
std::string ModelGcs::addEntity( const std::string& meshName, const std::string& matName, uint iShowMark, uchar iShowPri, uint particle )
{
	std::string name = mMainNode->getName() + "-EntityAtt-" + meshName +
		Ogre::StringConverter::toString(mCreateEntityAttNum++);

	//创建实例
	Ogre::Entity* ent =
		SystemGcs::getSingleton().getSceneManager()->createEntity(name, meshName);

	//
	ent->setUserObject( mPickObjectProxyPt );

	//加入主节点
	mMainNode->attachObject(ent);

	//共享骨骼
	if ( !ent->getMesh()->getSkeleton().isNull() )
	{
		if ( mShareEntity )
		{
			std::string skelName = ent->getMesh()->getSkeleton()->getName();
			//_loadSkeletonEntity(skelName);
			std::string skelOrg = mShareEntity->getMesh()->getSkeleton()->getName();

			//如果骨骼不一致就不共享,这里做容错处理
			if ( skelOrg != skelName )
			{
				LOGWAR( "Can't use different Skeleton. Mesh:'" + meshName + "' Skel:'" + skelName
					+ "' Skel ModelGcs:'" + skelOrg + "'" );
			}
			else
				ent->shareSkeletonInstanceWith(mShareEntity);
		}
		else
			mShareEntity = ent;

		if(ent->hasSkeleton())
		{
			for(uint i=0; i<ent->getMesh()->getNumSubMeshes(); i++)
			{
				uint count = ent->getMesh()->getSubMesh(i)->blendIndexToBoneIndexMap.size();
				//LOG( FormatStr("%s bone num is %d", meshName.c_str(), count) );
				//骨骼动画shader骨头最大数为50，增加的话需要改skinning.cg
				if(count > 50)
					EQ_EXCEPT( FormatStr("%s bone num %d is greater than hardware skinning bone limit", meshName.c_str(), count), "addEntity" );
			}
		}
	}

	//有默认骨骼动画，就播
	if(hasAnimation(MODEL_DEFAULT_SKELETON_ANIMATION_NAME))
	{
		setDefAnimation(MODEL_DEFAULT_SKELETON_ANIMATION_NAME);
		playAnimation(MODEL_DEFAULT_SKELETON_ANIMATION_NAME, true);
	}

	//同步可见状态
	ent->setVisible(mVisible);

	//显示屏蔽
	bool visible = true;
	if ( 0 != iShowMark )
	{	
		mNeedSysnVisible = true;

		//同等mark进行优先级比较，优先级低的隐藏
		for ( EntityAttMap::iterator it = mEntityAttMap.begin();
			it != mEntityAttMap.end();
			++it )
		{
			if ( (it->second.showMark & iShowMark) != 0 )
			{
				if ( iShowPri >= it->second.showPri )
				{
					//it->second.entity->setVisible( false );
					it->second.markVisible = false;
				}
				else
				{
					ent->setVisible( false );
					visible = false;
				}
			}
		}
	}


	//
	EntityAtt ea;
	ea.name = name;
	ea.entity = ent;
	ea.showMark = iShowMark;
	ea.showPri = iShowPri;
	ea.markVisible = visible;

	if(particle)
		ea.particleHandle = addParticle(particle);

	assert( mEntityAttMap.find(name) == mEntityAttMap.end() );
	mEntityAttMap.insert(EntityAttMap::value_type(name, ea));
	
	//改变材质
	changeEntityMat(name, matName);

	//设置光照监听
	if(mLightZone.size() > 0)
	{
		ZoneLightListener* zll = new ZoneLightListener(mLightZone);
		ent->setListener(zll);
	}

	//法线贴图的模型要生成切线
	Ogre::Technique* tec = ent->getSubEntity(0)->getMaterial()->getBestTechnique();
	if(tec && tec->getName()=="normalmap")
	{
		VideoCardCapabilities* cap = SystemGcs::getSingleton().getVideoCardCapabilities();
		if(cap && cap->isSupportSM2())
		{
			unsigned short src, dest;
			if (!ent->getMesh()->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
			{
				ent->getMesh()->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
			}

		}
	}

	return name;
}
//-----------------------------------------------------------------------
void ModelGcs::delEntity( const std::string& name )
{
	EntityAttMap::iterator it = mEntityAttMap.find( name );
	if ( it == mEntityAttMap.end() )
		return;

	const uint iMark = it->second.showMark;

	//如果共享模型与要删除模型一致, 则变更共享模型
	if ( mShareEntity && mShareEntity == it->second.entity )
	{
		const Ogre::Entity::EntitySet* entset = mShareEntity->getSkeletonInstanceSharingSet();
		Ogre::Entity::EntitySet::const_iterator it;
		bool found = false;
		for ( it = entset->begin(); it != entset->end(); ++it )
		{
			if ( *it != mShareEntity )
			{
				found = true;
				mShareEntity = *it;
				break;
			}
		}

		if ( !found )
		{
			std::string skelName = mShareEntity->getMesh()->getSkeleton()->getName();
			Ogre::Entity* ent = mShareEntity;
			mShareEntity = NULL;

			//得保持住共享骨骼, 否则骨骼动画和挂接点会出错.
			assert(mSkeletonEntity==NULL);
			_loadSkeletonEntity(skelName);
			mShareEntity->shareSkeletonInstanceWith(ent);
		}
	}

	//删除节点
	_destroyEntityAtt(it->second);
	mEntityAttMap.erase(it);	

	if ( 0 != iMark )
	{		
		// 收集所有相关节点
		std::vector<EntityAttMap::iterator> arrTemp;

		for ( it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
		{
			if ( (it->second.showMark & iMark) != 0 )
			{
				arrTemp.push_back( it );
			}
		}

		// 对arrTemp进行过滤, 取同等mark优先级最大的节点, 结果放入arr
		std::vector<EntityAttMap::iterator> arr;
		uint x = 1;
		for ( int n = 1; n <= 32; ++n, x = (x << 1) )
		{
			if ( iMark & x )
			{
				// 取得优先级最大的节点
				EntityAttMap::iterator itMax = mEntityAttMap.end();
				for ( std::vector<EntityAttMap::iterator>::iterator pIter = arrTemp.begin();
					pIter != arrTemp.end();
					++pIter )
				{
					const EntityAtt & ref = (*pIter)->second;
					if ( ref.showMark & x )
					{
						if ( itMax == mEntityAttMap.end() )
						{
							itMax = *pIter;
						}
						else
						{
							if ( itMax->second.showPri < ref.showPri )
								itMax = *pIter;
						}
					}
				}

				// 加入最大节点
				if ( itMax != mEntityAttMap.end() )
				{
					if ( std::find( arr.begin(), arr.end(), itMax ) == arr.end() )
						arr.push_back( itMax );
				}
			}			
		}		


		// 重新设置这些节点的可见性
		mNeedSysnVisible = true;
		for ( std::vector<EntityAttMap::iterator>::iterator pIter = arr.begin();
			pIter != arr.end();
			++pIter )
		{
			EntityAtt & ref = (*pIter)->second;	
			// 先设为可见, entity设为预设值
			ref.markVisible = true;
			//ref.entity->setVisible( this->mVisible );

			// 同等mark进行优先级比较，优先级低的隐藏
			for ( EntityAttMap::iterator it = mEntityAttMap.begin();
				it != mEntityAttMap.end();
				++it )
			{
				if ( (it->second.showMark & ref.showMark) != 0 )
				{
					if ( *pIter == it ) // 跳过自身节点
						continue;

					if ( ref.showPri >= it->second.showPri )
					{
						//it->second.entity->setVisible( false );
						it->second.markVisible = false;
					}
					else
					{
						//ref.entity->setVisible( false );
						ref.markVisible = false;
					}
				}
			}			
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::setEntityVisible( const std::string& name, bool visible )
{
	mNeedSysnVisible = true;

	EntityAttMap::iterator it = mEntityAttMap.find( name );
	if ( it != mEntityAttMap.end() )
	{
		//it->second.entity->setVisible(visible);
		it->second.visible = visible;

		const uint iShowMark = it->second.showMark;
		const uchar iShowPri = it->second.showPri;
		if ( 0 != iShowMark )
		{	
			for (it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it)
			{
				if(it->first == name)
					continue;

				if ( (it->second.showMark & iShowMark) != 0 )
				{
					//把优先级低(内衣裤)的可见设成相反的
					if ( iShowPri > it->second.showPri )
					{
						//it->second.entity->setVisible( !visible );
						it->second.markVisible = !visible;
					}
				}
			}
		}
	}

	LocatorAttMap::iterator itl;
	for(itl=mLocatorAttMap.begin(); itl!=mLocatorAttMap.end(); itl++)
	{
		if(itl->second.slotMap.find(name) != itl->second.slotMap.end())
		{
			//挂接点上的所有物件共享可见信息
			itl->second.visible = visible;
			break;
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::changeEntityMat( const std::string& entName, const std::string& matName )
{
	EntityAttMap::iterator it = mEntityAttMap.find(entName);
	if ( it == mEntityAttMap.end() )
		return;
	EntityAtt& att = it->second;

	//变更材质前, 应该先删除材质效果
	_resetEntityMat(att);

	Ogre::Entity* ent = att.entity;

	//如果材质不存在, 那么会自动调用白色材质, 这样便于调试
	std::vector<std::string> matList = Ogre::StringUtil::split(matName);
	if ( matList.size() == 1 && !matList[0].empty() )
	{
		ent->setMaterialName(matList[0]);
	}
	else if ( matList.size() > 1 )
	{
		if ( ent->getNumSubEntities() < matList.size() )
			matList.resize(ent->getNumSubEntities());

		for ( int i = 0; i < (int)matList.size(); ++i )
		{
			ent->getSubEntity(i)->setMaterialName(matList[i]);
		}
	}	
	else
	{
		//不变更材质
	}

	//在材质变更效果存在时, 需要做额外的修改
	_sysnEntityMatEffect(att, _hasMatEffect());
}
//-----------------------------------------------------------------------
void ModelGcs::changeEntityMat( const std::string& matName )
{
	EntityAttMap::iterator it;
	for(it=mEntityAttMap.begin(); it!=mEntityAttMap.end(); it++)
	{
		changeEntityMat(it->first, matName);
	}
}
//-----------------------------------------------------------------------
std::vector<std::string> ModelGcs::getEntityMat(const std::string& entName)
{
	std::vector<std::string> matList;
	EntityAttMap::iterator it = mEntityAttMap.find(entName);
	if (mEntityAttMap.end() == it)
	{
		return matList;
	}
	Ogre::Entity* ent = it->second.entity;
	for (uint i=0; i<ent->getNumSubEntities(); ++i)
	{
		matList.push_back(it->second.orgMat[i]);
	}
	return matList;
}
//-----------------------------------------------------------------------
std::vector<std::string> ModelGcs::getEntityMat(void)
{
	std::vector<std::string> matList;
	if (mEntityAttMap.empty())
	{
		return matList;
	}
	matList = getEntityMat(mEntityAttMap.begin()->first);
	return matList;
}
//-----------------------------------------------------------------------
DWORD ModelGcs::getAnimationLength( const std::string& name ) 
{
	if (name.empty())
		return 0;

	if ( !hasAnimation( name ) )
	{
		LOGWAR( "Model '" + mName + "' has not Animation '" + name +  "'!" );
		return 0;
	}

	Ogre::AnimationState* as = mShareEntity->getAnimationState(name);
	DWORD ret = static_cast<DWORD>(as->getLength() * 1000);
	return ret;
}
//-----------------------------------------------------------------------
void ModelGcs::playAnimation( const std::string& name, bool loop )
{
	if (mFadeOutAnimationState)
	{
		mFadeOutAnimationState->setWeight(1);
		mFadeOutAnimationState->setEnabled(false);
	}

	mFadeOutAnimationState = mCurrentAnimationState;
	if ( mFadeOutAnimationState )
		mFadeOutAnimationState->setLoop(false);		//需要过度的动画设置为非循环

	mFadeOutDelay = mCurrentAnimationState==NULL ? 0 :
						GraphicsTableManager::getSingleton().getAnimationBlend(mCurrentAnimationState->getAnimationName()+name);

	mLastDefAnimationName = "";

	Ogre::AnimationState* as = NULL;

	if ( !name.empty() )
	{
		if ( hasAnimation( name ) )
			as = mShareEntity->getAnimationState(name);
		else
			LOGWAR( "Model '" + mName + "' has not Animation '" + name +  "'!" );
	}

	mCurrentAnimationState = as;
	if ( mCurrentAnimationState )
	{
		if (mFadeOutAnimationState == mCurrentAnimationState)
			mFadeOutAnimationState = NULL;

		mCurrentAnimationState->setEnabled(true);
		mCurrentAnimationState->setTimePosition(0.0f);
		mCurrentAnimationState->setWeight(mFadeOutAnimationState ? 0 : 1);
		mCurrentAnimationState->setLoop(loop);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::clearAnimationLoop()
{
	if ( mLastDefAnimationName.empty() )
	{
		if ( mCurrentAnimationState && mCurrentAnimationState->getLoop() )
			mCurrentAnimationState->setLoop(false);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::setDefAnimation( const std::string& name )
{
	if ( name.empty() )
	{
		mDefaultAnimationName = "";
		return;
	}

	if ( hasAnimation( name ) )
	{
		mDefaultAnimationName = name;
	}
	else
		LOGWAR( "Model '" + mName + "' has not Animation '" + name +  "'!" );
}
//-----------------------------------------------------------------------
void ModelGcs::setModelMorph( int keyframe, const std::string& entName )
{
	std::vector<Ogre::Entity*> modList;

	if ( entName.empty() )
	{
		EntityAttMap::iterator it;
		modList.reserve(mEntityAttMap.size());

		if ( mShareEntity )
			modList.push_back(mShareEntity);

		for ( it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
		{
			//如果不是共享骨骼, 且不为mSHareEntity
			if ( !it->second.entity->sharesSkeletonInstance() && it->second.entity != mShareEntity )
				modList.push_back(it->second.entity);
		}
	}
	else
	{
		EntityAttMap::iterator it = mEntityAttMap.find(entName);
		if ( it != mEntityAttMap.end() )
			modList.push_back(it->second.entity);
	}


	//这个动画将由系统自动更新
	for ( std::vector<Ogre::Entity*>::iterator it = modList.begin(); it != modList.end(); ++it )
	{
		Ogre::Entity* ent = *it;

		Ogre::AnimationState* as = NULL;

		if ( !(ent->getAllAnimationStates() && ent->getAllAnimationStates()->hasAnimationState(MODEL_MORPH_ANIMATION_NAME)) )
			continue;

		as = ent->getAnimationState(MODEL_MORPH_ANIMATION_NAME);

		float timePos = keyframe * MODEL_MORPH_ANIMATION_KEYFRAME_TIME;

		if ( timePos == 0.0f )
		{
			as->setEnabled(false);
		}
		else
		{
			as->setEnabled(true);
			as->setWeight(1);
			as->setLoop(false);
			as->setTimePosition(timePos);
		}
	}
}
//-----------------------------------------------------------------------
int ModelGcs::hasModelMorph( const std::string& entName )
{
	std::vector<Ogre::Entity*> modList;

	if ( entName.empty() )
	{
		EntityAttMap::iterator it;
		modList.reserve(mEntityAttMap.size());

		if ( mShareEntity )
			modList.push_back(mShareEntity);

		for ( it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
		{
			//如果不是共享骨骼, 且不为mSHareEntity
			if ( !it->second.entity->sharesSkeletonInstance() && it->second.entity != mShareEntity )
				modList.push_back(it->second.entity);
		}
	}
	else
	{
		EntityAttMap::iterator it = mEntityAttMap.find(entName);
		if ( it != mEntityAttMap.end() )
			modList.push_back(it->second.entity);
	}


	//
	int maxLen = 0;
	for ( std::vector<Ogre::Entity*>::iterator it = modList.begin(); it != modList.end(); ++it )
	{
		Ogre::Entity* ent = *it;

		if ( ent->getAllAnimationStates() && ent->getAllAnimationStates()->hasAnimationState(MODEL_MORPH_ANIMATION_NAME) )
		{
			Ogre::AnimationState* as = ent->getAnimationState(MODEL_MORPH_ANIMATION_NAME);
			int le = (int)((as->getLength() + 0.0000001) / MODEL_MORPH_ANIMATION_KEYFRAME_TIME) + 1;
			if ( le > maxLen )
				maxLen = le;
		}
	}

	return maxLen;
}
//-----------------------------------------------------------------------
void ModelGcs::enableAnimation( bool enable )
{
	//还有bug, 只能处理当前的动作, 没有对子节点处理
	if (mCurrentAnimationState && mCurrentAnimationState->getEnabled() != enable)
		mCurrentAnimationState->setEnabled(enable);

	if (mFadeOutAnimationState && mFadeOutAnimationState->getEnabled() != enable)
		mFadeOutAnimationState->setEnabled(enable);
}
//-----------------------------------------------------------------------
void ModelGcs::pauseAnimation( bool enable, bool forceChild )
{
	mAnimPause = enable;

	if ( forceChild )
	{
		LocatorAttMap::iterator it;
		for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
		{
			LocatorAtt& la = it->second;
			for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
				its->second->pauseAnimation(enable,true);
		}
	}
}
//-----------------------------------------------------------------------
bool ModelGcs::hasAnimation( const std::string& name )
{
	if ( mShareEntity && mShareEntity->getAllAnimationStates()
		&& mShareEntity->getAllAnimationStates()->hasAnimationState(name) )
		return true;
	return false;
}
//-----------------------------------------------------------------------
bool ModelGcs::hasAnimation()
{
	if ( mShareEntity && mShareEntity->getAllAnimationStates() )
		return mShareEntity->getAllAnimationStates()->getAnimationStateIterator().hasMoreElements();
	return false;
}
//-----------------------------------------------------------------------
std::string ModelGcs::getCurrAnimationName()
{
	if ( mCurrentAnimationState )
		return mCurrentAnimationState->getAnimationName();
	else
		return "";
}
//-----------------------------------------------------------------------
void ModelGcs::setSelfIllum( const Ogre::ColourValue& color, bool forceChild )
{
	mSelfIllumColor = color;
	mSelfIllumEnable = true;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second);
	}


	//
	if ( forceChild )
	{
		LocatorAttMap::iterator it;
		for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
		{
			LocatorAtt& la = it->second;
			for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
				its->second->setSelfIllum(color,true);
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::clearSelfIllum( bool forceChild )
{
	mSelfIllumColor = Ogre::ColourValue::Black;
	mSelfIllumEnable = false;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second, true);
	}


	//
	if ( forceChild )
	{
		LocatorAttMap::iterator it;
		for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
		{
			LocatorAtt& la = it->second;
			for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
				its->second->clearSelfIllum(true);
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::addStrokes(bool forceChild)
{
	mStrokes = true;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second);
	}

	if ( forceChild )
	{
		LocatorAttMap::iterator it;
		for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
		{
			LocatorAtt& la = it->second;
			for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
				its->second->addStrokes(true);
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::delStrokes(bool forceChild)
{
	mStrokes = false;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second, true);
	}

	if ( forceChild )
	{
		LocatorAttMap::iterator it;
		for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
		{
			LocatorAtt& la = it->second;
			for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
				its->second->delStrokes(true);
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::setHighLight(const Ogre::ColourValue& color, bool forceChild)
{
	mHighLightColor = color;
	mHighLight = true;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second);
	}

	if ( forceChild )
	{
		LocatorAttMap::iterator it;
		for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
		{
			LocatorAtt& la = it->second;
			for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
				its->second->setHighLight(color, true);
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::clearHighLight(bool forceChild)
{
	mHighLight = false;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second, true);
	}

	if ( forceChild )
	{
		LocatorAttMap::iterator it;
		for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
		{
			LocatorAtt& la = it->second;
			for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
				its->second->clearHighLight(true);
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::setTextureAnimation(const char* info)
{
	mTextureAnimation = info;
	mTextureAnimationEnable = true;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::clearTextureAnimation()
{
	mTextureAnimationEnable = false;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second, true);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::setLightMat()
{
	if(mLightMat)
		return;

	mLightMat = true;
	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::clearLightMat()
{
	if(!mLightMat)
		return;

	mLightMat = false;
	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		_sysnEntityMatEffect(it->second, true);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::attachToLocator(const std::string& locName, ModelGcs* pModel,
							   const Ogre::Quaternion &offsetOrientation, const Ogre::Vector3 &offsetPosition)
{	
	LocatorAtt& la = _createOrRetrieveLocatorAtt(locName, offsetOrientation, offsetPosition);

	assert( la.slotMap.find(pModel->getName()) == la.slotMap.end() );

	la.slotMap[pModel->getName()] = pModel;

	pModel->setParentNode(la.locatorNode);
	pModel->getMainNode()->setOrientation(offsetOrientation);
	pModel->getMainNode()->setPosition(offsetPosition);
	pModel->_notifyAttach(this);

	//立刻刷新下
	_updateLocatorNode(0);
	_updateLocator(0);

	//设置光照监听
	if(mLightZone.size() > 0)
		pModel->setLightListener(mLightZone);
}
//-----------------------------------------------------------------------
ModelGcs* ModelGcs::detachFromLocator( const std::string modelName )
{
	LocatorAttMap::iterator it = mLocatorAttMap.begin();
	for ( ; it != mLocatorAttMap.end(); ++it )
	{
		SlotMap::iterator its = it->second.slotMap.find(modelName);
		if ( its != it->second.slotMap.end() )
		{			
			ModelGcs* mod = its->second;
			mod->setParentNode(NULL);
			mod->getMainNode()->setOrientation(Ogre::Quaternion::IDENTITY);
			mod->getMainNode()->setPosition(Ogre::Vector3::ZERO);
			mod->_notifyAttach(NULL);

			//删除slot
			it->second.slotMap.erase(its);

			//如果slot也空了, 删除这个挂接点
			if ( it->second.slotMap.empty() )
			{
				it->second.locatorNode->getParentSceneNode()->
					removeAndDestroyChild(it->second.locatorNode->getName());
				mLocatorAttMap.erase(it);
			}

			return mod;
		}
	}

	EQ_EXCEPT( "Can't find Model '" + modelName + "' in locator map!",
		"ModelGcs::detachFromLocator" );
	return NULL;
}
//-----------------------------------------------------------------------
void ModelGcs::destroyAndDetachFromLocator( const std::string modelName )
{
	ModelManagerGcs::getSingleton().destroyModel(detachFromLocator(modelName));
}
//-----------------------------------------------------------------------
void ModelGcs::setVisible(bool visible)
{
	if ( mVisible == visible )
		return;

	mVisible = visible;
	mNeedSysnVisible = true;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void ModelGcs::setParentNode( Ogre::SceneNode* parentNode, bool forceUnit )
{
	if(mMainNode && mMainNode->getParentSceneNode() == parentNode )
		return;

	if(mMainNode && mMainNode->getParentSceneNode())
		mMainNode->getParentSceneNode()->removeChild(mMainNode);

	if ( forceUnit )
	{
		mMainNode->setPosition(Ogre::Vector3::ZERO);
		mMainNode->setOrientation(Ogre::Quaternion::IDENTITY);
		mMainNode->setScale(Ogre::Vector3::UNIT_SCALE);
	}

	if(parentNode)
	{
		parentNode->addChild(mMainNode);
	}

}
//-----------------------------------------------------------------------
Ogre::AxisAlignedBox ModelGcs::getBoundingBox()
{
	if ( !mExternalBoundingBox.isNull() )
		return mExternalBoundingBox;
	else
		return Ogre::AxisAlignedBox::BOX_NULL;		//返回一个空的, 日后可能会修改
}
//-----------------------------------------------------------------------
void ModelGcs::setPickObjectProxy(PickObjectProxy* proxy)
{
	mPickObjectProxyPt = proxy;

	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		EntityAtt& ea = it->second;
		ea.entity->setUserObject(mPickObjectProxyPt);
	}

	// 目前还不支持挂接点的选取同步
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void ModelGcs::_createMainNode(Ogre::SceneNode* parentNode)
{
	assert( mMainNode == NULL );
	mMainNode = parentNode->createChildSceneNode();
}
//-----------------------------------------------------------------------
void ModelGcs::_setBoundingBoxFromExternal(const Ogre::AxisAlignedBox& box)
{
	mExternalBoundingBox = box;
}
//-----------------------------------------------------------------------
void ModelGcs::_loadSkeletonEntity( const std::string& skelName )
{
	if ( mSkeletonEntity )
	{
		return;
	}

	Ogre::SkeletonPtr pSkel = Ogre::SkeletonManager::getSingleton().load( skelName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

	Ogre::MeshPtr pMesh = loadNullMeshForSkeleton(pSkel, mExternalBoundingBox);

	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();

	// 命名规则为骨骼名加流水号
	mSkeletonEntity = mgr->createEntity(
		pMesh->getName() + "-" + Ogre::StringConverter::toString(mSkelEntNum++),
		pMesh->getName());

	//骨骼模型实例无需Query则设置为0
	mSkeletonEntity->setQueryFlags(0);

	assert( mMainNode );
	mMainNode->attachObject(mSkeletonEntity);

	//
	assert(mShareEntity==NULL);
	mShareEntity = mSkeletonEntity;
}
//-----------------------------------------------------------------------
void ModelGcs::_addSkelAnimation( std::string skelName )
{
	mSkeletonEntity->getSkeleton()->addLinkedSkeletonAnimationSource( skelName );
	mSkeletonEntity->refreshAvailableAnimationState();
}
//-----------------------------------------------------------------------
void ModelGcs::_update( float delta, bool forceAttachModel )
{
	if ( !forceAttachModel && isAttatch() )
		return;

	//同步可见性
	_syncVisible();

	//同步定位器可见性
	_syncLocatorVisible();

	//不可见 return
	if ( !mVisible )
		return;

	//更新动画
	_updateAnimation(delta);

	//更新定位器节点
	_updateLocatorNode(delta);

	//更新定位器
	_updateLocator(delta);

	//更新粒子
	_updateParticle();
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Ogre::AnimationStateSet* ModelGcs::_getAnimationStateSet()
{
	if ( mShareEntity )
		return mShareEntity->getAllAnimationStates();
	else
		return NULL;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void ModelGcs::_resetEntityMat( EntityAtt& ea )
{
	if ( ea.orgMat.empty() )
	{
		assert( ea.isSelfIllum == false );
		assert( ea.isStrokes == false );
		assert( ea.isHighLight == false );
		return;
	}

	uint c = ea.entity->getNumSubEntities();
	assert(ea.orgMat.size() == c);

	for ( uint i = 0; i < c; ++i )
	{
		Ogre::SubEntity* sub = ea.entity->getSubEntity(i);
		Ogre::MaterialManager::getSingleton().remove(sub->getMaterial()->getHandle());
		sub->setMaterialName(ea.orgMat[i]);
	}

	//
	ea.orgMat.clear();

	//
	ea.isSelfIllum = false;
	ea.isStrokes = false;
	ea.isHighLight = false;
	ea.isTextureAnimation = false;
	ea.isLightMat = false;
	//可能还有透明
}
//-----------------------------------------------------------------------
void ModelGcs::_cloneEntityMat( EntityAtt& ea, bool forceRecreate )
{
	if ( forceRecreate )
		_resetEntityMat(ea);

	if ( ea.orgMat.empty() )
	{
		uint c = ea.entity->getNumSubEntities();
		ea.orgMat.resize(c);
		for ( uint i = 0; i < c; ++i )
		{
			Ogre::SubEntity* sub = ea.entity->getSubEntity(i);
			const Ogre::MaterialPtr& mat = sub->getMaterial();

			ea.orgMat[i] = mat->getName();

			// Avoid name collision
			std::string newName;
			do {
				newName = mat->getName() + "-auto-" + Ogre::StringConverter::toString(Ogre::Math::UnitRandom());
			} while (Ogre::MaterialManager::getSingleton().resourceExists(newName));
			Ogre::MaterialPtr cloneMat = mat->clone(newName);

			sub->setMaterial(cloneMat);
		}
	}

	assert(ea.orgMat.size() == ea.entity->getNumSubEntities());
}
//-----------------------------------------------------------------------
bool ModelGcs::_hasMatEffect()
{
	return mSelfIllumEnable || mStrokes || mHighLight;
}
//-----------------------------------------------------------------------
void ModelGcs::_sysnEntityMatEffect(EntityAtt& ea, bool forceRecreate)
{
	// forceRecreate:
	// Add的时候无需重新创建，Del的时候强制重新创建，Change的时候只有在有特效时才需重新创建

	// 备份原始材质
	_cloneEntityMat(ea, forceRecreate);

	//这里可能有些需要保证先后顺序，自己把握~~~
	_sysnLightMat(ea);
	_sysnSelfIllum(ea);
	_sysnStrokes(ea);
	_sysnHighLight(ea);
	_sysnTextureAnimation(ea);
	//以后还有透明
}
//-----------------------------------------------------------------------
void ModelGcs::_sysnSelfIllum( EntityAtt& ea )
{
	if ( mSelfIllumEnable )
	{
		if(!ea.isSelfIllum)
		{
			// 还未设置自发光
			uint c = ea.entity->getNumSubEntities();
			for ( uint i = 0; i < c; ++i )
			{
				Ogre::MaterialPtr mat = ea.entity->getSubEntity(i)->getMaterial();

				Ogre::Technique* tec = mat->getBestTechnique();
				Ogre::Pass* pass = tec->getPass("BassPass");
				if(pass)
					pass->setSelfIllumination(pass->getSelfIllumination() + mSelfIllumColor);
			}
			ea.isSelfIllum = true;
		}
	}
   	else
   	{
   		ea.isSelfIllum = false;
   	}
}
//-----------------------------------------------------------------------
void ModelGcs::_sysnStrokes( EntityAtt& ea )
{
	if(mStrokes)
	{
		if(!ea.isStrokes)
		{
			// 还未加上描边
			uint c = ea.entity->getNumSubEntities();
			for ( uint i = 0; i < c; ++i )
			{
				Ogre::MaterialPtr mat = ea.entity->getSubEntity(i)->getMaterial();
				sEQCartoon.addCartoonPass(mat);
			}
			ea.isStrokes = true;
		}
	}
	else
	{
		ea.isStrokes = false;
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_sysnHighLight( EntityAtt& ea )
{
	if(mHighLight)
	{
		if(!ea.isHighLight)
		{
			uint c = ea.entity->getNumSubEntities();
			for ( uint i = 0; i < c; ++i )
			{
				Ogre::MaterialPtr mat = ea.entity->getSubEntity(i)->getMaterial();

				Ogre::Technique* tec = mat->getBestTechnique();
				Ogre::Pass* pass = tec->getPass("BassPass");
				if(pass)
 					pass->getTextureUnitState(0)->setColourOperationEx(Ogre::LBX_ADD, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, mHighLightColor);
			}
			ea.isHighLight = true;
		}
	}
	else
	{
		ea.isHighLight = false;
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_sysnTextureAnimation( EntityAtt& ea )
{
	if(mTextureAnimationEnable)
	{
		if(!ea.isTextureAnimation)
		{
			//暂定只换第一个子模型
			Ogre::MaterialPtr mat = ea.entity->getSubEntity(0)->getMaterial();
			Ogre::Technique* tec = mat->getBestTechnique();
			Ogre::Pass* pass = mat->getBestTechnique()->getPass("BassPass");
			if(pass)
			{
				std::vector<std::string> arr = Ogre::StringUtil::split( mTextureAnimation, " \r\n\t()," );
				if(arr.size() != 3)
					return;

				unsigned int numFrames = Ogre::StringConverter::parseUnsignedInt(arr[1]);
				Real duration = Ogre::StringConverter::parseReal(arr[2]);

				pass->getTextureUnitState(0)->setAnimatedTextureName(arr[0], numFrames, duration);
			}

			ea.isTextureAnimation = true;
		}
	}
	else
	{
		ea.isTextureAnimation = false;
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_sysnLightMat( EntityAtt& ea )
{
	if(mLightMat)
	{
		if(!ea.isLightMat)
		{
			//暂定只换第一个子模型
			std::string newName = ea.orgMat[0] + "_on";
			if(Ogre::MaterialManager::getSingleton().resourceExists(newName))
			{
				Ogre::MaterialPtr newMat = Ogre::MaterialManager::getSingleton().getByName(newName);
				//不使用之前克隆的材质
				Ogre::MaterialManager::getSingleton().remove(ea.entity->getSubEntity(0)->getMaterial()->getHandle());
				//新建个自己的克隆材质
				do {
					newName = newName + "-auto-" + Ogre::StringConverter::toString(Ogre::Math::UnitRandom());
				} while (Ogre::MaterialManager::getSingleton().resourceExists(newName));
				Ogre::MaterialPtr cloneMat = newMat->clone(newName);

				ea.entity->getSubEntity(0)->setMaterial(cloneMat);
			}

			ea.isLightMat = true;
		}
	}
	else
	{
		ea.isLightMat = false;
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_destroyEntityAtt( EntityAtt& ea )
{
	//复位材质
	_resetEntityMat(ea);

	delParticleByHandle(ea.particleHandle);

	//删除Entity
	ea.entity->getParentSceneNode()->detachObject(ea.entity);
	SystemGcs::getSingleton().getSceneManager()->destroyEntity(ea.entity);
	ea.entity = NULL;
}
//-----------------------------------------------------------------------
void ModelGcs::_updateAnimation( float delta )
{
	if ( mAnimPause )
		return;

	//计算默认动画
	_calcDefAnimation();


	// 速率不为0, 才有意义
	if ( mAnimationRate != 0 )
	{
		// 更新当前融合的骨骼动画
		if (mFadeOutAnimationState && mCurrentAnimationState)
		{
			assert( mFadeOutDelay >= 0.0f );
			assert( mAnimationRate >= 0.0f );

			//实际剩下的时间
			float realFadeLeft = mFadeOutDelay / mAnimationRate;

			if (realFadeLeft <= delta || realFadeLeft == 0.0f )
			{
				mFadeOutAnimationState->setWeight(0);
				mFadeOutAnimationState->setEnabled(false);
				mCurrentAnimationState->setWeight(1);
				mFadeOutAnimationState = NULL;
			}
			else
			{
				float weight = mCurrentAnimationState->getWeight();
				weight = weight + delta * (1 - weight) / realFadeLeft;

				mCurrentAnimationState->setWeight(weight);
				mFadeOutAnimationState->setWeight(1 - weight);

				mFadeOutAnimationState->addTime(delta * mAnimationRate);
			}

			mFadeOutDelay -= delta * mAnimationRate;
		}

		// 更新当前的骨骼动画
		if ( mCurrentAnimationState )
			mCurrentAnimationState->addTime(delta * mAnimationRate);
	}

}
//-----------------------------------------------------------------------
void ModelGcs::_calcDefAnimation()
{
	// 如果有默认动画,  
	bool flag = false;
	if ( !mDefaultAnimationName.empty() )
	{
		if ( mCurrentAnimationState )
		{
			//上个动画不是默认动画
			if ( mLastDefAnimationName.empty() )
			{
				//当前动作不是循环动画, 且播放接近或者完成,
 				if ( !mCurrentAnimationState->getLoop() )
 				{
					float blend = mFadeOutAnimationState==NULL ? 0 :
						GraphicsTableManager::getSingleton().getAnimationBlend(mCurrentAnimationState->getAnimationName()+mFadeOutAnimationState->getAnimationName());
 					if ( mCurrentAnimationState->getTimePosition() + blend >= mCurrentAnimationState->getLength() )
 						flag = true;
 				}
			}
			else	//上个动画是默认动画
			{
				//且和当前动画不一致, 
				if ( mLastDefAnimationName != mDefaultAnimationName )
					flag = true;
			}
		}
		else 	//没当前动画
			flag = true;
	}

	if ( flag )
	{
		playAnimation( mDefaultAnimationName, true);

		assert(mCurrentAnimationState);
		assert(mCurrentAnimationState->getAnimationName()==mDefaultAnimationName);
		mLastDefAnimationName = mDefaultAnimationName;
	}

}
//-----------------------------------------------------------------------
void ModelGcs::_clearShareAnimation()
{
	mCurrentAnimationState = NULL;
	mFadeOutAnimationState = NULL;
	mDefaultAnimationName = "";
	mLastDefAnimationName = "";
}
//-----------------------------------------------------------------------
ModelGcs::LocatorAtt& ModelGcs::_createOrRetrieveLocatorAtt( const std::string& locName, const Ogre::Quaternion &offsetOrientation,
															const Ogre::Vector3 &offsetPosition)
{
	LocatorAttMap::iterator it = mLocatorAttMap.find(locName);

	if ( it == mLocatorAttMap.end() )
	{
		//如果名字为空, 创建一个
		if ( locName.empty() )
		{
			return _createLocatorAtt("", offsetOrientation, offsetPosition);
		}

		Ogre::SkeletonInstance* skel = NULL;

		if ( mShareEntity )
			skel = mShareEntity->getSkeleton();

		if ( !skel )
		{
			//容错处理, 如果没有骨骼, 就添加到根节点
			LOGWAR( "Model '" + mName + "' has not Skeleton! Locator attach to Model main node!" );
			return _createLocatorAtt("", offsetOrientation, offsetPosition);
		}

		if ( !skel->hasBone(locName) )
		{
			//容错处理, 如果没有骨头, 就添加到根节点
			LOGWAR( "Model '" + mName + "' has not bone '" + locName + "'! Locator attach to Model main node!" );
			return _createLocatorAtt("", offsetOrientation, offsetPosition);
		}

		return _createLocatorAtt( locName, offsetOrientation, offsetPosition );
	}
	else
		return it->second;
}
//-----------------------------------------------------------------------
ModelGcs::LocatorAtt& ModelGcs::_createLocatorAtt( const std::string& locName, const Ogre::Quaternion &offsetOrientation,
												  const Ogre::Vector3 &offsetPosition)
{
	LocatorAtt& la = mLocatorAttMap[locName];

	assert( la.locatorNode == NULL );
	la.boneName = locName;
	la.locatorNode = mMainNode->createChildSceneNode();

	return la;
}
//-----------------------------------------------------------------------
void ModelGcs::_updateLocatorNode( float delta )
{

	// 手动更新动画
	// 当前有骨骼动画
	if ( mShareEntity && (mFadeOutAnimationState || mCurrentAnimationState) )
	{
		// 有连接到骨骼的定位器
		if ( (mLocatorAttMap.size() == 1 && mLocatorAttMap.find("") == mLocatorAttMap.end())
			|| mLocatorAttMap.size() > 1 )
			mShareEntity->_updateAnimation();
	}

	//!!!!!!挂接节点用这种方式实现好像不是很理想,有时间改掉这种方式!!!!!!!
	LocatorAttMap::iterator it;

	for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
	{
		LocatorAtt& la = it->second;
		if ( la.boneName.empty() )
			continue;

		Ogre::Bone* bone = mShareEntity->getSkeleton()->getBone(la.boneName);
		

		if ( la.locatorNode->getPosition() != bone->_getDerivedPosition() )
			la.locatorNode->setPosition(bone->_getDerivedPosition());
		if ( la.locatorNode->getOrientation() != bone->_getDerivedOrientation() )
			la.locatorNode->setOrientation(bone->_getDerivedOrientation());
		if ( la.locatorNode->getScale() != bone->_getDerivedScale() )
			la.locatorNode->setScale(bone->_getDerivedScale());
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_updateLocator( float delta )
{
	LocatorAttMap::iterator it;
	for ( it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
	{
		LocatorAtt& la = it->second;
		for ( SlotMap::iterator its = la.slotMap.begin(); its != la.slotMap.end(); ++its )
			its->second->_update(delta, true);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_updateParticle()
{
	std::map<uint, Particle*>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); )
	{
		Particle* p = it->second;
		if(ParticleMgr::getSingleton().isAlive(p))
		{
			if(!p->getBone().empty())
			{
				TransformInfo trans;
				if(p->getBone() == "Root")
				{
					switch(p->getTrans())
					{
					case TRANS_NONE:
						break;
					case TRANS_POSITION:
						trans.mPosition = mMainNode->_getDerivedPosition();
						break;
					case TRANS_ORIENTATION:
						trans.mRotation = mMainNode->_getDerivedOrientation();
						break;
					case TRANS_POSORI:
						trans.mRotation = mMainNode->_getDerivedOrientation();
						trans.mPosition = mMainNode->_getDerivedPosition();
						break;
					}
				}
				else
				{
					if (!mShareEntity->hasSkeleton())
						EQ_EXCEPT( "Entity doesn't have skeleton", "ModelGcs::_updateParticle" );

					Ogre::Bone* bone = mShareEntity->getSkeleton()->getBone(p->getBone());
					switch(p->getTrans())
					{
					case TRANS_NONE:
						break;
					case TRANS_POSITION:
						trans.mPosition = mMainNode->_getFullTransform() * bone->_getDerivedPosition();
						break;
					case TRANS_ORIENTATION:
						trans.mRotation = mMainNode->_getDerivedOrientation() * bone->_getDerivedOrientation();
						break;
					case TRANS_POSORI:
						trans.mRotation = mMainNode->_getDerivedOrientation() * bone->_getDerivedOrientation();
						trans.mPosition = mMainNode->_getFullTransform() * bone->_getDerivedPosition();
						break;
					}
				}
				p->setTransform(trans);
			}

			p->setVisible(mVisible);
		}
		else
		{
			ParticleMgr::getSingleton().removeParticle(p);
			it = mParticleMap.erase(it);
			continue;
		}

		it++;
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_notifyAttach(ModelGcs* parentModel)
{
	if ( !parentModel )			//取消挂接
	{
		mAttachToModelGcs = NULL;
		//可能还需求一些清除工作
	}
	else						//挂接
	{
		if ( mAttachToModelGcs )
			EQ_EXCEPT( "Model '" + mName + "' is attached!", "ModelGcs::_notifyAttach" );

		//
		mAttachToModelGcs = parentModel;


		//同步动画状态
		pauseAnimation(parentModel->isPauseAnimation(),true);
		//还有,允许动画部分没处理

		//同步可见性
		setVisible(parentModel->getVisible());
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_syncLocatorVisible()
{
	for ( LocatorAttMap::iterator it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
	{
		for ( SlotMap::iterator itt = it->second.slotMap.begin(); itt != it->second.slotMap.end(); ++itt )
		{
			itt->second->_syncVisible();
		}
	}
}
//-----------------------------------------------------------------------
void ModelGcs::_syncVisible()
{
	if ( !mNeedSysnVisible )
		return;
	mNeedSysnVisible = false;

	bool view;

	//同步模型组
	for ( EntityAttMap::iterator it = mEntityAttMap.begin(); it != mEntityAttMap.end(); ++it )
	{
		view = false;
		if ( mVisible && it->second.visible && it->second.markVisible )
			view = true;

		it->second.entity->setVisible(view);
	}

	//同步定位器的子模型
	for ( LocatorAttMap::iterator it = mLocatorAttMap.begin(); it != mLocatorAttMap.end(); ++it )
	{
		view = false;
		if ( mVisible && it->second.visible )
			view = true;

		for ( SlotMap::iterator itt = it->second.slotMap.begin(); itt != it->second.slotMap.end(); ++itt )
		{
			itt->second->setVisible(view);
		}
	}

	//同步隐藏粒子(2011/7/7 add by hezhr)
	for (std::map<uint, Particle*>::iterator it=mParticleMap.begin(); mParticleMap.end()!=it; ++it)
	{
		it->second->setVisible(mVisible);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::setLightListener(std::vector<uint>& zone)
{
	if(mLightZone == zone)
		return;

	mLightZone = zone;

	//设置共享模型光照
	EntityAttMap::iterator ite;
	for(ite=mEntityAttMap.begin(); ite!=mEntityAttMap.end(); ite++)
	{
		ZoneLightListener* zll;
		if(!ite->second.entity->getListener())
		{
			zll = new ZoneLightListener(zone);
			ite->second.entity->setListener(zll);
		}
		else
		{
			zll = dynamic_cast<ZoneLightListener*>(ite->second.entity->getListener());
		}

		assert(zll);
		zll->setZoneList(zone);
	}

	//设置挂接模型光照
	LocatorAttMap::iterator it;
	for (it=mLocatorAttMap.begin(); it!=mLocatorAttMap.end(); ++it)
	{
		SlotMap::iterator its;
		for (its=it->second.slotMap.begin(); its!=it->second.slotMap.end(); ++its)
		{
			its->second->setLightListener(zone);
		}
	}
}
//-----------------------------------------------------------------------
uint ModelGcs::addParticle(uint id)
{
	mParticleHandle++;

	Particle* p = ParticleMgr::getSingleton().createParticle(id, FormatStr("%s_Particle_%d", mName.c_str(),mParticleHandle).c_str());
	p->setVisible(mVisible);

	TransformInfo trans;
	switch(p->getTrans())
	{
	case TRANS_NONE:
		break;
	case TRANS_POSITION:
		trans.mPosition = mMainNode->_getDerivedPosition();
		break;
	case TRANS_ORIENTATION:
		trans.mRotation = mMainNode->_getDerivedOrientation();
		break;
	case TRANS_POSORI:
		trans.mRotation = mMainNode->_getDerivedOrientation();
		trans.mPosition = mMainNode->_getDerivedPosition();
		break;
	}
	p->setTransform(trans);

	if(p->isStartImmediately())
		p->start();

	mParticleMap.insert(std::make_pair(mParticleHandle, p));

	return mParticleHandle;
}
//-----------------------------------------------------------------------
void ModelGcs::delParticleByHandle(uint handle)
{
	std::map<uint, Particle*>::iterator it = mParticleMap.find(handle);
	if(it != mParticleMap.end())
	{
		ParticleMgr::getSingleton().removeParticle(it->second);
		mParticleMap.erase(it);
	}
}
//-----------------------------------------------------------------------
void ModelGcs::delParticleByID(uint id)
{
	std::map<uint, Particle*>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); )
	{
		if(it->second->getID() == id)
		{
			ParticleMgr::getSingleton().removeParticle(it->second);
			it = mParticleMap.erase(it);
			continue;
		}

		it++;
	}
}
//-----------------------------------------------------------------------
void ModelGcs::startParticle()
{
	std::map<uint, Particle*>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); it++)
		it->second->start();
}
//-----------------------------------------------------------------------
void ModelGcs::pauseParticle()
{
	std::map<uint, Particle*>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); it++)
		it->second->pause();
}
//-----------------------------------------------------------------------
void ModelGcs::stopParticle()
{
	std::map<uint, Particle*>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); it++)
		it->second->stop();
}
//-----------------------------------------------------------------------