/******************************************************************************
* 作者: 何展然
* 时间: 2011-3-17
* 描述: 好友图片
******************************************************************************/
#ifndef FriendPhoto_H
#define FriendPhoto_H

#include "EQMainPrerequisites.h"


class FriendPhoto
{
public:
	FriendPhoto();

	~FriendPhoto();

	void setAccount(std::string account);

	std::string getAccount(void);

	std::string getPhotoName(void);

	void savePhotoData(std::string photoData);

	void setNeedUpdate(bool update=true);

	bool isNeedUpdate(void);

	// 0（正确），-1（不存在winname名窗口），-2（其他错误）
	int setWindowPhoto(std::string winname);

protected:
	void _setPhotoName(std::string photoName);

protected:
	std::string m_account;	// 账号
	std::string m_curPhotoName;	// 当前图片名
	std::vector<std::string> m_photoNameVec;	// 图片名数组
	bool m_needUpdate;	// 需要更新标志
};


#endif // end of guard FriendPhoto_H
