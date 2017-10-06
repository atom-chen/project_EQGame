/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-4
* ����: �����ؼ�Դ�ļ����������ľ���ʵ��
******************************************************************************/
#include "CEGUIAnimate.h"


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	���캯��
	Animate::Animate(void) :
		mElapsed(-1),
		mWidth(-1),
		mHeight(-1)
	{
	}
	//-------------------------------------------------------------------------
	//	��������
	Animate::~Animate(void)
	{
	}
	//-------------------------------------------------------------------------
	//	��ʼ����������
	void Animate::initAnimate(int elapsed, float width, float height)
	{
		clearUp();
		mElapsed = elapsed;
		mWidth = width;
		mHeight = height;
	}
	//-------------------------------------------------------------------------
	//	��ȡ����ʱ����
	int Animate::getElapsed(void) const
	{
		assert(-1 != mElapsed);	//	�ȵ���initAnimate()���г�ʼ��
		return mElapsed;
	}
	//-------------------------------------------------------------------------
	//	��ȡ������ʱ��
	int Animate::getTimeTotal(void) const
	{
		assert(0 != mImageVec.size());	//	�ȵ���addAnimageFrame()
		return mTimeTotal;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�������
	float Animate::getWidth(void) const
	{
		assert(-1 != mWidth);	//	�ȵ���initAnimate()���г�ʼ��
		return mWidth;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�����߶�
	float Animate::getHeight(void) const
	{
		assert(-1 != mHeight);	//	�ȵ���initAnimate()���г�ʼ��
		return mHeight;
	}
	//-------------------------------------------------------------------------
	//	��Ӷ���֡
	void Animate::addAnimateFrame(Image img)
	{
		mImageVec.push_back(img);
		mTimeTotal = mElapsed * mImageVec.size();
	}
	//-------------------------------------------------------------------------
	//	��Ӷ���֡
	void Animate::addAnimateFrame(std::string imagesetName, std::string imageName)
	{
		Imageset *imgSet = NULL;
		std::string setName = imagesetName + ".imageset";
		if (ImagesetManager::getSingleton().isImagesetPresent(imagesetName))
		{
			imgSet = ImagesetManager::getSingleton().getImageset(imagesetName);
		} 
		else
		{
			imgSet = ImagesetManager::getSingleton().createImageset(setName);
		}
		assert(NULL != imgSet);
		const Image *img = &(imgSet->getImage(imageName));
		assert(NULL != img);
		addAnimateFrame(*img);
	}
	//-------------------------------------------------------------------------
	//	�������֡
	void Animate::clearUp(void)
	{
		mImageVec.clear();
		mElapsed = -1;
		mWidth = -1;
		mHeight = -1;
	}
	//-------------------------------------------------------------------------
	//	��ȡ����֡
	Image& Animate::getAnimateFrame(size_t index)
	{
		assert((index >= 0) && (index < mImageVec.size()));
		return mImageVec[index];
	}
	//-------------------------------------------------------------------------
	//	������������
	Animate createAnimate(void)
	{
		static Animate ani;	// �Ƚχ������,Ϊ���ṩ��Lua����
		return ani;
	}

}	//	end of namespace CEGUI
