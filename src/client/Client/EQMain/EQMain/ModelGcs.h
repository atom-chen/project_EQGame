/******************************************************************************
* 作者: Y3
* 时间: 2009-10-27
* 描述: 物件模型实例
******************************************************************************/
#ifndef ModelGcs_H
#define ModelGcs_H

#include "EQMainPrerequisites.h"

#include "PickObjectProxy.h"

/** 物件模型实例
@remarks
	对Ogre模型的进一步包装. 一个ModelGcs只有一个SceneNode, 只有一幅Skeleton.
	可以有多个模型实例(Ogre::Entity). 不过如果模型实例有骨骼, 那必须都一致, 且
	会自动共享骨骼. 目前的动画只支持骨骼动画. 如果要支持顶点动画, 结构估计得做
	一些修改. 支持节点(Node)动画, 则需要增加功能.
	ModelGcs可以通过attachToLocator方法来增加子模型. 对于有父子关系的SceneNode,
	挂件最合适.
@par
	模型和他挂接的子模型, 或者是子模型的挂接模型, 我们认为是一个完整逻辑模型实
	例, 所以他们一般共用父模型的所有状态. 比如mAnimPause, 以后还会增加透明之类.
@par
	如果打算单独控制每个模型实例的状态, 那就使用模型组吧. (模型组功能为开发)
@par
	一般不直接使用Ogre的实例类,而是用这个类.
@par
	不要直接new创建模型, 而是通过ModelManagerGcs::createModel来创建
@par
	目前可以初步认为, 一个逻辑单位的图形表现就是对应一个ModelGcs, 就是一个统一
	的实体操作.如果存在过多需要关注个体的操作,比如操作具体的Entity, 就请分成多
	个ModelGcs处理.
@par
	空间变化的函数通过getMainNode对应的Ogre::SceneNode来处理
@par
	允许一个模型同时具有骨骼且带有morph. 但是一般不允许多个模型同时共有骨骼所携
	带的morph. 因为共享骨骼动画机制, 会把其他模型的morph动画也一起共享.
@par
	骨骼共享机制需要优化, 对于无需共享骨骼的物件需要做优化处理.  骨骼动画的动作
	加入需要优化, 由于一个角色有几十个动作, 且大部分是无用动作, 所以创建时候无
	需一次全部加载. 更换材质的方案可能会因为透明和高亮所改变.
*/
class Particle;
class ModelGcs
{
public:
	/** 模型创建规则抽象类.
	@remarks 目前已经有SimpleCreateModelRule的实现
	*/
	class CreateModelRuleGcs
	{
	public:
		virtual ~CreateModelRuleGcs() {}

		virtual void create( ModelGcs* pModel ) = 0;
	};

	/// Entity属性
	struct EntityAtt
	{
		EntityAtt();

		std::string		name;
		Ogre::Entity*	entity;
		bool			visible;			// 只有当ModelGcs::mVisible、visible、markVisible同时为true时，这个entity才是可见的
		std::vector<std::string>	orgMat;	// 原始的材质名
		uint			showMark;			// 显示的Mark
		uchar			showPri;			// 显示的优先级
		uint			particleHandle;		// 粒子句柄
		bool			markVisible;		// 可视化
		bool			isSelfIllum;		// 是否已经有自发光   以后可以能还有透明
		bool			isStrokes;			// 是否描边
		bool			isHighLight;		// 是否高亮
		bool			isTextureAnimation;	// 是否有纹理动画
		bool			isLightMat;			// 是否使用灯光材质
	};
	typedef std::map<std::string, EntityAtt> EntityAttMap;

	/// 挂接槽列表
	typedef std::map<std::string, ModelGcs*> SlotMap;

	/// 定位器属性
	struct LocatorAtt
	{
		LocatorAtt() : locatorNode(NULL)
		{}

