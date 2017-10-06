/******************************************************************************
* ����: Y3
* ʱ��: 2005-6-7
* ����: ����TD
******************************************************************************/
#ifndef __SimplePaintAction_H__
#define __SimplePaintAction_H__

#include "PaintAction.h"
#include "Selection.h"


class GridSelection;

class SimplePaintAction : public PaintAction
{
public:
	static SimplePaintAction* create();

public:
	SimplePaintAction();
	~SimplePaintAction();

	const std::string& getName(void) const;

	void setParameter(const std::string& name, const std::string& value);
	std::string getParameter(const std::string& name) const;

protected:
	GridSelection* mCurrentGrids;
	GridSelection* mHintModified;
	GridSelection* mModifiedGrids;

	void _buildHitIndicator(const Ogre::Vector2& pt);
	void _doPaint(GridSelection* modified);

	void _onActive(bool active);

	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

protected:

	typedef std::vector< PaintPixmap > SimplePaintPixmapArray;

	SimplePaintPixmapArray mPixmapArray;

	/// ��ǰҪ��������Ĵ�СΪ64����׼��С��
	bool mbTexNormalSize;
	/// �Ƿ�ʹ���������
	bool mbUseRandomOrientation;

	/// ��ˢ�Ĵ�С����ͨ��ˢ��������64*64�������С����64��������ô�����Ҫͨ��setParameter��������
	/// ��ˢ�Ĵ�С�������
	int mXBrushSize;
	int mZBrushSize;


};


#endif // __SimplePaintAction_H__
