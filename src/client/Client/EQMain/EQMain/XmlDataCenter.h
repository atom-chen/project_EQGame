/******************************************************************************
* ����: HJX
* ʱ��: 2009-10-21
* ����: Xml��������( ������ⲿ�����xml����)
******************************************************************************/

#ifndef XmlDataCenter_H
#define XmlDataCenter_H

#include "EQMainPrerequisites.h"



class TiXmlDocument;


/// ע: �±�һ�ɴ�0��ʼ

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
	/// ����Դ����Xml
	void loadResource( const Ogre::DataStreamPtr & stream );
public:
	/// ����Xml�ļ�
	void readXml( const std::string & strXmlFile );
public:
	/// ��������Ķ�ά���
	void saveAsTextFile( const std::string & strTextFile );
public:
	void clear();
public:
	/// ����̨����ʾ�������
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
	/// ȡ�ñ���е�ĳ������
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
	/// Column�����Ϣ
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