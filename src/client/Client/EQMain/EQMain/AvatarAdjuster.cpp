#include "EQMainPCH.h"

#include "Avatar.h"
#include "AvatarAdjuster.h"
#include "AvatarTable.h"
#include "GraphicsTable.h"
#include "EQLog.h"
//-----------------------------------------------------------------------
AvatarAdjuster::AvatarAdjuster( Avatar * avt )
	: mCurHairColorPos(0)			
	, mCurSkinColorPos(0)			
	, mCurHairPos(0)
	, mCurBeardPos(0)
	, mCurHeadPos(0)
	, mCurBodyPos(0)
	, mHairColor(0)
	, mSkinColor(0)
	, mAvatar(avt)
	, mSex(0)
	, mbForCreateRole(false) 
{
	assert(avt);
	reset();
}
//-----------------------------------------------------------------------
void AvatarAdjuster::changeSex()
{
	// 男1 女2
	mSex = GT_MALE + GT_FEMALE - mSex;
	_changeAvatar();
}
//-----------------------------------------------------------------------
void AvatarAdjuster::_changeAvatar()
{
	const RoleRaceTable	& tb = AvatarTableManager::getSingleton().getRoleRaceTable();
	for ( RoleRaceTable::const_iterator it = tb.begin();
		it != tb.end();
		++it )
	{
		if ( it->second.gender == mSex )
		{
			mAvatar->swap( Avatar( it->second.id ) );
			return;
		}
	}
}
//-----------------------------------------------------------------------
void AvatarAdjuster::doNext( AvatarAdjustType adjustType )
{
	switch ( adjustType )
	{
	case atSex:
		changeSex();
		break;
	case atHairType:
		nextHairType();
		break;
	case atHairColor:
		nextHairColor();
		break;
	case atFace:
		nextFace();
		break;
	case atSkin:
		nextSkin();
		break;
	case atBeard:
		nextBeard();
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------
void AvatarAdjuster::doPrev( AvatarAdjustType adjustType )
{
	switch ( adjustType )
	{
	case atSex:
		changeSex();
		break;
	case atHairType:
		prevHairType();
		break;
	case atHairColor:
		prevHairColor();
		break;
	case atFace:
		prevFace();
		break;
	case atSkin:
		prevSkin();
		break;
	case atBeard:
		prevBeard();
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------
int	 AvatarAdjuster::getCurrentVal( AvatarAdjustType adjustType )
{
	int ret = -1;
	switch ( adjustType )
	{
	case atSex:		
		ret = mSex;	
		break;
	case atHairType:
		ret = mCurHairPos;
		++ret;
		break;
	case atHairColor:
		ret = mHairColor;
		break;
	case atFace:
		ret = mCurHeadPos;
		++ret;
		break;
	case atSkin:
		ret = mSkinColor;
		break;
	case atBeard:
		ret = mCurBeardPos;
		++ret;
		break;
	default:
		break;
	}
	return ret;
}
/// 发型
//-----------------------------------------------------------------------
void AvatarAdjuster::nextHairType()
{
	++mCurHairPos;
	updateHairType();
}

//-----------------------------------------------------------------------
void AvatarAdjuster::prevHairType()
{
	--mCurHairPos;
	updateHairType();
}

/// 发色
//-----------------------------------------------------------------------
void AvatarAdjuster::nextHairColor()
{
	++mCurHairColorPos;
	updateHairColor();
}

//-----------------------------------------------------------------------
void AvatarAdjuster::prevHairColor()
{
	--mCurHairColorPos;
	updateHairColor();
}

/// 脸形
//-----------------------------------------------------------------------
void AvatarAdjuster::nextFace()
{
	++mCurHeadPos;
	updateFace();
}

//-----------------------------------------------------------------------
void AvatarAdjuster::prevFace()
{
	--mCurHeadPos;
	updateFace();
}

/// 肤色
//-----------------------------------------------------------------------
void AvatarAdjuster::nextSkin()
{
	++mCurSkinColorPos;
	updateSkin();	
}

//-----------------------------------------------------------------------
void AvatarAdjuster::prevSkin()
{
	--mCurSkinColorPos;
	updateSkin();	
}

/// 胡子
//-----------------------------------------------------------------------
void AvatarAdjuster::nextBeard()
{
	// 女性无效
	if ( GT_FEMALE == mSex )
		return;
	
	++mCurBeardPos;
	updateBeard();
}

//-----------------------------------------------------------------------
void AvatarAdjuster::prevBeard()
{
	// 女性无效
	if ( GT_FEMALE == mSex )
		return;

	--mCurBeardPos;
	updateBeard();
}

//-----------------------------------------------------------------------
void AvatarAdjuster::reset()
{
	mCurHairColorPos = 0;			
	mCurSkinColorPos = 0;
	mCurHairPos = 0;
	mCurBeardPos = 0;
	mCurHeadPos = 0;
	mCurBodyPos = 0;


	mSex = mAvatar->getRoleRaceInfo()->gender;
	mHairColor = mAvatar->getRoleRaceInfo()->hairColor;
	mSkinColor = mAvatar->getRoleRaceInfo()->skinColor;

	_changeRoleSection( RS_HAIR, mAvatar->getRoleRaceInfo()->defHair );
	_changeRoleSection( RS_HEAD, mAvatar->getRoleRaceInfo()->defHead );
	_changeRoleSection( RS_BEARD, mAvatar->getRoleRaceInfo()->defBeard );
	//_changeRoleSection( RS_BODY, mAvatar->getRoleRaceInfo()->defBody );
	_changeRoleSection( RS_RAWEQUIP1, mAvatar->getRoleRaceInfo()->defWear1 );
	_changeRoleSection( RS_RAWEQUIP2, mAvatar->getRoleRaceInfo()->defWear2 );

	// 数据检查
	assert( mSex == GT_MALE || mSex == GT_FEMALE );
	/*{
		const int roleID = mAvatar->getRoleRaceInfo()->id;
		assert( *AvatarTableManager::getSingleton().getSkinColors( roleID ).begin() == mSkinColor );		
		assert( *AvatarTableManager::getSingleton().getHairColors( roleID ).begin() == mHairColor );		
		
		RoleSectionRows rows;

		AvatarTableManager::getSingleton().getRoleSectionRows( mAvatar->getRoleRaceInfo()->id, RS_HAIR, mHairColor, rows );
		if ( rows.size() > 0 )
			assert( rows.front()->modDisID == mAvatar->getRoleRaceInfo()->defHair );

		AvatarTableManager::getSingleton().getRoleSectionRows( mAvatar->getRoleRaceInfo()->id, RS_BEARD, mHairColor, rows );
		if ( rows.size() > 0 )
			assert( rows.front()->modDisID == mAvatar->getRoleRaceInfo()->defBeard );

		AvatarTableManager::getSingleton().getRoleSectionRows( mAvatar->getRoleRaceInfo()->id, RS_BODY, mSkinColor, rows );
		if ( rows.size() > 0 )
			assert( rows.front()->modDisID == mAvatar->getRoleRaceInfo()->defBody );

		AvatarTableManager::getSingleton().getRoleSectionRows( mAvatar->getRoleRaceInfo()->id, RS_HEAD, mSkinColor, rows );
		if ( rows.size() > 0 )
			assert( rows.front()->modDisID == mAvatar->getRoleRaceInfo()->defHead );

	}*/
}
//-----------------------------------------------------------------------
void AvatarAdjuster::updateHairType()
{
	_updateRoleSection( RS_HAIR, mHairColor, mCurHairPos );
}
//-----------------------------------------------------------------------
void AvatarAdjuster::changeHairColor( int val )
{
	mHairColor = val;
	
	// 更新对应发色的 头发
	_changeRoleSection( RS_HAIR, mHairColor, mCurHairPos );

	// 更新对应发色的 胡子
	_changeRoleSection( RS_BEARD, mHairColor, mCurBeardPos );
}
//-----------------------------------------------------------------------
void AvatarAdjuster::updateHairColor()
{
	const uint roleID = mAvatar->getRoleRaceInfo()->id;
	const AvatarTableManager::TColors colorArr = AvatarTableManager::getSingleton().getHairColors( roleID );

	if ( colorArr.size() == 0 )
		return;

	// 取得当前发色
	if ( mCurHairColorPos < 0 )
		mCurHairColorPos += colorArr.size();
	mCurHairColorPos = mCurHairColorPos % colorArr.size();

	AvatarTableManager::TColors::const_iterator it = colorArr.begin();
	std::advance( it, mCurHairColorPos );
	changeHairColor(*it);
}
//-----------------------------------------------------------------------
void AvatarAdjuster::updateFace()
{
	_updateRoleSection( RS_HEAD, mSkinColor, mCurHeadPos );
}
//-----------------------------------------------------------------------
void AvatarAdjuster::changeSkin( int skinColor )
{
	mSkinColor = skinColor;	
	// 更新对应肤色的Body
	_changeRoleSection( RS_BODY, mSkinColor, mCurBodyPos );
	
	// 更新对应肤色的Head
	_changeRoleSection( RS_HEAD, mSkinColor, mCurHeadPos );
}
//-----------------------------------------------------------------------
void AvatarAdjuster::doChange( AvatarAdjustType adjustType, int val )
{
	switch ( adjustType )
	{
	case atSex:		
		if ( mSex != val )
		{
			changeSex();
		}
		break;
	case atHairType:
		--val;
		if ( mCurHairPos != val )
		{
			mCurHairPos = val;
			updateHairType();
		}
		break;
	case atHairColor:
		if ( mHairColor != val )
		{
			changeHairColor(val);
		}
		break;
	case atFace:
		--val;
		if ( mCurHeadPos != val )
		{
			mCurHeadPos = val;
			updateFace();
		}
		break;
	case atSkin:
		if ( mSkinColor != val )
		{
			changeSkin(val);			
		}
		break;
	case atBeard:
		--val;
		if ( mCurBeardPos != val )
		{
			mCurBeardPos = val;
			updateBeard();
		}
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------
void AvatarAdjuster::updateSkin()
{
	const uint roleID = mAvatar->getRoleRaceInfo()->id;
	const AvatarTableManager::TColors colorArr = AvatarTableManager::getSingleton().getSkinColors( roleID );

	if ( colorArr.size() == 0 )
		return;

	// 取得当前肤色
	if ( mCurSkinColorPos < 0 )
		mCurSkinColorPos += colorArr.size();
	mCurSkinColorPos = mCurSkinColorPos % colorArr.size();

	AvatarTableManager::TColors::const_iterator it = colorArr.begin();

	std::advance( it, mCurSkinColorPos );

	changeSkin(*it);
}
//-----------------------------------------------------------------------
void AvatarAdjuster::updateBeard()
{
	_updateRoleSection( RS_BEARD, mHairColor, mCurBeardPos );
}
//-----------------------------------------------------------------------
void AvatarAdjuster::_updateRoleSection( uint type, uint color, int & refPos  ) 
{
	// 取某种类型所有部件
	RoleSectionRows rows;
	AvatarTableManager::getSingleton().getRoleSectionRows( mAvatar->getRoleRaceInfo()->id, type, color, 
		rows, true, this->mbForCreateRole );

	int size = rows.size();

	if ( size <= 0 )
		return;

	// 根据btnUpDown的Pos值计算出对应的 ModelDisplayRow
	if ( refPos < 0 )
		refPos += size;
	refPos = refPos % size;

	// 改变角色部件
	_changeRoleSection( type, color, refPos );
}

void AvatarAdjuster::_changeRoleSection( uint sectionType, uint color, uint index )
{
	// 取某种类型所有部件
	RoleSectionRows rows;
	AvatarTableManager::getSingleton().getRoleSectionRows( mAvatar->getRoleRaceInfo()->id, sectionType, color, 
		rows, true, this->mbForCreateRole );
	if ( rows.size() < index + 1 )
		return;

	const RoleSectionRow * rSec = rows[index];

	const int modelDisplayID = rSec->modDisID;

	_changeRoleSection( sectionType, modelDisplayID );
}

void AvatarAdjuster::_changeRoleSection( uint sectionType, uint modelDisplayID )
{
	mAvatar->changeRoleSection( sectionType, modelDisplayID );

	const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( modelDisplayID );
	if ( !r )
		return;	
	// 取mesh名称
	switch ( sectionType )
	{
	case RS_HEAD:
		mHeadMesh = r->mesh;
		break;
	case RS_BEARD:
		mBeardMesh = r->mesh;
		break;
	case RS_HAIR:
		mHairMesh = r->mesh;
		break;
	case RS_BODY:
		mBodyMesh = r->mesh;
		break;
	default:
		break;
	}
}
