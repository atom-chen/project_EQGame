#include "EQMainPCH.h"

#include "PickObjectProxy.h"


const std::string PickObjectProxy::msTypeName = "PickObjectProxy";
//-----------------------------------------------------------------------
PickObjectProxy::PickObjectProxy() :
mObject(NULL)
{
}
//-----------------------------------------------------------------------
PickObjectProxy::PickObjectProxy(PickObject* obj) :
mObject(obj)
{
}
//-----------------------------------------------------------------------
PickObjectProxy::~PickObjectProxy()
{
	//nothing
}
//-----------------------------------------------------------------------
const std::string& PickObjectProxy::getTypeName(void) const
{
	return msTypeName;
}
//-----------------------------------------------------------------------
PickObject* PickObjectProxy::pickUpObject(Ogre::MovableObject* movable)
{
	if ( !movable )
		return NULL;

	UserDefinedObject* udo = movable->getUserObject();
	if ( udo == NULL )
		return NULL;

	if ( udo->getTypeName() != PickObjectProxy::msTypeName )
		return NULL;

	PickObjectProxy* pxy = static_cast<PickObjectProxy*>(udo);
	return pxy->getObject();
}
