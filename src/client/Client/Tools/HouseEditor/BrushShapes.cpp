#include "HouseEditorPCH.h"

#include "BrushShapes.h"
#include "TerrainSelections.h"
#include "Action.h"
#include "MfcInputEventMgr.h"
#include "RootManager.h"



const std::string&
BoxBrushShape::getType(void) const
{
    static const std::string type = "BoxBrushShape";
    return type;
}

void
BoxBrushShape::build(Selection& selection, Real xcentre, Real zcentre, int xsize, int zsize)
{
    assert(0 < xsize && 0 < zsize);

    if (selection.getType() == "JunctionSelection")
    {
		Action *activeAction = MfcInputEventMgr::getSingleton().getEvent<Action>();
		// ֻ�е�ǰ�����߻򽵵͵��ε�actionʱ�Ž���Ȩ�صļ���
		bool computeWeight =  ( activeAction && 
			(activeAction->getName() == "RaiseHeightAction" || activeAction->getName() == "LowerHeightAction") );

		int realBrushXSize = 0;
		int realBrushZSize = 0;
		int affectRange = 0;
		int realBrushStartX = 0;
		int realBrushStartZ = 0;
		int realBrushEndX = 0;
		int realBrushEndZ = 0;

		// ����junction������junction����
		float middleX = 0.0f;
		float middleZ = 0.0f;

		float expValue = 0.0f;

		if (computeWeight)
		{
			// ���浱ǰ���õĻ�ˢ��С
			realBrushXSize = xsize;
			realBrushZSize = zsize;

			affectRange = RootManager::getSingleton().getJunctionWeightAffectRange();

			// ������»�ˢ��С��������Ӱ�췶Χ��������
			xsize += affectRange*2;
			zsize += affectRange*2;
		}		

        JunctionSelection& sel = static_cast<JunctionSelection&>(selection);
        sel.reset();
        Real cx = (xsize-1) * sel.getTerrainData()->mScale.x / 2;
        Real cz = (zsize-1) * sel.getTerrainData()->mScale.z / 2;
        Real xbais = xcentre - cx;
        Real zbais = zcentre - cz;
        std::pair<int, int> bais = sel.getTerrainData()->getJunction(xbais, zbais);

		if (computeWeight)
		{
			realBrushStartX = (xsize-realBrushXSize) / 2;
			realBrushStartZ = (zsize-realBrushZSize) / 2;

			realBrushEndX = realBrushStartX + realBrushXSize - 1;
			realBrushEndZ = realBrushStartZ + realBrushZSize - 1;

			expValue = RootManager::getSingleton().getJunctionWeightExpValue();

			middleX = (float)(realBrushEndX-realBrushStartX) / 2.0f + realBrushStartX;
			middleZ = (float)(realBrushEndZ-realBrushStartZ) / 2.0f + realBrushStartZ;
		}

        for (int z = 0; z < zsize; ++z)
        {
            for (int x = 0; x < xsize; ++x)
            {
				if (computeWeight)
				{
					const float E = 2.718282f;

					Real weight = 1.0f;

					// ���㲻��Ӱ�췶Χ�ڵ�junction��Ȩ��
					if ( x < realBrushStartX || x > realBrushEndX ||
						z < realBrushStartZ || z > realBrushEndZ )
					{
						// �������junction�����ĵ�֮��ľ���
						float dis = Ogre::Math::Sqrt( (x-middleX)*(x-middleX) + (z-middleZ)*(z-middleZ) );

						// ���õļ��㹫ʽΪ weight = e��(-kd)�η���kԽ��Ȩ���ܾ����Ӱ���Խ��kһ��Ϊ������
						weight = 1.0f / Ogre::Math::Pow(E, expValue*dis);

						assert (weight > 0 && weight <= 1);
					}

					sel.add(x+bais.first, z+bais.second, weight);
				}
				else
				{
					sel.add(x+bais.first, z+bais.second, 1.0f);
				}
				
            }
        }
    }
    else if (selection.getType() == "GridSelection")
    {
        GridSelection& sel = static_cast<GridSelection&>(selection);
        sel.reset();
        Real cx = (xsize-1) * sel.getTerrainData()->mScale.x / 2;
        Real cz = (zsize-1) * sel.getTerrainData()->mScale.z / 2;
        Real xbais = xcentre - cx;
        Real zbais = zcentre - cz;
        std::pair<int, int> bais = sel.getTerrainData()->getGrid(xbais, zbais);
        for (int z = 0; z < zsize; ++z)
        {
            for (int x = 0; x < xsize; ++x)
            {
                sel.add(x+bais.first, z+bais.second);
            }
        }
    }
    else
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
            "Doesn't support selection of type '" + selection.getType() + "'.",
            "BoxBrushShape::build");
    }
}

//////////////////////////////////////////////////////////////////////////


const std::string&
EllipseBrushShape::getType(void) const
{
    static const std::string type = "EllipseBrushShape";
    return type;
}

