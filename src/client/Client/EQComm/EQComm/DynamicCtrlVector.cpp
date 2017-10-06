/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-9-3
* ����: ��̬�ؼ��������
******************************************************************************/
#include "EQCommPCH.h"
#include "DynamicCtrlVector.h"


//-----------------------------------------------------------------------
//	���캯��
DynamicCtrlVector::DynamicCtrlVector() :
	mScrollbarShowType(SST_HIDE),
	mInterval(6.0f),
	mHorInterval(mInterval),
	mVerInterval(mInterval),
	mAlignType(AT_LEFT),
	mPane(NULL),
	mRoot(NULL)
{
	mScreenWidth = CEGUI::System::getSingleton().getRenderer()->getRect().getWidth();
	mScreenHeight = CEGUI::System::getSingleton().getRenderer()->getRect().getHeight();
}
//-----------------------------------------------------------------------
//	��������
DynamicCtrlVector::~DynamicCtrlVector()
{
}
//-----------------------------------------------------------------------
//	��ʼ����̬�ؼ��������
void DynamicCtrlVector::initialise(CEGUI::Window* parent)
{
	assert(NULL != parent);
	mRoot = parent;
	mPane = static_cast<CEGUI::ScrollablePane*>(CEGUI::WindowManager::getSingleton().createWindow("EQHomeLook/ScrollablePane"));
	assert(NULL != mPane);
	mPane->setArea(CEGUI::URect(CEGUI::UDim(0,0), CEGUI::UDim(0,0), CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
	mPane->setContentPaneAutoSized(false);
	mRoot->addChildWindow(mPane);
}
//-----------------------------------------------------------------------
//	��ʼ����̬�ؼ��������
void DynamicCtrlVector::initialise(std::string parentName)
{
	assert("" != parentName);
	CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(parentName);
	assert(NULL != win);
	initialise(win);
}
//-----------------------------------------------------------------------
//	��ӿؼ�
void DynamicCtrlVector::addControl(CEGUI::Window* ctrl, float x, float y)
{
	assert(NULL != ctrl);
	ctrlObject ctrlObj;
	ctrlObj.ctrl = ctrl;
	ctrlObj.x = x;
	ctrlObj.y = y;
	ctrlObj.hor = mHorInterval;
	ctrlObj.ver = mVerInterval;
	ctrlObj.alignType = mAlignType;
	mCtrlVector.push_back(ctrlObj);
	mPane->addChildWindow(ctrl);
	mAlignType = AT_LEFT;
	mHorInterval = mInterval;
	mVerInterval = mInterval;
}
//-----------------------------------------------------------------------
//	��ӿؼ�
void DynamicCtrlVector::addControl(std::string ctrlName, float x, float y)
{
	assert("" != ctrlName);
	CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(ctrlName);
	addControl(win, x, y);
}
//-----------------------------------------------------------------------
//	��ӿؼ�
void DynamicCtrlVector::addControl(CEGUI::Window* ctrl)
{
	assert(NULL != ctrl);
	ctrlObject ctrlObj;
	ctrlObj.ctrl = ctrl;
	ctrlObj.x = -1;
	ctrlObj.y = -1;
	ctrlObj.hor = mHorInterval;
	ctrlObj.ver = mVerInterval;
	ctrlObj.alignType = mAlignType;
	mCtrlVector.push_back(ctrlObj);
	mPane->addChildWindow(ctrl);
	mAlignType = AT_LEFT;
	mHorInterval = mInterval;
	mVerInterval = mInterval;
}
//-----------------------------------------------------------------------
//	��ӿؼ�
void DynamicCtrlVector::addControl(std::string ctrlName)
{
	assert("" != ctrlName);
	CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(ctrlName);
	assert(NULL != win);
	addControl(win);
}
//-----------------------------------------------------------------------
//	�ؼ��Ű�
void DynamicCtrlVector::controlTypeset(void)
{
	float width = 0.0f;
	float height = 0.0f;
	float oldHeight = 0.0f;		//	��ӿؼ�ǰ�ĸ߶�
	float totalHeight = 0.0f;	//	�ؼ�����ܸ߶�
	float rootWidth = mRoot->getWidth().asAbsolute(mScreenWidth);	
	float rootHeight = mRoot->getHeight().asAbsolute(mScreenHeight);

	for (size_t i=0; i<mCtrlVector.size(); i++)
	{
		ctrlObject& ctrlObj = mCtrlVector[i];
		float hor = ctrlObj.hor;
		float ver = ctrlObj.ver;
		float ctrlWidth = ctrlObj.ctrl->getWidth().asAbsolute(mScreenWidth);
		float ctrlHeight = ctrlObj.ctrl->getHeight().asAbsolute(mScreenHeight);

		if (ctrlObj.x==-1 && ctrlObj.y==-1)	//	�ؼ��Զ��Ű�ģʽ
		{
			switch (ctrlObj.alignType)
			{
			case AT_LEFT:
				{
					ctrlObj.ctrl->setPosition(CEGUI::UVector2(CEGUI::UDim(0, hor), CEGUI::UDim(0, totalHeight + ver)));
					height = totalHeight;
					width = 0.0f;
					totalHeight += ctrlHeight + ver;
					oldHeight = totalHeight - ctrlHeight - ver;
				}
				break;
			case AT_CENTER:
				{
					float x = (rootWidth - width - hor - ctrlWidth)/2 + width;
					ctrlObj.ctrl->setPosition(CEGUI::UVector2(CEGUI::UDim(0, x), CEGUI::UDim(0, oldHeight + ver)));
					height = oldHeight + ctrlHeight + ver;
				}
				break;
			case AT_RIGHT:
				{
					float x = rootWidth - hor - ctrlWidth;
					ctrlObj.ctrl->setPosition(CEGUI::UVector2(CEGUI::UDim(0, x), CEGUI::UDim(0, oldHeight + ver)));
					height = oldHeight + ctrlHeight + ver;
				}
				break;
			case RIGHT_SIDE:
				{
					ctrlObj.ctrl->setPosition(CEGUI::UVector2(CEGUI::UDim(0, width + hor), CEGUI::UDim(0, oldHeight + ver)));
					height = oldHeight + ctrlHeight + ver;
				}
				break;
			}

			width += ctrlWidth + hor;
			if (totalHeight < height)
			{
				totalHeight = height;
			}
		}
		else	//	������λ�ò�����
		{
			ctrlObj.ctrl->setPosition(CEGUI::UVector2(CEGUI::UDim(0, ctrlObj.x), CEGUI::UDim(0, ctrlObj.y)));
			if (totalHeight < (ctrlObj.y + ctrlHeight))
			{
				totalHeight = ctrlObj.y + ctrlHeight;
			}
		}
	}	//	end of loop for

	// ��������ʾ��ʽ
	switch (mScrollbarShowType)
	{
	case SST_SHOW:	// ������ʾ
		mPane->setShowVertScrollbar(true);
		mPane->setContentPaneArea(CEGUI::Rect(0, 0, 0, totalHeight + mVerInterval));
		break;
	case SST_HIDE:	// ���ǲ���ʾ
		mPane->setShowVertScrollbar(false);
		mRoot->setHeight(CEGUI::UDim(0, totalHeight + mVerInterval));
		break;
	case SST_AUTO:	// �Զ���ʾ
		if (rootHeight < totalHeight + mVerInterval)	// ���ݳ����߶ȣ���ʾ
		{
			mPane->setShowVertScrollbar(true);
			mPane->setContentPaneArea(CEGUI::Rect(0, 0, 0, totalHeight + mVerInterval));
		} 
		else	// ����ʾ
		{
			mPane->setShowVertScrollbar(false);
			mPane->setContentPaneArea(CEGUI::Rect(0, 0, 0, rootHeight));
		}
		break;
	}
}
//-----------------------------------------------------------------------
//	������ʾ������
void DynamicCtrlVector::setScrollbarVisible(std::string showType)
{
	if ("SST_SHOW" == showType)
	{
		mScrollbarShowType = SST_SHOW;
	}
	else if ("SST_HIDE" == showType)
	{
		mScrollbarShowType = SST_HIDE;
	}
	else if ("SST_AUTO" == showType)
	{
		mScrollbarShowType = SST_AUTO;
	}
}
//-----------------------------------------------------------------------
//	������ʾ������
void DynamicCtrlVector::setScrollbarPosition(float pos)
{
	if (pos < 0)
		mPane->getVertScrollbar()->setScrollPosition(0);
	else if (pos > mPane->getVertScrollbar()->getDocumentSize())
		mPane->getVertScrollbar()->setScrollPosition(mPane->getVertScrollbar()->getDocumentSize());
	else
		mPane->getVertScrollbar()->setScrollPosition(pos);
}
//-----------------------------------------------------------------------
//	���ÿؼ��������ؼ��ͱ߿��ˮƽ����ֱ��϶
void DynamicCtrlVector::setInterval(float val)
{
	mInterval = val;
}
//-----------------------------------------------------------------------
//	���ÿؼ���ˮƽǰ����϶
void DynamicCtrlVector::setHorInterval(float hor)
{
	mHorInterval = hor;
}
//-----------------------------------------------------------------------
//	���ÿؼ��Ĵ�ֱǰ����϶
void DynamicCtrlVector::setVerInterval(float ver)
{
	mVerInterval = ver;
}
//-----------------------------------------------------------------------
//	���ÿؼ��Ķ��뷽ʽ
int DynamicCtrlVector::setAlign(std::string alignType)
{
	assert("" != alignType);
	if ("AT_LEFT" == alignType)
	{
		mAlignType = AT_LEFT;
	} 
	else if ("AT_CENTER" == alignType)
	{
		mAlignType = AT_CENTER;
	}
	else if ("AT_RIGHT" == alignType)
	{
		mAlignType = AT_RIGHT;
	}
	else
	{
		return -1;
	}
	return 0;
}
//-----------------------------------------------------------------------
//	���ÿؼ���ǰһ���ؼ����ұ�
void DynamicCtrlVector::setAtRight(void)
{
	mAlignType = RIGHT_SIDE;
}
//-----------------------------------------------------------------------
//	�����崹ֱ�������Ŀ��
float DynamicCtrlVector::getScrollbarWidth(void)
{
	float scrW = mPane->getVertScrollbar()->getWidth().asAbsolute(mScreenWidth);
	return scrW;
}
//-----------------------------------------------------------------------
//	�Ƴ���̬�ؼ��������������пؼ�
void DynamicCtrlVector::removeAllControl(void)
{
	if (0 == mCtrlVector.size())
	{
		return;
	}
	for (size_t i=0; i<mCtrlVector.size(); i++)
	{
		ctrlObject& ctrlObj = mCtrlVector[i];
		mPane->removeChildWindow(ctrlObj.ctrl);
	}
	mCtrlVector.clear();
}
//-----------------------------------------------------------------------
//	������̬�ؼ������������
DynamicCtrlVector createDynamicCtrlVector(void)
{
	static DynamicCtrlVector dcv;	// �Ƚχ������,Ϊ���ṩ��Lua����
	return dcv;
}
//-----------------------------------------------------------------------