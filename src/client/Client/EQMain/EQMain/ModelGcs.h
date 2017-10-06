/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-27
* ����: ���ģ��ʵ��
******************************************************************************/
#ifndef ModelGcs_H
#define ModelGcs_H

#include "EQMainPrerequisites.h"

#include "PickObjectProxy.h"

/** ���ģ��ʵ��
@remarks
	��Ogreģ�͵Ľ�һ����װ. һ��ModelGcsֻ��һ��SceneNode, ֻ��һ��Skeleton.
	�����ж��ģ��ʵ��(Ogre::Entity). �������ģ��ʵ���й���, �Ǳ��붼һ��, ��
	���Զ��������. Ŀǰ�Ķ���ֻ֧�ֹ�������. ���Ҫ֧�ֶ��㶯��, �ṹ���Ƶ���
	һЩ�޸�. ֧�ֽڵ�(Node)����, ����Ҫ���ӹ���.
	ModelGcs����ͨ��attachToLocator������������ģ��. �����и��ӹ�ϵ��SceneNode,
	�Ҽ������.
@par
	ģ�ͺ����ҽӵ���ģ��, ��������ģ�͵Ĺҽ�ģ��, ������Ϊ��һ�������߼�ģ��ʵ
	��, ��������һ�㹲�ø�ģ�͵�����״̬. ����mAnimPause, �Ժ󻹻�����͸��֮��.
@par
	������㵥������ÿ��ģ��ʵ����״̬, �Ǿ�ʹ��ģ�����. (ģ���鹦��Ϊ����)
@par
	һ�㲻ֱ��ʹ��Ogre��ʵ����,�����������.
@par
	��Ҫֱ��new����ģ��, ����ͨ��ModelManagerGcs::createModel������
@par
	Ŀǰ���Գ�����Ϊ, һ���߼���λ��ͼ�α��־��Ƕ�Ӧһ��ModelGcs, ����һ��ͳһ
	��ʵ�����.������ڹ�����Ҫ��ע����Ĳ���,������������Entity, ����ֳɶ�
	��ModelGcs����.
@par
	�ռ�仯�ĺ���ͨ��getMainNode��Ӧ��Ogre::SceneNode������
@par
	����һ��ģ��ͬʱ���й����Ҵ���morph. ����һ�㲻������ģ��ͬʱ���й�����Я
	����morph. ��Ϊ���������������, �������ģ�͵�morph����Ҳһ����.
@par
	�������������Ҫ�Ż�, �������蹲������������Ҫ���Ż�����.  ���������Ķ���
	������Ҫ�Ż�, ����һ����ɫ�м�ʮ������, �Ҵ󲿷������ö���, ���Դ���ʱ����
	��һ��ȫ������. �������ʵķ������ܻ���Ϊ͸���͸������ı�.
*/
class Particle;
class ModelGcs
{
public:
	/** ģ�ʹ������������.
	@remarks Ŀǰ�Ѿ���SimpleCreateModelRule��ʵ��
	*/
	class CreateModelRuleGcs
	{
	public:
		virtual ~CreateModelRuleGcs() {}

		virtual void create( ModelGcs* pModel ) = 0;
	};

	/// Entity����
	struct EntityAtt
	{
		EntityAtt();

		std::string		name;
		Ogre::Entity*	entity;
		bool			visible;			// ֻ�е�ModelGcs::mVisible��visible��markVisibleͬʱΪtrueʱ�����entity���ǿɼ���
		std::vector<std::string>	orgMat;	// ԭʼ�Ĳ�����
		uint			showMark;			// ��ʾ��Mark
		uchar			showPri;			// ��ʾ�����ȼ�
		uint			particleHandle;		// ���Ӿ��
		bool			markVisible;		// ���ӻ�
		bool			isSelfIllum;		// �Ƿ��Ѿ����Է���   �Ժ�����ܻ���͸��
		bool			isStrokes;			// �Ƿ����
		bool			isHighLight;		// �Ƿ����
		bool			isTextureAnimation;	// �Ƿ���������
		bool			isLightMat;			// �Ƿ�ʹ�õƹ����
	};
	typedef std::map<std::string, EntityAtt> EntityAttMap;