void
EllipseBrushShape::build(Selection& selection, Real xcentre, Real zcentre, int xsize, int zsize)
{
    assert(0 < xsize && 0 < zsize);
    // a point is in an ellipse if:
    // (ax)^2 + (by)^2 <= 1
    // where a = 1/halfEllipseWidth and b = 1/halfEllipseHeight

    if (selection.getType() == "JunctionSelection")
    {
		Action *activeAction = MfcInputEventMgr::getSingleton().getEvent<Action>();

		// ֻ�е�ǰ�����߻򽵵͵��ε�actionʱ�Ž���Ȩ�صļ���
		bool computeWeight =  ( activeAction && 
			(activeAction->getName() == "RaiseHeightAction" || activeAction->getName() == "LowerHeightAction") );

		int realBrushXSize = 0;
		int realBrushZSize = 0;
		int affectRange = 0;
		int realBrushStartX = 0;
		int realBrushStartZ = 0;
		int realBrushEndX = 0;
		int realBrushEndZ = 0;
		// ����junction������junction����
		float middleX = 0.0f;
		float middleZ = 0.0f;


		float expValue = 0.0f;

		if (computeWeight)
		{
			// ���浱ǰ���õĻ�ˢ��С
			realBrushXSize = xsize;
			realBrushZSize = zsize;

			affectRange = RootManager::getSingleton().getJunctionWeightAffectRange();

			// ������»�ˢ��С��������Ӱ�췶Χ��������
			xsize += affectRange*2;
			zsize += affectRange*2;
		}		

        JunctionSelection& sel = static_cast<JunctionSelection&>(selection);
        sel.reset();

		// ������Բ�İ뾶
		float halfEllipseWidth = (float)(xsize-1) / 2;
		float halfEllipseHeight = (float)(zsize-1) / 2;

        Real a = 1 / halfEllipseWidth;
        Real b = 1 / halfEllipseHeight;

        Real cx = (xsize-1) * sel.getTerrainData()->mScale.x / 2;
        Real cz = (zsize-1) * sel.getTerrainData()->mScale.z / 2;
        Real xbais = xcentre - cx;
        Real zbais = zcentre - cz;
        std::pair<int, int> bais = sel.getTerrainData()->getJunction(xbais, zbais);

		if (computeWeight)
		{
			realBrushStartX = (xsize-realBrushXSize) / 2;
			realBrushStartZ = (zsize-realBrushZSize) / 2;

			realBrushEndX = realBrushStartX + realBrushXSize - 1;
			realBrushEndZ = realBrushStartZ + realBrushZSize - 1;

			expValue = RootManager::getSingleton().getJunctionWeightExpValue();
			
			// ����junction������junction����
			middleX = (float)(realBrushEndX-realBrushStartX) / 2.0f + realBrushStartX;
			middleZ = (float)(realBrushEndZ-realBrushStartZ) / 2.0f + realBrushStartZ;
		}

        for (int z = 0; z < zsize; ++z)
        {
            for (int x = 0; x < xsize; ++x)
            {
                float xp = x - halfEllipseWidth;
                float zp = z - halfEllipseHeight;
                Real factor = (a * a * xp * xp) + (b * b * zp * zp);
                if (factor > 1)
                    continue;

				if (computeWeight)
				{
					const float E = 2.718282f;

					Real weight = 1.0f;

					// ���㲻��Ӱ�췶Χ�ڵ�junction��Ȩ��
					if ( x < realBrushStartX || x > realBrushEndX ||
						z < realBrushStartZ || z > realBrushEndZ )
					{					
						// �������junction�����ĵ�֮��ľ���
						float dis = Ogre::Math::Sqrt( (x-middleX)*(x-middleX) + (z-middleZ)*(z-middleZ) );

						// ���õļ��㹫ʽΪ weight = e��(-kd)�η���kԽ��Ȩ���ܾ����Ӱ���Խ��kһ��Ϊ������
						weight = 1.0f / Ogre::Math::Pow(E, expValue*dis);

						assert (weight > 0 && weight <= 1);
					}

					sel.add(x+bais.first, z+bais.second, weight);
				}
				else
				{
					sel.add(x+bais.first, z+bais.second, 1.0f);
				}
            }
        }
    }
    else if (selection.getType() == "GridSelection")
    {
        GridSelection& sel = static_cast<GridSelection&>(selection);
        sel.reset();

		// ������Բ�İ뾶
		float halfEllipseWidth = (float)(xsize-1) / 2;
		float halfEllipseHeight = (float)(zsize-1) / 2;

        Real a = 1 / halfEllipseWidth;
        Real b = 1 / halfEllipseHeight;
        Real cx = (xsize-1) * sel.getTerrainData()->mScale.x / 2;
        Real cz = (zsize-1) * sel.getTerrainData()->mScale.z / 2;
        Real xbais = xcentre - cx;
        Real zbais = zcentre - cz;
        std::pair<int, int> bais = sel.getTerrainData()->getGrid(xbais, zbais);
        for (int z = 0; z < zsize; ++z)
        {
            for (int x = 0; x < xsize; ++x)
            {
                Real xp = x - halfEllipseWidth;
                Real zp = z - halfEllipseHeight;
                Real factor = (a * a * xp * xp) + (b * b * zp * zp);
                if (factor > 1)
                    continue;

                sel.add(x+bais.first, z+bais.second);
            }
        }
    }
    else
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
            "Doesn't support selection of type '" + selection.getType() + "'.",
            "EllipseBrushShape::build");
    }
}


