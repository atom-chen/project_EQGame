#include "EQCommPCH.h"
#include "NormalDriftBottle.h"

//--------------------------------------------------------------------
NormalDriftBottle::NormalDriftBottle(uint64 bottle_id, std::string acc, int type, std::string content, stime time, std::string imageset, std::string img)
	: DriftBottle(bottle_id,acc,type,content,time,imageset,img)
{

}
//--------------------------------------------------------------------
NormalDriftBottle::~NormalDriftBottle()
{

}