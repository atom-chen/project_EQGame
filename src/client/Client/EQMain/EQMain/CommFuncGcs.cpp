#include "EQMainPCH.h"

#include "CommFuncGcs.h"
#include "SystemGcs.h"

//--------------------------------------------------------------------------
//内部的
//--------------------------------------------------------------------------
static Ogre::AxisAlignedBox _getSkeletonBoundingBox(Ogre::Skeleton* skeleton)
{
	Ogre::AxisAlignedBox boundingBox;

	skeleton->reset();

	Ogre::Skeleton::BoneIterator itBone = skeleton->getBoneIterator();
	while (itBone.hasMoreElements())
	{
		Ogre::Bone* bone = itBone.getNext();
		const Ogre::Vector3& pos = bone->_getDerivedPosition();
		boundingBox.merge(pos);
	}

	// reset skeleton
	skeleton->reset();

	return boundingBox;
}
//--------------------------------------------------------------------------
class NullSkeletonMeshLoader : public Ogre::ManualResourceLoader
{
public:
	void loadResource(Ogre::Resource* resource)
	{
		Ogre::Mesh* mesh = static_cast<Ogre::Mesh*>(resource);
		mesh->setAutoBuildEdgeLists(false);
		mesh->setSkeletonName(mesh->getName());
	}
};

static NullSkeletonMeshLoader g_NullSkeletonMeshLoader;
//--------------------------------------------------------------------------
NullManualResourceLoader g_NullManualResourceLoader;
EQMeshSerializerListener g_EQMeshSerializerListener;



//--------------------------------------------------------------------------
//外部的
//--------------------------------------------------------------------------
void EQMeshSerializerListener::processMaterialName(Ogre::Mesh *mesh, std::string *name)
{

}
//--------------------------------------------------------------------------
void EQMeshSerializerListener::processSkeletonName(Ogre::Mesh *mesh, std::string *name)
{
	assert( !mesh->isManuallyLoaded() );	//不可是是手工创建

	const std::string& mshName = mesh->getName();

	std::string mshBase;
	std::string mshPath;
	Ogre::StringUtil::splitFilename(mshName, mshBase, mshPath);

	if (mshPath.empty())	//如果没有路径,返回
		return;

	//*name是不允许有路径的
	//std::string skeBase;
	//std::string skePath;
	//Ogre::StringUtil::splitFilename(*name, skeBase, skePath);
	//if (!skePath.empty())

	std::string newName = mshPath + *name;
	if ( !Ogre::ResourceGroupManager::getSingleton().resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, newName) )
		return;

	*name = newName;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
