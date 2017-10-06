#include "EQMainPCH.h"

#include "HSCSheetShadow.h"
#include "SystemGcs.h"
#include "HouseSceneDef.h"


static const std::string HSCSheetShadowMaterialName = "SystemMat/SheetShadow";

//-----------------------------------------------------------------------
Ogre::ManualObject* HSCSheetShadow::create(const Int2AABBBox& box)
{
	int x,y;
	box.getSize(x, y);
	assert( x != 0 && y != 0 );

	float minx = HOUSE_GRID_SIZE * box.mXmin - HOUSE_GRID_SIZE/2;
	float miny = HOUSE_GRID_SIZE * box.mYmin - HOUSE_GRID_SIZE/2;
	float maxx = HOUSE_GRID_SIZE * box.mXmax + HOUSE_GRID_SIZE/2;
	float maxy = HOUSE_GRID_SIZE * box.mYmax + HOUSE_GRID_SIZE/2;

	//
	static int num = 0;
	++num;

	std::string name = "--HSCSheetShadow--[auto]--" + Ogre::StringConverter::toString(num) + "--";

	Ogre::ManualObject* manual = SystemGcs::getSingleton().getSceneManager()->createManualObject(name);

	manual->begin(HSCSheetShadowMaterialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	manual->position(minx,	0, miny);
	manual->textureCoord(0, 1);
	manual->position(minx,	0, maxy);
	manual->textureCoord(0, 0);
	manual->position(maxx,	0, maxy);
	manual->textureCoord(1, 0);
	manual->position(maxx,	0, miny);
	manual->textureCoord(1, 1);

	manual->quad(0,1,2,3);

	manual->end();

	return manual;
}
