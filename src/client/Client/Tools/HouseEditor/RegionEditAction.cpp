#include "HouseEditorPCH.h"

#include "RegionEditAction.h"
#include "GraphicsSystem.h"
#include "RootManager.h"
#include "IntersectPoint.h"
#include "SystemGcs.h"
#include "SceneManager.h"
#include "OperatorManager.h"
#include "RegionEditOperator.h"
#include "SceneManager.h"
#include "TerrainData.h"
#include "SerializerUtil.h"

RegionEditManager::RegionEditManager()
: mListener(NULL)
,m_pSceneNode(NULL)
, m_ulCurMaxId(0)
, m_ulCurEditRegionId(0)
, m_ulCurEditPointId(0)
, m_ulCurEditLineId(0)

{
	// 初始化
	InitRegionEdit();

	m_bIsShow = false;
	m_bShowDeepBuf = true;
}

RegionEditManager::~RegionEditManager()
{
	ClearRegionData();

	if (m_pSceneNode)
	{
		m_pSceneNode->getParentSceneNode()->removeAndDestroyChild(m_pSceneNode->getName());
	}
}

unsigned long RegionEditManager::ReadRegionFromFileVer0(std::string strFileName)
{
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(strFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	DataStreamReader reader(stream);

	//FILE* pFile = fopen( strFileName.c_str(), "rb" );

	//if(NULL == pFile)
	//{
	//	// 不能打开文件.
	//	return 0xfffffffe;
	//}

	int			iVerLen = (int)sizeof(REGION_VER0);
	char*		pVer = new char[iVerLen];

	reader.readData(pVer, iVerLen-1);
	//fread( pVer, 1, iVerLen-1, pFile );
	pVer[iVerLen-1] = 0;
	if( strcmp( pVer, REGION_VER0 ) != 0 )
	{
		delete [] pVer;
		//fclose( pFile );
		return 0xfffffffe;
	}

	delete [] pVer;

	// 清空以前的数据.
	ClearRegionData();

	int			iRegionCount = 0;
	int			iPointCount  = 0;
	int         iId			 = 0;
	int         iFlag        = 0;
	unsigned long ulId       = 0;
	CUnreachRegion* pRegion  = NULL;
	Ogre::Vector3   position;
	uint		utmp;


	////////////////////////////////////////////////////////////////////
	//
	//  坐标转换参数
	//

	//SaveX
	float x = 0;
	//SaveZ
	float z = 0;

	// 缩放系数
	float fTerrainDataPosX = 0;
	float fTerrainDataPosZ = 0;


	// 读取区域的个数
	reader.readInts(&utmp);
	iRegionCount = utmp;
	//::fread(&iRegionCount, sizeof(iRegionCount), 1, pFile);

	for(int i = 0; i < iRegionCount; i++)
	{
		// 读取区域的id
		reader.readInts(&utmp);
		iId = utmp;
		//::fread(&iId, sizeof(iId), 1, pFile);

		// 设置当前正在编辑的id。
		m_ulCurEditRegionId = AddNewRegion();
		CUnreachRegion* pRegion = NULL;
		pRegion = m_UnreachRegionMap[m_ulCurEditRegionId];

		// 读取点的个数.
		reader.readInts(&utmp);
		iPointCount = utmp;
		//::fread(&iPointCount, sizeof(iPointCount), 1, pFile);
		for(int k = 0; k < iPointCount; k++)
		{
			// 读取x， z 数据
			reader.readFloats(&position.x);
			reader.readFloats(&position.z);
			//::fread(&position.x, sizeof(position.x), 1, pFile);
			//::fread(&position.z, sizeof(position.z), 1, pFile);
			//::fread(&position.y, sizeof(position.y), 1, pFile);

			TerrainData* terr = EQSceneManager::getSingleton().getTerrainData();
			if ( terr->isEmpty() )
				position.y = 0;
			else
				position.y = terr->getHeightAt(position.x, position.z);

			AddNewPoint(position);
		}

		//if( i != 133)
		if ( IsRegionValidate(m_ulCurEditRegionId) )
		{
			// 刷新新的区域
			ReshapeArea(m_ulCurEditRegionId);
		}

	}

	//::fclose(pFile);

	// 根据显示状态， 显示场景.
	HideScene(m_bIsShow);

	return 0;
}

unsigned long RegionEditManager::SaveRegionToFileVer0(std::string strFileName)
{
	FILE* pFile = NULL;
	pFile = ::fopen(strFileName.data(), "wb");

	if(NULL == pFile)
	{
		// 不能打开文件.
		return 0xfffffffe;
	}

	fwrite( REGION_VER0, 1, strlen(REGION_VER0), pFile );

	int			iRegionCount = 0;
	int			iPointCount  = 0;
	int         iId			 = 0;
	unsigned long ulId       = 0;

	CUnreachRegion* pRegion  = NULL;
	CPointInfo*		pPoint	 = NULL;
	Ogre::Vector3   position;

	UNREACHREGIONMAP::iterator itRegion;
	UNREACHREGIONMAP::iterator itEndRegion;

	POINT_MAP::iterator itPoint;
	POINT_MAP::iterator itEndPoint;

	// 存储区域的个数
	iRegionCount = m_UnreachRegionMap.size();
	::fwrite(&iRegionCount, sizeof(iRegionCount), 1, pFile);

	// 存储每个区域的信息
	itEndRegion = m_UnreachRegionMap.end();

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	// 保存把场景坐标转换成游戏逻辑坐标.
	//

	//SaveX
	float x = 0;
	//SaveZ
	float z = 0;



	for(itRegion = m_UnreachRegionMap.begin(); itRegion != itEndRegion; itRegion++)
	{
		pRegion = itRegion->second;
		if(pRegion)
		{
			// 存储区域的id
			iId = pRegion->m_ulId;
			::fwrite(&iId, sizeof(iId), 1, pFile);

			// 存储点的个数.
			iPointCount = pRegion->m_pointMap.size();
			::fwrite(&iPointCount, sizeof(iPointCount), 1, pFile);

			// 存储每个点的信息。
			itEndPoint = pRegion->m_pointMap.end();
			for(itPoint = pRegion->m_pointMap.begin(); itPoint != itEndPoint; itPoint++)
			{
				pPoint = itPoint->second;
				if(pPoint)
				{
					position  = pPoint->m_Position;
					x = position.x;
					z = position.z;

					::fwrite(&x, sizeof(x), 1, pFile);
					::fwrite(&z, sizeof(z), 1, pFile);
					//::fwrite(&position.y, sizeof(position.y), 1, pFile);
				}
			}
		}
	}

	::fclose(pFile);

	return 0;
}

unsigned long RegionEditManager::ReadRegionFromFile(std::string strFileName)
{
	if ( !Ogre::ResourceGroupManager::getSingleton().resourceExists( Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME , strFileName) )
		return 0xfffffffe;

	return ReadRegionFromFileVer0(strFileName);
}

unsigned long RegionEditManager::SaveRegionToFile(std::string strFileName)
{
	if ( m_UnreachRegionMap.empty() )
		return 0xfffffffe;

	return SaveRegionToFileVer0( strFileName );
}

////////////////////////////////////////////////////////////////////////////////////
//
// 不可行走区域管理函数
//

// 添加一个新的区域
unsigned long RegionEditManager::AddNewRegion()
{
	m_ulCurMaxId++;
	unsigned long ulId = m_ulCurMaxId;

	CUnreachRegion* pRegion = NULL;
	pRegion = new CUnreachRegion(ulId, m_pSceneNode);


	m_UnreachRegionMap[ulId] = pRegion;
	m_ulCurEditRegionId = ulId;
	return m_ulCurEditRegionId;
}

// 通过指定的id， 删除一个区域
unsigned long RegionEditManager::DelRegion(unsigned long ulId)
{

	CUnreachRegion* pRegion = NULL;
	if(m_UnreachRegionMap.count(ulId))
	{
		pRegion = m_UnreachRegionMap[ulId];
		if(pRegion)
		{
			if(m_pSceneNode)
			{
				pRegion->DetachAllEntity();

				//pRegion->m_pSceneNode->removeAndDestroyAllChildren();
				pRegion->DestroyAllChildSceneNode();
				m_pSceneNode->removeAndDestroyChild(pRegion->m_pSceneNode->getName());
				//

			}

			delete pRegion;

			UNREACHREGIONMAP::iterator it;
			it = m_UnreachRegionMap.find(ulId);
			m_UnreachRegionMap.erase(it);

		}

		return 0;
	}

	return 0;
}

void RegionEditManager::ClearRegionData()
{
	UNREACHREGIONMAP::iterator it;
	UNREACHREGIONMAP::iterator itEnd;

	CUnreachRegion* pRegion = NULL;
	itEnd = m_UnreachRegionMap.end();
	for(it = m_UnreachRegionMap.begin(); it != itEnd; it++)
	{
		pRegion = it->second;
		if(pRegion)
		{
			// 如果场景节点存在
			if(m_pSceneNode)
			{
				// 删除这个区域的所有实体.
				pRegion->DetachAllEntity();
				//pRegion->m_pSceneNode->removeAndDestroyAllChildren();

				// 删除区域的所有子节点
				pRegion->DestroyAllChildSceneNode();

				// 删除场景节点中区域节点.
				m_pSceneNode->removeAndDestroyChild(pRegion->m_pSceneNode->getName());
				//

			}

			delete pRegion;
		}
	}

	m_UnreachRegionMap.clear();

	m_ulCurMaxId = 0;

}



// 根据指定的id， 得到区域
CUnreachRegion* RegionEditManager::GetRegionById(unsigned long ulId)
{

	if(m_UnreachRegionMap.count(ulId))
	{
		CUnreachRegion* pRegion = NULL;
		pRegion = m_UnreachRegionMap[ulId];

		return pRegion;
	}

	return NULL;
}

// 选择一个区域
void RegionEditManager::SelRegion(unsigned long ulId, bool bIsChangeCamera)
{
	if(m_UnreachRegionMap.count(ulId))
	{
		m_ulCurEditRegionId = ulId;
		CUnreachRegion* pRegion = NULL;
		pRegion = m_UnreachRegionMap[ulId];
		pRegion->SetRegionSelMaterial();

		if(bIsChangeCamera)
		{
			if((pRegion->m_pLineShape)&&(pRegion->m_pLineShape->m_pointArray.size()))
			{

				Ogre::Vector3 point = pRegion->m_pLineShape->m_pointArray[0];
				SetCamera(point.x, point.z);

			}
		}

	}
}

// 不选择一个区域.
void RegionEditManager::UnSelRegion(unsigned long ulId)
{
	if(m_UnreachRegionMap.count(ulId))
	{
		CUnreachRegion* pRegion = NULL;
		pRegion = m_UnreachRegionMap[ulId];
		pRegion->SetRegionNorMaterial();


	}
}



void RegionEditManager::SetCamera(float fx, float fy)
{

	const Ogre::Vector3 OldPos = GraphicsSystem::getSingleton().getCamera()->getPosition();
	Ogre::Vector3 NewPos = OldPos;
	//Ogre::Vector3 dir(0, 1.0f, 0);
	NewPos.x = fx;
	NewPos.z = fy;
	GraphicsSystem::getSingleton().getCamera()->setPosition(NewPos);
	//getSceneManipulator()->getCamera()->setDirection(dir);

}

// 隐藏一个区域的面积
void RegionEditManager::HideRegionArea(unsigned long ulRegionId, bool bHide)
{
	CUnreachRegion* pRegion = NULL;

	// 得到当前正在编辑的区域的信息.
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{

		pRegion->HideAreaNode(bHide);

	}
}

// 当前的区域是否合法
bool RegionEditManager::IsRegionValidate(unsigned long ulRegionId)
{
	CUnreachRegion* pRegion = NULL;

	// 得到当前正在编辑的区域的信息.
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{
		if(pRegion->IsRegionValidate())
		{
			return true;
		}
	}

	return false;
}

// 重新创建显示的面积区域
void RegionEditManager::ReshapeArea(unsigned long ulRegionId)
{
	CUnreachRegion* pRegion = NULL;
	// 得到当前正在编辑的区域的信息.
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{	

		pRegion->ReShapeArea();
		pRegion->SetRegionNorMaterial();

	}
}

// 在当前编辑的区域中添加一个新的点
unsigned long RegionEditManager::AddNewPoint(Ogre::Vector3 position)
{
	if(0 == m_UnreachRegionMap.count(m_ulCurEditRegionId))
	{
		return 0 ;
	}

	CUnreachRegion* pRegion = NULL;

	pRegion = m_UnreachRegionMap[m_ulCurEditRegionId];
	if(pRegion)
	{

		m_ulCurEditPointId = pRegion->AddNewPoint(position);
		return m_ulCurEditPointId;
	}

	return 0;

}

// 指定区域id， 指定点的id， 指定位置， 添加一个小球.
void RegionEditManager::AddNewPoint(unsigned long ulRegionId, unsigned long ulPointId, Ogre::Vector3 position)
{
	// 得到当前正在编辑的区域的信息.
	CUnreachRegion* pRegion = NULL;
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{
		pRegion->AddNewPoint(ulPointId, position);

		if ( mListener )
			mListener->onRegionAddNewPoint(ulRegionId, ulPointId, position);
	}
}

// 指定区域id， 指定点的id， 指定位置， 删除一个小球.
void RegionEditManager::DelPoint(unsigned long ulRegionId, unsigned long ulPointId)
{
	// 得到当前正在编辑的区域的信息.
	CUnreachRegion* pRegion = NULL;
	CPointInfo*     pPoint  = NULL;

	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{
		pRegion->DelPointFromTail(ulPointId);

		if ( mListener )
			mListener->onRegionDelPoint(ulRegionId, ulPointId);
	}
}

bool RegionEditManager::EditPoint(const Ogre::Vector2& pt)
{
	Ogre::Vector3 position;
	unsigned long ulId = 0;

	bool hit = RootManager::getSingleton().getTerrainIntersects(pt.x, pt.y, position);
	if(hit)
	{
		ulId = AddNewPoint(position);
		if(0 == ulId)
		{
			return false;
		}

		m_preDoPointPos = position;
		m_curPointPos   = position;

		if ( mListener )
			mListener->onRegionAddNewPoint(m_ulCurEditRegionId, ulId, position);
		return true;
	}

	return false;
}

void RegionEditManager::SelPoint(unsigned long ulPointId)
{
	m_ulCurEditPointId = ulPointId;

	CUnreachRegion* pRegion = NULL;
	CPointInfo*     pPoint  = NULL;

	// 得到当前正在编辑的区域的信息.
	pRegion = GetRegionById(m_ulCurEditRegionId);
	if(pRegion)
	{
		pRegion->SetPrePointMaterial();
		pRegion->m_ulCurEditPointId = m_ulCurEditPointId;
		pPoint = pRegion->GetPointInfoById(ulPointId);
		if(pPoint)
		{
			pPoint->SetSelMaterial();
		}
	}
}

// 改变当前编辑的点的位置
void RegionEditManager::ChangeCurEditPointPos(const Ogre::Vector2& pt)
{
	CUnreachRegion* pRegion = NULL;
	CPointInfo*     pPoint  = NULL;

	// 得到当前正在编辑的区域的信息.
	pRegion = GetRegionById(m_ulCurEditRegionId);
	if(pRegion)
	{
		pPoint = pRegion->GetPointInfoById(m_ulCurEditPointId);
		if(pPoint)
		{	

			// 查找这个点的下标值.
			int iIndex = -1;
			POINT_MAP::iterator it;
			POINT_MAP::iterator itEnd;
			itEnd = pRegion->m_pointMap.end();
			for(it = pRegion->m_pointMap.begin(); it != itEnd; it++)
			{
				iIndex++;
				if(m_ulCurEditPointId == it->first)
				{
					break;
				}
			}


			// 得到修改后的位置.
			Ogre::Vector3 position;
			bool hit = RootManager::getSingleton().getTerrainIntersects(pt.x, pt.y, position);
			if(hit)
			{

				//暂时把判断能否添加一个点去掉.
				if(pRegion->CanChangePointPos(iIndex, position))
				{
					// 修改新的点的位置.
					pPoint->SetPos(position);

					// 当前编辑的点的位置.
					m_curPointPos = position;

					if(it != itEnd)
					{
						// 如果下标存在
						if((-1 != iIndex)&&(iIndex < (int)pRegion->m_pLineShape->m_pointArray.size()))
						{
							pRegion->m_pLineShape->m_pointArray[iIndex] = position;
							pRegion->m_pLineShape->FillPointData();
							//pRegion->ReShapeArea();
						}
					}

				}// if(pRegion->CanChangePointPos(iIndex, position))

			}// if(hit)

		}// if(pPoint)

	}// if(pRegion)
}

void RegionEditManager::ChangeCurEditPointPos(unsigned long ulRegionId, unsigned long ulPointId, Ogre::Vector3 position)
{

	CUnreachRegion* pRegion = NULL;
	CPointInfo*     pPoint  = NULL;

	// 得到当前正在编辑的区域的信息.
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{
		pPoint = pRegion->GetPointInfoById(ulPointId);
		if(pPoint)
		{	

			// 查找这个点的下标值.
			int iIndex = -1;
			POINT_MAP::iterator it;
			POINT_MAP::iterator itEnd;
			itEnd = pRegion->m_pointMap.end();
			for(it = pRegion->m_pointMap.begin(); it != itEnd; it++)
			{
				iIndex++;
				if(ulPointId == it->first)
				{
					break;
				}
			}

			pPoint->SetPos(position);

			// 如果下标存在
			if((-1 != iIndex)&&(iIndex < (int)pRegion->m_pLineShape->m_pointArray.size()))
			{
				pRegion->m_pLineShape->m_pointArray[iIndex] = position;
				pRegion->m_pLineShape->FillPointData();
				//pRegion->ReShapeArea();
			}

		}// if(pPoint)

	}// if(pRegion)

}

// 记录当前操作前一次点的位置.
void RegionEditManager::RecordPreDoPointPos()
{

	CUnreachRegion* pRegion = NULL;
	CPointInfo*     pPoint  = NULL;

	// 得到当前正在编辑的区域的信息.
	pRegion = GetRegionById(m_ulCurEditRegionId);
	if(pRegion)
	{
		pPoint = pRegion->GetPointInfoById(m_ulCurEditPointId);
		if(pPoint)
		{	
			m_preDoPointPos = pPoint->m_Position;		

		}// if(pPoint)

	}// if(pRegion)

}

void RegionEditManager::InitRegionEdit()
{
	CreateSceneNode();
}

void RegionEditManager::SetOldHitPointMaterial()
{
	IntersectPoint* pPoint = (IntersectPoint*)RootManager::getSingleton().getHitIndicator("IntersectPoint");
	pPoint->SetMaterial();
}

// 创建区域编辑的场景节点
void RegionEditManager::CreateSceneNode()
{
	m_pSceneNode = SystemGcs::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode();
	if(m_pSceneNode)
	{
		m_pSceneNode->setVisible(false);
	}

}

void RegionEditManager::HideScene(bool bHide)
{
	if(m_pSceneNode)
	{
		m_pSceneNode->setVisible(bHide);
		m_bIsShow = bHide;
	}
}

// 根据地形的高度改变编辑的区域。
void RegionEditManager::SetRegionHeight()
{
	UNREACHREGIONMAP::iterator it;
	UNREACHREGIONMAP::iterator itEnd;

	CUnreachRegion* pUnreachRegion = NULL;

	itEnd = m_UnreachRegionMap.end();
	for(it = m_UnreachRegionMap.begin(); it != itEnd; it++)
	{
		pUnreachRegion = it->second;
		if(pUnreachRegion)
		{
			pUnreachRegion->SetPointHeight(EQSceneManager::getSingleton().getTerrainData());
			pUnreachRegion->SetLineHeight(EQSceneManager::getSingleton().getTerrainData());
		}
	}
}

// 改变没有深度缓冲区的材质
void RegionEditManager::ChangeNoDeepMaterial()
{
	if(m_UnreachRegionMap.size())
	{
		CUnreachRegion* pRegion = NULL;
		pRegion = m_UnreachRegionMap.begin()->second;

		if(pRegion)
		{
			m_bShowDeepBuf = !m_bShowDeepBuf;
			pRegion->ChangeNoDeepMaterial(m_bShowDeepBuf);
		}

	}
}


///////////////////////////////////////////////////////////////////////////////////
// CRegionEditAction
//

CRegionEditAction::CRegionEditAction()
{
	m_Handle = RootManager::getSingleton().getRegionEditManager();

	m_iPreOperatorType = NO_OPERATOR;
}

CRegionEditAction* CRegionEditAction::create()
{
	return new CRegionEditAction();
}

const std::string& CRegionEditAction::getName(void) const
{
	static const std::string name = "CRegionEditAction";
	return name;
}


// 鼠标移动的操作
void CRegionEditAction::_onMove(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt.x, pt.y);
}

