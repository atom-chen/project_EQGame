/******************************************************************************
* ����: ��չȻ
* ʱ��: 2011-3-17
* ����: ����ͼƬ
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

	// 0����ȷ����-1��������winname�����ڣ���-2����������
	int setWindowPhoto(std::string winname);

protected:
	void _setPhotoName(std::string photoName);

protected:
	std::string m_account;	// �˺�
	std::string m_curPhotoName;	// ��ǰͼƬ��
	std::vector<std::string> m_photoNameVec;	// ͼƬ������
	bool m_needUpdate;	// ��Ҫ���±�־
};


#endif // end of guard FriendPhoto_H
