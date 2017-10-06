/******************************************************************************
* ����: Y3
* ʱ��: 2005-6-1
* ����: ����TD
******************************************************************************/
#ifndef __HeightAction_H__
#define __HeightAction_H__

#include "Action.h"


class JunctionSelection;

class HeightAction : public Action
{
public:
	HeightAction() {}
	~HeightAction();

	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );


protected:
	virtual void _onActive(bool active);

	void doFinish(JunctionSelection* origin, bool canceled);
};



#endif // __HeightAction_H__
