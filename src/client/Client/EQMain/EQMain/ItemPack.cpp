#include "EQMainPCH.h"
#include "ItemPack.h"
#include "Item.h"
#include "CommFunc.h"
#include "ItemTable.h"
//-------------------------------------------------------------------------------
void ItemGrid::reset()
{
	mLock = 0;
	mItem = NULL;
	mPData = NULL;
	mOverlay = 0;
}
//-------------------------------------------------------------------------------
ItemPack::~ItemPack(void)
{
	ItemPackLogic::clearItemPack(*this);
	std::vector<ItemGrid>::iterator iter = mTotalItemGrids.begin();
		for ( ; iter != mTotalItemGrids.end(); iter++)
		{
			delete iter->mItem;
		}

}
//-------------------------------------------------------------------------------
void ItemPack::init(uint sizePerPage,uint pagenum,int usage,int curPage)
{
	mTotalItemGrids.resize(sizePerPage * pagenum);
	mGridNumPerPage	= sizePerPage;
	mCurrentPage	= curPage;
	mPageNum		= pagenum;
	mUsage			= usage;
}
//--------------------------------------------------------------------------------
bool ItemPack::setItem( int globaIndex,unsigned __int64 instance_id,
					    int template_id,uint count,int lock )
{
	if (globaIndex >= 0
		&& (uint)globaIndex <= mPageNum * mGridNumPerPage
		&& globaIndex < (int)mTotalItemGrids.size())
	{
		dropItem(globaIndex);

		if( NULL == mTotalItemGrids[globaIndex].mItem 
			&& template_id > 0)
		{
			_setItem(globaIndex,template_id,instance_id,count,0,0,lock);
		}
	}
	else
		return false;

	return true;
}
//--------------------------------------------------------------------------------
bool ItemPack::setItem( int globaIndex,unsigned __int64 instance_id,
					   std::string delTime,
					   int template_id,uint count,int lock )
{
	if (globaIndex >= 0
		&& (uint)globaIndex <= mPageNum * mGridNumPerPage
		&& globaIndex < (int)mTotalItemGrids.size())
	{
		dropItem(globaIndex);

		if( NULL == mTotalItemGrids[globaIndex].mItem 
			&& template_id > 0)
		{
			_setItem(globaIndex,template_id,delTime,instance_id,count,0,0,lock);
		}
	}
	else
		return false;

	return true;
}
//--------------------------------------------------------------------------------
void ItemPack::resize( int size )
{
	if ((int)mTotalItemGrids.size() < size)
	{
		mTotalItemGrids.resize(size);
		_reCalculPageNum();
	}
}
//--------------------------------------------------------------------------------
void ItemPack::dropItem( int globaIndex )
{
	if (mTotalItemGrids[globaIndex].mItem)
	{
		delete(mTotalItemGrids[globaIndex].mItem);
		mTotalItemGrids[globaIndex].reset();
		return;
	}
}
//--------------------------------------------------------------------------------
const ItemGrid& ItemPack::getItemGirdByGlobaIdx(int globaIdx) const
{ 
	assert (globaIdx < (int)mTotalItemGrids.size());
	return mTotalItemGrids[globaIdx]; 
}
//--------------------------------------------------------------------------------
const ItemGrid* ItemPack::getItemGirdOfCurPage(int curPageIdx) const
{ 
	size_t idx = curPageIdx + (mCurrentPage-1)*mGridNumPerPage;
	if (idx < mTotalItemGrids.size())
	{
		return &mTotalItemGrids[idx]; 
	}
	else
	{
		return NULL;
	}
}
//--------------------------------------------------------------------------------
ItemGrid& ItemPack::_findItemGrid( int curPageIdx )
{
	assert(((uint)curPageIdx <= mGridNumPerPage) && (curPageIdx >= 0));
	return mTotalItemGrids[_findGlobaIndex(curPageIdx)];
}
//--------------------------------------------------------------------------------
int ItemPack::_findGlobaIndex(int curPageIdx)
{
	assert((uint)curPageIdx <= mGridNumPerPage && curPageIdx >= 0);
	return curPageIdx + (mCurrentPage-1)*mGridNumPerPage;
}
//--------------------------------------------------------------------------------
int ItemPack::delItem( int curPageIdx )
{
	ItemGrid& ig = _findItemGrid(curPageIdx);
	if (ig.mItem)
	{
		int data = ig.mData;
		delete (ig.mItem);
		mTotalItemGrids.erase( mTotalItemGrids.begin() + _findGlobaIndex(curPageIdx));
		
		if( !_reCalculPageNum() )
		{
			ItemGrid grid;
			mTotalItemGrids.push_back(grid);
		}

		if((mUsage & IPU_PAGE_NOEMPTY) != 0 )
			_removeEmptyPage();
		return data;
	}
	return 0;
}
//--------------------------------------------------------------------------------
int ItemPack::delItemByData(int data)
{
	std::vector<ItemGrid>::iterator it;
	for(it=mTotalItemGrids.begin(); it!=mTotalItemGrids.end(); it++)
	{
		if((*it).mData == data)
		{
			mTotalItemGrids.erase(it);

			if( !_reCalculPageNum() )
			{
				ItemGrid grid;
				mTotalItemGrids.push_back(grid);
			}

			if((mUsage & IPU_PAGE_NOEMPTY) != 0 )
				_removeEmptyPage();
			return data;
		}
	}
	return 0;
}

