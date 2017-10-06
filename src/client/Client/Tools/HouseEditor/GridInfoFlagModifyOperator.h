/******************************************************************************
* ����: Y3
* ʱ��: 2005-8-19
* ����: ����TD
******************************************************************************/
#ifndef __GRIDINFOFLAGMODIFYOPERATOR_H__
#define __GRIDINFOFLAGMODIFYOPERATOR_H__

#include "Operator.h"


/** �ԽǷ�ת�ߵ�operator������undo redo
*/
class GridInfoFlagModifyOperator : public Operator
{
public:

	struct Info
	{
		int x;
		int z;
		uint oldFlipFlag;
		uint newFlipFlag;
	};

	GridInfoFlagModifyOperator();
	~GridInfoFlagModifyOperator();

	void add(int x, int z, uint oldFlipFlag, uint newFlipFlag);

	bool empty(void) const;

	const std::string& getGroupName(void) const;

	std::string getDescription(void) const;

	std::string getHelp(void) const;

	void undo(void);

	void redo(void);

protected:

	void apply(uint Info::* flipFlag);

protected:

	typedef std::vector<Info> InfoList;

	InfoList mInfos;
	int mMinX, mMaxX, mMinZ, mMaxZ;

	std::string mIndicatorName;
	std::string mGroupName;
};

//////////////////////////////////////////////////////////////////////////
class FlipTerrainDiagonalOperator : public GridInfoFlagModifyOperator
{
public:
	FlipTerrainDiagonalOperator();
};

//////////////////////////////////////////////////////////////////////////
class DisableDecalOperator : public GridInfoFlagModifyOperator
{
public:
	DisableDecalOperator();
};


#endif // __GridInfoFlagModifyOperator_H__