#include "EQMainPCH.h"

#include "CommFuncGcs.h"

#include "Ogre/OgreFontManager.h"

//�����߶ε����ݽṹ�������յ�
struct SLineData
{
	SLineData()
	{
		kStartPos = kEndPos = Ogre::Vector3::ZERO;
	}

	SLineData(Ogre::Vector3 kPos)
	{
		kStartPos = kEndPos = kPos;
	}

	Ogre::Vector3 kStartPos; // �߶����λ��
	Ogre::Vector3 kEndPos; // �߶��յ�λ��
};

typedef std::vector<SLineData> VecLineData;

VecLineData m_vecRowLineData; // X������߶����ݼ���
VecLineData m_vecColLineData; // Z������߶����ݼ���

SLineData m_coreLineRow;    //�����ߺ���
SLineData m_coreLineCol;    //����������

//-----------------------------------------------------------------------
//��������������꣬�����ߵ�����
void Calculate(Ogre::Vector3 kMinimum, Ogre::Vector3 kMaximum, Ogre::Real kCellSize)
{
	DWORD dwCount = 0; // ������
	Ogre::Real rPos = kMinimum.x;
	while (rPos < kMaximum.x)
	{
		rPos = kMinimum.x + dwCount * kCellSize;
		if ( dwCount == ((kMaximum.x-kMinimum.x)/kCellSize)/2 )
		{
			m_coreLineRow.kStartPos.x = rPos;
			m_coreLineRow.kStartPos.y = kMinimum.y;
			m_coreLineRow.kStartPos.z = kMinimum.z;

			m_coreLineRow.kEndPos.x = rPos;
			m_coreLineRow.kEndPos.y = kMinimum.y;
			m_coreLineRow.kEndPos.z = kMaximum.z;
		}

		SLineData sLineData(Ogre::Vector3(0,0,0));
		sLineData.kStartPos.x = rPos;
		sLineData.kEndPos.x = sLineData.kStartPos.x = rPos;

		// -- Z������ -- //
		// ���
		sLineData.kStartPos.z = kMinimum.z;
		// �յ�
		sLineData.kEndPos.z = kMaximum.z;

		// �����¼
		m_vecRowLineData.push_back(sLineData);
		dwCount++;
	}

	// -- ������Z�ᴹֱ���߶���㡢�յ����� -- //
	dwCount = 0; // ������
	rPos = kMinimum.z;
	while (rPos < kMaximum.z)
	{
		rPos = kMinimum.z + dwCount * kCellSize;
		if ( rPos == (kMinimum.z+((kMaximum.z-kMinimum.z)/2)))
		{
			m_coreLineCol.kStartPos.x = kMinimum.x;
			m_coreLineCol.kStartPos.y = kMinimum.y;
			m_coreLineCol.kStartPos.z = rPos;

			m_coreLineCol.kEndPos.x = kMaximum.x;
			m_coreLineCol.kEndPos.y = kMinimum.y;
			m_coreLineCol.kEndPos.z = rPos;

		}
		SLineData sLineData(Ogre::Vector3(0, kMinimum.y, 0));
		sLineData.kEndPos.z = sLineData.kStartPos.z = rPos;

		// -- X������� -- //
		// ���				
		sLineData.kStartPos.x = kMinimum.x;
		// �յ�				
		sLineData.kEndPos.x = kMaximum.x;

		// �����¼
		m_vecColLineData.push_back(sLineData);
		dwCount++;
	}

}
//-----------------------------------------------------------------------

