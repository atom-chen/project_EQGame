#include "EQCommPCH.h"
#include "DragItem.h"

#include "EQLog.h"
#include "InputMgr.h"


//------------------------------------------------------------------------------
void DragItem::drag(std::string parentSlot,int index,int packtype)
{
	if (isDraging())//mDragWnd->isVisible())
	{
		LOGERR("DragItem 错误！在拖动中");
		return;
	}

	CEGUI::WindowManager&mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window		*parentslotwnd
						,*temp;
	parentslotwnd = mgr.getWindow(parentSlot);

	if (parentslotwnd)
	{
		temp = parentslotwnd->getChildAtIdx(0);

		if (temp)
		{
			CEGUI::Window* root = CEGUI::System::getSingleton().getGUISheet();
			
			temp->setAlpha(0.4f);
			mDragWnd->setProperty("Image",temp->getChildAtIdx(0)->getProperty("Image"));
			mDragWnd->setVisible(true);

			CEGUI::Point mp = CEGUI::MouseCursor::getSingleton().getPosition();
			mDragWnd->setPosition(CEGUI::UVector2(
				CEGUI::UDim(0.f,mp.d_x - 16),CEGUI::UDim(0.f,mp.d_y - 16)));
			mDragFromSlotWnd	= parentslotwnd;

			mIndex		= index;
			mPacktype	= packtype;

			mDraging = true;

			//if( NULL == root->getChild(mDragWnd->getName()) )
				root->addChildWindow(mDragWnd);

			return;
		}
		else
		{
			LOGERR("DragItem 错误！");
		}
	}
	else
	{
		LOGERR("DragItem 错误！不在拖动中");
	}
}
//------------------------------------------------------------------------------
bool DragItem::undo()
{
	if (isDraging())//mDragWnd->isVisible())
	{
		mDragWnd->setVisible(false);

		if (mDragFromSlotWnd)
		{
			if(mDragFromSlotWnd->getChildCount() >= 1)
				mDragFromSlotWnd->getChildAtIdx(0)->setAlpha(1.0f);
			mDraging = false;
			setSplitNum(0);
			mDragWnd->getChildAtIdx(0)->setText("");
			luabind::call_function<void>(sLuaMgr.getL(), "Storage_ClickCloseEquipTips");
			return true;
		}
		//else
		//	LOGERR("DragItem undo 错误！");
	}
	//else
	//	LOGERR("DragItem undo 错误！");

	return false;
}
//------------------------------------------------------------------------------
void DragItem::_updatePos(POINT posabs)
{
	if (mDragWnd->isVisible())
	{
		mDragWnd->setPosition(CEGUI::UVector2(
			CEGUI::UDim(0.f,(float)posabs.x - 16),CEGUI::UDim(0.f,(float)posabs.y - 16)));
	}
}
//------------------------------------------------------------------------------
void DragItem::init()
{
	if(mInited)
		return;

	sInputMgr.addListener(this);

	_addDragWnd();

	mInited = true;
}
//------------------------------------------------------------------------------
void DragItem:: _addItemNumTxt()
{
	//创建容器内物品数量显示文本框
	CEGUI::WindowManager& mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* itemNumTxt = mgr.createWindow("EQHomeLook/StaticText","dragItemNum");
	itemNumTxt->setProperty("MousePassThroughEnabled", "true");
	itemNumTxt->setProperty("AlwaysOnTop", "true");
	itemNumTxt->setProperty("BackgroundEnabled", "false");
	itemNumTxt->setProperty("FrameEnabled", "false");
	itemNumTxt->setProperty("HorzFormatting", "LeftAligned");
	itemNumTxt->setProperty("VertFormatting", "TopAligned");
	itemNumTxt->setProperty("Font", "simsun-10");
	itemNumTxt->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
	itemNumTxt->setProperty("UnifiedAreaRect", "{{0,0},{0,0},{0.5,0},{0.5,0}");
	itemNumTxt->setText("");
	mDragWnd->addChildWindow(itemNumTxt);
}
//------------------------------------------------------------------------------
void DragItem:: _addDragWnd()
{
	CEGUI::WindowManager& mgr = CEGUI::WindowManager::getSingleton();
	mDragWnd = mgr.createWindow("TaharezLook/StaticImage","dragitem");
	mDragWnd->setVisible(false);

	CEGUI::Window* root = CEGUI::System::getSingleton().getGUISheet();
	root->addChildWindow(mDragWnd);

	CEGUI::Point mp = CEGUI::MouseCursor::getSingleton().getPosition();

	mDragWnd->setPosition(CEGUI::UVector2(
		CEGUI::UDim(0.f,mp.d_x - 16),CEGUI::UDim(0.f,mp.d_y - 16)));
	mDragWnd->setSize(CEGUI::UVector2(
		CEGUI::UDim(0.f,32.f),CEGUI::UDim(0.f,32.f)));
	mDragWnd->setMousePassThroughEnabled(true);
	mDragWnd->setAlwaysOnTop(true);
	mDragWnd->setAlpha(0.8f);
	mDragWnd->setProperty("Image", "set:common1 image:Window7_Mid");

	mDragWnd->setProperty("FrameEnabled", "false");
	mDragWnd->setProperty("BackgroundEnabled", "false");
	_addItemNumTxt();
}