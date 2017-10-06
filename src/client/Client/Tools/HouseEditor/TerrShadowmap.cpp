// TerrShadowmap.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "TerrShadowmap.h"

#include "SceneManager.h"
#include "SceneInfo.h"
#include "Terrain.h"
#include "TerrainData.h"
#include "SystemGcs.h"
#include "EQLog.h"
#include "CommFunc.h"


#define RENDERTARGET_SIZE   1024

typedef unsigned char Byte;

static float g_Density = 0.5;
// CTerrShadowmap dialog

IMPLEMENT_DYNAMIC(CTerrShadowmap, CDialog)

CTerrShadowmap::CTerrShadowmap(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrShadowmap::IDD, pParent)
	, mIsBitmapNeedUpdate(true)
	, mIsProcessingLightmap(true)
	, mIsLightmapUpdated(false)

{

}

CTerrShadowmap::~CTerrShadowmap()
{
}

void CTerrShadowmap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TSM_DENSITY, mDensityCtl);
}

extern std::pair<bool, Real>
rayCollide(const Ogre::Ray& ray,
		   Ogre::MovableObject* movable)
{
	// Get local space axis aligned bounding box
	const Ogre::AxisAlignedBox& aabb = movable->getBoundingBox();

	// Matrix4 to transform local space to world space
	const Ogre::Matrix4& localToWorld = movable->_getParentNodeFullTransform();

	// Matrix4 to transform world space to local space
	Ogre::Matrix4 worldToLocal = localToWorld.inverse();

	// Matrix3 to transform world space normal to local space
	Ogre::Matrix3 worldToLocalN;
	worldToLocal.extract3x3Matrix(worldToLocalN);

	// Convert world space ray to local space ray
	// Note:
	//      By preserving the scale between world space and local space of the
	//      direction, we don't need to recalculate the distance later.
	Ogre::Ray localRay;
	localRay.setOrigin(worldToLocal * ray.getOrigin());
	localRay.setDirection(worldToLocalN * ray.getDirection());

	// Intersect with axis aligned bounding box, but because we transformed
	// ray to local space of the bounding box, so this test just like test
	// with oriented bounding box.
	std::pair<bool, Real> ret = localRay.intersects(aabb);

	return ret;
}