		bool				visible;		// 只有当ModelGcs::mVisible、visible同时为true时，这个entity才是可见的
		std::string			boneName;
		Ogre::SceneNode*	locatorNode;
		SlotMap				slotMap;
	};
	typedef std::map<std::string, LocatorAtt> LocatorAttMap;

public:
	ModelGcs( const std::string& name );
	~ModelGcs();

public:
	/** 增加一个模型实例
	@remarks 如果有骨骼则共用骨骼, 确保骨骼文件必须一致. 
	@param meshName 模型名
	@param matName 材质名. 为空,则不修改. 可以是指定格式的材质集合
	@param iShowMark 显示掩码
	@param iShowPri 显示优先级
	@return 返回EntityAtt::mName
	*/
	std::string addEntity( const std::string& meshName, const std::string& matName = "", uint iShowMark = 0, uchar iShowPri = 0, uint particle = 0 );

	/** 删除一个模型实例
	@param name 必须是EntityAtt::mName
	*/
	void delEntity( const std::string& name );

	/** 设置可见一个模型实例
	@param name 必须是EntityAtt::mName
	*/
	void setEntityVisible( const std::string& name, bool visible );

	/** 更换物件的材质
	@param entName 实例entity的名字
	@param matName 材质名. 可以是指定格式的材质集合
	*/
	void changeEntityMat( const std::string& entName, const std::string& matName );

	/** 更换物件的材质
	@param matName 材质名. 可以是指定格式的材质集合
	*/
	void changeEntityMat( const std::string& matName );

	/** 获取entity的材质 (2011/7/25 add by hezhr)
	@param entName 实例entity的名字
	*/
	std::vector<std::string> getEntityMat(const std::string& entName);

	/** 获取物件的材质 (2011/7/25 add by hezhr)
	*/
	std::vector<std::string> getEntityMat(void);

	/** 播放一个动画
	@remarks 如果上一个动画和当前动画一致,则从动画作第一帧开始播放. 否则会融合过度.
	@param name 动画名字
	@param blendTime 和上一个动画融合的时间.
	@param loop 是否是循环动画, 一般来说不应该是循环动画, 如果需要循环, 应该使用setDefAnimation
	*/
	void playAnimation( const std::string& name, bool loop = false );

	/** 取动画播放时间(单位：毫秒)
	*/
	DWORD getAnimationLength( const std::string& name );

	/** 清除当前动画循环
	@remarks 此函数只对playAnimation执行的动画作用. 如果当前动画是默认动画, 是无效的
	*/
	void clearAnimationLoop();

	/** 设置默认的动画
	@remarks
		没有其他动画情况下, 会自动播放默认动画, 默认动画一定是循环动画
	@param name 动画名字, 空字符串的话, 表示取消默认动画
	@param blendTime 和上一个动画融合的时间
	*/
	void setDefAnimation( const std::string& name );

	/** 设置模型保持在指定的帧
	@remarks 只对morph动画有效. 帧时间间隔写死为0.0333333秒. 第0帧的动画和原模型
		的坐标一致. 这部分的动画不会受到其他动画控制的影响. 这个动画名字必须为
		model_morph.
	@param keyframe 第几帧, 从0开始. 第0帧为动画复位, 也就是取消动画.
	@param entName 指定模型实例的名字. 如果为空, 表示执行所有文件的动画.
	*/
	void setModelMorph( int keyframe, const std::string& entName = "" );

	/** 是否有模型变形, 且返回帧数
	@param entName 指定模型实例的名字. 如果为空, 表示执行所有文件的动画.
	@return 返回指定模型的帧数, 如果没动画返回0. 如果entName为空, 返回所有模型的最大帧数.
	*/
	int hasModelMorph( const std::string& entName = "" );

	/// 修改动画速率
	void modifyAnimationRate( float rate ) { mAnimationRate = rate > 0 ? rate : 0; }

	/// 获取动画速率
	float getAnimationRate() const { return mAnimationRate; }

