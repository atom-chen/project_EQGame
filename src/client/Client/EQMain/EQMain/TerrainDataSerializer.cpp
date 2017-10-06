#include "EQMainPCH.h"
#include "TerrainData.h"
#include "CommFunc.h"

//////////////////////////////////////////////////////////////////////////
// All XML relative stuff place here, so we can replace another
// XML implementation easy
//////////////////////////////////////////////////////////////////////////

static void
readColourRGB(TiXmlElement* node, Ogre::ColourValue& colour)
{
    colour.r = xmlGetAttribFloat(node, "r");
    colour.g = xmlGetAttribFloat(node, "g");
    colour.b = xmlGetAttribFloat(node, "b");
}

static TiXmlElement*
writeColourRGB(TiXmlElement* node, const Ogre::ColourValue& colour)
{
    node->SetAttribute("r", colour.r);
    node->SetAttribute("g", colour.g);
    node->SetAttribute("b", colour.b);
    return node;
}



//////////////////////////////////////////////////////////////////////////

void
TerrainData::_loadBaseInfo(const std::string& filename, const std::string& groupName)
{

	if (filename.empty())
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
            "Filename supplied for terrain loading must be valid",
            "TerrainData::_loadBaseInfo");
    }

	std::string basename, path;
	Ogre::StringUtil::splitFilename(filename, basename, path);

	TiXmlElement *ele, *ele1, *ele2;
	TiXmlDocument doc;
	Ogre::DataStreamPtr stream = OpenResource(filename);	//groupName没用到
	doc.Parse( stream->getAsString().c_str() );

	TiXmlElement* root = doc.FirstChildElement("Terrain");
	if ( root == NULL )
	{
		LOGERR(filename + " is not a Terrain!");
		return;
	}

	mName = UTF8ToGB2312(xmlGetAttrib(root, "name").c_str());
	mXSize = xmlGetAttribInt(root, "xsize");
	mZSize = xmlGetAttribInt(root, "zsize");
	mTileSize = xmlGetAttribInt(root, "tileSize");

	//
	ele = root->FirstChildElement("scale");
	if( ele )
	{
		mScale = Ogre::Vector3(xmlGetAttribFloat(ele, "x"),
			xmlGetAttribFloat(ele, "y"),
			xmlGetAttribFloat(ele, "z"));

		if (mScale.x <= 0 || mScale.y <= 0 || mScale.z <= 0)
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Invalidate scale definition in terrain file",
				"TerrainData::_loadBaseInfo");
		}

		mPosition = Ogre::Vector3(- mScale.x * mXSize / 2,
			0,
			- mScale.z * mZSize / 2);
	}

	//
	ele = root->FirstChildElement("size");
	if ( ele )
	{
		//目前没用
	}

	//
	ele = root->FirstChildElement("position");
	if ( ele )
	{
		//目前没用
	}

	//
	ele = root->FirstChildElement("centre");
	if ( ele )
	{
		mPosition = Ogre::Vector3(xmlGetAttribFloat(ele, "x") - mScale.x * mXSize / 2,
			xmlGetAttribFloat(ele, "y"),
			xmlGetAttribFloat(ele, "z") - mScale.z * mZSize / 2);
	}

	//
	ele = root->FirstChildElement("heightmap");
	if ( ele )
	{
		const std::string& type = xmlGetAttrib(ele, "type", "standard");
		std::string& filename = UTF8ToGB2312(xmlGetAttrib(ele, "filename").c_str());
		if ( !path.empty() )
		{
			std::string b, p;
			Ogre::StringUtil::splitFilename(filename, b, p);
			if ( p.empty() )
				filename = path + b;
		}

		if (type != "image" && type != "standard" && type != "raw")
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Invalidate height data type",
				"TerrainData::_loadBaseInfo");
		}

		mHeightmapType = type;
		mHeightmapFilename = filename;
		if (mHeightmapFilename.empty())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Invalidate height data filename",
				"TerrainData::_loadBaseInfo");
		}
	}

	//
	ele = root->FirstChildElement("gridInfo");
	if ( ele )
	{
		const std::string& type = xmlGetAttrib(ele, "type", "standard");
		std::string& filename = UTF8ToGB2312(xmlGetAttrib(ele, "filename").c_str());
		if ( !path.empty() )
		{
			std::string b, p;
			Ogre::StringUtil::splitFilename(filename, b, p);
			if ( p.empty() )
				filename = path + b;
		}

		if (type != "standard")
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Invalidate height info type",
				"TerrainData::_loadBaseInfo");
		}

		mGridInfoFilename = filename;
		if (mGridInfoFilename.empty())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Invalidate grid info filename",
				"TerrainData::_loadBaseInfo");
		}
	}

	//
	ele = root->FirstChildElement("lightmap");
	if ( ele )
	{
		const std::string& type = xmlGetAttrib(ele, "type", "standard");
		std::string& filename = UTF8ToGB2312(xmlGetAttrib(ele, "filename").c_str());
		if ( !path.empty() )
		{
			std::string b, p;
			Ogre::StringUtil::splitFilename(filename, b, p);
			if ( p.empty() )
				filename = path + b;
		}

		if (type != "standard" &&
			type != "image")
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Invalidate lightmap type",
				"TerrainData::_loadBaseInfo");
		}

		mLightmapType = type;
		mLightmapFilename = filename;
		if (mLightmapFilename.empty())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Invalidate lightmap filename",
				"TerrainData::_loadBaseInfo");
		}
	}

	//
	ele = root->FirstChildElement("textures");
	if ( ele )
	{
		ele1 = ele->FirstChildElement("texture");
		while(ele1)
		{
			const std::string& type = xmlGetAttrib(ele1, "type", "image");
			const std::string& filename = UTF8ToGB2312(xmlGetAttrib(ele1, "filename").c_str());

			if (type != "image")
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
					"Invalidate texture type",
					"TerrainData::_loadBaseInfo");
			}

			mTextures.push_back(filename);

			ele1 = ele1->NextSiblingElement("texture");
		}
	}

	//
	ele = root->FirstChildElement("pixmaps");
	if ( ele )
	{
		ele1 = ele->FirstChildElement("pixmap");
		while(ele1)
		{
			TerrainData::Pixmap pixmap;
			pixmap.textureId = xmlGetAttribUint(ele1, "textureId");
			pixmap.left = xmlGetAttribFloat(ele1, "left", 0);
			pixmap.top = xmlGetAttribFloat(ele1, "top", 0);
			pixmap.right = xmlGetAttribFloat(ele1, "right", 1);
			pixmap.bottom = xmlGetAttribFloat(ele1, "bottom", 1);

			if (!(0 <= pixmap.left && pixmap.left <= 1 &&
				0 <= pixmap.top && pixmap.top <= 1 &&
				0 <= pixmap.right && pixmap.right <= 1 &&
				0 <= pixmap.bottom && pixmap.bottom <= 1))
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
					"Invalidate pixmap definition in terrain file <"
					+ Ogre::StringConverter::toString(pixmap.left) + " "
					+ Ogre::StringConverter::toString(pixmap.top) + " "
					+ Ogre::StringConverter::toString(pixmap.right) + " "
					+ Ogre::StringConverter::toString(pixmap.bottom) + ">",
					"TerrainData::_loadBaseInfo");
			}

			mPixmaps.push_back(pixmap);

			ele1 = ele1->NextSiblingElement("pixmap");
		}
	}

	//
	ele = root->FirstChildElement("materials");
	if ( ele )
	{
		ele1 = ele->FirstChildElement("template");
		while(ele1)
		{
			const std::string& name = xmlGetAttrib(ele1, "name");
			const std::string& material = xmlGetAttrib(ele1, "material");

			std::pair<TerrainData::MaterialTemplates::iterator, bool> inserted =
				mMaterialTemplates.insert(TerrainData::MaterialTemplates::value_type(
				name, material));
			if (!inserted.second)
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM,
					"Duplicate material template '" + inserted.first->first + "' in terrain file",
					"TerrainData_xmlHandler::startElement");
			}

			ele1 = ele1->NextSiblingElement("template");
		}

		ele1 = ele->FirstChildElement("fog_replacement");
		while(ele1)
		{
			TerrainData::FogReplacement fogReplacement;
			fogReplacement.programNames[Ogre::FOG_NONE] = xmlGetAttrib(ele1, "none");
			fogReplacement.programNames[Ogre::FOG_EXP] = xmlGetAttrib(ele1, "exp");
			fogReplacement.programNames[Ogre::FOG_EXP2] = xmlGetAttrib(ele1, "exp2");
			fogReplacement.programNames[Ogre::FOG_LINEAR] = xmlGetAttrib(ele1, "linear");
			mFogReplacements.push_back(fogReplacement);

			ele1 = ele1->NextSiblingElement("fog_replacement");
		}

		ele1 = ele->FirstChildElement("surface");
		if ( ele1 )
		{
			ele2 = ele1->FirstChildElement("ambient");
			if ( ele2 )
			{
				readColourRGB(ele2, mSurfaceParams.ambient);
			}

			ele2 = ele1->FirstChildElement("diffuse");
			if ( ele2 )
			{
				readColourRGB(ele2, mSurfaceParams.diffuse);
			}

			ele2 = ele1->FirstChildElement("specular");
			if ( ele2 )
			{
				readColourRGB(ele2, mSurfaceParams.specular);
				mSurfaceParams.shininess = xmlGetAttribFloat(ele2, "shininess", msDefaultSurfaceParams.shininess);
			}

			ele2 = ele1->FirstChildElement("emissive");
			if ( ele2 )
			{
				readColourRGB(ele2, mSurfaceParams.emissive);
			}
		}

	}
}