// 此处传进的sunDirection已经经过normalise了
float CTerrShadowmap::IntersectObjects(Ogre::Viewport* pViewport, Ogre::HardwarePixelBuffer* pPixelBuffer,
													const Ogre::Ray& ray, const Ogre::RaySceneQueryResult& queryResult)
{
	float fResult = 1.0f;

	TerrainData* pTerrainData = EQSceneManager::getSingleton().getTerrainData();

	// 若只创建一次renderTarget,则在opengl下没有办法正确渲出阴影图
	// d3d在渲大地图时会无故死机（尚未找出原因）
	// 暂时只好每一次渲建筑阴影时都重新创建renderTarget;
	// 这里就用d3d先试试  --Y3
	for (Ogre::RaySceneQueryResult::const_iterator it = queryResult.begin(); it != queryResult.end(); ++it)
	{
		Ogre::MovableObject* movable = it->movable;
		if (movable->getMovableType() != Ogre::EntityFactory::FACTORY_TYPE_NAME)
		{
			continue;
		}

		//TODO: 以后需要对不同类型的物件做选择. 比如室内的家具就没必要处理

		const Ogre::AxisAlignedBox& boundingBox = movable->getWorldBoundingBox(true);
		if (boundingBox.isNull())
		{
			continue;
		}

		Ogre::Entity* pEntity = static_cast<Ogre::Entity*>(movable);

		std::pair<bool, Real> collideResult = rayCollide(ray, pEntity);
		if (collideResult.first)
		{
			PixelBox& pixelBox = mPixelBoxMap[pEntity->getName()];
			pixelBox.mIsNeedRemove = false;
			if (NULL == pixelBox.data)
			{
				Ogre::SceneManager* pSceneManager = SystemGcs::getSingleton().getSceneManager();
				Ogre::RenderSystem* pRenderSystem = Ogre::Root::getSingleton().getRenderSystem();
				Ogre::Camera* pCamera = pViewport->getCamera();

				Ogre::Viewport* pOldViewport = pRenderSystem->_getViewport();
				pRenderSystem->_setViewport(pViewport);

				Ogre::Vector3 center = boundingBox.getCenter();
				Ogre::Real radius = std::max(
					(boundingBox.getMaximum() - center).length(),
					movable->getBoundingRadius());

				// Setup camera
				Ogre::Real nearClip = RENDERTARGET_SIZE / 2;
				const Ogre::Real adj = 10;
				pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
				//pCamera->setFOVy(Ogre::Degree(90.0f));
				pCamera->setOrthoWindow(nearClip * 2, nearClip * 2);
				pCamera->setNearClipDistance(nearClip);
				pCamera->setFarClipDistance(nearClip + adj + radius * 2);
				pCamera->setPosition(center + ray.getDirection() * (nearClip + adj + radius));
				pCamera->setDirection(-ray.getDirection());

				Ogre::Real fScale;
				Ogre::Rectangle rectangle;
				Ogre::Matrix4 viewProjMatrix = pCamera->getProjectionMatrix() * pCamera->getViewMatrix();
				{
					//Ogre::Vector3 corners[8];
					const Ogre::Vector3* corners = boundingBox.getAllCorners();
					Ogre::Vector3 vector = viewProjMatrix * corners[0];
					rectangle.left = rectangle.right  = vector.x;
					rectangle.top  = rectangle.bottom = vector.y;
					for (int j = 1; j < 8; j++)
					{
						vector = viewProjMatrix * corners[j];
						if (vector.x < rectangle.left)
						{
							rectangle.left = vector.x;
						}
						if (vector.x > rectangle.right)
						{
							rectangle.right = vector.x;
						}
						if (vector.y < rectangle.top)
						{
							rectangle.top = vector.y;
						}
						if (vector.y > rectangle.bottom)
						{
							rectangle.bottom = vector.y;
						}
					}
					fScale = std::max(rectangle.right - rectangle.left, rectangle.bottom - rectangle.top);
					//fScale /= 2.0f;
				}

				pixelBox.mViewProjMatrix = viewProjMatrix;
				Ogre::Vector3 centerProj = viewProjMatrix * center;
				pixelBox.mCenter = centerProj;
				Ogre::RenderOperation renderOperation;
				unsigned short numWorldTransforms;
				std::vector<Ogre::Matrix4> matrices;
				Ogre::Pass* pPass;

				nearClip = RENDERTARGET_SIZE / 2 * fScale;
				pCamera->setOrthoWindow(nearClip*2, nearClip*2);
				pCamera->setNearClipDistance(nearClip);
				pCamera->setFarClipDistance(nearClip + adj + radius * 2);
				pCamera->setPosition(center + ray.getDirection() * (nearClip + adj + radius));
				pCamera->setDirection(-ray.getDirection());

				pRenderSystem->clearFrameBuffer(
					pViewport->getClearBuffers(), 
					Ogre::ColourValue::White);
				pRenderSystem->_beginFrame();
				for (unsigned int i = 0; i < pEntity->getNumSubEntities(); ++i)
				{
					Ogre::SubEntity* pSubEntity = pEntity->getSubEntity(i);
					Ogre::MaterialPtr clonedMaterial = pSubEntity->getMaterial()->clone("TLOD<>");
					clonedMaterial->load();
					pSubEntity->getRenderOperation(renderOperation);
					numWorldTransforms = pSubEntity->getNumWorldTransforms();
					matrices.resize(numWorldTransforms);
					pSubEntity->getWorldTransforms(&matrices[0]);
					pPass = clonedMaterial->getBestTechnique()->getPass(0);
					pPass->setLightingEnabled(true);
					pPass->setFog(true);
					pPass->setAmbient(0.0f, 0.0f, 0.0f);
					pPass->setDiffuse(0.0f, 0.0f, 0.0f, pPass->getDiffuse().a);
					pPass->setSpecular(0.0f, 0.0f, 0.0f, 1.0f);
					pSceneManager->manualRender(
						&renderOperation, pPass, pViewport,
						matrices[0], pCamera->getViewMatrix(), pCamera->getProjectionMatrixRS(), false);
					Ogre::MaterialManager::getSingleton().remove(clonedMaterial->getHandle());
				}
				pRenderSystem->_endFrame();

				// Ogre OpenGL render system need change the render target for
				// render to texture up-to-date
				pRenderSystem->_setViewport(pOldViewport);

				rectangle.left -= centerProj.x;
				rectangle.top -= centerProj.y;
				rectangle.right -= centerProj.x;
				rectangle.bottom -= centerProj.y;
				fScale /= RENDERTARGET_SIZE / 2;
				rectangle.left /= fScale;
				rectangle.top /= fScale;
				rectangle.right /= fScale;
				rectangle.bottom /= fScale;

				pixelBox.mRectangle = rectangle;
				pixelBox.mScale = fScale;
				long left = rectangle.left + RENDERTARGET_SIZE / 2 + 0.5f;
				long top = rectangle.top + RENDERTARGET_SIZE / 2 + 0.5f;
				long right = rectangle.right + RENDERTARGET_SIZE / 2 + 0.5f;
				long bottom = rectangle.bottom + RENDERTARGET_SIZE / 2 + 0.5f;
				Ogre::Image::Box box(left, top, right, bottom);
				pixelBox = box;
				pixelBox.setConsecutive();
				pixelBox.format = Ogre::PF_L8;
				pixelBox.data = new unsigned char[(box.right - box.left) * (box.bottom - box.top)];
				//pPixelBuffer->blitToMemory(box, pixelBox);



				// D3D 在这里blitToMemory搓的可以,怪不得导致当机
				Ogre::PixelBox pb(RENDERTARGET_SIZE, RENDERTARGET_SIZE, 1, Ogre::PF_L8);
				pb.data = new unsigned char[pb.getConsecutiveSize()];
				Ogre::Image::Box b(0,0,RENDERTARGET_SIZE,RENDERTARGET_SIZE);
				pPixelBuffer->blitToMemory(b, pb);

				Ogre::PixelBox npb = pb.getSubVolume(Ogre::Box(left, top, right, bottom));
				assert(npb.getConsecutiveSize()==(box.right - box.left) * (box.bottom - box.top));

				uchar* dst = (uchar*)pixelBox.data;
				uchar* src = (uchar*)npb.data;
				size_t rowSize = box.getWidth();
				for ( int y = top; y < bottom; ++y )
				{
					memcpy(dst, src, rowSize);

					src += npb.rowPitch;
					dst += rowSize;
				}
				

				delete[] pb.data;


				pixelBox.mName = pEntity->getName();
				mPixelBoxPointerVector.push_back(&pixelBox);

#if 0
				Ogre::Image newImage;
				newImage.loadDynamicImage((Ogre::uchar *)pixelBox.data,box.right - box.left,box.bottom - box.top,1,Ogre::PF_L8);
				newImage.save("D:\\temp.png");
#endif

			}

			Ogre::Vector3 position = ray.getOrigin();
			int minX = INT_MAX, maxX = INT_MIN;
			int minY = INT_MAX, maxY = INT_MIN;
			for (int k1 = 0; k1 < 2; k1++, position.y += (float)pTerrainData->mScale.y / 16)	//8改16
			{
				position.x = ray.getOrigin().x;
				for (int k0 = 0; k0 < 2; k0++, position.x += (float)pTerrainData->mScale.x / 16)
				{
					Ogre::Vector3 projPosition = pixelBox.mViewProjMatrix * position;
					projPosition.x -= pixelBox.mCenter.x;
					projPosition.y -= pixelBox.mCenter.y;
					projPosition /= pixelBox.mScale;

					int x = (int)(projPosition.x - pixelBox.mRectangle.left + 0.5f);
					int y = pixelBox.getHeight() - (int)(projPosition.y - pixelBox.mRectangle.top + 0.5f);
					if (x < minX) minX = x;
					if (x >= maxX) maxX = x + 1;
					if (y < minY) minY = y;
					if (y >= maxY) maxY = y + 1;
				}
			}
			if (minX < 0) minX = 0;
			if (minY < 0) minY = 0;
			if (maxX > (int)pixelBox.getWidth()) maxX = pixelBox.getWidth();
			if (maxY > (int)pixelBox.getHeight()) maxY = pixelBox.getHeight();

			if (maxX > minX && maxY > minY)
			{
				int sum = 0;
				for (int y = minY; y < maxY; y++)
				{
					for (int x = minX; x < maxX; x++)
					{
						sum += *((unsigned char *)pixelBox.data + y * pixelBox.rowPitch + x);
					}
				}
				float fLight = (float)sum / ((maxX - minX) * (maxY - minY) * 255);
				fResult *= fLight;
			}
		}
	}

	 return fResult;
}

