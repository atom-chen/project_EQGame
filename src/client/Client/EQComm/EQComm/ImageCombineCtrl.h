/******************************************************************************
* ����: ��չȻ
* ʱ��: 2011-4-25
* ����: ͼ����Ͽؼ�
******************************************************************************/
#ifndef ImageCombineCtrl_H
#define ImageCombineCtrl_H

#include "EQMainPrerequisites.h"
#include "CEGUI.h"


class ImageCombineCtrl
{
public:
	// �Զ���ͼƬ��ʽ
	struct ImageDef
	{
		ImageDef(std::string imageset, std::string image, float width, float height)
			:imageset(imageset),image(image),width(width),height(height){}

		std::string imageset;	// imageset��
		std::string image;		// image��
		float width;			// ��
		float height;			// ��
	};

public:
	ImageCombineCtrl(void);
	~ImageCombineCtrl(void);

	/**
	*	��	�ܣ���ʼ����������ã�
	*	��	����row: ������col: ������combineCtrl:��Ͽؼ���
	*	����ֵ��-1�����󣩣�0����ȷ��
	*/
	int init(int row, int col, std::string combineCtrl);

	/**
	*	��	�ܣ�����ͼ���м���ã����ô���=��ʼ��ʱ���õ�row*col��
	*	��	����imageset: imagese����image: ͼ����
	*	����ֵ��0����ȷ����-1��imageset�����ڣ���-2��imageδ���壩
	*/
	int addImage(std::string imageset, std::string image);

	/**
	*	��	�ܣ������Ͽؼ�����
	*	��	����void
	*	����ֵ��void
	*/
	void clear(void);

	/**
	*	��	�ܣ����ô���ͼƬ������ʹ�ã����������溯����
	*	��	����winname: ��������imageset: imageset����image: ͼƬ��
	*	����ֵ��0����ȷ����-1��winname�����ڣ���-2��imageset�����ڣ���-3��image�����ڣ���-4������ͼƬ����
	*/
	int setWindowImage(std::string winname, std::string imageset, std::string image);

protected:
	/**
	*	��	�ܣ���Ͽؼ�
	*	��	����void
	*	����ֵ��void
	*/
	void _combine(void);

	/**
	*	��	�ܣ����ô���ͼƬ
	*	��	����winname: ��������imageset: imageset����image: ͼƬ����side: ͼƬ�ָ�Ĵ�С
	*	����ֵ��0����ȷ����-1�������д���-2������ͼƬ����
	*/
	int _setImage(std::string winname, std::string imageset, std::string image, float side);

	/**
	*	��	�ܣ�ȡ�����ô���ͼƬ
	*	��	����winname: ������
	*	����ֵ��0����ȷ����-1��winname�����ڣ�
	*/
	int _unsetImage(std::string winname);

	/**
	*	��	�ܣ�����ͼƬ
	*	��	����imageset: ��������image: ͼƬ����x: ͼƬx���꣬y: ͼƬy���꣬w: ͼƬ��h: ͼƬ��
	*	����ֵ��0����ȷ����-1�������д�
	*/
	int _defineImage(std::string imageset, std::string image, float x, float y, float w, float h);

protected:
	std::vector<ImageDef> m_imageVec;		// ��Ͽؼ�ͼ��
	std::vector<CEGUI::Window*> m_imageWin;	// ��Ͽؼ����ڼ�
	CEGUI::Window* m_combineCtrl;			// ��Ͽؼ�
	size_t m_row;							// ��Ͽؼ�����
	size_t m_col;							// ��Ͽؼ�����

};	//	end of class ImageCombineCtrl

/**
*	��	�ܣ�����ͼ����Ͽؼ�����
*	��	����void
*	����ֵ��ͼ����Ͽؼ�����
*/
ImageCombineCtrl createImageCombineCtrl(void);


#endif	//	end of guard ImageCombineCtrl_H