Ogre::ManualObject* CreateManualGrid(Ogre::SceneManager *mManager,
									 Ogre::String mGridName,	
									 Ogre::Real mSize,
									 Ogre::Real mGridNum,
									 int mGapNum,                       
									 Ogre::ColourValue mLineColour,
									 Ogre::ColourValue mCoreLineColour,
									 Ogre::ColourValue mGapLineColour )
{
	// -- ������ʼ�� -- //
	m_vecRowLineData.clear();
	m_vecColLineData.clear();

	Ogre::Vector3 m_kminipos;   //�������С����
	Ogre::Vector3 m_kmaxpos;    //������������

	m_kminipos.x = -mGridNum*mSize;
	m_kminipos.y = 0;
	m_kminipos.z = -mGridNum*mSize;

	m_kmaxpos.x = mGridNum*mSize;
	m_kmaxpos.y = 0;
	m_kmaxpos.z = mGridNum*mSize;


	// -- ����������С��и��� -- //
	Calculate(m_kminipos, m_kmaxpos, mSize);


	// -- �������� -- //

	assert(NULL != mManager);
	if(NULL == mManager)
	{
		return NULL;
	}

	// -- �����ֶ����󲢳�ʼ�� -- //


	Ogre::ManualObject* mManualObject; // �ֶ�����
	mManualObject = mManager->createManualObject(mGridName);


	assert(NULL != mManualObject);
	if(NULL == mManualObject)
	{
		return NULL;
	}

	mManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	mManualObject->colour(mLineColour);

	// -- �з��� -- //
	int iIndex = 0;
	VecLineData::iterator itr = m_vecRowLineData.begin();
	for(; m_vecRowLineData.end() != itr; itr++)
	{
		// ���	
		mManualObject->position(itr->kStartPos);
		mManualObject->index(iIndex);
		iIndex ++;

		// �յ�
		mManualObject->position(itr->kEndPos);					
		mManualObject->index(iIndex);			
		iIndex++;				

	}
	// -- �з��� -- //
	itr = m_vecColLineData.begin();
	for(; m_vecColLineData.end() != itr; itr++)
	{
		// ���
		mManualObject->position(itr->kStartPos);
		mManualObject->index(iIndex);
		iIndex++;

		// �յ�
		mManualObject->position(itr->kEndPos);
		mManualObject->index(iIndex);
		iIndex++;
	}

	// -- ������������ɫ--//
	mManualObject->position( m_coreLineRow.kStartPos);
	mManualObject->index(iIndex);
	mManualObject->colour(mCoreLineColour);
	iIndex++;

	mManualObject->position(  m_coreLineRow.kEndPos);					
	mManualObject->index(iIndex);
	iIndex++;

	mManualObject->position( m_coreLineCol.kStartPos);
	mManualObject->index(iIndex);
	iIndex ++;

	mManualObject->position(  m_coreLineCol.kEndPos);					
	mManualObject->index(iIndex);
	iIndex++;

	//--���ü������ɫ--//
	if ( 0 == mGapNum || mGapNum > mGridNum)
	{
		mManualObject->end();
		return mManualObject;
	}


	int j = mGridNum/mGapNum;
	int RowNum = m_coreLineRow.kStartPos.x;
	int ColNum = m_coreLineCol.kStartPos.z;
	int m_value = mSize*mGapNum;
	for ( int i =  1 ; i <= j; i++) 
	{
		//----������----
		m_coreLineRow.kStartPos.x = RowNum + i*m_value;
		mManualObject->position( m_coreLineRow.kStartPos);
		mManualObject->index(iIndex);
		iIndex++;
		mManualObject->colour(mGapLineColour);

		m_coreLineRow.kEndPos.x = RowNum + i*m_value;
		mManualObject->position( m_coreLineRow.kEndPos);
		mManualObject->index(iIndex);
		iIndex++;

		m_coreLineRow.kStartPos.x = RowNum - i*m_value;
		mManualObject->position( m_coreLineRow.kStartPos);
		mManualObject->index(iIndex);
		iIndex++;

		m_coreLineRow.kEndPos.x = RowNum - i*m_value;
		mManualObject->position( m_coreLineRow.kEndPos);
		mManualObject->index(iIndex);
		iIndex++;


		//----������--
		m_coreLineCol.kStartPos.z = ColNum +i*m_value;
		mManualObject->position( m_coreLineCol.kStartPos);
		mManualObject->index(iIndex);
		iIndex++;

		m_coreLineCol.kEndPos.z = ColNum +i*m_value;
		mManualObject->position( m_coreLineCol.kEndPos);
		mManualObject->index(iIndex);
		iIndex++;

		m_coreLineCol.kStartPos.z = ColNum -i*m_value;
		mManualObject->position( m_coreLineCol.kStartPos);
		mManualObject->index(iIndex);
		iIndex++;

		m_coreLineCol.kEndPos.z = ColNum -i*m_value;
		mManualObject->position( m_coreLineCol.kEndPos);
		mManualObject->index(iIndex);
		iIndex++;

	}

	mManualObject->end();
	return mManualObject;

}