void
TerrainData::_saveBaseInfo(const std::string& filename) const
{
	if (filename.empty())
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
            "Filename supplied for terrain saving must be valid",
            "TerrainData::_saveBaseInfo");
    }

	TiXmlDocument doc;
	TiXmlElement *ele, *ele1;

	TiXmlDeclaration dec;
	dec.Parse( "<?xml version='1.0' encoding='UTF-8'standalone='yes' ?>", 0, TIXML_ENCODING_UNKNOWN );
	doc.InsertEndChild( dec );

	TiXmlElement* root = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement("Terrain")));
	root->SetAttribute("name", GB2312ToUTF8(mName.c_str()));
	root->SetAttribute("xsize", mXSize);
	root->SetAttribute("zsize", mZSize);
	root->SetAttribute("tileSize", mTileSize);

	if (mScale != Ogre::Vector3::UNIT_SCALE)
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("scale")));
		ele->SetDoubleAttribute("x", mScale.x);
		ele->SetDoubleAttribute("y", mScale.y);
		ele->SetDoubleAttribute("z", mScale.z);
	}

	if (mCentre != Ogre::Vector3::ZERO)
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("centre")));
		ele->SetDoubleAttribute("x", mCentre.x);
		ele->SetDoubleAttribute("y", mCentre.y);
		ele->SetDoubleAttribute("z", mCentre.z);
	}

	if (!mHeightmapFilename.empty())
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("heightmap")));
		ele->SetAttribute("filename", GB2312ToUTF8(mHeightmapFilename.c_str()));
		ele->SetAttribute("type", "standard");
	}

	if (!mGridInfoFilename.empty())
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("gridInfo")));
		ele->SetAttribute("filename", GB2312ToUTF8(mGridInfoFilename.c_str()));
		ele->SetAttribute("type", "standard");
	}

	if (!mLightmapFilename.empty())
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("lightmap")));
		ele->SetAttribute("filename", GB2312ToUTF8(mLightmapFilename.c_str()));
		ele->SetAttribute("type", mLightmapType);
	}

	if (!mTextures.empty())
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("textures")));
		for (TextureArray::const_iterator it = mTextures.begin(); it != mTextures.end(); ++it)
		{
			const std::string& texture = *it;
			ele1 = static_cast<TiXmlElement*>(ele->InsertEndChild(TiXmlElement("texture")));
			ele1->SetAttribute("filename", GB2312ToUTF8(texture.c_str()));
			ele1->SetAttribute("type", "image");
		}
	}

	if (!mPixmaps.empty())
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("pixmaps")));
		for (PixmapArray::const_iterator it = mPixmaps.begin(); it != mPixmaps.end(); ++it)
		{
			const Pixmap& pixmap = *it;
			ele1 = static_cast<TiXmlElement*>(ele->InsertEndChild(TiXmlElement("pixmap")));
			ele1->SetAttribute("textureId", pixmap.textureId);
			if (pixmap.left != 0 || pixmap.top != 0 || pixmap.right != 1 || pixmap.bottom != 1)
			{
				//这个函数会截断, 大概之保留6个有效位, 目前看来应该基本够用. --Y3
				ele1->SetDoubleAttribute("left", pixmap.left);
				ele1->SetDoubleAttribute("top", pixmap.top);
				ele1->SetDoubleAttribute("right", pixmap.right);
				ele1->SetDoubleAttribute("bottom", pixmap.bottom);
			}
		}
	}

	if (!mMaterialTemplates.empty() ||
		!mFogReplacements.empty() ||
		mSurfaceParams != msDefaultSurfaceParams)
	{
		ele = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("materials")));

		for (MaterialTemplates::const_iterator it = mMaterialTemplates.begin(); it != mMaterialTemplates.end(); ++it)
		{
			const std::string& name = it->first;
			const std::string& material = it->second;
			ele1 = static_cast<TiXmlElement*>(ele->InsertEndChild(TiXmlElement("template")));
			ele1->SetAttribute("name", name);
			ele1->SetAttribute("material", material);
		}

		for (FogReplacements::const_iterator it = mFogReplacements.begin(); it != mFogReplacements.end(); ++it)
		{
			const FogReplacement& fogReplacement = *it;
			ele1 = static_cast<TiXmlElement*>(ele->InsertEndChild(TiXmlElement("fog_replacement")));
			ele1->SetAttribute("none", fogReplacement.programNames[Ogre::FOG_NONE]);
			ele1->SetAttribute("exp", fogReplacement.programNames[Ogre::FOG_EXP]);
			ele1->SetAttribute("exp2", fogReplacement.programNames[Ogre::FOG_EXP2]);
			ele1->SetAttribute("linear", fogReplacement.programNames[Ogre::FOG_LINEAR]);
		}

		// 这部分, 其实不该有 --Y3
		if (mSurfaceParams != msDefaultSurfaceParams)
		{
			ele1 = static_cast<TiXmlElement*>(ele->InsertEndChild(TiXmlElement("surface")));

			if (mSurfaceParams.ambient != msDefaultSurfaceParams.ambient)
			{
				writeColourRGB(static_cast<TiXmlElement*>(ele1->InsertEndChild(TiXmlElement("ambient"))), mSurfaceParams.ambient);
			}

			if (mSurfaceParams.diffuse != msDefaultSurfaceParams.diffuse)
			{
				writeColourRGB(static_cast<TiXmlElement*>(ele1->InsertEndChild(TiXmlElement("diffuse"))), mSurfaceParams.diffuse);
			}

			if (mSurfaceParams.specular != msDefaultSurfaceParams.specular ||
				mSurfaceParams.shininess != msDefaultSurfaceParams.shininess)
			{
				writeColourRGB(static_cast<TiXmlElement*>(ele1->InsertEndChild(TiXmlElement("specular"))), mSurfaceParams.specular)
					->SetAttribute("shininess", mSurfaceParams.shininess);
			}

			if (mSurfaceParams.emissive != msDefaultSurfaceParams.emissive)
			{
				writeColourRGB(static_cast<TiXmlElement*>(ele1->InsertEndChild(TiXmlElement("emissive"))), mSurfaceParams.emissive);
			}
		}
	}

	doc.SaveFile(filename);


}

