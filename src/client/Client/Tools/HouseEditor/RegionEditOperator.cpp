#include "HouseEditorPCH.h"

#include "RegionEditOperator.h"
#include "RegionEditAction.h"


////////////////////////////////////////////////////////////////////////////////
//
// ���� CRegionEditOperator ��ʵ�ֺ���
//
CRegionEditOperator::CRegionEditOperator(void)
{
	m_pRegionAction = NULL;
}

CRegionEditOperator::~CRegionEditOperator(void)
{
}

void CRegionEditOperator::SetRegionAction(RegionEditManager* pRegionAction)
{
	m_pRegionAction = pRegionAction;
}

// ��������ID
void CRegionEditOperator::SetRegionId(unsigned long ulRegionId)
{
	m_ulRegionId = ulRegionId;
}

// ���õ�ID
void CRegionEditOperator::SetPointId(unsigned long ulPointId)
{
	m_ulPointId = ulPointId;
}

// ����undo�� redo ������Ҫ��λ��.
void CRegionEditOperator::SetPosition(Ogre::Vector3 vectorUnDoPos, Ogre::Vector3 vectorReDoPos)
{
	m_vectorUnDoPos = vectorUnDoPos;
	m_vectorReDoPos = vectorReDoPos;
}



/////////////////////////////////////////////////////////////////////////////////
//
// �޸�һ�����redo�� undo����.
//
//
CRegionEditModifyOperator::CRegionEditModifyOperator(void)
{

}

CRegionEditModifyOperator::~CRegionEditModifyOperator(void)
{

}


/** Retrieves a group name of this operator
@remarks
The OperatorManager will manager grouping operators by group name,
so undo/redo can be perform independent with each operator group.
@par
The operators must use same group name when their may be influence
by each other.
@returns
group name of this operator
*/
const std::string& CRegionEditModifyOperator::getGroupName(void) const
{
	static std::string strGroupName = "RegionModify";
	return strGroupName;
}

/** Retrieves a description of this operator
@remarks
The description should be display as a menu text.
@returns
description of this operator
*/
std::string CRegionEditModifyOperator::getDescription(void) const
{
	return std::string("RegionModify");
}

/** Retrieves a help text of this operator
@remarks
The help text should be use as a detail description of this operator,
when use selecting menu item of this operator, this help text will
display in status line.
@returns
help text of this operator
*/
std::string CRegionEditModifyOperator::getHelp(void) const
{
	return std::string("RegionModify");
}

/** Perform undo operate
*/
void CRegionEditModifyOperator::undo(void)
{
	if(m_pRegionAction)
	{
		m_pRegionAction->ChangeCurEditPointPos(m_ulRegionId, m_ulPointId, m_vectorUnDoPos);
	}
}

/** Perform redo operate
*/
void CRegionEditModifyOperator::redo(void)
{
	if(m_pRegionAction)
	{
		m_pRegionAction->ChangeCurEditPointPos(m_ulRegionId, m_ulPointId, m_vectorReDoPos);
	}
}







/////////////////////////////////////////////////////////////////////////////////
//
// ���һ�����redo�� undo����.
//
//


CRegionEditAddPointOperator::CRegionEditAddPointOperator(void)
{

}

CRegionEditAddPointOperator::~CRegionEditAddPointOperator(void)
{

}


/** Retrieves a group name of this operator
@remarks
The OperatorManager will manager grouping operators by group name,
so undo/redo can be perform independent with each operator group.
@par
The operators must use same group name when their may be influence
by each other.
@returns
group name of this operator
*/
const std::string& CRegionEditAddPointOperator::getGroupName(void) const
{
	static std::string strGroupName = "AddNewPoint";

	return strGroupName;
}

/** Retrieves a description of this operator
@remarks
The description should be display as a menu text.
@returns
description of this operator
*/
std::string CRegionEditAddPointOperator::getDescription(void) const
{
	return "AddNewPoint";
}

/** Retrieves a help text of this operator
@remarks
The help text should be use as a detail description of this operator,
when use selecting menu item of this operator, this help text will
display in status line.
@returns
help text of this operator
*/
std::string CRegionEditAddPointOperator::getHelp(void) const
{
	return "AddNewPoint";
}

/** Perform undo operate
*/
void CRegionEditAddPointOperator::undo(void)
{
	if(m_pRegionAction)
	{
		m_pRegionAction->DelPoint(m_ulRegionId, m_ulPointId);
	}
}

/** Perform redo operate
*/
void CRegionEditAddPointOperator::redo(void)
{
	if(m_pRegionAction)
	{
		m_pRegionAction->AddNewPoint(m_ulRegionId, m_ulPointId, m_vectorReDoPos);
	}

}
