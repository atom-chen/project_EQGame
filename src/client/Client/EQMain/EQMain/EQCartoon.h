/******************************************************************
* 作者: 麦_Mike
* 时间: 20010-7-29
* 描述: cartoon管理
*******************************************************************/
#ifndef EQCARTOON_H
#define EQCARTOON_H
#include <Ogre/Ogre.h>

class EQCartoon
{
public:
	static EQCartoon& getSingleton()
	{
		static EQCartoon singleton;
		return singleton;
	}
	//------------------------------------------------------------------------------
	void addCartoonPass(Ogre::MaterialPtr mat)
	{
		if (!mCartoonCustomPass)
		{
			// unsupported
			;
		}
		else
		{
			Ogre::Pass *pass = mat->getTechnique(0)->createPass();
			(*pass) = (*_deriveCartoonPass());
		}
	}
	//------------------------------------------------------------------------------
	void delCartoonPass(Ogre::MaterialPtr mat)
	{
		Ogre::Technique	*tech = mat->getTechnique(0);
		assert(tech);

		for (int i = 0; i < tech->getNumPasses(); i++)
		{
			if(tech->getPass(i)->getName() == "cartoon")
				tech->removePass(i);
		}
	}

private:
	Ogre::Pass			*mCartoonCustomPass;
	Ogre::GpuProgramParametersSharedPtr mCartoonCustomVPParams;
	Ogre::String		mCartoonCustomVPName;
	//------------------------------------------------------------------------------
	EQCartoon()
	{
		mCartoonCustomPass = 0;
		_setCartoonShaderMaterial("cartoonShader_01");
	}
	~EQCartoon(){}
	//------------------------------------------------------------------------------
	void _setCartoonShaderMaterial(const Ogre::String& name)
	{
		if (name.empty())
		{
			mCartoonCustomPass = 0;
		}
		else
		{
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(name);
			if (mat.isNull())
			{
				//LOGERR("no mater")
			}
			mat->load();
			if (!mat->getBestTechnique())
			{
				// unsupported
				mCartoonCustomPass = 0;
			}
			else
			{
				mCartoonCustomPass = mat->getBestTechnique()->getPass(0);
				mCartoonCustomPass->setName("cartoon");
				if (mCartoonCustomPass->hasVertexProgram())
				{
					// Save vertex program and params in case we have to swap them out
					mCartoonCustomVPName = 
						mCartoonCustomPass->getVertexProgramName();
					mCartoonCustomVPParams = 
						mCartoonCustomPass->getVertexProgramParameters();

				}
			}
		}
	}
	//---------------------------------------------------------------------
	Ogre::Pass* _deriveCartoonPass(const Ogre::Pass* pass=NULL)
	{
		if (NULL == mCartoonCustomPass)
		{
			return NULL;
		}
		Ogre::Pass* retPass = mCartoonCustomPass;

		// reset vp?
		if (mCartoonCustomPass->getVertexProgramName() !=
			mCartoonCustomVPName)
		{
			mCartoonCustomPass->setVertexProgram(
				mCartoonCustomVPName, false);
			if(mCartoonCustomPass->hasVertexProgram())
			{
				mCartoonCustomPass->setVertexProgramParameters(
					mCartoonCustomVPParams);
			}
		}
		return retPass;
	}
};

#define sEQCartoon EQCartoon::getSingleton()
#endif