Ogre::MeshPtr loadNullMeshForSkeleton(const Ogre::SkeletonPtr& skeleton, Ogre::AxisAlignedBox boundingBox)
{
	Ogre::MeshManager& mm = Ogre::MeshManager::getSingleton();

	Ogre::MeshPtr mesh = mm.getByName(skeleton->getName());
	if (mesh.isNull())
	{
		mesh = mm.createManual(skeleton->getName(), skeleton->getGroup(), &g_NullSkeletonMeshLoader);
		mesh->load();
		if ( boundingBox == Ogre::AxisAlignedBox::BOX_NULL )
			mesh->_setBounds(_getSkeletonBoundingBox(mesh->getSkeleton().get()));
		else
			mesh->_setBounds(boundingBox);
	}

	return mesh;
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
SimpleCreateModelRule::SimpleCreateModelRule( const std::string& meshName, const std::string& matName, uint particle ) :
mMeshName(meshName),
mMatName(matName),
mParticle(particle)
{

}
//--------------------------------------------------------------------------
void SimpleCreateModelRule::create( ModelGcs * pModel )
{
	pModel->_createMainNode( SystemGcs::getSingleton().getSceneManager()->getRootSceneNode() );

	pModel->addEntity( mMeshName, mMatName );
	if(mParticle)
		pModel->addParticle(mParticle);
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
Ogre::ColourValue extractColorFromString( const std::string& str )
{
	Ogre::ColourValue colour;

	if ( str.size() != 7 && str.size() != 9 )
		return colour;

	if ( str[0] != '#' )
		return colour;

	char buf[3];
	buf[2] = 0;
	long l;

	buf[0] = str[1];
	buf[1] = str[2];
	l = strtol(buf, NULL, 16);
	colour.r = l/255.0f;

	buf[0] = str[3];
	buf[1] = str[4];
	l = strtol(buf, NULL, 16);
	colour.g = l/255.0f;

	buf[0] = str[5];
	buf[1] = str[6];
	l = strtol(buf, NULL, 16);
	colour.b = l/255.0f;

	if ( str.size() == 7 )
		return colour;

	buf[0] = str[7];
	buf[1] = str[8];
	l = strtol(buf, NULL, 16);
	colour.a = l/255.0f;

	return colour;
}



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
Ogre::ManualObject* createSphere(Ogre::SceneManager *sceneMgr, const float r, const int nRings, const int nSegments)
{
	static int num = 0;
	++num;

	std::string name = "--ManualObjectSphere--[auto]--" + Ogre::StringConverter::toString(num) + "--";


	Ogre::ManualObject * manual = sceneMgr->createManualObject(name);
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	float fDeltaRingAngle = (Ogre::Math::PI / nRings);
	float fDeltaSegAngle = (2 * Ogre::Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			manual->position( x0, y0, z0);
			manual->normal(Ogre::Vector3(x0, y0, z0).normalisedCopy());
			manual->textureCoord((float) seg / (float) nSegments, (float) ring / (float) nRings);

			if (ring != nRings) {
				// each vertex (except the last) has six indicies pointing to it
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex);               
				manual->index(wVerticeIndex + nSegments);
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex + 1);
				manual->index(wVerticeIndex);
				wVerticeIndex ++;
			}
		}; // end for seg
	} // end for ring
	manual->end();

	return manual;
}
//--------------------------------------------------------------------------
Ogre::MeshPtr createSphereMesh(Ogre::SceneManager *sceneMgr, const float r, const int nRings, const int nSegments)
{
	static int num = 0;
	++num;

	std::string name = "--MeshPtrSphere--[auto]--" + Ogre::StringConverter::toString(num) + "--";


	Ogre::ManualObject* manual = createSphere(sceneMgr, r, nRings, nSegments);

	//
	Ogre::MeshPtr mesh = manual->convertToMesh(name);
	mesh->_setBounds( Ogre::AxisAlignedBox( Ogre::Vector3(-r, -r, -r), Ogre::Vector3(r, r, r) ), false );

	mesh->_setBoundingSphereRadius(r);
	unsigned short src, dest;
	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
	{
		mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
	}

	//删除
	sceneMgr->destroyManualObject(manual);

	return mesh;
}
//--------------------------------------------------------------------------
Ogre::MaterialPtr createPureColourMaterial(const Ogre::ColourValue& colour)
{
	Ogre::RGBA rgba;
	Ogre::Root::getSingleton().convertColourValue(colour, &rgba);

	Ogre::StringUtil::StrStreamType str;
	str << "--PureColour--" << std::hex << std::setfill('0') << std::setw(8) << rgba;
	Ogre::String name = str.str();

	Ogre::ResourcePtr res = Ogre::MaterialManager::getSingleton().getByName(name);
	if (!res.isNull())
		return res;

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->setAmbient(0, 0, 0);
	material->setDiffuse(0, 0, 0, colour.a);
	material->setSelfIllumination(colour);
	material->setFog(true, Ogre::FOG_NONE);
	if (colour.a != 1)
		material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	return material;
}
//--------------------------------------------------------------------------
Ogre::MaterialPtr createColourMaterial(const Ogre::ColourValue& colour)
{
	if (colour == Ogre::ColourValue::White)
		return Ogre::MaterialManager::getSingleton().getByName("BaseWhite");

	Ogre::RGBA rgba;
	Ogre::Root::getSingleton().convertColourValue(colour, &rgba);

	Ogre::StringUtil::StrStreamType str;
	str << "--CustomColour--" << std::hex << std::setfill('0') << std::setw(8) << rgba;
	Ogre::String name = str.str();

	Ogre::ResourcePtr res = Ogre::MaterialManager::getSingleton().getByName(name);
	if (!res.isNull())
		return res;

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->setAmbient(colour);
	material->setDiffuse(colour);
	if (colour.a != 1)
		material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	return material;
}
//--------------------------------------------------------------------------
Ogre::MaterialPtr createColourMaterial(const Ogre::ColourValue& colour, const Ogre::ColourValue& specular, Real shininess)
{
	if (shininess == 0 || specular == Ogre::ColourValue::Black)
		return createColourMaterial(colour);

	Ogre::RGBA rgba, rgbaSpecular;
	Ogre::Root::getSingleton().convertColourValue(colour, &rgba);
	Ogre::Root::getSingleton().convertColourValue(specular, &rgbaSpecular);

	Ogre::StringUtil::StrStreamType str;
	str << "--CustomColour--" << std::hex << std::setfill('0')
		<< std::setw(8) << rgba
		<< std::setw(0) << '_'
		<< std::setw(8) << rgbaSpecular
		<< std::setw(0) << '_' << shininess;
	std::string name = str.str();

	Ogre::ResourcePtr res = Ogre::MaterialManager::getSingleton().getByName(name);
	if (!res.isNull())
		return res;

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->setAmbient(colour);
	material->setDiffuse(colour);
	material->setSpecular(specular);
	material->setShininess(shininess);
	if (colour.a != 1)
		material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	return material;
}
//--------------------------------------------------------------------------
void fixupProjectiveMaterial(const Ogre::MaterialPtr& material, const Ogre::Frustum* frustum)
{
	// apply texture projective to each texture unit state
	Ogre::Material::TechniqueIterator ti = material->getTechniqueIterator();
	while (ti.hasMoreElements())
	{
		Ogre::Technique* technique = ti.getNext();
		Ogre::Technique::PassIterator pi = technique->getPassIterator();
		while (pi.hasMoreElements())
		{
			Ogre::Pass* pass = pi.getNext();
			Ogre::Pass::TextureUnitStateIterator tusi = pass->getTextureUnitStateIterator();
			while (tusi.hasMoreElements())
			{
				Ogre::TextureUnitState* tus = tusi.getNext();
				// Set projector only if texture addressing mode is clamp and doesn't
				// exists any texture effect that conflict with projective texturing
#if ((OGRE_VERSION_MAJOR << 16) | (OGRE_VERSION_MINOR << 8) | OGRE_VERSION_PATCH) >= 0x010100
				const Ogre::TextureUnitState::UVWAddressingMode& am = tus->getTextureAddressingMode();
				if (am.u == tus->TAM_CLAMP || am.u == tus->TAM_BORDER &&
					am.v == tus->TAM_CLAMP || am.v == tus->TAM_BORDER &&
					am.w == tus->TAM_CLAMP || am.w == tus->TAM_BORDER)
#else
				if (tus->getTextureAddressingMode() == tus->TAM_CLAMP)
#endif
				{
					typedef std::multimap<Ogre::TextureUnitState::TextureEffectType, Ogre::TextureUnitState::TextureEffect> EffectMap;
					const EffectMap& effects = tus->getEffects();
					if (effects.find(tus->ET_ENVIRONMENT_MAP) == effects.end())
					{
						tus->setProjectiveTexturing(true, frustum);
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------------------
Ogre::MaterialPtr createProjectiveMaterial(const std::string& materialTemplateName, const Ogre::Frustum* frustum)
{
	if (materialTemplateName.empty())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
			"You must give a material template name for create projective material.",
			"createProjectiveMaterial");
	}

	// Build the material from template
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(materialTemplateName);
	if (material.isNull())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND,
			"Can't found material with name " + materialTemplateName +
			"Have you forgotten to define it in a  .material script?",
			"createProjectiveMaterial");
	}

	material = material->clone(material->getName() + "/Projectived/" + frustum->getName());
	fixupProjectiveMaterial(material, frustum);

	return material;
}
//--------------------------------------------------------------------------
Ogre::ManualObject* createLine(Ogre::SceneManager *sceneMgr, const std::vector<Ogre::Vector3>& ptList, const Ogre::ColourValue& colour, Ogre::RenderOperation::OperationType ro)
{
	assert(ptList.size()>=2);

	static int num = 0;
	++num;

	std::string name = "--Line3D--[auto]--" + Ogre::StringConverter::toString(num) + "--";

	std::string matName;
	if (colour.a == 1.0f)
		matName = "SystemMat/Comm-2";
	else
		matName = "SystemMat/Comm-1";

	Ogre::ManualObject* manual = sceneMgr->createManualObject(name);

	manual->begin(matName, ro);

	for ( size_t i = 0; i < ptList.size(); ++i )
	{
		manual->position(ptList[i]);
		manual->colour(colour);
	}

	manual->end();

	return manual;
}
