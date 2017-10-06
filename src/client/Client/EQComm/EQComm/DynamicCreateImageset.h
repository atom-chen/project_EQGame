/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-11-26
* ����: ��̬����imageset������
******************************************************************************/
#ifndef DynamicCreateImagesetMgr_H
#define DynamicCreateImagesetMgr_H

#include "EQMainPrerequisites.h"
#include "CEGUI.h"
#include "Ogre.h"


/*************************************************************************
* ��̬����imageset������
*************************************************************************/
class DynamicCreateImagesetMgr : public Ogre::Singleton<DynamicCreateImagesetMgr>
{
public:
	typedef std::map<std::string, std::string> ImageToSet;	// imageName��imagesetName��ӳ��
	typedef std::map<std::string, std::string> DataToImage;	// imageData��imageName��ӳ��

	// �Զ�����������
	struct TextureTypedef
	{
		TextureTypedef():type(""),tex(NULL){}
		std::string type;		// ���ͣ���32*32��96*96�ȣ�
		CEGUI::Texture *tex;	// ����
	};

public:
	/**
	*	���캯��
	*/
	DynamicCreateImagesetMgr();

	/**
	*	��������
	*/
	~DynamicCreateImagesetMgr();

	/**
	*	��	�ܣ�����ͼƬ
	*	��	����imageData: ͼƬ���ݣ�base64��ʽ����imageName: ͼƬ���ƣ�����ͬ����
	*	����ֵ��0����ȷ����-1������ͬ�����ݵ�ͼƬ����-2������ͬ��imageName����-3��base64����ʧ�ܣ���-4��ͼƬ��ʽ���󣩣�-5������imagesetʧ�ܣ�
	*/
	int saveImage(std::string imageData, std::string imageName);

	/**
	*	��	�ܣ�����ͼƬ�������image����imageset��һ����
	*	��	����imageset: imageset����fileName: ͼƬ�ļ���������׺��
	*	����ֵ��0����ȷ����-1������ͬ��imageset����-2������imagesetʧ�ܣ�
	*/
	int saveImageEx(std::string imageset, std::string fileName);

	/**
	*	��	�ܣ�����imageName��ȡ��Ӧ��imagesetName
	*	��	����imageName: ͼƬ��
	*	����ֵ��imageset����""��û�ж�Ӧ��imagesetName��
	*/
	std::string getImagesetName(std::string imageName);

	/**
	*	��	�ܣ�����imageData��ȡimageName
	*	��	����imageData: ͼƬ����
	*	����ֵ��ͼƬ����""��û�ж�Ӧ��imageName��
	*/
	std::string getImageName(std::string imageData);

	/**
	*	��	�ܣ�����imageset
	*	��	����imagesetName: ����
	*	����ֵ��void
	*/
	void destroyImageset(std::string imagesetName);

	/**
	*	��	�ܣ�������Դ��Ŀ¼
	*	��	����resourceGroup:��Դ�飬type:��������section: ������
	*	����ֵ��void
	*/
	void setResourceGroupDirectory(std::string resourceGroup, std::string type, std::string section);

	/**
	*	��	�ܣ�����ͼƬ�����õ�ָ��������
	*	��	����winname: �������ƣ�imageData: ͼƬ���ݣ�base64��ʽ����imageName: ͼƬ���ƣ�����ͬ����
	*	����ֵ��0����ȷ����-1��������winname�����ڣ���-2��������imageName����-3������ͼƬʧ�ܣ�
	*/
	int setWindowImage(std::string winname, std::string imageData, std::string imageName);

	/**
	*	��	�ܣ���ָ��ͼƬ���õ�ָ��������
	*	��	����winname: �������ƣ�imageName: ͼƬ����
	*	����ֵ��0����ȷ����-1��������winname�����ڣ���-2��������imageName��
	*/
	int setWindowImage(std::string winname, std::string imageName);

private:
	/**
	*	��	�ܣ�����ͼƬ
	*	��	����img: ͼƬ��imageName: ͼƬ����
	*	����ֵ��0����ȷ����-1��ͼƬ��ʽ���󣩣�-2������imagesetʧ�ܣ�
	*/
	int _saveImage(Ogre::Image& img, std::string imageName);

	/**
	*	��	�ܣ��ڻ���������Ӧλ��ճ��ͼƬ
	*	��	����buf: ��������n: λ�ã�img: ͼƬ
	*	����ֵ��0����ȷ����-1������n��������ճ��ͼƬ������
	*/
	int _pasteToBuffer(int* buf, int n, Ogre::Image img);

	/**
	*	��	�ܣ���̬����imageset
	*	��	����flag: imageset���Ʊ�ʶ
	*	����ֵ��CEGUI::Imageset*
	*/
	CEGUI::Imageset* _createImageset(int flag);

	/**
	*	��	�ܣ����ó�Ա����
	*	��	����tex: ����imgSet: ͼƬ��
	*	����ֵ��int
	*/
	void _setMemberVariable(CEGUI::Texture* tex, CEGUI::Imageset* imgSet);

	/**
	*	��	�ܣ�����һ������������ճ������ͼƬ
	*	��	����width: ͼƬ��ȣ�height: ͼƬ�߶�
	*	����ֵ������ճ��ͼƬ����
	*/
	int _countImageNum(int width, int height);

private:
	int *mBuffer;	// �ڴ滺����
	TextureTypedef mCurTexture;		// ��ǰ����
	CEGUI::Imageset *mCurImageset;	// ��ǰimageset
	int mImagesetNameFlag;			// imagesetName��ʶ
	int mPos;		// ͼƬ�ڻ������е�λ��
	ImageToSet	mImageToSet;		// imageName��imagesetName��ӳ��
	DataToImage mDataToImage;		// imageData��imageName��ӳ��

};	// end of class DynamicCreateImagesetMgr

/**
*	��	�ܣ�base64���루����output���ڴ���Ҫ���������ͷţ�
*	��	����input:	base64�ַ���
*			inputLength:	base64�ַ�������
*			output:	�������ַ���
*	����ֵ���������ַ����ĳ��ȣ�0������ʧ�ܣ�
*/
int base64Decode(const char* input, int inputLength, char** output);


#endif	// end of guard DynamicCreateImagesetMgr_H
