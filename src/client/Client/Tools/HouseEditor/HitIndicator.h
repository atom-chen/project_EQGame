/******************************************************************************
* ����: Y3
* ʱ��: 2005-6-1
* ����: ָʾ��. ����TD
******************************************************************************/
#ifndef __HitIndicator_H__
#define __HitIndicator_H__



class HitIndicator
{
public:
	HitIndicator() {}
	virtual ~HitIndicator() {}

public:
	virtual const std::string& getName(void) const = 0;

public:

	virtual void reshape(void);
	virtual void hide(void);
	virtual void refresh(void);
	void setHitPoint(const Ogre::Vector2& pt);
	void setHitPoint(Real x, Real y);

protected:
	virtual void _setHitPoint(Real x, Real y);
};


#endif // __HitIndicator_H__
