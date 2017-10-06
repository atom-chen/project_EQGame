/******************************************************************************
* 作者: Y3
* 时间: 2010-05-20
* 描述: 来自TD
******************************************************************************/
#ifndef __TerrainTileEditable_H__
#define __TerrainTileEditable_H__

#include "EQMainPrerequisites.h"

#include "TerrainTile.h"


class TerrainData;

class TerrainTileEditable : public TerrainTile
{
protected:
    /// renderable list
    RenderableList mRenderables;

	/// shared vertex data
	std::vector<Ogre::VertexData*> mVertexDatas;

    bool mGeometryOutOfDate;

    void fillLayerBuffer(size_t layer, const Ogre::HardwareVertexBufferSharedPtr& buffer);

	void initBuffers(TerrainData* data, int xbase, int zbase, int xsize, int zsize);
    void freeBuffers(void);

	void createGeometry(TerrainData* data, int xbase, int zbase, int xsize, int zsize);
	void destoryGeometry(void);

public:
    TerrainTileEditable(Ogre::SceneNode* parent, Terrain *creator,
					 int xbase, int zbase, int xsize, int zsize);
    ~TerrainTileEditable();

    const Ogre::MaterialPtr* getOverrideMaterial(size_t layerIndex) const;

    virtual void _notifyHeightModified(int xstart, int zstart, int xend, int zend);
    virtual void _notifyGridInfoModified(int xstart, int zstart, int xend, int zend);

public:
    /** Returns the type of the movable. */
    const Ogre::String& getMovableType(void) const;

    /** Internal method add Renderable subclass instances to the rendering queue. */
    void _updateRenderQueue(Ogre::RenderQueue* queue);

	/// @copydoc MovableObject::visitRenderables
	void visitRenderables(Ogre::Renderable::Visitor* visitor, 
		bool debugRenderables = false);

};



#endif // 