//--------------------------------------------------------------------------------
uint ItemPack::getCurPage()
{
	return mCurrentPage;
}
//--------------------------------------------------------------------------------
uint ItemPack::getPageNum()
{
	return mPageNum;
}
//--------------------------------------------------------------------------------
bool ItemPack::changePage(uint page)
{
	if(page >=1 && page <= mPageNum)
	{
		mCurrentPage = page;
	}
	else
	{
		LOGERR("包的页面从1开始");
		return false;
	}

	return true;
}
//
void ItemPack::_removeEmptyPage()
{
	//第一页不能删
	for (uint page=2; page<= mPageNum; page++)
	{
		if (_isPageItemEmpty(page))
		{
			_deletePage(page);
		}
	}
}
//
void ItemPack::_deletePage(int page)
{
	assert(page<=(int)mPageNum);
	for (uint index=page*mGridNumPerPage-1; 
		index >= (page-1)*mGridNumPerPage; index--)
	{
		ItemGrid& ig = mTotalItemGrids[index];
		delete (ig.mItem); 
		mTotalItemGrids.erase( mTotalItemGrids.begin() + index);
	}
	_reCalculPageNum();
}
//
bool ItemPack::_isPageItemEmpty(int page)
{
	for (uint index=(page-1)*mGridNumPerPage; 
		index < page*mGridNumPerPage; index++)
	{
		if(mTotalItemGrids[index].mItem != NULL)
			return false;
	}

	return true;
}
//
bool ItemPack::_reCalculPageNum()
{
	uint oldNum = mPageNum;

	mPageNum = (mTotalItemGrids.size() / mGridNumPerPage);
	mPageNum = mPageNum * mGridNumPerPage < mTotalItemGrids.size() 
				? mPageNum + 1 : mPageNum;

	//当前页不能大于总页数
	if(mCurrentPage > mPageNum)
		mCurrentPage = mPageNum;

	return oldNum != mPageNum;
}
//--------------------------------------------------------------------------------
bool ItemPack::lastPage()
{
	if(mCurrentPage == 1)
		return false;

	return changePage(--mCurrentPage);
}
//--------------------------------------------------------------------------------
bool ItemPack::nextPage()
{
	if(mCurrentPage == getPageNum())
		return false;

	return changePage(++mCurrentPage);
}
//-------------------------------------------------------------------------------
bool ItemPack::insertItem(int template_id, int data, void* pdata, uint count, unsigned __int64 instance_id)
{
	if(template_id > 0)
	{
		for(uint i=0; i<mGridNumPerPage * mPageNum; i++)
		{
			if( NULL == mTotalItemGrids[i].mItem  && template_id > 0)
			{
				_setItem(i, template_id, instance_id, count, data, pdata);
				_reCalculPageNum();
				return true;
			}
		}

		resize(mGridNumPerPage*(mPageNum+1));
		return insertItem(template_id,data,pdata,count,instance_id);
	}

	return false;
}
//--------------------------------------------------------------------------------
void ItemPack::clear()
{
	mTotalItemGrids.clear();
	init(mGridNumPerPage);
}
//--------------------------------------------------------------------------------
void ItemPack::_setItem( uint globaIdx, int template_id, unsigned __int64 instance_id, uint overlay, int data, void* pdata, int lock )
{
	mTotalItemGrids[globaIdx].mItem = new Item(template_id);
	mTotalItemGrids[globaIdx].mItem->setInstanceID(instance_id);
	mTotalItemGrids[globaIdx].mOverlay	= overlay;
	mTotalItemGrids[globaIdx].mData		= data;
	mTotalItemGrids[globaIdx].mPData	= pdata;
	mTotalItemGrids[globaIdx].mLock		= lock;
}
//--------------------------------------------------------------------------------
void ItemPack::_setItem( uint globaIdx, int template_id, std::string delTime, unsigned __int64 instance_id, uint overlay, int data, void* pdata, int lock )
{
	mTotalItemGrids[globaIdx].mItem = new Item(template_id, delTime);
	mTotalItemGrids[globaIdx].mItem->setInstanceID(instance_id);
	mTotalItemGrids[globaIdx].mOverlay	= overlay;
	mTotalItemGrids[globaIdx].mData		= data;
	mTotalItemGrids[globaIdx].mPData	= pdata;
	mTotalItemGrids[globaIdx].mLock		= lock;
}
//--------------------------------------------------------------------------------
void ItemPackLogic::clearItemPack(ItemPack & pack)
{
	for ( uint i = 0; i < pack.getGridNumPerPage(); ++i )
		pack.setItem(i, 0, 0, 0, 0);
}
//--------------------------------------------------------------------------------
uint ItemPackLogic::getTakeOffSlots(const ItemPack & pack, uint equipOnSlots)
{
	uint takeOffSlots = equipOnSlots;

	for ( std::vector<ItemGrid>::const_iterator it = pack.mTotalItemGrids.begin();
		it != pack.mTotalItemGrids.end();
		++it )
	{
		if ( !it->mItem )
			continue;

		const ItemEquipRow * r = it->mItem->getItemEquipInfo();
		if ( !r)
			continue;

		// 如果某个物品也占用欲穿上的装备栏, 把这个物品的占用栏位也加入到结果中
		if ( (r->equip_placeholder & equipOnSlots) != 0 )
		{
			takeOffSlots = (takeOffSlots | r->equip_placeholder);
		}
	}
	return takeOffSlots;
}