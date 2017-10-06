/******************************************************************************
* 作者: Y3
* 时间: 2005-8-19
* 描述: 来自TD
******************************************************************************/
#ifndef __FAIRYMODIFYTERRAINHEIGHTOPERATOR_H__
#define __FAIRYMODIFYTERRAINHEIGHTOPERATOR_H__

#include "Operator.h"


class ModifyTerrainHeightOperator : public Operator
{
public:
	ModifyTerrainHeightOperator();
	~ModifyTerrainHeightOperator();

	void add(int x, int z, Real oldHeight, Real newHeight);
	bool empty(void) const;

	/** @copydoc Operator::getGroupName */
	const std::string& getGroupName(void) const;

	/** @copydoc Operator::getDescription */
	std::string getDescription(void) const;

	/** @copydoc Operator::getHelp */
	std::string getHelp(void) const;

	/** @copydoc Operator::undo */
	void undo(void);

	/** @copydoc Operator::redo */
	void redo(void);

protected:

	struct Info
	{
		int x;
		int z;
		Real oldHeight;
		Real newHeight;
	};

	typedef std::vector<Info> InfoList;

	InfoList mInfos;
	int mMinX, mMaxX, mMinZ, mMaxZ;

	void apply(Real Info::* height);
};


#endif 
