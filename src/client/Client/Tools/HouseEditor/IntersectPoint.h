/******************************************************************************
* 作者: Y3
* 时间: 2005-6-4
* 描述: 来自TD
******************************************************************************/
#ifndef __IntersectPoint_H__
#define __IntersectPoint_H__

#include "HitIndicator.h"


class IntersectPoint : public HitIndicator
{
protected:
	Ogre::SceneNode* mIntersectNode;
	Ogre::Entity* mIntersectEntity;

public:
	IntersectPoint();
	~IntersectPoint();

	const std::string& getName(void) const;

	void reshape(void);
	void hide(void);

	// 设置碰撞小球原来的材质
	void SetMaterial();

	// 设置碰撞小球的新材质.
	void SetMaterial(std::string& strMaterialName);



protected:
	void _setHitPoint(Real x, Real y);
};



#endif // __IntersectPoint_H__
