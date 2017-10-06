#include "EQMainPCH.h"

#include "Avatar.h"
#include "AvatarAdjuster.h"
#include "EQMain.h"
#include "SystemGcs.h"
#include "GraphicsTable.h"
#include "AvatarTable.h"
#include "ItemTable.h"
#include "Item.h"




//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
class CreateHangModelRule : public ModelGcs::CreateModelRuleGcs
{
public:
	explicit CreateHangModelRule( const ModelDisplayRow * pDisplay ) 
		: mDisplay( pDisplay )
	{
	}
public:
	virtual void create( ModelGcs* pModel )
	{
		//创建主节点
		pModel->_createMainNode( SystemGcs::getSingleton().getSceneManager()->getRootSceneNode() );

		//创建默认模型
		pModel->addEntity( mDisplay->mesh, mDisplay->mat, mDisplay->showMark, mDisplay->showPri, mDisplay->particle );
	}
protected:
	const ModelDisplayRow * mDisplay;
};
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Avatar::~Avatar()
{
	if ( mAvatarAdjuster )
		delete mAvatarAdjuster;

	if ( mModel )
		ModelManagerGcs::getSingleton().destroyModel(mModel);
}
//-----------------------------------------------------------------------
Avatar::Avatar( uint id ) 
	: mRoleRaceInfo( NULL )
	, mModel( NULL )
	, mHairRepType( 0 )
	, mAvatarAdjuster(NULL)
	, mSlotVisible(ES_ALL)
	, mBaseHair(0)
	, mCurrHair(0)
{
	mRoleRaceInfo = AvatarTableManager::getSingleton().getRoleRaceRow( id );
	if ( !mRoleRaceInfo )
		EQ_EXCEPT( "找不到角色种类", "Avatar::Avatar( uint id ) " );

	AvatarCreateRule r( this );
	mModel = ModelManagerGcs::getSingleton().createModel( &r );
	if ( !mModel )
		EQ_EXCEPT( "创建角色模型失败", "Avatar::Avatar( uint id ) " );
}
//-----------------------------------------------------------------------
void Avatar::equipOn( uint slot, uint equipDisplayID, uint holdSlot )
{
	//此slot上已有装备，先卸掉
	if(mShareEntitys.find(slot)!=mShareEntitys.end() || mHangEntitys.find(slot)!=mHangEntitys.end())
		takeOff(slot);

	//其他slot占据着此slot，也得卸掉
	ExtraHoldSlot::iterator it;
	for(it=mExtraHoldSlot.begin(); it!=mExtraHoldSlot.end(); it++)
	{
		if(it->second & slot)
		{
			takeOff(it->first);
			break;
		}
	}

	//如果有额外占据slot
	uint extrahold = holdSlot & (~slot);
	if(extrahold != 0)
		mExtraHoldSlot[slot] = extrahold;

	const EquipDisplayRow * pDisplay = AvatarTableManager::getSingleton().getEquipDisplayRow( equipDisplayID );
	if( !pDisplay )
		return;

	// 处理共享骨格对象
	addShareModel( slot, pDisplay->share1 );
	addShareModel( slot, pDisplay->share2 );
	addShareModel( slot, pDisplay->share3 );

	// 装备栏物品挂接
	const EquipSlotHangRow * pSlotHangInfo = AvatarTableManager::getSingleton().getEquipSlotHangRow( pDisplay->hangID, slot );
	if ( pSlotHangInfo )
	{
		// 处理挂接对象
		addHangModel( slot, pDisplay->hang1, pSlotHangInfo->bone1 );
		addHangModel( slot, pDisplay->hang2, pSlotHangInfo->bone2 );

		// 处理头发
		if ( 0 < pSlotHangInfo->hairRep )
		{
			// 如果角色有头发
			if ( mBaseHair )
			{
				const HairReplaceRow * hairRow = AvatarTableManager::getSingleton().getHairReplaceRow( mBaseHair );
				if ( hairRow )
				{
					if ( pSlotHangInfo->hairRep <= GetArraySize<uint>( hairRow->type ) )
					{
						mHairRepType = pSlotHangInfo->hairRep;
						uint repHair = hairRow->type[pSlotHangInfo->hairRep - 1];
						if ( repHair == 0 )
							LOGWAR(FormatStr("找不到对应%d替换发型%d", pSlotHangInfo->rID, pSlotHangInfo->hairRep));
						// 如果发型有变，依据情况替换发型
						if ( repHair != mCurrHair )
						{
							if ( mRoleSections.find( RS_HAIR ) != mRoleSections.end() )
								getModel()->delEntity( mRoleSections[RS_HAIR] );

							if ( repHair )
							{
								const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( repHair );
								if ( !r )
									return;

								mRoleSections[RS_HAIR] = getModel()->addEntity( r->mesh, r->mat, r->showMark, r->showPri, r->particle );
							}						

							//针织帽之类的替换发型有可能为0
							mCurrHair = repHair;
							mRepHair = repHair;
						}						
					}
				}
			}
		}
		else
		{
			//太阳帽之类的有可能不需要替换发型
 			if ( ES_HAT == slot )
 			{
 				mCurrHair = mBaseHair;
 				mRepHair = mBaseHair;
 			}
		}
	}	
}
//-----------------------------------------------------------------------
void Avatar::takeOffAll()
{
	uint slot;
	// 删除共享模型
	{
		ShareEntitys::iterator it = mShareEntitys.begin();
		ShareEntitys::iterator ittemp;
		for ( ;it != mShareEntitys.end(); )
		{
			ittemp = it;
			slot = (*it).first;

			for ( std::vector<std::string>::iterator itStr = it->second.begin();
				itStr != it->second.end();
				++itStr )
			{
				mModel->delEntity( *itStr );
			}

			it++;
			mShareEntitys.erase( ittemp );

			// 如果卸下帽子，则还原发型
			if(isHoldSlot(slot, ES_HAT))
				replaceHair(mBaseHair);
		}
	}

	// 删除挂接模型
	{
		HangEntitys::iterator it = mHangEntitys.begin();
		HangEntitys::iterator ittemp;
		for ( ;it != mHangEntitys.end(); )
		{
			ittemp = it;
			slot = (*it).first;

			for ( std::vector<std::string>::iterator itStr = it->second.begin();
				itStr != it->second.end();
				++itStr )
			{
				mModel->destroyAndDetachFromLocator( *itStr );
			}

			it++;
			mHangEntitys.erase( ittemp );

			// 如果卸下帽子，则还原发型
			if(isHoldSlot(slot, ES_HAT))
				replaceHair(mBaseHair);
		}
	}

	mExtraHoldSlot.clear();

	return;
}
//-----------------------------------------------------------------------
void Avatar::takeOff( uint slot )
{
	// 有额外占据的slot，也得卸掉
	ExtraHoldSlot::iterator ite = mExtraHoldSlot.find(slot);
	if(ite != mExtraHoldSlot.end())
	{
		std::vector<uint> arr = UIntToBitsetArray(ite->second);
		for(size_t i=0; i<arr.size(); i++)
			takeOff(arr[i]);

		mExtraHoldSlot.erase(ite);
	}

	// 删除共享模型
	{
		ShareEntitys::iterator it = mShareEntitys.find( slot );
		if ( it != mShareEntitys.end() )
		{
			for ( std::vector<std::string>::iterator itStr = it->second.begin();
				itStr != it->second.end();
				++itStr )
			{
				mModel->delEntity( *itStr );
			}
			mShareEntitys.erase( it );
		}
	}

	// 删除挂接模型
	{
		HangEntitys::iterator it = mHangEntitys.find( slot );
		if ( it != mHangEntitys.end() )
		{
			for ( std::vector<std::string>::iterator itStr = it->second.begin();
				itStr != it->second.end();
				++itStr )
			{
				mModel->destroyAndDetachFromLocator( *itStr );
			}
			mHangEntitys.erase( it );
		}
	}

	// 如果卸下帽子，则还原发型
	if ( ES_HAT == slot )
		replaceHair(mBaseHair);
}
//-----------------------------------------------------------------------
const RoleRaceRow * Avatar::getRoleRaceInfo() const
{
	return mRoleRaceInfo;
}
//-----------------------------------------------------------------------
void Avatar::swap( Avatar & r )
{
	std::swap( mModel, r.mModel );
	std::swap( mRoleRaceInfo, r.mRoleRaceInfo );

	std::swap( mShareEntitys, r.mShareEntitys );
	std::swap( mHangEntitys, r.mHangEntitys );

	std::swap( mBaseHair, r.mBaseHair );
	std::swap( mCurrHair, r.mCurrHair );
	std::swap( mHairRepType, r.mHairRepType );

	std::swap( mRoleSections, r.mRoleSections );

	std::swap( mAvatarAdjuster, r.mAvatarAdjuster );	
}
//-----------------------------------------------------------------------
ModelGcs * Avatar::getModel()
{
	return mModel;
}
//-----------------------------------------------------------------------
void Avatar::addHangModel( uint slot, uint modelDisplayID, const std::string & boneName )
{
	if ( boneName.empty() )
		return;

	const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( modelDisplayID );
	if ( !r )
		return;
	
	CreateHangModelRule rule( r );
	ModelGcs * mo = ModelManagerGcs::getSingleton().createModel( &rule );
	
	mModel->attachToLocator( boneName, mo );

	mHangEntitys[slot].push_back( mo->getName() );

	if(mHangEntitys[slot].size() != 0)
		mModel->setEntityVisible(mHangEntitys[slot][0], (mSlotVisible&slot)!=0);
}
//-----------------------------------------------------------------------
void Avatar::addShareModel( uint slot, uint modelDisplayID )
{
	const ModelDisplayRow * row = GraphicsTableManager::getSingleton().getModelDisplayRow( modelDisplayID );
	if ( !row )
		return;

	mShareEntitys[slot].push_back( mModel->addEntity( row->mesh, row->mat, row->showMark, row->showPri, row->particle ) );

	for(int i=0; i<(int)mShareEntitys[slot].size(); i++)
		mModel->setEntityVisible(mShareEntitys[slot][i], (mSlotVisible&slot)!=0);
}
//-----------------------------------------------------------------------
void Avatar::changeRoleSection( uint sectionType, uint modelDisplayID )
{
	std::map<uint, std::string>::iterator itFind = mRoleSections.find( sectionType );
	if ( itFind != mRoleSections.end() )
	{
		// 先卸下
		mModel->delEntity( itFind->second );
		mRoleSections.erase( itFind );
	}

	const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( modelDisplayID );
	if ( !r )
		return;	
		
	// 再装上
	assert( mRoleSections.find( sectionType ) == mRoleSections.end() );
	mRoleSections[sectionType] = mModel->addEntity( r->mesh, r->mat, r->showMark, r->showPri, r->particle );

	// 如果是发型，更新原始发型后，要重新替换发型
	if ( sectionType == RS_HAIR )
	{
		mBaseHair = modelDisplayID;
		mCurrHair = modelDisplayID;

		if ( mHairRepType != 0 )
		{
			const HairReplaceRow * hairRow = AvatarTableManager::getSingleton().getHairReplaceRow( mBaseHair );
			if ( hairRow )
			{
				if ( mHairRepType <= GetArraySize<uint>( hairRow->type ) )
				{
					uint repHair = hairRow->type[mHairRepType - 1];
					// 如果发型有变
					if ( repHair != mCurrHair )
					{
						if ( mRoleSections.find( RS_HAIR ) != mRoleSections.end() )
							getModel()->delEntity( mRoleSections[RS_HAIR] );

						if ( repHair )
						{
							const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( repHair );
							if ( !r )
								return;

							mRoleSections[RS_HAIR] = getModel()->addEntity( r->mesh, r->mat, r->showMark, r->showPri, r->particle );							
							mCurrHair = repHair;
							mRepHair = repHair;
						}
					}						
				}
			}			
		}
	}	
}
//-----------------------------------------------------------------------
void Avatar::changeRoleSection( uint sectionType, uint color, uint index )
{
	// 取某种类型所有部件
	RoleSectionRows rows;
	AvatarTableManager::getSingleton().getRoleSectionRows( getRoleRaceInfo()->id, sectionType, color, rows );
	if ( rows.size() < index + 1 )
		return;

	const RoleSectionRow * rSec = rows[index];

	const int modelDisplayID = rSec->modDisID;
	changeRoleSection( sectionType, modelDisplayID );
}
//-----------------------------------------------------------------------
AvatarAdjuster * Avatar::getAdjuster()
{
	// 需要的时候才创建
	if ( !mAvatarAdjuster )
	{
		mAvatarAdjuster = new AvatarAdjuster(this);
	}
	return mAvatarAdjuster;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Avatar::AvatarCreateRule::AvatarCreateRule( Avatar * pAvatar )
	: mAvatar( pAvatar )
{
	if ( !mAvatar )
		EQ_EXCEPT( "pAvatar 不能为空", "AvatarCreateRule::AvatarCreateRule( Avatar * pAvatar )" );
}
//-----------------------------------------------------------------------
void Avatar::AvatarCreateRule::create( ModelGcs * pModel )
{
	//创建主节点
	pModel->_createMainNode( SystemGcs::getSingleton().getSceneManager()->getRootSceneNode() );

	const RoleRaceRow * pRoleRaceInfo = mAvatar->getRoleRaceInfo();

	if ( pRoleRaceInfo )
	{	
		//按模型初始比例等比缩放节点
		float scaleValue = pRoleRaceInfo->initScale * 0.01;
		pModel->getMainNode()->setScale(1.f,1.f,1.f);
		pModel->getMainNode()->scale(scaleValue, scaleValue, scaleValue);

		//读入骨骼模型
		pModel->_loadSkeletonEntity( pRoleRaceInfo->skeleton );
		
		//读入动作文件
		std::vector<std::string> arr;
		AvatarTableManager::getSingleton().getSkelActions( pRoleRaceInfo->skeleton, arr );

		for ( size_t i = 0; i < arr.size(); ++i )
		{
			if ( !ExistResource( arr[i] ) )
			{
				std::string str = "找不到动作文件" + arr[i];
				LOGERR( str );
				continue;
			}
			pModel->_addSkelAnimation( arr[i] );
		}

		// 取得角色部件, 并装上去(注：同种类型，只穿一件)		
		RoleSectionRows rows;
		AvatarTableManager::getSingleton().getRoleSectionRows( mAvatar->getRoleRaceInfo()->id, rows );
		for ( RoleSectionRows::const_iterator it = rows.begin(); it != rows.end(); ++it )
		{
			const RoleSectionRow & r = **it;
			
			const ModelDisplayRow * m = GraphicsTableManager::getSingleton().getModelDisplayRow( r.modDisID );
			if ( m )
			{
				if ( mAvatar->mRoleSections.find( r.type ) == mAvatar->mRoleSections.end() )
				{					
					mAvatar->mRoleSections[r.type] = pModel->addEntity( m->mesh, m->mat, m->showMark, m->showPri, m->particle );				
					if ( RS_HAIR == r.type )
					{
						mAvatar->mBaseHair = r.modDisID;
						mAvatar->setCurrHair( r.modDisID );
					}
				}
			}			
		}		
	}	
}
//-----------------------------------------------------------------------
void Avatar::hideSlot( uint slotArr )
{
	std::vector<uint> slot = UIntToBitsetArray(slotArr);

	ShareEntitys::iterator site;
	HangEntitys::iterator hite;
	for(int i=0; i<(int)slot.size(); i++)
	{
		site = mShareEntitys.find(slot[i]);
		if(site != mShareEntitys.end())
		{
			for(int j=0; j<(int)mShareEntitys[slot[i]].size(); j++)
				mModel->setEntityVisible(mShareEntitys[slot[i]][j], false);

			// 如果卸下帽子，则还原发型
			if(isHoldSlot(slot[i], ES_HAT))
				replaceHair(mBaseHair);
		}
		
		hite = mHangEntitys.find(slot[i]);
		if(hite != mHangEntitys.end())
		{
			if(mHangEntitys[slot[i]].size() != 0)
				mModel->setEntityVisible(mHangEntitys[slot[i]][0], false);

			// 如果卸下帽子，则还原发型
			if(isHoldSlot(slot[i], ES_HAT))
				replaceHair(mBaseHair);
		}
	}

	mSlotVisible = ~slotArr;
}
//-----------------------------------------------------------------------
void Avatar::restoreSlot()
{
	uint temp = ~mSlotVisible;
	std::vector<uint> slot = UIntToBitsetArray(temp);

	ShareEntitys::iterator site;
	HangEntitys::iterator hite;
	for(int i=0; i<(int)slot.size(); i++) 
	{
		site = mShareEntitys.find(slot[i]);
		if(site != mShareEntitys.end())
		{
			for(int j=0; j<(int)mShareEntitys[slot[i]].size(); j++)
				mModel->setEntityVisible(mShareEntitys[slot[i]][j], true);

			// 如果卸下帽子，则还原发型
			if(isHoldSlot(slot[i], ES_HAT))
				replaceHair(mRepHair);
		}

		hite = mHangEntitys.find(slot[i]);
		if(hite != mHangEntitys.end())
		{
			if(mHangEntitys[slot[i]].size() != 0)
				mModel->setEntityVisible(mHangEntitys[slot[i]][0], true);

			// 如果卸下帽子，则还原发型
			if(isHoldSlot(slot[i], ES_HAT))
				replaceHair(mRepHair);
		}
	}

	mSlotVisible = ES_ALL;
}
//-----------------------------------------------------------------------
void Avatar::replaceHair(uint repHair)
{
	std::string hair;
	RoleSectionMap::iterator it = mRoleSections.find( RS_HAIR );
	if ( it != mRoleSections.end() )
		hair = it->second;

	if ( !hair.empty() )
		getModel()->delEntity( hair );

	if ( repHair )
	{
		const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( repHair );
		if ( !r )
			return;

		mRoleSections[RS_HAIR] = getModel()->addEntity( r->mesh, r->mat, r->showMark, r->showPri, r->particle );
	}

	mCurrHair = repHair;
	mHairRepType = 0;
}
//-----------------------------------------------------------------------
bool Avatar::isHoldSlot(uint slot, EquipSlotType slotType)
{
	if(slotType == slot)
		return true;

	ExtraHoldSlot::iterator ite = mExtraHoldSlot.find(slot);
	if(ite != mExtraHoldSlot.end())
	{
		std::vector<uint> arr = UIntToBitsetArray(ite->second);
		for(size_t i=0; i<arr.size(); i++)
		{
			if(arr[i] == slotType)
				return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------

