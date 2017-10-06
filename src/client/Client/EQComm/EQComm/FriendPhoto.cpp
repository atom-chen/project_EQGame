/******************************************************************************
* 作者: 何展然
* 时间: 2011-3-17
* 描述: 好友图片
******************************************************************************/
#include "EQCommPCH.h"
#include "FriendPhoto.h"
#include "DynamicCreateImageset.h"


//-----------------------------------------------------------------------
FriendPhoto::FriendPhoto()
:m_account("")
,m_curPhotoName("")
,m_needUpdate(true)
{
}
//-----------------------------------------------------------------------
FriendPhoto::~FriendPhoto()
{
	m_photoNameVec.clear();
}
//-----------------------------------------------------------------------
void FriendPhoto::setAccount(std::string account)
{
	m_account = account;
}
//-----------------------------------------------------------------------
std::string FriendPhoto::getAccount(void)
{
	return m_account;
}
//-----------------------------------------------------------------------
std::string FriendPhoto::getPhotoName(void)
{
	return m_curPhotoName;
}
//-----------------------------------------------------------------------
void FriendPhoto::savePhotoData(std::string photoData)
{
	char buf[4] = "";
	_snprintf_s(buf, sizeof(buf), "_%d", m_photoNameVec.size());
	std::string photoName = getAccount() + buf;
	switch (DynamicCreateImagesetMgr::getSingleton().saveImage(photoData, photoName))
	{
	case 0:		// 图片保存成功
		_setPhotoName(photoName);
		m_photoNameVec.push_back(photoName);
		break;
	case -1:	// 存在相同数据的图片
		_setPhotoName(DynamicCreateImagesetMgr::getSingleton().getImageName(photoData));
		break;
	case -2:	// 存在同名图片
		_setPhotoName(photoName);
		break;
	default:	// 图片保存错误
		_setPhotoName("");
	}
}
//-----------------------------------------------------------------------
void FriendPhoto::setNeedUpdate(bool update)
{
	m_needUpdate = update;
}
//-----------------------------------------------------------------------
bool FriendPhoto::isNeedUpdate(void)
{
	return m_needUpdate;
}
//-----------------------------------------------------------------------
int FriendPhoto::setWindowPhoto(std::string winname)
{
	return DynamicCreateImagesetMgr::getSingleton().setWindowImage(winname, getPhotoName());
}
//-----------------------------------------------------------------------
void FriendPhoto::_setPhotoName(std::string photoName)
{
	m_curPhotoName = photoName; 
}
