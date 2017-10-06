#include "HouseEditorPCH.h"

#include "Action.h"

#if OGRE_VERSION >= 0x010300
#define UNIMPLEMENTED_FEATURE ERR_NOT_IMPLEMENTED
#endif


Action::Action()
: mActived(false)
{
}

Action::~Action()
{
	if ( mActived )
		setActive(false);
}

void Action::setParameter(const std::string& name, const std::string& value)
{
	OGRE_EXCEPT(Ogre::Exception::UNIMPLEMENTED_FEATURE,
		"Parameter '" + name + "' doesn't support by Action",
		"Action::setParameter");
}

std::string Action::getParameter(const std::string& name) const
{
	OGRE_EXCEPT(Ogre::Exception::UNIMPLEMENTED_FEATURE,
		"Parameter '" + name + "' doesn't support by Action",
		"Action::getParameter");
}

void Action::setActive(bool active)
{
	if (mActived != active || active == false)	//‘ˆº”¡À|| active == false
	{
		mActived = active;
		_onActive(active);
	}
}

void Action::_onActive(bool active)
{
}

