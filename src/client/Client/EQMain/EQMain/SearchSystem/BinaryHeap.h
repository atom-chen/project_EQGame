/******************************************************************************
* 作者: Internet
* 时间: 2010-03-01
* 描述: 二叉堆
******************************************************************************/
#ifndef SSystem_BinaryHeap_H
#define SSystem_BinaryHeap_H


#include "SSystemCommon.h"

namespace SearchSystem
{
#define DeLChild(x) ((x) = (((x) << 1) + 1))
#define DeParent(x) ((x) = (((x) - 1) >> 1))

	//typedef int HeapKey;
	typedef int HeapDat;


	typedef struct _HeapPair
	{
		union {int i; float f;}  Key;
		HeapDat Dat;

		int Compare(const _HeapPair& Data, bool isFloat)
		{
			if ( isFloat )
			{
				const float eps = 0.000001f;
				float d = Key.f - Data.Key.f;
				if ( d > eps )
					return 1;
				else if ( d < -eps )
					return -1;
				else
					return 0;
			}
			else
				return Key.i - Data.Key.i;
		}
	}HeapPair;

	typedef std::vector<int> IntArray;
	typedef std::vector<HeapPair> HeapArray;




	class CBinaryHeap
	{

	public:

		CBinaryHeap();
		~CBinaryHeap();

		void            Create(size_t nSize);
		void            Destroy();
		void            FreeExtra();
		HeapPair*       GetTop();
		void            Insert(HeapPair* pData, bool isFloat = false);
		void            DeleteTop(bool isFloat = false);
		int             GetDataIndex(size_t nDataIndex);
		void            Update(size_t nIndex, HeapPair* pNewData, bool isFloat = false);

	protected:

		void            ShiftDown(size_t nIndex, bool isFloat);
		void            ShiftUp(size_t nIndex, bool isFloat);

	protected:

		HeapArray       m_arrData;
		IntArray        m_arrDatToIdx;
		size_t          m_nSize; 
	};
}
#endif