	/// 开关动画
	void enableAnimation( bool enable );

	/// 暂停动画
	void pauseAnimation( bool enable, bool forceChild = true );

	/// 动画是否暂停
	bool isPauseAnimation() { return mAnimPause; }

	/// 是否有指定动画, 特指有带骨骼的动画
	bool hasAnimation( const std::string& name );

	/// 是否有动画, 特指有带骨骼的动画
	bool hasAnimation();

	/// 获取当前动画的名字, 没动画则返回空
	std::string getCurrAnimationName();

	/// 设置自发光的值, 这主要用与物体高亮
	void setSelfIllum( const Ogre::ColourValue& color, bool forceChild = true );

	/// 取消自发光
	void clearSelfIllum( bool forceChild = true );

	/// 是否有自发光
	bool hasSelfIllum() { return mSelfIllumEnable; }

	/// 描边
	void addStrokes(bool forceChild=true);
	void delStrokes(bool forceChild=true);

	/// 高亮
	void setHighLight(const Ogre::ColourValue& color, bool forceChild = true);
	void clearHighLight(bool forceChild = true);

	/// 纹理动画
	void setTextureAnimation(const char* info);
	void clearTextureAnimation();

	/// 灯光材质
	void setLightMat();
	void clearLightMat();

	/** 连接模型实例到指定的定位器
	@param locName 定位器的名字. 情况只有2种:
		1.bone名,表示attach到骨骼;
		2.空,表示attach到模型根节点
	@param pModel 模型名字
	@param offsetOrientation 位置偏移
	@param offsetPosition 旋转角度
	*/
	void attachToLocator(const std::string& locName, ModelGcs* pModel,
		const Ogre::Quaternion& offsetOrientation = Ogre::Quaternion::IDENTITY,
		const Ogre::Vector3& offsetPosition = Ogre::Vector3::ZERO);

	/** 从定位器组分离模型实例
	@remarks 分离出来的实例的场景节点没有父节点
	*/
	ModelGcs* detachFromLocator( const std::string modelName );

	/// 从定位器组分离模型实例, 且删除这个实例
	void destroyAndDetachFromLocator( const std::string modelName );

	/// 是否挂接在别的模型上
	bool isAttatch() { return mAttachToModelGcs != NULL;}

	/// 获取当前挂接的父模型
	ModelGcs* getAttachModel() { return mAttachToModelGcs; }

	/// 获取可见性
	bool getVisible() { return mVisible; }

	/// 设置可见性
	void setVisible(bool visible);

	//TODO: 设置透明等
	//粒子效果, 可能会扩展LocatorAtt或者这个类
public:
	/// 获取唯一的名字
	const std::string& getName() const { return mName; }

	/// 获取模型在场景的节点
	Ogre::SceneNode* getMainNode() { return mMainNode; }
	const Ogre::SceneNode* getMainNode() const { return mMainNode; }

	/// 设置父节点
	void setParentNode( Ogre::SceneNode* parentNode = NULL, bool forceUnit = false );

	/// 获取BoundingBox
	Ogre::AxisAlignedBox getBoundingBox();

	/// 设置选取代理
	void setPickObjectProxy(PickObjectProxy* proxy);

	/// 创建灯光监听
	void setLightListener(std::vector<uint>& zone);

public:
	/// 增加粒子
	uint addParticle(uint id);

	/// 删除粒子
	void delParticleByHandle(uint handle);

	/// 删除指定粒子
	void delParticleByID(uint id);

	/// 启动粒子
	void startParticle();

	/// 暂停粒子
	void pauseParticle();

	/// 停止粒子
	void stopParticle();

public:
	/// 创建主场景节点
	void _createMainNode(Ogre::SceneNode* parentNode);

	/// 设置外部的包围盒
	void _setBoundingBoxFromExternal(const Ogre::AxisAlignedBox& box);

