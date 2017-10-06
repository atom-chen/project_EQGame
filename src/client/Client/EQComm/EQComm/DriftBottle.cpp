#include "EQCommPCH.h"
#include "DriftBottle.h"

//-----------------------------------------------------
DriftBottle::DriftBottle(uint64 bottle_id, std::string acc, int type, std::string content, stime time, std::string imageset, std::string img)
						:mDriftBottle_info(bottle_id,acc,type,content,time)
						,mBottleTexture(imageset, img)
{
	
}
//-----------------------------------------------------
DriftBottle::~DriftBottle()
{

}