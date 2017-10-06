#include "EQOctreeSpirit.h"
#include "EQOctreeNode.h"


void EQOctreeSpirit::deattach()
{
	if ( mAtt )
		mAtt->unattatch();
}