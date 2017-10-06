#ifndef EQCOMPOSITOR
#define EQCOMPOSITOR

//---------------------------------------------------------------------------
class GaussianListener: public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	// Array params - have to pack in groups of 4 since this is how Cg generates them
	// also prevents dependent texture read problems if ops don't require swizzle
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexOffsetsVert[15][4];
public:
	GaussianListener();
	virtual ~GaussianListener();
	void notifyViewportSize(int width, int height);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};


class PostFilter
{
public:
	PostFilter(const String& name, const String& compositorName);
	virtual ~PostFilter();

public:

	virtual const String& getType(void) const = 0;

	const String& getName(void) const
	{
		return mName;
	}

	const String& getCompositorName(void) const
	{
		return mCompositorName;
	}

	Ogre::Viewport* getViewport(void) const
	{
		return mViewport;
	}

	Ogre::CompositorInstance* getCompositorInstance(void) const
	{
		return mCompositorInstance;
	}

	void setEnabled(bool enable);
	bool getEnabled(void) const;

	virtual void setParameter(const String& name, const String& value);
	virtual String getParameter(const String& name) const;

	virtual void _init(Ogre::Viewport* vp);

	virtual void _notifyViewportSizeChanged(void);

protected:
	String mName;
	String mCompositorName;
	Ogre::Viewport* mViewport;
	Ogre::CompositorInstance* mCompositorInstance;
};

#endif