	/// �ҽӲ��б�
	typedef std::map<std::string, ModelGcs*> SlotMap;

	/// ��λ������
	struct LocatorAtt
	{
		LocatorAtt() : locatorNode(NULL)
		{}

		bool				visible;		// ֻ�е�ModelGcs::mVisible��visibleͬʱΪtrueʱ�����entity���ǿɼ���
		std::string			boneName;
		Ogre::SceneNode*	locatorNode;
		SlotMap				slotMap;
	};
	typedef std::map<std::string, LocatorAtt> LocatorAttMap;

public:
	ModelGcs( const std::string& name );
	~ModelGcs();

public:
	/** ����һ��ģ��ʵ��
	@remarks ����й������ù���, ȷ�������ļ�����һ��. 
	@param meshName ģ����
	@param matName ������. Ϊ��,���޸�. ������ָ����ʽ�Ĳ��ʼ���
	@param iShowMark ��ʾ����
	@param iShowPri ��ʾ���ȼ�
	@return ����EntityAtt::mName
	*/
	std::string addEntity( const std::string& meshName, const std::string& matName = "", uint iShowMark = 0, uchar iShowPri = 0, uint particle = 0 );

	/** ɾ��һ��ģ��ʵ��
	@param name ������EntityAtt::mName
	*/
	void delEntity( const std::string& name );

	/** ���ÿɼ�һ��ģ��ʵ��
	@param name ������EntityAtt::mName
	*/
	void setEntityVisible( const std::string& name, bool visible );

	/** ��������Ĳ���
	@param entName ʵ��entity������
	@param matName ������. ������ָ����ʽ�Ĳ��ʼ���
	*/
	void changeEntityMat( const std::string& entName, const std::string& matName );

	/** ��������Ĳ���
	@param matName ������. ������ָ����ʽ�Ĳ��ʼ���
	*/
	void changeEntityMat( const std::string& matName );

	/** ��ȡentity�Ĳ��� (2011/7/25 add by hezhr)
	@param entName ʵ��entity������
	*/
	std::vector<std::string> getEntityMat(const std::string& entName);

	/** ��ȡ����Ĳ��� (2011/7/25 add by hezhr)
	*/
	std::vector<std::string> getEntityMat(void);

	/** ����һ������
	@remarks �����һ�������͵�ǰ����һ��,��Ӷ�������һ֡��ʼ����. ������ںϹ���.
	@param name ��������
	@param blendTime ����һ�������ںϵ�ʱ��.
	@param loop �Ƿ���ѭ������, һ����˵��Ӧ����ѭ������, �����Ҫѭ��, Ӧ��ʹ��setDefAnimation
	*/
	void playAnimation( const std::string& name, bool loop = false );

	/** ȡ��������ʱ��(��λ������)
	*/
	DWORD getAnimationLength( const std::string& name );

	/** �����ǰ����ѭ��
	@remarks �˺���ֻ��playAnimationִ�еĶ�������. �����ǰ������Ĭ�϶���, ����Ч��
	*/
	void clearAnimationLoop();

	/** ����Ĭ�ϵĶ���
	@remarks
		û���������������, ���Զ�����Ĭ�϶���, Ĭ�϶���һ����ѭ������
	@param name ��������, ���ַ����Ļ�, ��ʾȡ��Ĭ�϶���
	@param blendTime ����һ�������ںϵ�ʱ��
	*/
	void setDefAnimation( const std::string& name );

