/******************************************************************************
* 作者: 何展然
* 时间: 2010-11-26
* 描述: 动态创建imageset管理器
******************************************************************************/
#include "EQCommPCH.h"
#include "DynamicCreateImageset.h"

#define	TEXTURE_SIDE 512		// 正方形纹理的边长
#define TEXTRUE_AREA (TEXTURE_SIDE*TEXTURE_SIDE)	// 正方形纹理的大小


//-----------------------------------------------------------------------
// 单件
template<> DynamicCreateImagesetMgr* Ogre::Singleton<DynamicCreateImagesetMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
// 构造函数
DynamicCreateImagesetMgr::DynamicCreateImagesetMgr()
:mBuffer(NULL)
,mCurImageset(NULL)
,mImagesetNameFlag(0)
,mPos(0)
{
	mBuffer = new int[TEXTRUE_AREA];
	for (int i=0; i<TEXTRUE_AREA; i++)	// 缓冲区清0
	{
		*(mBuffer+i) = 0;
	}
}
//-----------------------------------------------------------------------
// 析构函数
DynamicCreateImagesetMgr::~DynamicCreateImagesetMgr()
{
	delete []mBuffer;
	mBuffer = NULL;
}
//-----------------------------------------------------------------------
// 保存图片
int DynamicCreateImagesetMgr::saveImage(std::string imageData, std::string imageName)
{
	if ("" != getImageName(imageData))		// 存在相同数据的图片
	{
		return -1;
	}
	if ("" != getImagesetName(imageName))	// 存在同名图像
	{
		return -2;
	}

	char *dataBuf = NULL;
	int len = base64Decode(imageData.c_str(), imageData.size(), &dataBuf);
	if (0 == len)
	{
		return -3;
	}
#pragma push_macro("new")
#undef new
	Ogre::DataStreamPtr odc(OGRE_NEW Ogre::MemoryDataStream((void*)dataBuf, len, false));
#pragma pop_macro("new")
	Ogre::Image img;
	img.load(odc);
	free(dataBuf);	// 释放内存，dataBuf在base64Decode函数里有调用malloc分配内存
	switch (_saveImage(img, imageName))
	{
	case -1:
		return -4;
	case -2:
		return -5;
	default:
		break;
	}
	mDataToImage.insert(std::make_pair(imageData, imageName));
	return 0;
}
//-----------------------------------------------------------------------
// 保存图片
int DynamicCreateImagesetMgr::saveImageEx(std::string imageset, std::string fileName)
{
	if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imageset))
	{
		return -1;
	}
	CEGUI::Imageset* set = CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile(imageset, fileName);
	if (NULL == set)
	{
		return -2;
	}
	CEGUI::Point imgPos(0.0f, 0.0f);		// 图片的坐标
	CEGUI::Size imgSize(set->getNativeResolution().d_width, set->getNativeResolution().d_height);		// 图片的大小
	CEGUI::Point imgOffset(0.0f, 0.0f);		// 偏移量
	set->defineImage(imageset, imgPos, imgSize, imgOffset);	// 定义图片
	mImageToSet.insert(std::make_pair(imageset, imageset));
	return 0;
}
//-----------------------------------------------------------------------
// 保存图片
int DynamicCreateImagesetMgr::_saveImage(Ogre::Image& img, std::string imageName)
{
	//img.save("D:\\"+imageName+".png");	// 保存图片到文件（测试用）
	switch (img.getFormat())	// 判断图片格式
	{
	case Ogre::PF_R8G8B8:
	case Ogre::PF_A8R8G8B8:
		break;
	default:
		return -1;
	}

	int width = img.getWidth();
	int height = img.getHeight();
	char buf[28] = "";
	_snprintf_s(buf, sizeof(buf), "%d*%d", width, height);
	std::string type = buf;
	if ((0==mImagesetNameFlag) || (type!=mCurTexture.type))	// 第一次动态创建imageset；图片所需纹理类型与当前纹理类型不一样
	{
		if (NULL == _createImageset(mImagesetNameFlag))	// 创建imageset失败
		{
			return -2;
		}
		mCurTexture.type = type;
	}

	if (-1 == _pasteToBuffer(mBuffer, mPos, img))	// 拷贝数据到纹理对应的内存
	{
		if (NULL == _createImageset(mImagesetNameFlag))
		{
			return -2;
		}
		_saveImage(img, imageName);
		return 0;
	}
	mCurTexture.tex->loadFromMemory((void*)mBuffer, TEXTURE_SIDE, TEXTURE_SIDE, CEGUI::Texture::PF_RGBA);	// 从内存加载
	float xPos = (mPos%(TEXTURE_SIDE/width))*width;
	float yPos = (mPos/(TEXTURE_SIDE/height))*height;
	CEGUI::Point imgPos(xPos, yPos);		// 图片的坐标
	CEGUI::Size imgSize(width, height);		// 图片的大小
	CEGUI::Point imgOffset(0.0f, 0.0f);		// 偏移量
	mCurImageset->defineImage(imageName, imgPos, imgSize, imgOffset);	// 定义图片
	mImageToSet.insert(std::make_pair(imageName, mCurImageset->getName().c_str()));
	mPos++;
	return 0;
}
//-----------------------------------------------------------------------
// 在缓冲区的相应位置粘贴图片
int DynamicCreateImagesetMgr::_pasteToBuffer(int* buf, int n, Ogre::Image img)
{
	int width = img.getWidth();
	int height = img.getHeight();
	if ((n<0) || (n>=_countImageNum(width, height)))	// n大于内存buf中最多能存储的数量
	{
		return -1;
	}
	int row = n/(TEXTURE_SIDE/height);	// 行
	int col = n%(TEXTURE_SIDE/width);	// 列
	int *bufTemp = buf + (row*TEXTURE_SIDE*height) + (col*width);
	for (int h=0; h<height; ++h)	// 计算行
	{
		int *p = bufTemp + (h*TEXTURE_SIDE);
		for (int w=0; w<width; ++w)	// 计算列
		{
			Ogre::ColourValue value = img.getColourAt(w, h, 0);
			CEGUI::colour col(value.r, value.g, value.b, value.a);
			*(p+w) = col;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------
// 动态创建imageset
CEGUI::Imageset* DynamicCreateImagesetMgr::_createImageset(int flag)
{
	CEGUI::Texture *tex = CEGUI::System::getSingleton().getRenderer()->createTexture(TEXTURE_SIDE);	// 创建一张边长为texSize的正方形纹理
	if (NULL == tex)
	{
		return NULL;
	}
	char buf[28] = "";
	_snprintf_s(buf, sizeof(buf), "%d", flag);
	std::string imagesetBaseName = "DynamicImageset_";
	std::string imagesetName = imagesetBaseName + buf;
	CEGUI::Imageset *imgSet =  CEGUI::ImagesetManager::getSingleton().createImageset(imagesetName, tex);	// 根据纹理创建imageset
	if (NULL == imgSet)
	{
		return NULL;
	}
	_setMemberVariable(tex, imgSet);
	return imgSet;
}
//-----------------------------------------------------------------------
// 设置成员变量
void DynamicCreateImagesetMgr::_setMemberVariable(CEGUI::Texture* tex, CEGUI::Imageset* imgSet)
{
	mCurTexture.tex = tex;
	mCurImageset = imgSet;
	for (int i=0; i<TEXTRUE_AREA; ++i)	// 缓冲区清0
	{
		*(mBuffer+i) = 0;
	}
	mImagesetNameFlag++;
	mPos = 0;
}
//-----------------------------------------------------------------------
// 计算一张纹理最多可以粘贴几张图片
int DynamicCreateImagesetMgr::_countImageNum(int width, int height)
{
	int row = TEXTURE_SIDE/height;
	int col = TEXTURE_SIDE/width;
	int num = (row <= col) ? row: col;
	return (num*num);
}
//-----------------------------------------------------------------------
// 根据imageName获取对应的imagesetName
std::string DynamicCreateImagesetMgr::getImagesetName(std::string imageName)
{
	ImageToSet::const_iterator iter = mImageToSet.find(imageName);
	if (mImageToSet.end() == iter)
	{
		return "";
	}
	return iter->second;
}
//-----------------------------------------------------------------------
// 根据imageData获取imageName
std::string DynamicCreateImagesetMgr::getImageName(std::string imageData)
{
	DataToImage::const_iterator iter = mDataToImage.find(imageData);
	if (mDataToImage.end() == iter)
	{
		return "";
	}
	return iter->second;
}
//-----------------------------------------------------------------------
// 销毁imageset
void DynamicCreateImagesetMgr::destroyImageset(std::string imagesetName)
{
	if (!CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imagesetName))
	{
		return;
	}
	if ((NULL!=mCurImageset) && (mCurImageset->getName()==imagesetName))
	{
		mCurImageset = NULL;
	}
	CEGUI::ImagesetManager::getSingleton().destroyImageset(imagesetName);
	ImageToSet::const_iterator iter = mImageToSet.begin();
	for (iter; mImageToSet.end()!=iter; ++iter)
	{
		if (iter->second == imagesetName)
		{
			DataToImage::const_iterator it = mDataToImage.begin();
			for (it; mDataToImage.end()!=it; ++it)
			{
				if (it->second == iter->first)
				{
					mDataToImage.erase(it);
					break;
				}
			}
			mImageToSet.erase(iter);
			break;
		}
	}
}
//-----------------------------------------------------------------------
// 设置资源组目录
void DynamicCreateImagesetMgr::setResourceGroupDirectory(std::string resourceGroup, std::string type, std::string section)
{
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation((char*)resourceGroup.c_str(), (char*)type.c_str(), (char*)section.c_str());
}
//-----------------------------------------------------------------------
// 保存图片并设置到指定窗口中
int DynamicCreateImagesetMgr::setWindowImage(std::string winname, std::string imageData, std::string imageName)
{
	CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	if (NULL == win)
	{
		return -1;
	}
	std::string imagesetName;
	switch (saveImage(imageData, imageName))
	{
	case 0:		// 保存图片正确
	case -2:	// 存在同名图片
		{
			imagesetName = getImagesetName(imageName);
			if ("" == imagesetName)
			{
				return -2;
			}
		}
		break;
	case -1:	// 存在相同数据的图片
		{
			imagesetName = getImagesetName(getImageName(imageData));
			if ("" == imagesetName)
			{
				return -2;
			}
		}
		break;
	default:	// 图片保存错误
		return -3;
	}
	win->setProperty("Image", "set:" + imagesetName + " image:" + imageName);
	return 0;
}
//-----------------------------------------------------------------------
// 把指定图片设置到指定窗口中
int DynamicCreateImagesetMgr::setWindowImage(std::string winname, std::string imageName)
{
	CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	if (NULL == win)
	{
		return -1;
	}
	std::string imagesetName = getImagesetName(imageName);
	if ("" == imagesetName)
	{
		return -2;
	}
	win->setProperty("Image", "set:" + imagesetName + " image:" + imageName);
	return 0;
}
//-----------------------------------------------------------------------
// base64解码核心函数
int _base64Decode(const char* input, int input_len, char* output, int* output_len)
{
	const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static char inalphabet[256], decoder[256];
	int i, bits, c, char_count, errors=0;
	int input_idx = 0;
	int output_idx = 0;

	for (i=sizeof(alphabet)-1; i>= 0; i--) 
	{
		inalphabet[alphabet[i]] = 1;
		decoder[alphabet[i]] = i;
	}

	char_count = 0;
	bits = 0;
	for(input_idx=0; input_idx<input_len; input_idx++) 
	{
		c = input[input_idx];
		if ('=' == c)
		{
			break;
		}
		if ((c>255) || (!inalphabet[c]))
		{
			continue;
		}
		bits += decoder[c];
		char_count++;
		if (4 == char_count)
		{
			output[output_idx++] = (bits >> 16);
			output[output_idx++] = ((bits >> 8) & 0xff);
			output[output_idx++] = (bits & 0xff);
			bits = 0;
			char_count = 0;
		} 
		else 
		{
			bits <<= 6;
		}
	}

	if('=' == c)
	{
		switch (char_count) 
		{
		case 1:
			fprintf(stderr, "base64Decode: encoding incomplete: at least 2 bits missing");
			errors++;
			break;
		case 2:
			output[output_idx++] = (bits >> 10);
			break;
		case 3:
			output[output_idx++] = (bits >> 16);
			output[output_idx++] = ((bits >> 8) & 0xff);
			break;
		}
	} 
	else if (input_idx < input_len) 
	{
		if (char_count) 
		{
			fprintf(stderr, "base64 encoding incomplete: at least %d bits truncated", ((4 - char_count) * 6));
			errors++;
		}
	}

	*output_len = output_idx;
	return errors;
}
//-----------------------------------------------------------------------
// base64解码
int base64Decode(const char* input, int inputLength, char** output)
{
	int outputLength = 0;

	//should be enough to store 6-bit buffers in 8-bit buffers
	if (inputLength != 0)
	{
		*output = (char*)malloc(inputLength * 3.0f / 4.0f + 1);
	}
	if( *output ) 
	{
		int ret = _base64Decode(input, inputLength, *output, &outputLength);

		if (ret > 0)
		{
			printf("Base64Utils: error decoding");
			free(*output);
			*output = NULL;			
			outputLength = 0;
		}
	}
	return outputLength;
}
