/******************************************************************************
* ����: Y3
* ʱ��: 2005-6-10
* ����: ����TD
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

	/// �Ѳ�ͬ���͵�������ڲ�ͬ��AutoTexGenPixmapArray
	typedef std::vector< PaintPixmap > AutoTexGenPixmapArray;

	/// һ����ˢ�����������һ���������ڲ�ͬ��������
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

	/// �ڲ�ʵ�������ѡ������
	GridSelection* mCurrentInnerGrids;
	/// ��߱�Ե��ѡ������
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

	/// �ж�ָ���������Ƿ��ڵ�ǰ����
	bool isTexInCurrentGroup( ushort id ) ;

	/// ����ָ����id���������������
	TexType findTexTypeByPixmapId( ushort id ); 

	/// ���浱ǰ�༭�������е�������. --�ĳɾ�̬ Y3
	static BrushMap mBrushMap;

	/// ��ǰ���õ�������
	Brush *mCurrentBrush;

	/// ��ǰ�����������
	std::string mCurrentBrushName;

	/// ���ڱ��浱ǰ��ѡ�����������
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

	/// ��ǰ��ѡ��box��������Ͻǣ�������û�г�����Ե��������Ͻǳ����˱�Ե����ô���ֵ�ͻ��Ǹ��ģ�
	int mCurrentBoxSelectionMinX;
	int mCurrentBoxSelectionMinZ;

	/// ����ˢ�ӱ�Ե��selection
	// �������ֲ�������һ��brushshape��ʵ�ֵģ�������Ӱ�쵽��Ĳ�������������ߵȣ������Ծ͸ĳ�
	// һ��������
	// FIX IT �������ֻ����ʱ���ӵģ�ֻ������box�λ�ˢ��Ӧ�޸�Ϊͨ�õģ��������κ���״�Ļ�ˢ
	void buildBoxEdgeSelection(GridSelection& selection, Real xcentre, Real zcentre, int xsize, int zsize);

	/// ��ǰ�������Ƿ��ѳ�ʼ�����
	bool mInitCurrentBrush;
};

#endif // __AutoTexPaintAction_H__