	/** ����ģ�ͱ�����ָ����֡
	@remarks ֻ��morph������Ч. ֡ʱ����д��Ϊ0.0333333��. ��0֡�Ķ�����ԭģ��
		������һ��. �ⲿ�ֵĶ��������ܵ������������Ƶ�Ӱ��. ����������ֱ���Ϊ
		model_morph.
	@param keyframe �ڼ�֡, ��0��ʼ. ��0֡Ϊ������λ, Ҳ����ȡ������.
	@param entName ָ��ģ��ʵ��������. ���Ϊ��, ��ʾִ�������ļ��Ķ���.
	*/
	void setModelMorph( int keyframe, const std::string& entName = "" );

	/** �Ƿ���ģ�ͱ���, �ҷ���֡��
	@param entName ָ��ģ��ʵ��������. ���Ϊ��, ��ʾִ�������ļ��Ķ���.
	@return ����ָ��ģ�͵�֡��, ���û��������0. ���entNameΪ��, ��������ģ�͵����֡��.
	*/
	int hasModelMorph( const std::string& entName = "" );

	/// �޸Ķ�������
	void modifyAnimationRate( float rate ) { mAnimationRate = rate > 0 ? rate : 0; }

	/// ��ȡ��������
	float getAnimationRate() const { return mAnimationRate; }

	/// ���ض���
	void enableAnimation( bool enable );

	/// ��ͣ����
	void pauseAnimation( bool enable, bool forceChild = true );

	/// �����Ƿ���ͣ
	bool isPauseAnimation() { return mAnimPause; }

	/// �Ƿ���ָ������, ��ָ�д������Ķ���
	bool hasAnimation( const std::string& name );

	/// �Ƿ��ж���, ��ָ�д������Ķ���
	bool hasAnimation();

	/// ��ȡ��ǰ����������, û�����򷵻ؿ�
	std::string getCurrAnimationName();

	/// �����Է����ֵ, ����Ҫ�����������
	void setSelfIllum( const Ogre::ColourValue& color, bool forceChild = true );

	/// ȡ���Է���
	void clearSelfIllum( bool forceChild = true );

	/// �Ƿ����Է���
	bool hasSelfIllum() { return mSelfIllumEnable; }

	/// ���
	void addStrokes(bool forceChild=true);
	void delStrokes(bool forceChild=true);

	/// ����
	void setHighLight(const Ogre::ColourValue& color, bool forceChild = true);
	void clearHighLight(bool forceChild = true);

	/// ������
	void setTextureAnimation(const char* info);
	void clearTextureAnimation();

	/// �ƹ����
	void setLightMat();
	void clearLightMat();

	/** ����ģ��ʵ����ָ���Ķ�λ��
	@param locName ��λ��������. ���ֻ��2��:
		1.bone��,��ʾattach������;
		2.��,��ʾattach��ģ�͸��ڵ�
	@param pModel ģ������
	@param offsetOrientation λ��ƫ��
	@param offsetPosition ��ת�Ƕ�
	*/
	void attachToLocator(const std::string& locName, ModelGcs* pModel,
		const Ogre::Quaternion& offsetOrientation = Ogre::Quaternion::IDENTITY,
		const Ogre::Vector3& offsetPosition = Ogre::Vector3::ZERO);

	/** �Ӷ�λ�������ģ��ʵ��
	@remarks ���������ʵ���ĳ����ڵ�û�и��ڵ�
	*/
	ModelGcs* detachFromLocator( const std::string modelName );

	/// �Ӷ�λ�������ģ��ʵ��, ��ɾ�����ʵ��
	void destroyAndDetachFromLocator( const std::string modelName );

	/// �Ƿ�ҽ��ڱ��ģ����
	bool isAttatch() { return mAttachToModelGcs != NULL;}

	/// ��ȡ��ǰ�ҽӵĸ�ģ��
	ModelGcs* getAttachModel() { return mAttachToModelGcs; }

	/// ��ȡ�ɼ���
	bool getVisible() { return mVisible; }

	/// ���ÿɼ���
	void setVisible(bool visible);

	//TODO: ����͸����
	//����Ч��, ���ܻ���չLocatorAtt���������
public:
	/// ��ȡΨһ������
	const std::string& getName() const { return mName; }

