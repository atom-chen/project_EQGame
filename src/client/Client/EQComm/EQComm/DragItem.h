/******************************************************************
* 作者: 麦_Mike
* 时间: 20010-7-29
* 描述: 游戏物品拖动管理
*******************************************************************/
#ifndef DRAGDROPITEM_H
#define DRAGDROPITEM_H

#include "InputMgr.h"

#include "CEGUI.h"
#include <OIS.h>
#include "GameEventMgr.h"
#include "CursorMgr.h"
#include "FitmentEvent.h"
#include "luabind/luabind/luabind.hpp"

class DragItem : public EQInputEventListener
{
public:
	DragItem(void)
		: mDragWnd(NULL)
		,mDraging(false)
		,mInited(false)
		,mSplitNum(0)
		,EQInputEventListener(epfDragItem)
	{}
	~DragItem(void)
	{}
	static DragItem& getSingleton()
	{
		static DragItem singleton;
		return singleton;
	}
	void 	init();

	/*拖动接口，物品点击拖动后调用
	@param parent:		slot window
	@param index:		物品在包中索引
	@param packtype:	包的类型
	*/
	void 	drag(std::string parentSlot,int index,int packtype);

	//是否有物品在拖动中
	bool 	isDraging(){
		return mDraging;}
	
	//撤消物品拖动
	bool 	undo();

	//得到物品在包中的索引，物品包的类型
	int		getIndex(){return mIndex;}
	int		getPacktype(){return mPacktype;}

	//
	void	setSplitNum(int num)
	{
		mSplitNum = num;
		char strnum[16] = "/0";
		sprintf(strnum,"%d",num);
		if(mSplitNum > 0)
			mDragWnd->getChildAtIdx(0)->setText(strnum);
	}
	int	getSplitNum(){return mSplitNum;}

	void	setVisible(bool v)	
	{ 
		if(mDragWnd && isDraging())
			mDragWnd->setVisible(v);
		else
			LOGERR("DragItem 错误！不在拖动中");
	}

	bool	isVisible()
	{
		return mDragWnd ? mDragWnd->isVisible() : false;
	}

	//输入事件
	virtual void MouseMove	( UINT& flags, POINT delta, POINT point ) 
	{
		_updatePos(point);
	}
	virtual void LButtonUp	( UINT& flags, POINT point ) 
	{
		if ((flags & epfUI) == 0)
		{
			//if( undo())
			//{
			//	flags |= epfDragItem;
			//	sEventMgr.fireEvent("DRAG_ITEM_UNDOED");
			//}

			if (sCursorMgr.getSpliting())
				return;
			if (sCursorMgr.getState() == CursorMgr::CS_SPLIT_ITEM)
			{
				flags |= epfDragItem;
				sEventMgr.fireEvent("DRAG_ITEM_UNDOED");
			}
			else
			{	
				if( undo())
				{
					flags |= epfDragItem;
					sEventMgr.fireEvent("DRAG_ITEM_UNDOED");
				}
			}
		}
	}
	virtual void RButtonUp	( UINT& flags, POINT point ) 
	{
		if (sFitMgr.isFiting())
			return;

		if (sCursorMgr.getSpliting())
			return;

		if (sCursorMgr.getState() == CursorMgr::CS_SPLIT_ITEM)
		{
			if (sCursorMgr.getSpliting())
				return;
			flags |= epfDragItem;
			sEventMgr.fireEvent("DRAG_ITEM_UNDOED");
		}
		else if (sCursorMgr.getState() == CursorMgr::CS_HYPERLINK)
		{
			flags |= epfDragItem;
			sEventMgr.fireEvent("DRAG_ITEM_UNDOED");
		}
		else
		{
			if( undo())
			{
				flags |= epfDragItem;
				sEventMgr.fireEvent("DRAG_ITEM_UNDOED");			
				luabind::call_function<void>(sLuaMgr.getL(), "Storage_ClickCloseEquipTips");
			}
		}
	}
	virtual void KeyUp		( UINT& flags, UINT nChar, UINT nRepCnt )
	{
		if ( OIS::KC_ESCAPE == nChar )
		{
			if( undo())
			{
				flags |= epfDragItem;
				sEventMgr.fireEvent("DRAG_ITEM_UNDOED");
			}
		}
	}
private:
	void _updatePos(POINT posabs);
	void _addDragWnd();
	void _addItemNumTxt();

	CEGUI::Window	*mDragWnd
					,*mDragFromSlotWnd
					;

	int				mIndex
					,mPacktype
					,mSplitNum
					;
	bool			mDraging
					,mInited;
};

#define sDragItem DragItem::getSingleton()
#endif