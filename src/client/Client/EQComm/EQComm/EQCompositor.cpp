#include "EQClientPCH.h"
#include "EQCompositor.h"


#include <OgreException.h>
#include <OgreCompositorManager.h>
#include <OgreCompositorInstance.h>


PostFilter::PostFilter(const String& name, const String& compositorName)
: mName(name)
, mCompositorName(compositorName)
, mViewport()
, mCompositorInstance()
{
}

PostFilter::~PostFilter()
{
	if (mCompositorInstance && mViewport)
	{
		Ogre::CompositorManager::getSingleton().removeCompositor(mViewport, mCompositorName);
	}
}

void PostFilter::setEnabled(bool enable)
{
	if (mCompositorInstance)
	{
		mCompositorInstance->setEnabled(enable);
	}
}

bool PostFilter::getEnabled(void) const
{
	return mCompositorInstance && mCompositorInstance->getEnabled();
}

void PostFilter::setParameter(const String& name, const String& value)
{
	OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
		"Parameter '" + name + "' doesn't exists.",
		"PostFilter::setParameter");
}

String PostFilter::getParameter(const String& name) const
{
	OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
		"Parameter '" + name + "' doesn't exists.",
		"PostFilter::getParameter");
}

void PostFilter::_init(Ogre::Viewport* vp)
{
	mViewport = vp;
	mCompositorInstance = Ogre::CompositorManager::getSingleton().addCompositor(vp, mCompositorName);
}

void PostFilter::_notifyViewportSizeChanged(void)
{
	if (mCompositorInstance)
	{
		if (mCompositorInstance->getEnabled())
		{
			mCompositorInstance->setEnabled(false);
			mCompositorInstance->setEnabled(true);
		}
	}
}

/*************************************************************************
GaussianListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
GaussianListener::GaussianListener()
{
}
//---------------------------------------------------------------------------
GaussianListener::~GaussianListener()
{
}
//---------------------------------------------------------------------------
void GaussianListener::notifyViewportSize(int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
	// Calculate gaussian texture offsets & weights
	float deviation = 3.0f;
	float texelSize = 1.0f / (float)std::min(mVpWidth, mVpHeight);

	// central sample, no offset
	mBloomTexOffsetsHorz[0][0] = 0.0f;
	mBloomTexOffsetsHorz[0][1] = 0.0f;
	mBloomTexOffsetsVert[0][0] = 0.0f;
	mBloomTexOffsetsVert[0][1] = 0.0f;
	mBloomTexWeights[0][0] = mBloomTexWeights[0][1] =
		mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
	mBloomTexWeights[0][3] = 1.0f;

	// 'pre' samples
	for(int i = 1; i < 8; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = Ogre::Math::gaussianDistribution(i, 0, deviation);
		mBloomTexWeights[i][3] = 1.0f;
		mBloomTexOffsetsHorz[i][0] = i * texelSize;
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = i * texelSize;
	}
	// 'post' samples
	for(int i = 8; i < 15; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
		mBloomTexWeights[i][3] = 1.0f;

		mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
	}
}
//---------------------------------------------------------------------------
void GaussianListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch(pass_id)
	{
	case 701: // blur horz
		{
			// horizontal bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
			fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
			fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	case 700: // blur vert
		{
			// vertical bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
			fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15);
			fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	}
}
//---------------------------------------------------------------------------
void GaussianListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}
//---------------------------------------------------------------------------

