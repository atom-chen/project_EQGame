/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-19
* ����: ������Դ
******************************************************************************/
#ifndef HouseRes_H
#define HouseRes_H

#include "EQMainPrerequisites.h"

#include "SubHouseRes.h"

/** ���ݵĲ�����Դ
@remarks ��Ogre::ResourceManager�̳�, ��ѭogre����Դʹ��. ��Դ��Ӧ���ļ�����չ��
	(.house), ��һ��xml�ļ�
@par �������ļ���ʽ����ʹ�ñ��, ����Ϊ����ļ��Ǵӱ༭��������. �Ͼ���Ogre::Resource
	�̳�, ʹ�����鷳�ܶ�
@par �̳�Ogre::ResourceҲ��ͦ�鷳����, ����Ҫ��Ӧ�ṩmanager. �ܱ���ͱ����. �����
	�͵�����һ��ʵ��.
@par Ŀǰ�����ֻ��Ϊ������HouseScene. ������Ͼ�û�κ���ϵ��. ���Ogre::Mesh����
	Ogre::Entity��һֱ����ָ�������ǲ�һ����.
*/
class HouseRes : public Ogre::Resource
{
	friend class HouseScene;
	friend class OgreMaxSceneLoader;
public:
	typedef std::map<int, FloorHouseRes*> FloorHouseResMap;
	typedef std::map<std::string, ComponenetHouseRes*> ComponenetHouseResMap;
	typedef std::vector<LinkGridRes> LinkGridList;
public:
	/** Default constructor - used by HouseResManager
	@warning
		Do not call this method directly.
	*/
	HouseRes(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle,
		const Ogre::String& group, bool isManual = false, Ogre::ManualResourceLoader* loader = 0);
	~HouseRes();

	/// �������������.
	/// ��Ϊû�ṩManualResourceLoader���ڵķ���
	virtual bool isReloadable(void) const { return false; }

	/// ����¥��
	FloorHouseRes* createFloorHouseRes(int no);

	/// �������Ӹ�
	void addLinkGrid( const LinkGridRes & lgr ) { mLinkGridList.push_back( lgr ); }
	const LinkGridList & getLinkGridList() const { return mLinkGridList; }

	/// �Ƿ������¥��
	bool hasFloorHouseRes(int no) const;

	/// ��ȡ¥��
	FloorHouseRes* getFloorHouseRes(int no);

	/// �������ݲ�����Դ
	ComponenetHouseRes* createComponenetHouseRes(const std::string& name);

	/// �Ƿ��з��ݲ�����Դ 
	bool hasComponenetHouseRes(const std::string& name) const;

	/// ����xml
	void exportClientXML(const std::string& fileName, bool clientUse);
	void exportServerXML(const std::string& fileName);

	/// ����xml
	void importXML(Ogre::DataStreamPtr stream);

protected:
	void processFloors(TiXmlElement* node);
	void processGrids(TiXmlElement* node, FloorHouseRes* flr);
	void processWalls(TiXmlElement* node, FloorHouseRes* flr);
	void processComponents(TiXmlElement* node);
	void processLinkGrids(TiXmlElement* node);
	void processComponent(TiXmlElement* node);

protected:
	/// @copydoc Resource::loadImpl
	void loadImpl(void);
	/// @copydoc Resource::unloadImpl
	void unloadImpl(void);
	/// @copydoc Resource::calculateSize
	size_t calculateSize(void) const {return 0;}

protected:
	FloorHouseResMap		mFloorHouseResMap;
	ComponenetHouseResMap	mComponenetHouseResMap;
	LinkGridList			mLinkGridList;

	/// ���ݵ���ʼ����
	int						mFloorRootNO;

	/// ����·��
	std::string				mAddPath;

	/// ���Ӳ�����
	std::string				mAddMatFullName;

	/// maybe
	//std::string				mVertion;
};


/** Specialisation of SharedPtr to allow SharedPtr to be assigned to HouseResPtr 
@note Has to be a subclass since we need operator=.
We could templatise this instead of repeating per Resource subclass, 
except to do so requires a form VC6 does not support i.e.
ResourceSubclassPtr<T> : public SharedPtr<T>
��һ������ָ����ô�鷳, ����boost�ĺ���. ����һ���򵥵Ĺ�������.
��FontPtr��������, ��Font���滻ΪHouseRes. Ȼ�����Ogre::��ok
*/
class HouseResPtr : public Ogre::SharedPtr<HouseRes> 
{
public:
	HouseResPtr() : Ogre::SharedPtr<HouseRes>() {}
	explicit HouseResPtr(HouseRes* rep) : Ogre::SharedPtr<HouseRes>(rep) {}
	HouseResPtr(const HouseResPtr& r) : Ogre::SharedPtr<HouseRes>(r) {} 
	HouseResPtr(const Ogre::ResourcePtr& r) : Ogre::SharedPtr<HouseRes>()
	{
		// lock & copy other mutex pointer
		OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
		{
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<HouseRes*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
	}

	/// Operator used to convert a ResourcePtr to a HouseResPtr
	HouseResPtr& operator=(const Ogre::ResourcePtr& r)
	{
		if (pRep == static_cast<HouseRes*>(r.getPointer()))
			return *this;
		release();
		// lock & copy other mutex pointer
		OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
		{
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<HouseRes*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
		else
		{
			// RHS must be a null pointer
			assert(r.isNull() && "RHS must be null if it has no mutex!");
			setNull();
		}
		return *this;
	}
};


#endif