bool CTerrShadowmap::PrepareLights()
{
	mLightPositions.clear();
	mLightColors.clear();

	//
	Ogre::Vector4 pos;

	Ogre::Light* l = AmbientMgr::getSingleton().getMainLight();
	assert(l);
	pos.x = -l->getDirection().x;
	pos.y = -l->getDirection().y;
	pos.z = -l->getDirection().z;
	pos.w = 0;

	CString cstr;
	mDensityCtl.GetWindowText(cstr);
	std::string str = cstr.GetBuffer();
	float d = Ogre::StringConverter::parseReal(str);
	if ( d <= 0.0 || d >= 1.0 )
	{
		MessageBox("阴影浓度在0~1之间", "错误");
		return false;
	}

	g_Density = d;


	//
	mLightPositions.push_back(pos);
	mLightColors.push_back(1.0-d);

	return true;
}

BEGIN_MESSAGE_MAP(CTerrShadowmap, CDialog)
	ON_BN_CLICKED(IDC_TSM_UPDATE_SHADOWMAP, &CTerrShadowmap::OnBnClickedTsmUpdateShadowmap)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TSM_OPT1, &CTerrShadowmap::OnBnClickedTsmOpt1)
	ON_BN_CLICKED(IDC_TSM_OPT2, &CTerrShadowmap::OnBnClickedTsmOpt2)
	ON_BN_CLICKED(IDC_TSM_OPT3, &CTerrShadowmap::OnBnClickedTsmOpt3)
