/******************************************************************************
* ����: ��չȻ
* ʱ��: 2011-4-18
* ����: ΢��
******************************************************************************/
#ifndef MicroBlog_H
#define MicroBlog_H

#include "EQMainPrerequisites.h"
#include "MsgHandleRegist.h"
#include "NetPacket.h"
#include "QQWeibo.h"


class MicroBlogManager : public Ogre::Singleton<MicroBlogManager>, public MemberMsgHandleRegistHelper
{
public:
	// ΢������
	enum MicroBlogType
	{
		MBT_QQ = 1,		// QQ΢��
		MBT_SINA = 2,	// ����΢��
	};

	// ������������
	enum SendOperatorType
	{
		SOT_NONE,		// �ղ���
		SOT_UPLOAD,		// ֻ������ͼƬ��΢������
		SOT_UPDATEHEAD,	// ֻ����΢��ͷ��
		SOT_ALL			// ͬʱ�������ֲ���
	};

public:
	MicroBlogManager(void);
	~MicroBlogManager(void);

	/*	��ȡȫ��
	*	dirPath����ͼ�����Ŀ¼·��
	*/
	std::string fullScreenShot(std::string dirPath);
	/*	��ȡָ������
	*	x��y��w��h����Ҫ��ȡ��λ������Ϳ�ߣ�dirPath����ͼ�����Ŀ¼·��
	*/
	std::string partScreenShot(float x, float y, float w, float h, std::string dirPath);

	void handle_weibo_access_secret(MsgPack &recBuff);

	void SendTo(MicroBlogType mbType, SendOperatorType soType, std::string text, std::string image);	// �ϴ�

	void saveAccessSecret(MicroBlogType mbType, std::string verifier);	// ������֤��

	void getTokenOfQQ();	// ��QQ��Ȩ��վ

protected:
	void _sendToQQ(SendOperatorType soType, std::string text, std::string image);

	void _uploadToQQ(std::string text, std::string image);

	void _updateHeadToQQ(std::string image);

	void _saveQQ(std::string verifier);

protected:
	QQWeibo m_qqWeibo;
	SendOperatorType m_sendType;
	std::string m_qqText;
	std::string m_qqImage;

};	//	end of class MicroBlogManager


#endif	//	end of guard MicroBlog_H
