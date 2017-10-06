/******************************************************************************
* 作者: HJX
* 时间: 2009-10-21
* 描述: Xml数据中心( 保存从外部导入的xml数据)
******************************************************************************/

#ifndef XmlDataCenter_H
#define XmlDataCenter_H

#include "EQMainPrerequisites.h"



class TiXmlDocument;


/// 注: 下标一律从0开始

class XmlDataTable
{
public:
	typedef std::vector<std::string> TStrings;
	typedef TStrings TColumns;
	typedef std::vector<TStrings> TRows;
	typedef std::vector<size_t> TColumnWidthInfos;
public:
	virtual ~XmlDataTable();
public:
	/// 从资源载入Xml
	void loadResource( const Ogre::DataStreamPtr & stream );
public:
	/// 载入Xml文件
	void readXml( const std::string & strXmlFile );
public:
	/// 生成整齐的二维表格
	void saveAsTextFile( const std::string & strTextFile );
public:
	void clear();
public:
	/// 控制台下显示表格内容
	void showOnConsole() const;
public:
	void setName( const std::string & val );
public:
	const std::string & getName() const;
public:
	size_t getColumnIndex( const std::string & strColName ) const;
public:
	const TColumns & columns() const;
public:
	const TRows & rows() const;
public:
	/// 取得表格中的某项数据
	const std::string & cell( size_t iRow, size_t iCol ) const;
	const std::string & cell( size_t iRow, const std::string & strCol ) const;
	std::string cell( size_t iRow, size_t iCol, bool toGB2312 ) const;
	std::string cell( size_t iRow, const std::string & strCol, bool toGB2312 ) const;

public:
	bool existsCell( size_t iRow, size_t iCol ) const;
public:
	bool existsColumnIndex( size_t iCol ) const;
	bool existsColumnName( const std::string & strColName ) const;
public:
	bool existsRowIndex( size_t iRow ) const;
public:
	size_t getColumnCount() const;
public:
	size_t getRowCount() const;
public:
	bool isEmpty() const;
protected:
	void loadXmlDocument( TiXmlDocument & myDoc );
	const char* fetchColText( const char* text );
protected:
	/// Column宽度信息
	TColumnWidthInfos mWidthInfos; 
	TColumns mCols;
	TRows mRows;
	std::string mStrName;
};


class XmlDataSet
{
public:
	typedef std::vector<XmlDataTable> TDataTables;	
public:
	virtual ~XmlDataSet();
public:
	XmlDataTable & operator [] ( const std::string & strTable );
public:
	XmlDataTable * getTable( const std::string & strTable );
public:
	XmlDataTable * addTable( const std::string & strTable );
public:
	void removeTable( const std::string & strTable );
public:
	size_t getTableCount() const;
public:
	TDataTables & tables();
public:
	const TDataTables & tables() const;
protected:
	TDataTables mTables;
};

class XmlDataCenter 
{
public:
	static XmlDataSet & getDataSet();
};

#endif