// 鼠标按下的操作.
void CRegionEditAction::_onBegin(const Ogre::Vector2& pt)
{
	// 记录当前操作， 的点的位置.
	m_Handle->RecordPreDoPointPos();
}

void CRegionEditAction::_onDrag(const Ogre::Vector2& pt)
{
	if ( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
	{
		// 如果shift键按下允许修改不可行走区域点的位置.
		m_Handle->ChangeCurEditPointPos(pt);
		m_iPreOperatorType = MODITY_POINT_POSITION;
	}
	else//
	{
		RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt.x, pt.y);
	}
}


// 鼠标抬起的操作
void CRegionEditAction::_onEnd(const Ogre::Vector2& pt, bool canceled)
{
	if(!getActive())
	{
		return;
	}

	if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		// 如果shift键按下允许修改不可行走区域点的位置.
		CRegionEditModifyOperator* pRegionEditModifyOperator = NULL;
		pRegionEditModifyOperator = new CRegionEditModifyOperator;

		pRegionEditModifyOperator->SetRegionAction(m_Handle);
		pRegionEditModifyOperator->SetPointId(m_Handle->m_ulCurEditPointId);
		pRegionEditModifyOperator->SetRegionId(m_Handle->m_ulCurEditRegionId);
		pRegionEditModifyOperator->SetPosition(m_Handle->m_preDoPointPos, m_Handle->m_curPointPos);
		RootManager::getSingleton().getOperatorManager()->addOperator(pRegionEditModifyOperator);
	}
	else if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		UNREACHREGIONMAP::iterator it;
		UNREACHREGIONMAP::iterator itBegin;
		UNREACHREGIONMAP::iterator itEnd;
		CUnreachRegion* pUnReachRegion = NULL;

		itEnd  = m_Handle->m_UnreachRegionMap.end();
		for(it = m_Handle->m_UnreachRegionMap.begin();  it != itEnd; it++)
		{
			pUnReachRegion = it->second;

			if(pUnReachRegion)
			{
				if(pUnReachRegion->IsMouseSel(pt.x, pt.y))
				{
					// 选中
					m_Handle->UnSelRegion(m_Handle->m_ulCurEditRegionId);
					m_Handle->SelRegion(pUnReachRegion->m_ulId, false);

					// 通知界面
					if ( m_Handle->mListener )
						m_Handle->mListener->onRegionSel(pUnReachRegion->m_ulId);
					break;
				}
			}
		}

		// 没有选中
	}
	else
	{
		// 鼠标抬起时对点的操作.
		if(m_Handle->EditPoint(pt))
		{
			CRegionEditAddPointOperator* pAddNewPointOperator = NULL;
			pAddNewPointOperator = new CRegionEditAddPointOperator;

			pAddNewPointOperator->SetRegionAction(m_Handle);
			pAddNewPointOperator->SetPointId(m_Handle->m_ulCurEditPointId);
			pAddNewPointOperator->SetRegionId(m_Handle->m_ulCurEditRegionId);
			pAddNewPointOperator->SetPosition(m_Handle->m_preDoPointPos, m_Handle->m_curPointPos);
			RootManager::getSingleton().getOperatorManager()->addOperator(pAddNewPointOperator);

			m_iPreOperatorType = ADD_NEW_POINT;
		}

	}




}

void CRegionEditAction::_onActive(bool active)
{

	if (!active)
	{
		IntersectPoint* pPoint = (IntersectPoint*)RootManager::getSingleton().getHitIndicator("IntersectPoint");
		pPoint->SetMaterial();
		pPoint->hide();
	}
	else
	{
		IntersectPoint* pPoint = (IntersectPoint*)RootManager::getSingleton().getHitIndicator("IntersectPoint");
		std::string strMaterialName = "Region/PointTemp";
		pPoint->SetMaterial(strMaterialName);//

	}
}

void CRegionEditAction::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		m_Handle->ChangeNoDeepMaterial();
		processed = true;
	}
}