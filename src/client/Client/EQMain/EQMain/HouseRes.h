/******************************************************************************
* 作者: Y3
* 时间: 2009-11-19
* 描述: 房屋资源
******************************************************************************/
#ifndef HouseRes_H
#define HouseRes_H

#include "EQMainPrerequisites.h"

#include "SubHouseRes.h"

/** 房屋的布局资源
@remarks 从Ogre::ResourceManager继承, 遵循ogre的资源使用. 资源对应的文件是扩展名
	(.house), 是一个xml文件
@par 定这种文件格式而不使用表格, 是因为这个文件是从编辑器导出的. 毕竟从Ogre::Resource
	继承, 使代码麻烦很多
@par 继承Ogre::Resource也是挺麻烦的事, 还需要对应提供manager. 能避免就避免把. 这个类
	就当成是一个实验.
@par 目前这个类只是为了生成HouseScene. 生成完毕就没任何联系了. 这和Ogre::Mesh生成
	Ogre::Entity后还一直保持指针连接是不一样的.
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

	/// 重载了这个方法.
	/// 因为没提供ManualResourceLoader对于的方法
	virtual bool isReloadable(void) const { return false; }

	/// 创建楼层
	FloorHouseRes* createFloorHouseRes(int no);

	/// 增加连接格
	void addLinkGrid( const LinkGridRes & lgr ) { mLinkGridList.push_back( lgr ); }
	const LinkGridList & getLinkGridList() const { return mLinkGridList; }

	/// 是否有这个楼层
	bool hasFloorHouseRes(int no) const;

	/// 获取楼层
	FloorHouseRes* getFloorHouseRes(int no);

	/// 创建房屋部件资源
	ComponenetHouseRes* createComponenetHouseRes(const std::string& name);

	/// 是否有房屋部件资源 
	bool hasComponenetHouseRes(const std::string& name) const;

	/// 导出xml
	void exportClientXML(const std::string& fileName, bool clientUse);
	void exportServerXML(const std::string& fileName);

	/// 导入xml
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

	/// 房屋的起始层编号
	int						mFloorRootNO;

	/// 附加路径
	std::string				mAddPath;

	/// 附加材质名
	std::string				mAddMatFullName;

	/// maybe
	//std::string				mVertion;
};


/** Specialisation of SharedPtr to allow SharedPtr to be assigned to HouseResPtr 
@note Has to be a subclass since we need operator=.
We could templatise this instead of repeating per Resource subclass, 
except to do so requires a form VC6 does not support i.e.
ResourceSubclassPtr<T> : public SharedPtr<T>
搞一个智能指针这么麻烦, 还是boost的好用. 介绍一个简单的构建方法.
把FontPtr拷贝过来, 把Font都替换为HouseRes. 然后加上Ogre::就ok
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
