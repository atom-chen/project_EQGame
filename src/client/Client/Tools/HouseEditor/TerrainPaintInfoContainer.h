/******************************************************************************
* 作者: Y3
* 时间: 2005-5-25
* 描述: 地形贴图信息容器等. 来自TD
******************************************************************************/
#ifndef __TerrainPaintInfoContainer_H__
#define __TerrainPaintInfoContainer_H__



enum TexType
{
	InnerTex,
	InnerCornerTex,	
	OuterCornerTex,		
	EdgeTex,	
	DuijiaoTex,
	InnerTransTex,
	Other,
};

enum ExtraTexType
{
	LT, RT, LB, RB, LE, RE, TE, BE, EOther
};

struct TextureInfo
{
	TextureInfo( std::string texName, uint w, uint h, uint lc, uint tc, 
		std::string bName, TexType type, ExtraTexType eType, uint rt = 0 )
		: ownerTextureName(texName), width(w), height(h), leftCorner(lc), rotateType(rt),
		topCorner(tc), brushName(bName), textureType(type), extraTexType(eType)
	{
	}

	TextureInfo()
		: ownerTextureName(), width(0), height(0), leftCorner(), topCorner(), brushName(), 
		textureType(Other), rotateType(0), extraTexType(EOther)
	{
	}

	void clear(void)
	{
		textureName.clear();
		width = height = leftCorner = topCorner = 0;
		brushName.clear();
		ownerTextureName.clear();
		textureType = Other;
		rotateType = 0;
		extraTexType = EOther;
	}

	std::string  textureName; // 单个纹理的名称，自动生成，用于在画刷树中的显示
	// 以下四个参数的单位不是像素，而是标准纹理大小（64）的倍数，如1，2，4
	uint    width;
	uint    height;
	// 该纹理左上角在大纹理中的位置
	uint    leftCorner;
	uint    topCorner;
	std::string  brushName;
	TexType textureType;
	std::string  ownerTextureName;   // 所属的大纹理的名称
	uint    rotateType;	// 纹理的标准左上角现在旋转到哪个角上
	ExtraTexType extraTexType;
};

typedef std::vector<TextureInfo> TextureInfos;
// key为画刷名称，value为该画刷的纹理数组
typedef std::map<std::string, TextureInfos> TextureInfoMap;

typedef std::map<std::string, std::string> TexNameAndBrushMap;

/** 用于保存当前画刷中的信息
*/
class TerrainPaintInfoContainer
{
public:

	TerrainPaintInfoContainer();
	~TerrainPaintInfoContainer();

	void addTextureInfo( TextureInfo &info );

	const TextureInfoMap &getTextureInfoMap(void) const
	{
		return mTextureInfoMap;
	}

	void clearTextureInfos(void)
	{
		mTextureInfoMap.clear();
	}

	void addTexNameAndBrushMapInfo( const std::string &texName, const std::string &brushName );

	const std::string &findBrushNameByTexName( const std::string &texName ) const;

	const TextureInfo &findTextureInfoByTexName( const std::string &texName );

public:

	/// 纹理类型枚举类型与字符串的对应表
	typedef std::map<std::string, TexType> StringTexTypeMap;
	StringTexTypeMap mStringTexTypeMap;
	typedef std::map<std::string, ExtraTexType> StringExtraTexTypeMap;
	StringExtraTexTypeMap mStringExtraTexTypeMap;

protected:

	bool isTextureInfoOk( const TextureInfo &info );

protected:

	TextureInfoMap mTextureInfoMap;
	TexNameAndBrushMap mTexNameAndBrushMap;
};


#endif