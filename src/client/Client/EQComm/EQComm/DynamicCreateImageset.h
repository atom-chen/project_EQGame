/******************************************************************************
* 作者: 何展然
* 时间: 2010-11-26
* 描述: 动态创建imageset管理器
******************************************************************************/
#ifndef DynamicCreateImagesetMgr_H
#define DynamicCreateImagesetMgr_H

#include "EQMainPrerequisites.h"
#include "CEGUI.h"
#include "Ogre.h"


/*************************************************************************
* 动态创建imageset管理器
*************************************************************************/
class DynamicCreateImagesetMgr : public Ogre::Singleton<DynamicCreateImagesetMgr>
{
public:
	typedef std::map<std::string, std::string> ImageToSet;	// imageName和imagesetName的映射
	typedef std::map<std::string, std::string> DataToImage;	// imageData和imageName的映射

	// 自定义纹理类型
	struct TextureTypedef
	{
		TextureTypedef():type(""),tex(NULL){}
		std::string type;		// 类型（如32*32，96*96等）
		CEGUI::Texture *tex;	// 纹理
	};

public:
	/**
	*	构造函数
	*/
	DynamicCreateImagesetMgr();

	/**
	*	析构函数
	*/
	~DynamicCreateImagesetMgr();

	/**
	*	功	能：保存图片
	*	参	数：imageData: 图片数据（base64格式），imageName: 图片名称（不可同名）
	*	返回值：0（正确），-1（存在同样数据的图片），-2（存在同名imageName），-3（base64解码失败），-4（图片格式错误），-5（创建imageset失败）
	*/
	int saveImage(std::string imageData, std::string imageName);

	/**
	*	功	能：保存图片（保存后，image名和imageset名一样）
	*	参	数：imageset: imageset名，fileName: 图片文件名（含后缀）
	*	返回值：0（正确），-1（存在同名imageset），-2（创建imageset失败）
	*/
	int saveImageEx(std::string imageset, std::string fileName);

	/**
	*	功	能：根据imageName获取对应的imagesetName
	*	参	数：imageName: 图片名
	*	返回值：imageset名（""，没有对应的imagesetName）
	*/
	std::string getImagesetName(std::string imageName);

	/**
	*	功	能：根据imageData获取imageName
	*	参	数：imageData: 图片数据
	*	返回值：图片名（""，没有对应的imageName）
	*/
	std::string getImageName(std::string imageData);

	/**
	*	功	能：销毁imageset
	*	参	数：imagesetName: 名称
	*	返回值：void
	*/
	void destroyImageset(std::string imagesetName);

	/**
	*	功	能：设置资源组目录
	*	参	数：resourceGroup:资源组，type:类型名，section: 区域名
	*	返回值：void
	*/
	void setResourceGroupDirectory(std::string resourceGroup, std::string type, std::string section);

	/**
	*	功	能：保存图片并设置到指定窗口中
	*	参	数：winname: 窗口名称，imageData: 图片数据（base64格式），imageName: 图片名称（不可同名）
	*	返回值：0（正确），-1（不存在winname名窗口），-2（不存在imageName），-3（保存图片失败）
	*/
	int setWindowImage(std::string winname, std::string imageData, std::string imageName);

	/**
	*	功	能：把指定图片设置到指定窗口中
	*	参	数：winname: 窗口名称，imageName: 图片名称
	*	返回值：0（正确），-1（不存在winname名窗口），-2（不存在imageName）
	*/
	int setWindowImage(std::string winname, std::string imageName);

private:
	/**
	*	功	能：保存图片
	*	参	数：img: 图片，imageName: 图片名称
	*	返回值：0（正确），-1（图片格式错误），-2（创建imageset失败）
	*/
	int _saveImage(Ogre::Image& img, std::string imageName);

	/**
	*	功	能：在缓冲区的相应位置粘贴图片
	*	参	数：buf: 缓冲区，n: 位置，img: 图片
	*	返回值：0（正确），-1（错误，n超出最多可粘贴图片数量）
	*/
	int _pasteToBuffer(int* buf, int n, Ogre::Image img);

	/**
	*	功	能：动态创建imageset
	*	参	数：flag: imageset名称标识
	*	返回值：CEGUI::Imageset*
	*/
	CEGUI::Imageset* _createImageset(int flag);

	/**
	*	功	能：设置成员变量
	*	参	数：tex: 纹理，imgSet: 图片集
	*	返回值：int
	*/
	void _setMemberVariable(CEGUI::Texture* tex, CEGUI::Imageset* imgSet);

	/**
	*	功	能：计算一张纹理最多可以粘贴几张图片
	*	参	数：width: 图片宽度，height: 图片高度
	*	返回值：最多可粘贴图片数量
	*/
	int _countImageNum(int width, int height);

private:
	int *mBuffer;	// 内存缓冲区
	TextureTypedef mCurTexture;		// 当前纹理
	CEGUI::Imageset *mCurImageset;	// 当前imageset
	int mImagesetNameFlag;			// imagesetName标识
	int mPos;		// 图片在缓冲区中的位置
	ImageToSet	mImageToSet;		// imageName到imagesetName的映射
	DataToImage mDataToImage;		// imageData到imageName的映射

};	// end of class DynamicCreateImagesetMgr

/**
*	功	能：base64解码（参数output的内存需要被调用者释放）
*	参	数：input:	base64字符串
*			inputLength:	base64字符串长度
*			output:	解码后的字符串
*	返回值：解码后的字符串的长度（0，解码失败）
*/
int base64Decode(const char* input, int inputLength, char** output);


#endif	// end of guard DynamicCreateImagesetMgr_H
