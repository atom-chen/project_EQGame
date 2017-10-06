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
	// ��ʼ��
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
	//	// ���ܴ��ļ�.
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

	// �����ǰ������.
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
	//  ����ת������
	//

	//SaveX
	float x = 0;
	//SaveZ
	float z = 0;

	// ����ϵ��
	float fTerrainDataPosX = 0;
	float fTerrainDataPosZ = 0;


	// ��ȡ����ĸ���
	reader.readInts(&utmp);
	iRegionCount = utmp;
	//::fread(&iRegionCount, sizeof(iRegionCount), 1, pFile);

	for(int i = 0; i < iRegionCount; i++)
	{
		// ��ȡ�����id
		reader.readInts(&utmp);
		iId = utmp;
		//::fread(&iId, sizeof(iId), 1, pFile);

		// ���õ�ǰ���ڱ༭��id��
		m_ulCurEditRegionId = AddNewRegion();
		CUnreachRegion* pRegion = NULL;
		pRegion = m_UnreachRegionMap[m_ulCurEditRegionId];

		// ��ȡ��ĸ���.
		reader.readInts(&utmp);
		iPointCount = utmp;
		//::fread(&iPointCount, sizeof(iPointCount), 1, pFile);
		for(int k = 0; k < iPointCount; k++)
		{
			// ��ȡx�� z ����
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
			// ˢ���µ�����
			ReshapeArea(m_ulCurEditRegionId);
		}

	}

	//::fclose(pFile);

	// ������ʾ״̬�� ��ʾ����.
	HideScene(m_bIsShow);

	return 0;
}

unsigned long RegionEditManager::SaveRegionToFileVer0(std::string strFileName)
{
	FILE* pFile = NULL;
	pFile = ::fopen(strFileName.data(), "wb");

	if(NULL == pFile)
	{
		// ���ܴ��ļ�.
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

	// �洢����ĸ���
	iRegionCount = m_UnreachRegionMap.size();
	::fwrite(&iRegionCount, sizeof(iRegionCount), 1, pFile);

	// �洢ÿ���������Ϣ
	itEndRegion = m_UnreachRegionMap.end();

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	// ����ѳ�������ת������Ϸ�߼�����.
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
			// �洢�����id
			iId = pRegion->m_ulId;
			::fwrite(&iId, sizeof(iId), 1, pFile);

			// �洢��ĸ���.
			iPointCount = pRegion->m_pointMap.size();
			::fwrite(&iPointCount, sizeof(iPointCount), 1, pFile);

			// �洢ÿ�������Ϣ��
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
// �����������������
//

// ���һ���µ�����
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

// ͨ��ָ����id�� ɾ��һ������
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
			// ��������ڵ����
			if(m_pSceneNode)
			{
				// ɾ��������������ʵ��.
				pRegion->DetachAllEntity();
				//pRegion->m_pSceneNode->removeAndDestroyAllChildren();

				// ɾ������������ӽڵ�
				pRegion->DestroyAllChildSceneNode();

				// ɾ�������ڵ�������ڵ�.
				m_pSceneNode->removeAndDestroyChild(pRegion->m_pSceneNode->getName());
				//

			}

			delete pRegion;
		}
	}

	m_UnreachRegionMap.clear();

	m_ulCurMaxId = 0;

}



// ����ָ����id�� �õ�����
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

// ѡ��һ������
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

// ��ѡ��һ������.
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

// ����һ����������
void RegionEditManager::HideRegionArea(unsigned long ulRegionId, bool bHide)
{
	CUnreachRegion* pRegion = NULL;

	// �õ���ǰ���ڱ༭���������Ϣ.
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{

		pRegion->HideAreaNode(bHide);

	}
}