END_MESSAGE_MAP()


// CTerrShadowmap message handlers

void CTerrShadowmap::OnBnClickedTsmUpdateShadowmap()
{
	if ( !PrepareLights() )
		return;

	memset(mLightmapImage.getData(), 0xFF, mLightmapImage.getSize());

	if (mLightPositions.size() == 0)
	{
		mIsBitmapNeedUpdate = true;
		mIsLightmapUpdated = true;
		MessageBox("None of directional lights cast shadow!", "Terrain Shadowmap");
		return;
	}


	TerrainData* pTerrainData = EQSceneManager::getSingleton().getTerrainData();
	const SceneInfo* pSceneInfo = EQSceneManager::getSingleton().getSceneInfo();
	const Terrain* pTerrain = EQSceneManager::getSingleton().getTerrain();
	Ogre::SceneManager* pSceneManager = SystemGcs::getSingleton().getSceneManager();

	// Create RTT
	Ogre::TexturePtr mTexture = Ogre::TextureManager::getSingleton().createManual(
		"TLOD<>", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		RENDERTARGET_SIZE, RENDERTARGET_SIZE, 1, 0, Ogre::PF_L8,
		Ogre::TU_RENDERTARGET, 0);
	Ogre::HardwarePixelBufferSharedPtr mPixelBuffer = mTexture->getBuffer(0, 0);
	Ogre::RenderTexture* mRenderTexture = mPixelBuffer->getRenderTarget(0);
	mRenderTexture->setAutoUpdated(false);

	// Create camera and viewport
	Ogre::Camera* mCamera = pSceneManager->createCamera("TLOD<>");
	Ogre::Viewport* mViewport = mRenderTexture->addViewport(mCamera);
	mViewport->setBackgroundColour(Ogre::ColourValue::White);
	mViewport->setShadowsEnabled(false);
	mViewport->setOverlaysEnabled(false);
	mViewport->setSkiesEnabled(false);

	Ogre::RaySceneQuery* pQuery = pSceneManager->createRayQuery(Ogre::Ray());

	int lightmapWidth = mLightmapImage.getWidth();
	int lightmapHeight = mLightmapImage.getHeight();

	float fColour;

	std::vector<float>::iterator itColor, itColor0;
	std::vector<Ogre::Vector4>::iterator itPosition;
	int l;

	int tmpPro = clock();

	for (itPosition = mLightPositions.begin(), itColor = mLightColors.begin(), l = 0;
		itPosition != mLightPositions.end() && mIsProcessingLightmap;
		++itPosition, ++itColor, ++l)
	{
		Byte* pyLight = mLightmapImage.getData();

		for (std::vector<PixelBox*>::iterator itPixelBoxPointer = mPixelBoxPointerVector.begin();
			itPixelBoxPointer != mPixelBoxPointerVector.end();
			++itPixelBoxPointer)
		{
			if (NULL != *itPixelBoxPointer && NULL != (*itPixelBoxPointer)->data)
			{
				delete[] (*itPixelBoxPointer)->data;
				(*itPixelBoxPointer)->data = NULL;
			}
		}
		mPixelBoxMap.clear();
		mPixelBoxPointerVector.clear();


		for (int z = 0; z < pTerrainData->mZSize && mIsProcessingLightmap; z++)
		{
			for (float tz = 0.0f; tz < 1.0f && mIsProcessingLightmap; tz += 0.125f)
			{
				for (std::vector<PixelBox*>::iterator itPixelBoxPointer = mPixelBoxPointerVector.begin();
					itPixelBoxPointer != mPixelBoxPointerVector.end();
					++itPixelBoxPointer)
				{
					if (NULL != *itPixelBoxPointer)
					{
						(*itPixelBoxPointer)->mIsNeedRemove = true;
					}
				}
				for (int x = 0; x < pTerrainData->mXSize && mIsProcessingLightmap; x++)
				{
					for (float tx = 0.0f; tx < 1.0f && mIsProcessingLightmap; tx += 0.125f)
					{
						Ogre::Ray ray;
						Ogre::Vector3 pos = pTerrainData->getPositionAtGrid(x + tx, z + tz);
						Ogre::Vector3 dir = Ogre::Vector3(itPosition->x, itPosition->y, itPosition->z) - pos * itPosition->w;
						dir.normalise();
						pos += dir * 0.01;  // Adjust position so never intersect with itself
						ray.setOrigin(pos);
						ray.setDirection(dir);

						// Collide with terrain
						if (!pTerrainData->getIntersects(ray, pos, NULL, false, true, true))
						{
							pQuery->setRay(ray);
							const Ogre::RaySceneQueryResult& queryResult = pQuery->execute();
							if (!queryResult.empty())
							{
								// 根据美术要求，阴影浓度只与参予计算的灯光的亮度值有关
								fColour = (1.0f - IntersectObjects(mViewport, mPixelBuffer.get(), ray, queryResult)) * (1.0f - *itColor);
								int nColour = (int)(fColour * 255 + 0.5f);
								if (*(pyLight) - nColour < 0)
								{
									*pyLight = 0;
								}
								else
								{
									*pyLight -= nColour;
								}
							}
						}
						else
						{
							// 根据美术要求，阴影浓度只与参予计算的灯光的亮度值有关
							fColour = (1.0f - *itColor);
							int nColour = (int)(fColour * 255 + 0.5f);
							if (*(pyLight) - nColour < 0)
							{
								*pyLight = 0;
							}
							else
							{
								*pyLight -= nColour;
							}
						}

						pyLight++;

						//这里可以考虑加入消息循环
					}
				}
				for (std::vector<PixelBox*>::iterator itPixelBoxPointer = mPixelBoxPointerVector.begin();
					itPixelBoxPointer != mPixelBoxPointerVector.end();
					++itPixelBoxPointer)
				{
					if (NULL != *itPixelBoxPointer && (*itPixelBoxPointer)->mIsNeedRemove)
					{
						if (NULL != (*itPixelBoxPointer)->data)
						{
							delete[] (*itPixelBoxPointer)->data;
							(*itPixelBoxPointer)->data = NULL;
						}
						mPixelBoxMap.erase((*itPixelBoxPointer)->mName);
						*itPixelBoxPointer = NULL;
					}
				}
			}
			int maxProgress = mLightPositions.size() * pTerrainData->mZSize;
			int progress = (int)(((float)l * pTerrainData->mZSize + z) / maxProgress * 99.0f + 0.5f);
			
			if ( tmpPro + 5000 < clock() )
			{
				LOG("阴影贴图 " + Ogre::StringConverter::toString(progress) + "%" );
				tmpPro += 5000;
			}
		}
	}

	pSceneManager->destroyQuery(pQuery);
	pSceneManager->destroyCamera(mCamera);
	Ogre::TextureManager::getSingleton().remove(mTexture->getHandle());
	mPixelBuffer.setNull();
	mTexture.setNull();

//#if 0

	if (lightmapHeight > 2 && lightmapWidth > 2)
	{
		// 对阴影图做n次模糊
		for (int i = 0; i < 2 && mIsProcessingLightmap; i++)
		{
			Byte* pyLine = mLightmapImage.getData() + lightmapWidth + 1;
			for (int z = 1; z < lightmapHeight - 1 && mIsProcessingLightmap; z++, pyLine += lightmapWidth)
			{
				_asm
				{
					xor ebx,    ebx;                        ;   ebx = 0;
					mov edi,    pyLine;                     ;   edi = pyLine;
					mov edx,    lightmapWidth;              ;   edx = lightmapWidth;(pitch)
						mov esi,    edx;                        ;   esi = lightmapWidth;
					neg esi;                                ;   esi = -esi;         (-pitch)
						mov ecx,    lightmapWidth;              ;   ecx = lightmapWidth;
					sub ecx,    2;                          ;   ecx -= 2;
blur:               xor ax,     ax;                         ;   ax = 0;
					mov al,     [edi + 0xFFFFFFFF];         ;   ax = *(pyLine - 1);
					mov bl,     [edi + 1];                  ;   bx = *(pyLine + 1);
					add ax,     bx;                         ;   ax += bx;
					mov bl,     [edi + esi];                ;   bx = *(pyLine - pitch);
					add ax,     bx;                         ;   ax += bx;
					mov bl,     [edi + edx];                ;   bx = *(pyLine + pitch);
					add ax,     bx;                         ;   ax += bx;
					shr ax,     2;                          ;   ax /= 4;
					mov [edi],  al;                         ;   *pyLine = ax;
					inc edi;                                ;   pyLine++;
					dec ecx;                                ;   ecx--;
					jnz blur;                               ;   if (0 != ecx) goto blur;
				}
			}
			//这里可以考虑加入消息循环

			// 模糊算法效果不理想啊, 尤其在边缘处理的时候, 以后考虑更好的算法
			//Byte* tmpBuf = new Byte[mLightmapImage.getSize()];
			////memcpy(tmpBuf, mLightmapImage.getData(), mLightmapImage.getSize());
			//Byte* pyLine = tmpBuf + lightmapWidth + 1;
			//Byte* sLine = mLightmapImage.getData() + lightmapWidth + 1;
			//for (int z = 1; z < lightmapHeight - 1 && mIsProcessingLightmap; z++, pyLine += lightmapWidth, sLine+=lightmapWidth)
			//{
			//	Byte* py = pyLine;
			//	Byte* s = sLine;
			//	for (int x = 1; x < lightmapWidth - 1; x++, py++, s++)
			//	{
			//		int a = 0;
			//		a += *(s-1);
			//		a += *(s+1);
			//		a += *(s-lightmapWidth);
			//		a += *(s+lightmapWidth);
			//		a >>= 2;
			//		*py = (Byte)a;
			//	}
			//}
			//memcpy(mLightmapImage.getData(), tmpBuf, mLightmapImage.getSize());
			//delete[] tmpBuf;
		}
	}
//#endif

	LOG("阴影贴图 100%" );

	MessageBox("阴影贴图更新完毕!", "提示", MB_ICONINFORMATION );
}

