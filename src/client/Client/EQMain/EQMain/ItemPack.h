/******************************************************************
* ����: ��_Mike
* ʱ��: 20010-3-29
* ����: ��Ϸ��Ʒ���Ϲ���
*******************************************************************/
#ifndef ItemPack_H
#define ItemPack_H

#include "Item.h"

//ItemPack Usage
#define IPU_PAGE_NONE		0
#define IPU_PAGE_NOEMPTY	(1 << 1)

/**-------------------------------------
����Ʒ�ĸ���
*/
struct ItemGrid
{
	ItemGrid():mItem(NULL),mOverlay(0),mPData(NULL){}
	Item*	mItem;		//��Ʒ
	uint	mOverlay;	//��������
	int		mLock;
	int		mData;
	void*	mPData;

	void reset();
};

/**-------------------------------------------------------------
�ɶ������Ʒ�ĸ�����ϳɵ���Ʒ��
*/
class ItemPack
{
public:
	friend class ItemPackLogic;

	/*	��Ʒ������,���ֺ�EnumDef.h���
		enum PACKAGE_TYPE������ֵһ��*/
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

	/*��ʼ����С
	*/
	void init(uint sizePerPage,uint pagenum=1,int usage=IPU_PAGE_NOEMPTY,int curPage=1);
	void clear();

	/*��ָ�����ӷ�����Ʒ
	*/
	bool setItem(int globaIndex,unsigned __int64 instance_id, 
				 int template_id,uint count,int lock);
	bool setItem(int globaIndex,unsigned __int64 instance_id,
				 std::string delTime,
				 int template_id,uint count,int lock);
	
	/*������Ŀ�λ������Ʒ
	*/
	bool insertItem(int template_id, int data=0, 
					void* pdata=NULL, uint count=1, 
					unsigned __int64 instance_id=0);
	
	/*ҳ���������ҳ��
	*/
	bool changePage(uint page);
	bool lastPage();
	bool nextPage();
	uint getCurPage();
	uint getPageNum();
	uint getGridNumPerPage(){return mGridNumPerPage;}
	
	/*��������õ�ǰҳ�����
	*/
	const ItemGrid*				getItemGirdOfCurPage(int curPageIdx) const ;
	const ItemGrid&				getItemGirdByGlobaIdx(int globaIdx) const;
	
	/*���������Ʒ����
	*/
	const std::vector<ItemGrid>&getTotalItemGrids() const 
	{ return mTotalItemGrids; }

	/*drop�������(���ӻ���)��delɾ������
	*/
	void dropItem(int globaIndex);
	int delItem(int curPageIdx);
	int delItemByData(int data);

	/*����������Ʒ����������
	*/
	void resize(int size);

protected:
	std::vector<ItemGrid>	mTotalItemGrids;	//���е���Ʒ����
	uint					mPageNum			//ҳ������
							,mGridNumPerPage	//ÿҳ�ĸ�������					
							,mCurrentPage		//��ǰ��ʾҳ��
							,mUsage				//ʹ�÷���
							;

private:
	/*�ж�ҳ���Ƿ�û����Ʒ
	*/
	bool _isPageItemEmpty(int page);
	void _deletePage(int page);

	void _removeEmptyPage();
	/*������Ʒ
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
	/*������Ʒ����
	*/
	ItemGrid&	_findItemGrid(int curPageIdx);
	
	/*���Ұ�ȫ������
	*/
	int			_findGlobaIndex(int curPageIdx);
	
	/*���¼���ҳ��
	*/
	bool		_reCalculPageNum();

};

// AvatarEditor�༭��ʹ�õ�һЩ�߼�, EQClientҲ���ܻ��õ�
class ItemPackLogic
{
public:
	static void clearItemPack(ItemPack & pack);

	/** ȡ�����Ҫ���µ�װ����
		@param equipOnSlot ��������Ʒռ�õ�װ����
		@return            ��Ҫ���µ�װ����
	*/
	static uint getTakeOffSlots(const ItemPack & pack, uint equipOnSlots);
};

#endif