// ��ǰ�������Ƿ�Ϸ�
bool RegionEditManager::IsRegionValidate(unsigned long ulRegionId)
{
	CUnreachRegion* pRegion = NULL;

	// �õ���ǰ���ڱ༭���������Ϣ.
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

// ���´�����ʾ���������
void RegionEditManager::ReshapeArea(unsigned long ulRegionId)
{
	CUnreachRegion* pRegion = NULL;
	// �õ���ǰ���ڱ༭���������Ϣ.
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{	

		pRegion->ReShapeArea();
		pRegion->SetRegionNorMaterial();

	}
}

// �ڵ�ǰ�༭�����������һ���µĵ�
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

// ָ������id�� ָ�����id�� ָ��λ�ã� ���һ��С��.
void RegionEditManager::AddNewPoint(unsigned long ulRegionId, unsigned long ulPointId, Ogre::Vector3 position)
{
	// �õ���ǰ���ڱ༭���������Ϣ.
	CUnreachRegion* pRegion = NULL;
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{
		pRegion->AddNewPoint(ulPointId, position);

		if ( mListener )
			mListener->onRegionAddNewPoint(ulRegionId, ulPointId, position);
	}
}

// ָ������id�� ָ�����id�� ָ��λ�ã� ɾ��һ��С��.
void RegionEditManager::DelPoint(unsigned long ulRegionId, unsigned long ulPointId)
{
	// �õ���ǰ���ڱ༭���������Ϣ.
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

	// �õ���ǰ���ڱ༭���������Ϣ.
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

// �ı䵱ǰ�༭�ĵ��λ��
void RegionEditManager::ChangeCurEditPointPos(const Ogre::Vector2& pt)
{
	CUnreachRegion* pRegion = NULL;
	CPointInfo*     pPoint  = NULL;

	// �õ���ǰ���ڱ༭���������Ϣ.
	pRegion = GetRegionById(m_ulCurEditRegionId);
	if(pRegion)
	{
		pPoint = pRegion->GetPointInfoById(m_ulCurEditPointId);
		if(pPoint)
		{	

			// �����������±�ֵ.
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


			// �õ��޸ĺ��λ��.
			Ogre::Vector3 position;
			bool hit = RootManager::getSingleton().getTerrainIntersects(pt.x, pt.y, position);
			if(hit)
			{

				//��ʱ���ж��ܷ����һ����ȥ��.
				if(pRegion->CanChangePointPos(iIndex, position))
				{
					// �޸��µĵ��λ��.
					pPoint->SetPos(position);

					// ��ǰ�༭�ĵ��λ��.
					m_curPointPos = position;

					if(it != itEnd)
					{
						// ����±����
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

	// �õ���ǰ���ڱ༭���������Ϣ.
	pRegion = GetRegionById(ulRegionId);
	if(pRegion)
	{
		pPoint = pRegion->GetPointInfoById(ulPointId);
		if(pPoint)
		{	

			// �����������±�ֵ.
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

			// ����±����
			if((-1 != iIndex)&&(iIndex < (int)pRegion->m_pLineShape->m_pointArray.size()))
			{
				pRegion->m_pLineShape->m_pointArray[iIndex] = position;
				pRegion->m_pLineShape->FillPointData();
				//pRegion->ReShapeArea();
			}

		}// if(pPoint)

	}// if(pRegion)

}

// ��¼��ǰ����ǰһ�ε��λ��.
void RegionEditManager::RecordPreDoPointPos()
{

	CUnreachRegion* pRegion = NULL;
	CPointInfo*     pPoint  = NULL;

	// �õ���ǰ���ڱ༭���������Ϣ.
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

// ��������༭�ĳ����ڵ�
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

// ���ݵ��εĸ߶ȸı�༭������
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

// �ı�û����Ȼ������Ĳ���
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


// ����ƶ��Ĳ���
void CRegionEditAction::_onMove(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt.x, pt.y);
}

// ��갴�µĲ���.
void CRegionEditAction::_onBegin(const Ogre::Vector2& pt)
{
	// ��¼��ǰ������ �ĵ��λ��.
	m_Handle->RecordPreDoPointPos();
}

void CRegionEditAction::_onDrag(const Ogre::Vector2& pt)
{
	if ( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
	{
		// ���shift�����������޸Ĳ�������������λ��.
		m_Handle->ChangeCurEditPointPos(pt);
		m_iPreOperatorType = MODITY_POINT_POSITION;
	}
	else//
	{
		RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt.x, pt.y);
	}
}


// ���̧��Ĳ���
void CRegionEditAction::_onEnd(const Ogre::Vector2& pt, bool canceled)
{
	if(!getActive())
	{
		return;
	}

	if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		// ���shift�����������޸Ĳ�������������λ��.
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
					// ѡ��
					m_Handle->UnSelRegion(m_Handle->m_ulCurEditRegionId);
					m_Handle->SelRegion(pUnReachRegion->m_ulId, false);

					// ֪ͨ����
					if ( m_Handle->mListener )
						m_Handle->mListener->onRegionSel(pUnReachRegion->m_ulId);
					break;
				}
			}
		}

		// û��ѡ��
	}
	else
	{
		// ���̧��ʱ�Ե�Ĳ���.
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