	/// 加载骨骼文件, 且创建一个共用骨骼的空模型
	void _loadSkeletonEntity( const std::string& skelName );

	/// 增加骨骼动画. 务必在_loadSkeletonEntity之后执行
	void _addSkelAnimation( std::string skelName );

	/** 更新
	@param delta 更新的间隔时间
	@param forceAttachModel 强制更新有父模型的模型
	*/
	void _update( float delta, bool forceAttachModel = false );

public:	//编辑器函数
	/// 获取动画集
	Ogre::AnimationStateSet* _getAnimationStateSet();

protected:
	void _resetEntityMat( EntityAtt& ea );
	void _cloneEntityMat( EntityAtt& ea, bool forceRecreate = false );
	bool _hasMatEffect();
	void _sysnEntityMatEffect(EntityAtt& ea, bool forceRecreate=false);
	void _sysnSelfIllum( EntityAtt& ea );
	void _sysnStrokes( EntityAtt& ea );
	void _sysnHighLight( EntityAtt& ea );
	void _sysnTextureAnimation( EntityAtt& ea );
	void _sysnLightMat( EntityAtt& ea );
	void _destroyEntityAtt( EntityAtt& ea );
	void _updateAnimation( float delta );
	void _calcDefAnimation();
	void _clearShareAnimation();
	LocatorAtt& _createOrRetrieveLocatorAtt( const std::string& locName, const Ogre::Quaternion &offsetOrientation,
		const Ogre::Vector3 &offsetPosition);
	LocatorAtt& _createLocatorAtt( const std::string& locName, const Ogre::Quaternion &offsetOrientation,
		const Ogre::Vector3 &offsetPosition);
	void _updateLocatorNode( float delta );
	void _updateLocator( float delta );
	void _updateParticle();
	void _notifyAttach(ModelGcs* parentModel);
	void _syncVisible();
	void _syncLocatorVisible();

protected:
	ModelGcs();

protected: //数据成员
	/// 模型组
	EntityAttMap				mEntityAttMap;

	/// 定位器组
	LocatorAttMap				mLocatorAttMap;

	/// 模型的唯一名字
	std::string					mName;

	Ogre::SceneNode*			mMainNode;

	Ogre::AxisAlignedBox		mExternalBoundingBox;

	/// 代理对象的指针, 使用者必须确保这个值的有效性
	PickObjectProxy*			mPickObjectProxyPt;

	/// 共享骨骼模型的基础实例, 这是一个空实例. 目前他主要以实例存在, 大部分功能转移到mShareEntity
	Ogre::Entity*				mSkeletonEntity;

	/// 共享骨骼模型中的某个实例对象的指针, 一定有骨骼
	Ogre::Entity*				mShareEntity;

	// 动画相关
	Ogre::AnimationState*		mCurrentAnimationState;
	Ogre::AnimationState*		mFadeOutAnimationState;
	float						mFadeOutDelay;
	float						mAnimationRate;
	std::string					mDefaultAnimationName;
	std::string					mLastDefAnimationName;

	// 自发光相关
	bool						mSelfIllumEnable;
	Ogre::ColourValue			mSelfIllumColor;

	// 描边
	bool						mStrokes;

	// 高亮
	bool						mHighLight;
	Ogre::ColourValue			mHighLightColor;

	// 纹理动画
	bool						mTextureAnimationEnable;
	std::string					mTextureAnimation;

	// 灯光材质
	bool						mLightMat;

	/// 连接的父节点
	ModelGcs*					mAttachToModelGcs;
protected: //状态属性
	/// 动画暂停
	bool						mAnimPause;

	/// 可见性
	bool						mVisible;
	bool						mNeedSysnVisible;

	//
	static ulong				mSkelEntNum;
	static ulong				mCreateEntityAttNum;

	/// 所属光照区域
	std::vector<uint>			mLightZone;

	uint						mParticleHandle;
	std::map<uint, Particle*>	mParticleMap;
};



#endif
