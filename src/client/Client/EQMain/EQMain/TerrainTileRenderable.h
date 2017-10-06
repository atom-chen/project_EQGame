/******************************************************************************
* ����: Y3
* ʱ��: 2010-05-20
* ����: ����TD
******************************************************************************/
#ifndef __TerrainTileRenderable_H__
#define __TerrainTileRenderable_H__

#include "EQMainPrerequisites.h"

#include "BasicRenderable.h"


class TerrainTile;

//////////////////////////////////////////////////////////////////////////

class TerrainTileRenderable : public BasicRenderable
{
public:
    TerrainTileRenderable(TerrainTile *parent);
    ~TerrainTileRenderable();

    /** Overridden - see Renderable. */
    const Ogre::LightList& getLights(void) const;
};


#endif // 