void
TerrainData::_fixupMissingInfo(void)
{
    if (mFogReplacements.empty())
    {
        FogReplacement fogReplacement;

        fogReplacement.programNames[Ogre::FOG_NONE] = "Terrain/OneLayer_ps";
        fogReplacement.programNames[Ogre::FOG_EXP] = "Terrain/OneLayer_ps%fog_exp";
        fogReplacement.programNames[Ogre::FOG_EXP2] = "Terrain/OneLayer_ps%fog_exp2";
        fogReplacement.programNames[Ogre::FOG_LINEAR] = "Terrain/OneLayer_ps%fog_linear";
        mFogReplacements.push_back(fogReplacement);

        fogReplacement.programNames[Ogre::FOG_NONE] = "Terrain/TwoLayer_ps";
        fogReplacement.programNames[Ogre::FOG_EXP] = "Terrain/TwoLayer_ps%fog_exp";
        fogReplacement.programNames[Ogre::FOG_EXP2] = "Terrain/TwoLayer_ps%fog_exp2";
        fogReplacement.programNames[Ogre::FOG_LINEAR] = "Terrain/TwoLayer_ps%fog_linear";
        mFogReplacements.push_back(fogReplacement);

        fogReplacement.programNames[Ogre::FOG_NONE] = "Terrain/OneLayerLightmap_ps";
        fogReplacement.programNames[Ogre::FOG_EXP] = "Terrain/OneLayerLightmap_ps%fog_exp";
        fogReplacement.programNames[Ogre::FOG_EXP2] = "Terrain/OneLayerLightmap_ps%fog_exp2";
        fogReplacement.programNames[Ogre::FOG_LINEAR] = "Terrain/OneLayerLightmap_ps%fog_linear";
        mFogReplacements.push_back(fogReplacement);

        fogReplacement.programNames[Ogre::FOG_NONE] = "Terrain/TwoLayerLightmap_ps";
        fogReplacement.programNames[Ogre::FOG_EXP] = "Terrain/TwoLayerLightmap_ps%fog_exp";
        fogReplacement.programNames[Ogre::FOG_EXP2] = "Terrain/TwoLayerLightmap_ps%fog_exp2";
        fogReplacement.programNames[Ogre::FOG_LINEAR] = "Terrain/TwoLayerLightmap_ps%fog_linear";
        mFogReplacements.push_back(fogReplacement);
    }
}