BOOL CTerrShadowmap::OnInitDialog()
{

	CDialog::OnInitDialog();

	mDensityCtl.SetWindowText(Ogre::StringConverter::toString(g_Density).c_str());

	//
	if ( EQSceneManager::getSingleton().getSceneInfo()->isTerrainEmpty() )
		return FALSE;
	
	Terrain* pTerrain = EQSceneManager::getSingleton().getTerrain();
	TerrainData* pTerrainData = pTerrain->getData();

	mLightmapImage.loadDynamicImage(0, pTerrainData->mXSize * 8, pTerrainData->mZSize * 8, 1, Ogre::PF_L8);
	Ogre::uchar* newImageData = OGRE_ALLOC_T(Ogre::uchar, mLightmapImage.getSize(), Ogre::MEMCATEGORY_GENERAL);
	mLightmapImage.loadDynamicImage(newImageData, pTerrainData->mXSize * 8, pTerrainData->mZSize * 8, 1, Ogre::PF_L8, true);

	if (pTerrainData->mLightmapImage)
	{
		if (mLightmapImage.getWidth() == pTerrainData->mLightmapImage->getWidth() &&
			mLightmapImage.getHeight() == pTerrainData->mLightmapImage->getHeight() &&
			mLightmapImage.getDepth() == pTerrainData->mLightmapImage->getDepth())
		{
			Ogre::PixelUtil::bulkPixelConversion(pTerrainData->mLightmapImage->getPixelBox(), mLightmapImage.getPixelBox());
		}
		else
		{
			Ogre::Image::scale(pTerrainData->mLightmapImage->getPixelBox(), mLightmapImage.getPixelBox());
		}
	}
	else
	{
		memset(mLightmapImage.getData(), 0xFF, mLightmapImage.getSize());

		if ( !pTerrainData->mLightmapFilename.empty() 
			&& Ogre::StringUtil::endsWith(pTerrainData->mLightmapFilename, ".png")
			&& ExistResource(pTerrainData->mLightmapFilename) )
		{
			Ogre::Image img;
			img.load(OpenResource(pTerrainData->mLightmapFilename));

			if (img.getDepth() != 1 ||
				img.getNumFaces() != 1 ||
				img.getWidth() != pTerrainData->mXSize * 8 ||
				img.getHeight() != pTerrainData->mZSize * 8)
			{
				//LOGWAR("Terrain lightmap 格式不合法!");
			}
			else
			{
				mLightmapImage = img;
			}
		}
	}

	mLightmapQuality = pTerrain->getLightmapQuality();

	if ( mLightmapQuality == Terrain::LMQ_NONE )
		((CButton *)GetDlgItem(IDC_TSM_OPT1))->SetCheck(TRUE);
	else if ( mLightmapQuality == Terrain::LMQ_LOW )
		((CButton *)GetDlgItem(IDC_TSM_OPT2))->SetCheck(TRUE);
	else if ( mLightmapQuality == Terrain::LMQ_HIGH )
		((CButton *)GetDlgItem(IDC_TSM_OPT3))->SetCheck(TRUE);
	else
		assert(0);



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrShadowmap::OnClose()
{
	for (std::vector<PixelBox*>::iterator itPixelBoxPointer = mPixelBoxPointerVector.begin();
		itPixelBoxPointer != mPixelBoxPointerVector.end();
		++itPixelBoxPointer)
	{
		if (NULL != *itPixelBoxPointer && NULL != (*itPixelBoxPointer)->data)
		{
			delete[] (*itPixelBoxPointer)->data;
			(*itPixelBoxPointer)->data = NULL;
		}
	}
	mPixelBoxMap.clear();
	mPixelBoxPointerVector.clear();

	CDialog::OnClose();
}

void CTerrShadowmap::OnBnClickedTsmOpt1()
{
	mLightmapQuality = Terrain::LMQ_NONE;
}

void CTerrShadowmap::OnBnClickedTsmOpt2()
{
	mLightmapQuality = Terrain::LMQ_LOW;
}

void CTerrShadowmap::OnBnClickedTsmOpt3()
{
	mLightmapQuality = Terrain::LMQ_HIGH;
}
