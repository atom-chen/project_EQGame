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
		//�������ڵ�
		pModel->_createMainNode( SystemGcs::getSingleton().getSceneManager()->getRootSceneNode() );

		//����Ĭ��ģ��
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
		EQ_EXCEPT( "�Ҳ�����ɫ����", "Avatar::Avatar( uint id ) " );

	AvatarCreateRule r( this );
	mModel = ModelManagerGcs::getSingleton().createModel( &r );
	if ( !mModel )
		EQ_EXCEPT( "������ɫģ��ʧ��", "Avatar::Avatar( uint id ) " );
}
//-----------------------------------------------------------------------
void Avatar::equipOn( uint slot, uint equipDisplayID, uint holdSlot )
{
	//��slot������װ������ж��
	if(mShareEntitys.find(slot)!=mShareEntitys.end() || mHangEntitys.find(slot)!=mHangEntitys.end())
		takeOff(slot);

	//����slotռ���Ŵ�slot��Ҳ��ж��
	ExtraHoldSlot::iterator it;
	for(it=mExtraHoldSlot.begin(); it!=mExtraHoldSlot.end(); it++)
	{
		if(it->second & slot)
		{
			takeOff(it->first);
			break;
		}
	}

	//����ж���ռ��slot
	uint extrahold = holdSlot & (~slot);
	if(extrahold != 0)
		mExtraHoldSlot[slot] = extrahold;

	const EquipDisplayRow * pDisplay = AvatarTableManager::getSingleton().getEquipDisplayRow( equipDisplayID );
	if( !pDisplay )
		return;

	// ������Ǹ����
	addShareModel( slot, pDisplay->share1 );
	addShareModel( slot, pDisplay->share2 );
	addShareModel( slot, pDisplay->share3 );

	// װ������Ʒ�ҽ�
	const EquipSlotHangRow * pSlotHangInfo = AvatarTableManager::getSingleton().getEquipSlotHangRow( pDisplay->hangID, slot );
	if ( pSlotHangInfo )
	{
		// ����ҽӶ���
		addHangModel( slot, pDisplay->hang1, pSlotHangInfo->bone1 );
		addHangModel( slot, pDisplay->hang2, pSlotHangInfo->bone2 );

		// ����ͷ��
		if ( 0 < pSlotHangInfo->hairRep )
		{
			// �����ɫ��ͷ��
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
							LOGWAR(FormatStr("�Ҳ�����Ӧ%d�滻����%d", pSlotHangInfo->rID, pSlotHangInfo->hairRep));
						// ��������б䣬��������滻����
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

							//��֯ñ֮����滻�����п���Ϊ0
							mCurrHair = repHair;
							mRepHair = repHair;
						}						
					}
				}
			}
		}
		else
		{
			//̫��ñ֮����п��ܲ���Ҫ�滻����
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
	// ɾ������ģ��
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

			// ���ж��ñ�ӣ���ԭ����
			if(isHoldSlot(slot, ES_HAT))
				replaceHair(mBaseHair);
		}
	}

	// ɾ���ҽ�ģ��
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

			// ���ж��ñ�ӣ���ԭ����
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
	// �ж���ռ�ݵ�slot��Ҳ��ж��
	ExtraHoldSlot::iterator ite = mExtraHoldSlot.find(slot);
	if(ite != mExtraHoldSlot.end())
	{
		std::vector<uint> arr = UIntToBitsetArray(ite->second);
		for(size_t i=0; i<arr.size(); i++)
			takeOff(arr[i]);

		mExtraHoldSlot.erase(ite);
	}

	// ɾ������ģ��
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

	// ɾ���ҽ�ģ��
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

	// ���ж��ñ�ӣ���ԭ����
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
		// ��ж��
		mModel->delEntity( itFind->second );
		mRoleSections.erase( itFind );
	}

	const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( modelDisplayID );
	if ( !r )
		return;	
		
	// ��װ��
	assert( mRoleSections.find( sectionType ) == mRoleSections.end() );
	mRoleSections[sectionType] = mModel->addEntity( r->mesh, r->mat, r->showMark, r->showPri, r->particle );

	// ����Ƿ��ͣ�����ԭʼ���ͺ�Ҫ�����滻����
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
					// ��������б�
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
	// ȡĳ���������в���
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
	// ��Ҫ��ʱ��Ŵ���
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
		EQ_EXCEPT( "pAvatar ����Ϊ��", "AvatarCreateRule::AvatarCreateRule( Avatar * pAvatar )" );
}
//-----------------------------------------------------------------------
void Avatar::AvatarCreateRule::create( ModelGcs * pModel )
{
	//�������ڵ�
	pModel->_createMainNode( SystemGcs::getSingleton().getSceneManager()->getRootSceneNode() );

	const RoleRaceRow * pRoleRaceInfo = mAvatar->getRoleRaceInfo();

	if ( pRoleRaceInfo )
	{	
		//��ģ�ͳ�ʼ�����ȱ����Žڵ�
		float scaleValue = pRoleRaceInfo->initScale * 0.01;
		pModel->getMainNode()->setScale(1.f,1.f,1.f);
		pModel->getMainNode()->scale(scaleValue, scaleValue, scaleValue);

		//�������ģ��
		pModel->_loadSkeletonEntity( pRoleRaceInfo->skeleton );
		
		//���붯���ļ�
		std::vector<std::string> arr;
		AvatarTableManager::getSingleton().getSkelActions( pRoleRaceInfo->skeleton, arr );

		for ( size_t i = 0; i < arr.size(); ++i )
		{
			if ( !ExistResource( arr[i] ) )
			{
				std::string str = "�Ҳ��������ļ�" + arr[i];
				LOGERR( str );
				continue;
			}
			pModel->_addSkelAnimation( arr[i] );
		}

		// ȡ�ý�ɫ����, ��װ��ȥ(ע��ͬ�����ͣ�ֻ��һ��)		
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

			// ���ж��ñ�ӣ���ԭ����
			if(isHoldSlot(slot[i], ES_HAT))
				replaceHair(mBaseHair);
		}
		
		hite = mHangEntitys.find(slot[i]);
		if(hite != mHangEntitys.end())
		{
			if(mHangEntitys[slot[i]].size() != 0)
				mModel->setEntityVisible(mHangEntitys[slot[i]][0], false);

			// ���ж��ñ�ӣ���ԭ����
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

			// ���ж��ñ�ӣ���ԭ����
			if(isHoldSlot(slot[i], ES_HAT))
				replaceHair(mRepHair);
		}

		hite = mHangEntitys.find(slot[i]);
		if(hite != mHangEntitys.end())
		{
			if(mHangEntitys[slot[i]].size() != 0)
				mModel->setEntityVisible(mHangEntitys[slot[i]][0], true);

			// ���ж��ñ�ӣ���ԭ����
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

