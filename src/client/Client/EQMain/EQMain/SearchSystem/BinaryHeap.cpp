#include "BinaryHeap.h"

namespace SearchSystem
{
	//-------------------------------------------------------------------------
	CBinaryHeap::CBinaryHeap()
	{
		m_nSize = 0;
	}

	//-------------------------------------------------------------------------
	CBinaryHeap::~CBinaryHeap()
	{
		Destroy();
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::Create(size_t nSize)
	{
		m_arrData.resize(nSize);
		m_arrDatToIdx.resize(nSize);
		m_nSize = 0;
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::Destroy()
	{
		m_nSize = 0;
		FreeExtra();
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::FreeExtra()
	{
		m_arrData.resize(m_nSize);
		m_arrDatToIdx.resize(m_nSize);
	}

	//-------------------------------------------------------------------------
	HeapPair* CBinaryHeap::GetTop()
	{
		if(m_nSize == 0)
			return NULL;

		return &m_arrData[0];
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::DeleteTop(bool isFloat)
	{
		if(m_nSize == 0)
			return;

		m_arrData[0] = m_arrData[--m_nSize];
		m_arrDatToIdx[m_arrData[m_nSize].Dat] = 0;
		ShiftDown(0, isFloat);
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::Insert(HeapPair* pData, bool isFloat)
	{
		if(m_nSize == m_arrData.size())
		{
			int m_nInc = 4096 / sizeof(HeapPair);
			m_arrData.resize(m_nSize + m_nInc);
			m_arrDatToIdx.resize(m_nSize + m_nInc);
		}

		m_arrData[m_nSize] = *pData;
		m_arrDatToIdx[m_arrData[m_nSize].Dat] = m_nSize; // ¸Ä
		ShiftUp(++m_nSize - 1, isFloat);
	}

	//-------------------------------------------------------------------------
	int CBinaryHeap::GetDataIndex(size_t nDataIndex)
	{
		return m_arrDatToIdx[nDataIndex];
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::Update(size_t nIndex, HeapPair* pNewData, bool isFloat)
	{
		if(nIndex >= m_nSize)
			return;

		if(pNewData == NULL)
			return;

		int Compare = m_arrData[nIndex].Compare(*pNewData, isFloat);

		if(Compare < 0)
		{
			m_arrData[nIndex] = *pNewData;
			ShiftDown(nIndex, isFloat);
		}
		else if(Compare > 0)
		{
			m_arrData[nIndex] = *pNewData;
			ShiftUp(nIndex, isFloat);
		}
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::ShiftDown(size_t nIndex, bool isFloat)
	{
		HeapPair _D = m_arrData[nIndex];
		size_t _I = nIndex;

		for(DeLChild(nIndex); nIndex < m_nSize; DeLChild(nIndex))
		{
			if(nIndex + 1 < m_nSize &&
				m_arrData[nIndex].Compare(m_arrData[nIndex + 1], isFloat) > 0)
				nIndex++;

			if(m_arrData[nIndex].Compare(_D, isFloat) < 0)
			{
				m_arrData[_I] = m_arrData[nIndex];
				m_arrDatToIdx[m_arrData[_I].Dat] = _I;
				_I = nIndex;
			}
			else
				break;
		}

		m_arrData[_I] = _D;
		m_arrDatToIdx[m_arrData[_I].Dat] = _I;
	}

	//-------------------------------------------------------------------------
	void CBinaryHeap::ShiftUp(size_t nIndex, bool isFloat)
	{
		HeapPair _D = m_arrData[nIndex];
		size_t _I = nIndex;

		for(DeParent(nIndex); _I > 0; DeParent(nIndex))
		{
			if(m_arrData[nIndex].Compare(_D, isFloat) > 0)
			{
				m_arrData[_I] = m_arrData[nIndex];
				m_arrDatToIdx[m_arrData[_I].Dat] = _I;
				_I = nIndex;
			}
			else
				break;
		}

		m_arrData[_I] = _D;
		m_arrDatToIdx[m_arrData[_I].Dat] = _I;
	}
}


