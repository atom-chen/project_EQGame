#include "HouseEditorPCH.h"
#include "IntersectPoint.h"

#include "CommFuncGcs.h"
#include "RootManager.h"
#include "SystemGcs.h"


IntersectPoint::IntersectPoint()
: mIntersectNode(NULL)
, mIntersectEntity(NULL)
{
	// Create intersection node and entity

	mIntersectNode = RootManager::getSingleton().getIndicatorRootSceneNode()->createChildSceneNode();

	Real radius = 1;
	int rings = 16;
	int segments = 16;
	Ogre::MeshPtr sphereMesh = createSphereMesh(SystemGcs::getSingleton().getSceneManager(), radius, rings, segments);
	Ogre::MaterialPtr material = createColourMaterial(
		Ogre::ColourValue(1, 0, 0, 0.75),
		Ogre::ColourValue(0, 1, 0),
		1);

	mIntersectEntity = SystemGcs::getSingleton().getSceneManager()->createEntity(mIntersectNode->getName(), sphereMesh->getName());
	mIntersectEntity->setQueryFlags(0);
	//mIntersectEntity->setNormaliseNormals(true);
	mIntersectEntity->setMaterialName(material->getName());
	mIntersectEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_7);
	mIntersectNode->attachObject(mIntersectEntity);
	mIntersectNode->setVisible(false);

	reshape();
}

IntersectPoint::~IntersectPoint()
{
	if (mIntersectNode)
	{
		//mIntersectNode->destroy();
		mIntersectNode->removeAndDestroyAllChildren();
		mIntersectNode->getCreator()->destroySceneNode(mIntersectNode);
	}
	if (mIntersectEntity)
	{
		//mIntersectEntity->destroy();
		SystemGcs::getSingleton().getSceneManager()->destroyEntity(mIntersectEntity);
	}
}

const std::string& IntersectPoint::getName(void) const
{
	static const std::string name = "IntersectPoint";
	return name;
}

void IntersectPoint::hide(void)
{
	mIntersectNode->setVisible(false);
}

void IntersectPoint::_setHitPoint(Real x, Real y)
{
	Ogre::Vector3 position;
	bool hit = RootManager::getSingleton().getTerrainIntersects(x, y, position);
	mIntersectNode->setVisible(hit);
	if (hit)
	{
		mIntersectNode->setPosition(position);
	}
}

void IntersectPoint::reshape(void)
{
	// Adjust intersection scale
	Real scale = RootManager::getSingleton().getBaseScale() / (5 * mIntersectEntity->getMesh()->getBoundingSphereRadius());
	mIntersectNode->setScale(scale, scale, scale);
}


// ������ײС��Ĳ���.
void IntersectPoint::SetMaterial(std::string& strMaterialName)
{
	mIntersectEntity->setMaterialName(strMaterialName);
}

// ������ײС��ԭ���Ĳ���.
void IntersectPoint::SetMaterial()
{
	Ogre::MaterialPtr material = createColourMaterial(
		Ogre::ColourValue(1, 0, 0, 0.75),
		Ogre::ColourValue(0, 1, 0),
		1);

	mIntersectEntity->setMaterialName(material->getName());
}