//-----------------------------------------------------------------------
Ogre::ManualObject* CreateManualQuad(Ogre::SceneManager *sceneMgr, float x, float z, const Ogre::ColourValue& colour )
{
	assert( x > 0 && z > 0 );

	static int num = 0;
	++num;

	std::string name = "--Quad--[auto]--" + Ogre::StringConverter::toString(num) + "--";

	std::string matName;
	if (colour.a == 1.0f)
		matName = "SystemMat/Comm-2";
	else
		matName = "SystemMat/Comm-1";

	float hx = x/2.0;
	float hz = z/2.0;

	Ogre::ManualObject* manual = sceneMgr->createManualObject(name);

	manual->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	manual->colour(colour);
	manual->position(-hx,	0, -hz);
	//manual->colour(colour);
	manual->position(hx,	0, -hz);
	//manual->colour(colour);
	manual->position(hx,	0, hz );
	//manual->colour(colour);
	manual->position(-hx,	0, hz);
	//manual->colour(colour);

	manual->quad(0,3,2,1);

	manual->end();

	return manual;

}

//-----------------------------------------------------------------------
Ogre::ManualObject* CreateManualQuad( Ogre::SceneManager * sceneMgr, 
											const Ogre::Vector3 & a, 
											const Ogre::Vector3 & b, 
											const Ogre::Vector3 & c, 
											const Ogre::Vector3 & d, 
											const Ogre::ColourValue & colour )
{
	static int num = 0;
	++num;

	std::string name = "--Quad2--[auto]--" + Ogre::StringConverter::toString(num) + "--";

	std::string matName;
	if (colour.a == 1.0f)
		matName = "SystemMat/Comm-2";
	else
		matName = "SystemMat/Comm-1";

	Ogre::ManualObject* manual = sceneMgr->createManualObject(name);

	manual->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	manual->colour(colour);
	manual->position( a );
	manual->position( b );
	manual->position( c );
	manual->position( d );

	manual->quad(0,3,2,1);

	manual->end();

	return manual;
}
//-----------------------------------------------------------------------
Ogre::ManualObject* CreateHollowQuad(Ogre::SceneManager *sceneMgr, float radius, float thickness, const Ogre::ColourValue& colour )
{
	assert( radius > 0 && thickness > 0 && radius > thickness );

	static int num = 0;
	++num;

	std::string name = "--HollowQuad--[auto]--" + Ogre::StringConverter::toString(num) + "--";

	std::string matName;
	if (colour.a == 1.0f)
		matName = "SystemMat/Comm-2";
	else
		matName = "SystemMat/Comm-1";

	Ogre::ManualObject* manual = sceneMgr->createManualObject(name);

	manual->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	manual->position(-radius,			0, -radius			);		//0
	manual->colour(colour);
	manual->position(radius-thickness,	0, -radius			);		//1
	manual->colour(colour);
	manual->position(radius,			0, -radius			);		//2
	manual->colour(colour);
	manual->position(radius,			0, radius-thickness	);		//3
	manual->colour(colour);
	manual->position(radius,			0, radius			);		//4
	manual->colour(colour);
	manual->position(-radius+thickness,	0, radius			);		//5
	manual->colour(colour);
	manual->position(-radius,			0, radius			);		//6
	manual->colour(colour);
	manual->position(-radius,			0, -radius+thickness);		//7
	manual->colour(colour);
	manual->position(-radius+thickness,	0, -radius+thickness);		//8
	manual->colour(colour);
	manual->position(radius-thickness,	0, -radius+thickness);		//9
	manual->colour(colour);
	manual->position(radius-thickness,	0, radius-thickness	);		//10
	manual->colour(colour);
	manual->position(-radius+thickness,	0, radius-thickness	);		//11
	manual->colour(colour);


	//manual->quad(0,1,9,7);
	//manual->quad(2,3,10,1);
	//manual->quad(4,5,11,3);
	//manual->quad(6,7,8,5);

	//����xz��Ļ�Ƿ���, ����Ϊ������y��, ��Ҳ�÷�����
	manual->quad(0,7,9,1);
	manual->quad(2,1,10,3);
	manual->quad(4,3,11,5);
	manual->quad(6,5,8,7);

	manual->end();

	return manual;
}

