/******************************************************************************
* 作者: Y3
* 时间: 2010-5-31
* 描述: 地表选择. 来自TD
******************************************************************************/
#ifndef __TerrainSelections_H__
#define __TerrainSelections_H__

#include "Selection.h"
#include "Terrain.h"


class TerrainSelection : public Selection
{
protected:

public:
	TerrainSelection() {}

	Terrain* getTerrain(void) const;
	TerrainData* getTerrainData(void) const;
};

class JunctionSelection : public TerrainSelection
{
public:
	struct Junction {
		int x, z;
		Real height;
		Real weight;

		Junction()
		{
		}

		Junction(int x, int z, Real height, Real weight)
			: x(x)
			, z(z)
			, height(height)
			, weight(weight)
		{
		}
	};

	typedef std::map<size_t, Junction> JunctionMap;

protected:
	JunctionMap mJunctions;

public:
	JunctionSelection();
	~JunctionSelection();

	const std::string& getType(void) const;
	bool empty(void) const;
	void reset(void);
	void apply(void);
	void notifyModified(void) const;

public:
	const JunctionMap& getJunctions(void) const
	{
		return mJunctions;
	}

	bool add(int x, int z, Real weight);
	bool remove(int x, int z);
	bool exist(int x, int z) const;
	bool setValue(int x, int z, Real height);
};

class GridSelection : public TerrainSelection
{
public:
	struct Grid {
		int x, z;
		TerrainData::GridInfo info;

		Grid()
		{
		}

		Grid(int x, int z, const TerrainData::GridInfo& info)
			: x(x)
			, z(z)
			, info(info)
		{
		}
	};

	typedef std::map<size_t, Grid> GridMap;

protected:
	GridMap mGrids;

public:
	GridSelection();
	~GridSelection();

	const std::string& getType(void) const;
	bool empty(void) const;
	void reset(void);
	void apply(void);
	void notifyModified(void) const;

public:
	const GridMap& getGrids(void) const
	{
		return mGrids;
	}

	bool add(int x, int z);
	bool remove(int x, int z);
	bool exist(int x, int z) const;
	bool setValue(int x, int z, const TerrainData::GridInfo& info);
};


#endif // __TerrainSelections_H__
