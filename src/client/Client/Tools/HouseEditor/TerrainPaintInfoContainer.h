/******************************************************************************
* ����: Y3
* ʱ��: 2005-5-25
* ����: ������ͼ��Ϣ������. ����TD
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

	std::string  textureName; // ������������ƣ��Զ����ɣ������ڻ�ˢ���е���ʾ
	// �����ĸ������ĵ�λ�������أ����Ǳ�׼�����С��64���ı�������1��2��4
	uint    width;
	uint    height;
	// ���������Ͻ��ڴ������е�λ��
	uint    leftCorner;
	uint    topCorner;
	std::string  brushName;
	TexType textureType;
	std::string  ownerTextureName;   // �����Ĵ����������
	uint    rotateType;	// ����ı�׼���Ͻ�������ת���ĸ�����
	ExtraTexType extraTexType;
};

typedef std::vector<TextureInfo> TextureInfos;
// keyΪ��ˢ���ƣ�valueΪ�û�ˢ����������
typedef std::map<std::string, TextureInfos> TextureInfoMap;

typedef std::map<std::string, std::string> TexNameAndBrushMap;

/** ���ڱ��浱ǰ��ˢ�е���Ϣ
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

	/// ��������ö���������ַ����Ķ�Ӧ��
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