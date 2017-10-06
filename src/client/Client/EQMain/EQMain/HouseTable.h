/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-24
* ����: ������ص����ݱ�
******************************************************************************/
#ifndef HouseTable_H
#define HouseTable_H

#include "EQMainPrerequisites.h"

#include "CompareVectorTemplate.h"

/**��ҷ��ݻ������ñ��ο���������by_��
*/
struct PlayerHouseRow
{
	int		id;						//00001000
	std::string	name;				//>���ӵ�������</name>
	uint	level;					//>1</level>
	uint	type;					//>1</type>
	int		scene_id;				// ����id����Ӧscene.xml��
	std::string	house_furniture;	//>default1.xml</house_furniture>
	uint	maxPlayer;				// ������������
	int		previewBorn;			//Ԥ����ͷλ��
};
typedef std::map<ushort, PlayerHouseRow>		PlayerHouseTable; 

/** �����������. 
@remarks ������Щ��Ԥ�ȶ��������.(0-127)Ϊ��������.  �û��Զ�������Ӧ��128��ʼ
*/
enum HouseComponentPropertyType
{
	HCP_NONE				= 0,
	HCP_STATIC				= 1,			//�򵥵�����
	HCP_FLOOR				= 2,			//�ذ�
	HCP_OUT_WALL			= 3,			//��ǽ
	HCP_IN_WALL				= 4,			//��ǽ
	HCP_ROOF				= 5,			//�ݶ�
	HCP_TCX					= 6,			//T����
};

enum HouseGridType
{
	HGT_FRAME				= 1,			//�������
};



/** ���ݲ�������
*/
struct HouseCompPropRow
{
	typedef CompareVector2Template<int> GridPos;
	typedef std::vector<GridPos> GridPosArray;

	HouseCompPropRow()
		: PropID( 0 )
		, BeWatchMode( 0 )
		, SelfHeight( 0 )
		, OffsetHeight( 0 )
		, AcceptLay( 0 )
		, LayType( 0 )
		, CanRep( false )
		, LightID( 0 )
		, HoldType( 0 )
	{
	}

	ushort			PropID;			// ����Ψһid
	std::string 	Name;			// ��������
	bool			CanRep;			// �ɷ��滻
	uchar			BeWatchMode;	// ���۲�ģʽ
	GridPosArray	HoldGrid;		// ռ������
	GridPosArray	StopGrid;		// �赲����
	GridPosArray	LayGrid;		// �ڷ�����
	ushort			SelfHeight;		// ����߶�(����)	
	ushort			OffsetHeight;	// ƫ�Ƹ߶�(����)
	uint			LayType;		// ��������(��λ��ʾ)
	uint			AcceptLay;		// �ɷ������ϵ�����(��λ��ʾ)
	uint			LightID;		// Я����Դ
	uchar			HoldType;		// ռ�����ͣ����������ж��Ƿ��ǿ�ǽ���
};
typedef std::map<ushort, HouseCompPropRow>		HouseCompPropTable; 


/** ���ݲ�����
*/
struct HouseCompRow
{
	HouseCompRow()
		: ID( 0 )
		, PropID( 0 )
		, DisID( 0 )
		, LoopID( 0 )
	{
	}

	uint				ID;			// ����ID
	std::string			Name;		// ������
	ushort				PropID;		// ���ݲ�������
	uint				DisID;		// ͼ�α���ID
	Ogre::Vector3		Pos;		// λ��
	Ogre::Quaternion	Rot;		// ��ת
	Ogre::Vector3		Scale;		// ����
	uint				LoopID;		// ���β˵�
	uint				LampType;	// �ƾ����ͣ����ڿ��صƶ�����
	std::vector<uint>	Particle;	// ����
};
typedef stdext::hash_map<uint, HouseCompRow>		HouseCompTable; 


/** �����������ͱ�
*/
struct HouseGridTypeRow
{
	HouseGridTypeRow()
		: TypeID( 0 )
		, ShowGridFitment( false )
		, AcceptLay( 0 )
	{
	}

	ushort				TypeID;		// ����ΨһID
	std::string			Name;		// ��������
	Ogre::ColourValue	ColorSign;	// �������ɫ��־
	bool				ShowGridFitment;	// �ڼ�װģʽ��ʾ�������
	uint				AcceptLay;		// �ɷ������ϵ�����(��λ��ʾ)
	bool				InDoor;		//�������ڻ�����
};
typedef std::map<ushort, HouseGridTypeRow>		HouseGridTypeTable; 


/** ������ر�������
*/
class HouseTableManager : public Ogre::Singleton<HouseTableManager>
{
public:
	const PlayerHouseRow*		getPlayerHouseRow( int id ) const; 
	const HouseCompPropRow*		getHouseCompPropRow( ushort id ) const; 
	const HouseCompRow*			getHouseCompRow( uint id ) const; 
	const HouseGridTypeRow*		getHouseGridTypeRow( ushort id ) const;
public:
	const PlayerHouseTable&		getPlayerHouseTable() const {return mPlayerHouseTable;} 
	const HouseCompPropTable&	getHouseCompPropTable() const {return mHouseCompPropTable;} 
	const HouseCompTable&		getHouseCompTable() const {return mHouseCompTable;} 
	const HouseGridTypeTable&	getHouseGridTypeTable() const {return mHouseGridTypeTable;} 

	void load();

private:
	bool extract2DPos( const std::string & s, int & x, int & y );
private:
	void loadPlayerHouseTable();
	void loadHouseCompPropTable();
	void loadHouseCompTable();
	void loadHouseGridTypeTable();

private:	
	PlayerHouseTable	mPlayerHouseTable;
	HouseCompPropTable	mHouseCompPropTable; 
	HouseCompTable		mHouseCompTable;
	HouseGridTypeTable	mHouseGridTypeTable;

};


#endif