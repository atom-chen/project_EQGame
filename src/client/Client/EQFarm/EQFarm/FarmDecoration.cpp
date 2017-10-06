#include "EQFarmPCH.h"
#include "FarmDecoration.h"
#include "NetPacket.h"
#include "FarmScene.h"
#include "FarmTable.h"
#include "GraphicsTable.h"
#include "SceneManager.h"
#include "GameMgr.h"
#include "EQCamera.h"
#include "FunnyFarm.h"
//------------------------------------------------------------------------------
FarmDecoration::FarmDecoration(void)
:mFarmScene(NULL)
,mServerData(NULL)
,mOldServerData(NULL)
{
}
//------------------------------------------------------------------------------
FarmDecoration::~FarmDecoration(void)
{
	if (mServerData)
	{
		delete mServerData;
		mServerData = NULL;
	}
	if (mOldServerData)
	{
		delete mOldServerData;
		mOldServerData = NULL;
	}
}

//------------------------------------------------------------------------------
const ModelDisplayRow* FarmDecoration::findModelDisplay(int itemid,int index)
{
	const FarmDecorationRow *row = sFarmTableMgr.getFarmDecorationRow(itemid);

	if (row == NULL)
		return NULL;

	if ((uint)index < row->static_model.size())
	{
		return GraphicsTableManager::getSingleton().getModelDisplayRow(row->static_model[index]);
	}
	else
		return NULL;

}
//------------------------------------------------------------------------------
void FarmDecoration::updateObjects()
{
	std::string mesh,mat;
	int itemid;
	const ModelDisplayRow* modelrow;
	char buff[64] = "/0";
	int size=0;

	itemid = mServerData->background.template_id;
	const FarmDecorationRow *row = sFarmTableMgr.getFarmDecorationRow(itemid);
	if (row != NULL)
	{
		changeScene(row->scene_name);
		/*sprintf(buff,"background_%d",i+1);
		mesh = modelrow->mesh;
		mat = modelrow->mat;
		mFarmScene->changeFarmObject(buff,mesh,mat);*/
	}

	itemid = mServerData->fence.template_id;
	if (itemid != mOldServerData->fence.template_id)
	{
		row = sFarmTableMgr.getFarmDecorationRow(mOldServerData->fence.template_id);
		if (row != NULL)
		{
			size = row->static_model.size();

			for (int i=0; i< size;i++)
			{
				modelrow = GraphicsTableManager::getSingleton().getModelDisplayRow(row->static_model[i]);
				if (modelrow)
				{
					sprintf(buff,"fence_%d",i+1);
					mFarmScene->removeFarmObject(buff);
				}
			}
		}

		row = sFarmTableMgr.getFarmDecorationRow(itemid);
		if (row != NULL)
		{
			size = row->static_model.size();

			for (int i=0; i< size;i++)
			{
				modelrow = GraphicsTableManager::getSingleton().getModelDisplayRow(row->static_model[i]);
				if (modelrow)
				{
					sprintf(buff,"fence_%d",i+1);
					mesh = modelrow->mesh;
					mat = modelrow->mat;
					mFarmScene->changeFarmObject(buff,mesh,mat);
				}
			}
		}
	}

	//itemid = mServerData->fence.template_id;
	//modelrow = findModelDisplay(itemid);
	//if (modelrow)
	//{
	//	mesh = modelrow->mesh;
	//	mat = modelrow->mat;
	//	mFarmScene->changeFarmObject("fence",mesh,mat);
	//}

	//itemid = mServerData->background.template_id;
	//modelrow = findModelDisplay(itemid);
	//if (modelrow)
	//{
	//	mesh = modelrow->mesh;
	//	mat = modelrow->mat;
	//	mFarmScene->changeFarmObject("background",mesh,mat);
	//}

	itemid = mServerData->house.template_id;

	if (itemid != mOldServerData->house.template_id)
	{
		modelrow = findModelDisplay(mOldServerData->house.template_id);
		if (modelrow)
		{
			mFarmScene->removeFarmObject("house");
		}

		modelrow = findModelDisplay(itemid);
		if (modelrow)
		{
			mesh = modelrow->mesh;
			mat = modelrow->mat;
			mFarmScene->changeFarmObject("house",mesh,mat);
		}
	}

	itemid = mServerData->doghouse.template_id;
	if (itemid != mOldServerData->doghouse.template_id)
	{
		modelrow = findModelDisplay(mOldServerData->doghouse.template_id);
		if (modelrow)
		{
			mFarmScene->removeFarmObject("doghouse");
		}

		modelrow = findModelDisplay(itemid);
		if (modelrow)
		{
			mesh = modelrow->mesh;
			mat = modelrow->mat;
			mFarmScene->changeFarmObject("doghouse",mesh,mat);
		}
	}

	itemid = mServerData->dog.template_id;
	if (itemid != mOldServerData->dog.template_id)
	{
		modelrow = findModelDisplay(mOldServerData->dog.template_id);
		if (modelrow)
		{
			mFarmScene->removeFarmObject("dog");
			mFarmScene->removeFarmObject("board");
		}

		modelrow = findModelDisplay(itemid);
		if (modelrow)
		{
			mesh = modelrow->mesh;
			mat = modelrow->mat;
			mFarmScene->changeFarmObject("dog",mesh,mat);
			if (0 > sFunnyFarm.getCommDogFoodRemainSecs())
			{
				modelrow = findModelDisplay(85000);
				if (modelrow)
				{
					mesh = modelrow->mesh;
					mat = modelrow->mat;
					mFarmScene->changeFarmObject("board",mesh,mat);
				}
			}
			else
			{
				mFarmScene->removeFarmObject("board");
			}
		}
	}
	else
	{
		if (0 > sFunnyFarm.getCommDogFoodRemainSecs())
		{
			if(itemid != 0)
			{
				modelrow = findModelDisplay(85000);
				if (modelrow)
				{
					mesh = modelrow->mesh;
					mat = modelrow->mat;
					mFarmScene->changeFarmObject("board",mesh,mat);
				}
			}
		}
		else
		{
			mFarmScene->removeFarmObject("board");
		}
	}

	itemid = mServerData->signpost.template_id;
	if (itemid != mOldServerData->signpost.template_id)
	{
		modelrow = findModelDisplay(mOldServerData->signpost.template_id);
		if (modelrow)
		{
			mFarmScene->removeFarmObject("signpost");
		}
			
		modelrow = findModelDisplay(itemid);
		if (modelrow)
		{
			mesh = modelrow->mesh;
			mat = modelrow->mat;
			mFarmScene->changeFarmObject("signpost",mesh,mat);
		}
	}

	itemid = mServerData->scarecrow.template_id;
	if (itemid != mOldServerData->scarecrow.template_id)
	{
		modelrow = findModelDisplay(mOldServerData->scarecrow.template_id);
		if (modelrow)
		{
			mFarmScene->removeFarmObject("scarecrow");
		}
		
		modelrow = findModelDisplay(itemid);
		if (modelrow)
		{
			mesh = modelrow->mesh;
			mat = modelrow->mat;
			mFarmScene->changeFarmObject("scarecrow",mesh,mat);
		}
	}

	itemid = mServerData->billboard.template_id;
	if (itemid != mOldServerData->billboard.template_id)
	{
		modelrow = findModelDisplay(mOldServerData->billboard.template_id);
		if (modelrow)
		{
			mFarmScene->removeFarmObject("billboard");
		}
		modelrow = findModelDisplay(itemid);
		if (modelrow)
		{
			mesh = modelrow->mesh;
			mat = modelrow->mat;
			mFarmScene->changeFarmObject("billboard",mesh,mat);
		}
	}
}
//------------------------------------------------------------------------------
void FarmDecoration::setServerData( farm_decoration* fd )
{
	if (NULL == mServerData)
	{
		mServerData = new farm_decoration;
		mOldServerData = new farm_decoration;
	}
	memcpy(mOldServerData,mServerData,sizeof(farm_decoration));
	memcpy(mServerData,fd,sizeof(farm_decoration));
}
//------------------------------------------------------------------------------
void FarmDecoration::setFarmScene( FarmScene* fs )
{
	mFarmScene = fs;
}

void FarmDecoration::changeScene(const std::string& sceneName,bool forceReload)
{
	if (!forceReload &&
		strcmp(sceneName.c_str(),mSceneName.c_str()) == 0)
	{
		return;
	}
	EQSceneManager::getSingleton().loadHouseAndScene(sceneName, GameMgr::getSingleton().getMainCamera()->getOgreCam());
	mSceneName = sceneName;
}