//-----------------------------------------------------------------------
Ogre::ManualObject* CreateNumberQuad(Ogre::SceneManager *sceneMgr, float x, float z, Ogre::uint number)
{
	assert( x > 0 && z > 0 );

	static int num = 0;
	++num;

	std::string name = "--NumberQuad--[auto]--" + Ogre::StringConverter::toString(num) + "--";

	float hx = (x-30)/2.0;
	float hz = (z-30)/2.0;

	Ogre::ManualObject* manual = sceneMgr->createManualObject(name);

	Ogre::FontPtr mpFont = Ogre::FontManager::getSingleton().getByName( "BlueHighway" );
	mpFont->load();
	Ogre::MaterialPtr mpMaterial = mpFont->getMaterial();
	mpMaterial->setDepthCheckEnabled(false);
	mpMaterial->setLightingEnabled(false);
	mpMaterial->setCullingMode(Ogre::CULL_NONE);

	manual->begin(mpMaterial->getName().c_str(), Ogre::RenderOperation::OT_TRIANGLE_LIST);

	char temp[2];
	if(number < 10)
	{
		sprintf_s(temp, 2, "%u", number);
		const Ogre::Font::UVRect& uvRect1 = mpFont->getGlyphTexCoords(Ogre::DisplayString(temp).begin().getCharacter());

		manual->position(-hx, 0, -hz);
		manual->textureCoord(uvRect1.left, uvRect1.top);
		manual->position(-hx, 0, hz);
		manual->textureCoord(uvRect1.left, uvRect1.bottom-0.015);
		manual->position(hx, 0, hz);
		manual->textureCoord(uvRect1.right, uvRect1.bottom-0.015);
		manual->position(hx, 0, -hz);
		manual->textureCoord(uvRect1.right, uvRect1.top);

		manual->quad(0,1,2,3);
	}
	else
	{
		sprintf_s(temp, 2, "%u", number%100/10);
		const Ogre::Font::UVRect& uvRect1 = mpFont->getGlyphTexCoords(Ogre::DisplayString(temp).begin().getCharacter());
		sprintf_s(temp, 2, "%u", number%10);
		const Ogre::Font::UVRect& uvRect2 = mpFont->getGlyphTexCoords(Ogre::DisplayString(temp).begin().getCharacter());

		manual->position(-hx, 0, -hz);
		manual->textureCoord(uvRect1.left, uvRect1.top);
		manual->position(-hx, 0, hz);
		manual->textureCoord(uvRect1.left, uvRect1.bottom-0.015);
		manual->position(0, 0, hz);
		manual->textureCoord(uvRect1.right, uvRect1.bottom-0.015);
		manual->position(0, 0, -hz);
		manual->textureCoord(uvRect1.right, uvRect1.top);

		manual->position(0, 0, -hz);
		manual->textureCoord(uvRect2.left, uvRect2.top);
		manual->position(0, 0, hz);
		manual->textureCoord(uvRect2.left, uvRect2.bottom-0.015);
		manual->position(hx, 0, hz);
		manual->textureCoord(uvRect2.right, uvRect2.bottom-0.015);
		manual->position(hx, 0, -hz);
		manual->textureCoord(uvRect2.right, uvRect2.top);

		manual->quad(0,1,2,3);
		manual->quad(4,5,6,7);
	}

	manual->end();

	return manual;

}