	/// ��ȡģ���ڳ����Ľڵ�
	Ogre::SceneNode* getMainNode() { return mMainNode; }
	const Ogre::SceneNode* getMainNode() const { return mMainNode; }

	/// ���ø��ڵ�
	void setParentNode( Ogre::SceneNode* parentNode = NULL, bool forceUnit = false );

	/// ��ȡBoundingBox
	Ogre::AxisAlignedBox getBoundingBox();

	/// ����ѡȡ����
	void setPickObjectProxy(PickObjectProxy* proxy);

	/// �����ƹ����
	void setLightListener(std::vector<uint>& zone);

public:
	/// ��������
	uint addParticle(uint id);

	/// ɾ������
	void delParticleByHandle(uint handle);

	/// ɾ��ָ������
	void delParticleByID(uint id);

	/// ��������
	void startParticle();

	/// ��ͣ����
	void pauseParticle();

	/// ֹͣ����
	void stopParticle();

public:
	/// �����������ڵ�
	void _createMainNode(Ogre::SceneNode* parentNode);

	/// �����ⲿ�İ�Χ��
	void _setBoundingBoxFromExternal(const Ogre::AxisAlignedBox& box);

	/// ���ع����ļ�, �Ҵ���һ�����ù����Ŀ�ģ��
	void _loadSkeletonEntity( const std::string& skelName );

	/// ���ӹ�������. �����_loadSkeletonEntity֮��ִ��
	void _addSkelAnimation( std::string skelName );

	/** ����
	@param delta ���µļ��ʱ��
	@param forceAttachModel ǿ�Ƹ����и�ģ�͵�ģ��
	*/
	void _update( float delta, bool forceAttachModel = false );

public:	//�༭������
	/// ��ȡ������
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

protected: //���ݳ�Ա
	/// ģ����
	EntityAttMap				mEntityAttMap;

	/// ��λ����
	LocatorAttMap				mLocatorAttMap;

	/// ģ�͵�Ψһ����
	std::string					mName;

	Ogre::SceneNode*			mMainNode;

	Ogre::AxisAlignedBox		mExternalBoundingBox;

	/// ��������ָ��, ʹ���߱���ȷ�����ֵ����Ч��
	PickObjectProxy*			mPickObjectProxyPt;

	/// �������ģ�͵Ļ���ʵ��, ����һ����ʵ��. Ŀǰ����Ҫ��ʵ������, �󲿷ֹ���ת�Ƶ�mShareEntity
	Ogre::Entity*				mSkeletonEntity;

	/// �������ģ���е�ĳ��ʵ�������ָ��, һ���й���
	Ogre::Entity*				mShareEntity;

	// �������
	Ogre::AnimationState*		mCurrentAnimationState;
	Ogre::AnimationState*		mFadeOutAnimationState;
	float						mFadeOutDelay;
	float						mAnimationRate;
	std::string					mDefaultAnimationName;
	std::string					mLastDefAnimationName;

	// �Է������
	bool						mSelfIllumEnable;
	Ogre::ColourValue			mSelfIllumColor;

	// ���
	bool						mStrokes;

	// ����
	bool						mHighLight;
	Ogre::ColourValue			mHighLightColor;

	// ������
	bool						mTextureAnimationEnable;
	std::string					mTextureAnimation;

	// �ƹ����
	bool						mLightMat;

	/// ���ӵĸ��ڵ�
	ModelGcs*					mAttachToModelGcs;
protected: //״̬����
	/// ������ͣ
	bool						mAnimPause;

	/// �ɼ���
	bool						mVisible;
	bool						mNeedSysnVisible;

	//
	static ulong				mSkelEntNum;
	static ulong				mCreateEntityAttNum;

	/// ������������
	std::vector<uint>			mLightZone;

	uint						mParticleHandle;
	std::map<uint, Particle*>	mParticleMap;
};



#endif
