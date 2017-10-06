/******************************************************************
* 作者: 麦_Mike
* 时间: 20010-3-29
* 描述: 游戏物品集合管理
*******************************************************************/
#ifndef ItemPack_H
#define ItemPack_H

#include "Item.h"

//ItemPack Usage
#define IPU_PAGE_NONE		0
#define IPU_PAGE_NOEMPTY	(1 << 1)

/**-------------------------------------
放物品的格子
*/
struct ItemGrid
{
	ItemGrid():mItem(NULL),mOverlay(0),mPData(NULL){}
	Item*	mItem;		//物品
	uint	mOverlay;	//叠加数量
	int		mLock;
	int		mData;
	void*	mPData;

	void reset();
};

/**-------------------------------------------------------------
由多个放物品的格子组合成的物品包
*/
class ItemPack
{
public:
	friend class ItemPackLogic;

	/*	物品包类型,保持和EnumDef.h里的
		enum PACKAGE_TYPE｛｝的值一至*/
	enum PackType
	{	PT_NONE,
		PT_WAREHOUSE,
		PT_BODY,
		PT_FARMDEPOT,
		PT_STORAGE,
		PT_GIFT,
		PT_SHOPCENTER,
		PT_SHOPCENTER_RECOMMEND,
		PT_FITMODE_SHOPCENTER,
		PT_FITMODE_SHOPPINGCART,
		PT_NPCSHOP,
		PT_TRADEPACK,
		PT_OUTBOX,
		PT_INBOX,
		PT_IITSLOTS,
	};

	ItemPack(void){}
	virtual~ItemPack(void);

	/*初始包大小
	*/
	void init(uint sizePerPage,uint pagenum=1,int usage=IPU_PAGE_NOEMPTY,int curPage=1);
	void clear();

	/*在指定格子放置物品
	*/
	bool setItem(int globaIndex,unsigned __int64 instance_id, 
				 int template_id,uint count,int lock);
	bool setItem(int globaIndex,unsigned __int64 instance_id,
				 std::string delTime,
				 int template_id,uint count,int lock);
	
	/*在最近的空位放置物品
	*/
	bool insertItem(int template_id, int data=0, 
					void* pdata=NULL, uint count=1, 
					unsigned __int64 instance_id=0);
	
	/*页面操作，换页面
	*/
	bool changePage(uint page);
	bool lastPage();
	bool nextPage();
	uint getCurPage();
	uint getPageNum();
	uint getGridNumPerPage(){return mGridNumPerPage;}
	
	/*从索引获得当前页面格子
	*/
	const ItemGrid*				getItemGirdOfCurPage(int curPageIdx) const ;
	const ItemGrid&				getItemGirdByGlobaIdx(int globaIdx) const;
	
	/*获得所有物品格子
	*/
	const std::vector<ItemGrid>&getTotalItemGrids() const 
	{ return mTotalItemGrids; }

	/*drop清空数据(格子还在)，del删除数据
	*/
	void dropItem(int globaIndex);
	int delItem(int curPageIdx);
	int delItemByData(int data);

	/*重新设置物品格子总数量
	*/
	void resize(int size);

protected:
	std::vector<ItemGrid>	mTotalItemGrids;	//所有的物品格子
	uint					mPageNum			//页面数量
							,mGridNumPerPage	//每页的格子数量					
							,mCurrentPage		//当前显示页面
							,mUsage				//使用方法
							;

private:
	/*判断页面是否没有物品
	*/
	bool _isPageItemEmpty(int page);
	void _deletePage(int page);

	void _removeEmptyPage();
	/*设置物品
	*/
	void _setItem( uint globaIdx, int template_id, 
				   unsigned __int64 instance_id=0, 
				   uint overlay=1, int data=0, 
				   void* pdata=0, int lock=0 );
	void _setItem( uint globaIdx, int template_id,
				   std::string delTime,
				   unsigned __int64 instance_id=0, 
				   uint overlay=1, int data=0, 
				   void* pdata=0, int lock=0 );
	/*查找物品格子
	*/
	ItemGrid&	_findItemGrid(int curPageIdx);
	
	/*查找包全局索引
	*/
	int			_findGlobaIndex(int curPageIdx);
	
	/*重新计算页数
	*/
	bool		_reCalculPageNum();

};

// AvatarEditor编辑器使用的一些逻辑, EQClient也可能会用到
class ItemPackLogic
{
public:
	static void clearItemPack(ItemPack & pack);

	/** 取玩家需要脱下的装备栏
		@param equipOnSlot 欲穿上物品占用的装备栏
		@return            需要脱下的装备栏
	*/
	static uint getTakeOffSlots(const ItemPack & pack, uint equipOnSlots);
};

#endif
