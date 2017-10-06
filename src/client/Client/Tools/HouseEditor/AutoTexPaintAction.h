/******************************************************************************
* 作者: Y3
* 时间: 2005-6-10
* 描述: 来自TD
******************************************************************************/
#ifndef __AutoTexPaintAction_H__
#define __AutoTexPaintAction_H__

#include "PaintAction.h"
#include "TerrainPaintInfoContainer.h"


class GridSelection;

class AutoTexPaintAction : public PaintAction
{
public:
	static AutoTexPaintAction* create();
	static void clearBrushMap();

public:

	AutoTexPaintAction();
	~AutoTexPaintAction();

	const std::string& getName(void) const;

	void setParameter(const std::string& name, const std::string& value);
	std::string getParameter(const std::string& name) const;


protected:        

	/// 把不同类型的纹理放在不同的AutoTexGenPixmapArray
	typedef std::vector< PaintPixmap > AutoTexGenPixmapArray;

	/// 一个画刷，里面包含了一组纹理，放在不同的数组中
	struct Brush
	{
		AutoTexGenPixmapArray edgeTexArray;
		AutoTexGenPixmapArray lEdgeTexArray;
		AutoTexGenPixmapArray rEdgeTexArray;
		AutoTexGenPixmapArray tEdgeTexArray;
		AutoTexGenPixmapArray bEdgeTexArray;

		AutoTexGenPixmapArray innerCornerTexArray;
		AutoTexGenPixmapArray ltInnerCornerTexArray;
		AutoTexGenPixmapArray rtInnerCornerTexArray;
		AutoTexGenPixmapArray lbInnerCornerTexArray;
		AutoTexGenPixmapArray rbInnerCornerTexArray;

		AutoTexGenPixmapArray outerCornerTexArray;
		AutoTexGenPixmapArray ltOuterCornerTexArray;
		AutoTexGenPixmapArray rtOuterCornerTexArray;
		AutoTexGenPixmapArray lbOuterCornerTexArray;
		AutoTexGenPixmapArray rbOuterCornerTexArray;

		AutoTexGenPixmapArray innerTexArray;
		AutoTexGenPixmapArray duijiaoTexArray;
	};

	/// group brush map
	typedef std::map< std::string, Brush * > BrushMap;

	/// 内部实心纹理的选择区域
	GridSelection* mCurrentInnerGrids;
	/// 外边边缘的选择区域
	GridSelection* mCurrentOuterGrids;

	GridSelection* mHintModified;
	GridSelection* mModifiedGrids;

	void _buildHitIndicator(const Ogre::Vector2& pt);
	void _doPaint(GridSelection* modified);

	void _onActive(bool active);

	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

	/// 判断指定的纹理是否在当前组中
	bool isTexInCurrentGroup( ushort id ) ;

	/// 根据指定的id，返回纹理的类型
	TexType findTexTypeByPixmapId( ushort id ); 

	/// 保存当前编辑器中所有的纹理组. --改成静态 Y3
	static BrushMap mBrushMap;

	/// 当前所用的纹理组
	Brush *mCurrentBrush;

	/// 当前纹理组的名称
	std::string mCurrentBrushName;

	/// 用于保存当前所选组的纹理数组
	AutoTexGenPixmapArray *mCurrentInnerArray;

	AutoTexGenPixmapArray *mCurrentInnerCornerArray;
	AutoTexGenPixmapArray *mCurrentLTInnerCornerArray;
	AutoTexGenPixmapArray *mCurrentRTInnerCornerArray;
	AutoTexGenPixmapArray *mCurrentLBInnerCornerArray;
	AutoTexGenPixmapArray *mCurrentRBInnerCornerArray;

	AutoTexGenPixmapArray *mCurrentOuterCornerArray;
	AutoTexGenPixmapArray *mCurrentLTOuterCornerArray;
	AutoTexGenPixmapArray *mCurrentRTOuterCornerArray;
	AutoTexGenPixmapArray *mCurrentLBOuterCornerArray;
	AutoTexGenPixmapArray *mCurrentRBOuterCornerArray;

	AutoTexGenPixmapArray *mCurrentEdgeArray;
	AutoTexGenPixmapArray *mCurrentLEdgeArray;
	AutoTexGenPixmapArray *mCurrentREdgeArray;
	AutoTexGenPixmapArray *mCurrentTEdgeArray;
	AutoTexGenPixmapArray *mCurrentBEdgeArray;

	AutoTexGenPixmapArray *mCurrentDuijiaoArray;

	/// 当前所选的box区域的左上角（不管有没有超过边缘，如果左上角超过了边缘，那么这个值就会是负的）
	int mCurrentBoxSelectionMinX;
	int mCurrentBoxSelectionMinZ;

	/// 构建刷子边缘的selection
	// 本来这种操作是用一个brushshape来实现的，不过会影响到别的操作（如地形升高等），所以就改成
	// 一个函数了
	// FIX IT 这个函数只是临时增加的，只适用于box形画刷，应修改为通用的，适用于任何形状的画刷
	void buildBoxEdgeSelection(GridSelection& selection, Real xcentre, Real zcentre, int xsize, int zsize);

	/// 当前纹理组是否已初始化完毕
	bool mInitCurrentBrush;
};

#endif // __AutoTexPaintAction_H__
