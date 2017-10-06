#include "EQMainPCH.h"
#include "XmlDataCenter.h"
#include "tinyXml/tinyxml.h"
#include "tinyXml/tinystr.h"
#include <fstream>
#include "CommFunc.h"


// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
// XmlDataTable¿‡
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
XmlDataTable::~XmlDataTable()
{
}
// ------------------------------------------------------------------------
void XmlDataTable::loadResource( const Ogre::DataStreamPtr & stream )
{
	TiXmlDocument myDoc;
	myDoc.Parse( stream->getAsString().c_str() );
	loadXmlDocument( myDoc );
}
// ------------------------------------------------------------------------
void XmlDataTable::readXml( const std::string & strXmlFile )
{	
	TiXmlDocument myDoc;
	myDoc.LoadFile( strXmlFile.c_str() );
	myDoc.Print();
	loadXmlDocument( myDoc );
}
// ------------------------------------------------------------------------
void XmlDataTable::loadXmlDocument( TiXmlDocument & myDoc )
{
	clear();
	
	TiXmlElement * pRoot = myDoc.RootElement();

	if ( !pRoot )
		return;

	
	for ( TiXmlElement * pRow = pRoot->FirstChildElement(); 
		pRow; 
		pRow = pRow->NextSiblingElement() )
	{
		TStrings strs;
		strs.resize( mCols.size() );

		for ( TiXmlElement * pCol = pRow->FirstChildElement(); 
			pCol; 
			pCol = pCol->NextSiblingElement() )
		{
			std::string strColName = pCol->Value();
			Ogre::StringUtil::trim( strColName );

			size_t idx = 0;
			bool exist = existsColumnName( strColName );

			if ( exist )
				idx = getColumnIndex( strColName );			

			if ( !exist )
			{
				mCols.push_back( strColName );
				strs.push_back( fetchColText(pCol->GetText()) );
				Ogre::StringUtil::trim( strs.back() );
				size_t n = ( mCols.back().size() > strs.back().size() ) ? mCols.back().size() : strs.back().size();
				mWidthInfos.push_back( n );
			}
			else
			{
				strs[idx] = fetchColText(pCol->GetText());
				Ogre::StringUtil::trim( strs[idx] );

				size_t n = strs[idx].size();
				if ( mWidthInfos[idx] < n )
					mWidthInfos[idx] = n;
			}
		}

		mRows.push_back( strs );			
	}

	size_t nCol = getColumnCount();
	for ( TRows::iterator it = mRows.begin(); it != mRows.end(); ++it )
	{
		assert((*it).size() <= nCol);
		if ( nCol != (*it).size() )
			(*it).resize(nCol);
	}
}
// ------------------------------------------------------------------------
const char* XmlDataTable::fetchColText( const char* text )
{
	if ( !text )
		return "";

	std::string str = text;
	Ogre::StringUtil::trim(str);
	if ( strcmp( str.c_str(), "(null)" ) == 0 )
		return "";

	return text;
}
// ------------------------------------------------------------------------
void XmlDataTable::saveAsTextFile( const std::string & strTextFile )
{
	std::ofstream fs;
	fs.open( strTextFile.c_str() );
	
	const size_t iStep = 2;
	std::string strSpace;

	XmlDataTable::TColumnWidthInfos::const_iterator itInfo = mWidthInfos.begin();
	for ( XmlDataTable::TColumns::const_iterator it = mCols.begin();
		it != mCols.end();
		++it )
	{
		strSpace.resize( *itInfo - it->size() + iStep );
		std::fill( strSpace.begin(), strSpace.end(), ' ' );

		fs << *it << strSpace;			
		++itInfo;
	}
	fs << std::endl << std::endl;


	for ( XmlDataTable::TRows::const_iterator it = mRows.begin();
		it != mRows.end();
		++it )
	{
		itInfo = mWidthInfos.begin();
		for ( XmlDataTable::TStrings::const_iterator iCell = it->begin();
			iCell != it->end();
			++iCell )
		{
			fs << *iCell;
			strSpace.resize( *itInfo - iCell->size() + iStep );
			fs << strSpace;

			++itInfo;
		}
		fs << std::endl;
	}		
}
// ------------------------------------------------------------------------
void XmlDataTable::clear()
{
	mCols.clear();
	mRows.clear();
	mWidthInfos.clear();
}
// ------------------------------------------------------------------------
void XmlDataTable::showOnConsole() const
{
	const size_t iStep = 2;
	std::string strSpace;

	XmlDataTable::TColumnWidthInfos::const_iterator itInfo = mWidthInfos.begin();
	for ( XmlDataTable::TColumns::const_iterator it = mCols.begin();
		it != mCols.end();
		++it )
	{
		strSpace.resize( *itInfo - it->size() + iStep );
		std::fill( strSpace.begin(), strSpace.end(), ' ' );

		std::cout << *it << strSpace;			
		++itInfo;
	}
	std::cout << std::endl << std::endl;


	for ( XmlDataTable::TRows::const_iterator it = mRows.begin();
		it != mRows.end();
		++it )
	{
		itInfo = mWidthInfos.begin();
		for ( XmlDataTable::TStrings::const_iterator iCell = it->begin();
			iCell != it->end();
			++iCell )
		{
			std::cout << *iCell;
			strSpace.resize( *itInfo - iCell->size() + iStep );
			std::cout << strSpace;

			++itInfo;
		}
		std::cout << std::endl;
	}
}
// ------------------------------------------------------------------------
void XmlDataTable::setName( const std::string & val )
{
	mStrName = val;
}
// ------------------------------------------------------------------------
const std::string & XmlDataTable::getName() const
{
	return mStrName;
}
// ------------------------------------------------------------------------
size_t XmlDataTable::getColumnIndex( const std::string & strColName ) const
{
	TColumns::const_iterator strIter = std::find( mCols.begin(), mCols.end(), strColName );
	if ( strIter == mCols.end() )
		EQ_EXCEPT( "Column '" + strColName + "' can't find!", "XmlDataTable::getColumnIndex" );

	return strIter - mCols.begin();			
}
// ------------------------------------------------------------------------
const XmlDataTable::TColumns & XmlDataTable::columns() const
{
	return mCols;
}
// ------------------------------------------------------------------------
const XmlDataTable::TRows & XmlDataTable::rows() const
{
	return mRows;
}
// ------------------------------------------------------------------------
const std::string & XmlDataTable::cell( size_t iRow, size_t iCol ) const
{		
	assert( existsCell(iRow,iCol) );
	return mRows[iRow][iCol];
}
// ------------------------------------------------------------------------
const std::string & XmlDataTable::cell( size_t iRow, const std::string & strCol ) const
{
	assert( existsRowIndex(iRow) );
	return mRows[iRow][getColumnIndex(strCol)];
}
// ------------------------------------------------------------------------
std::string XmlDataTable::cell( size_t iRow, size_t iCol, bool toGB2312 ) const
{
	const std::string& str = cell(iRow, iCol);

	if ( toGB2312 )
		return UTF8ToGB2312( str.c_str() );
	else
		return str;
}
// ------------------------------------------------------------------------
std::string XmlDataTable::cell( size_t iRow, const std::string & strCol, bool toGB2312 ) const
{
	const std::string& str = cell(iRow, strCol);

	if ( toGB2312 )
		return UTF8ToGB2312( str.c_str() );
	else
		return str;
}
// ------------------------------------------------------------------------
bool XmlDataTable::existsCell( size_t iRow, size_t iCol ) const
{
	return existsRowIndex( iRow ) && existsColumnIndex( iCol );
}
// ------------------------------------------------------------------------
bool XmlDataTable::existsColumnIndex( size_t iCol ) const
{
	return iCol < mCols.size();
}
// ------------------------------------------------------------------------
bool XmlDataTable::existsColumnName( const std::string & strColName ) const
{
	TColumns::const_iterator strIter = std::find( mCols.begin(), mCols.end(), strColName );
	return  strIter != mCols.end();
}
// ------------------------------------------------------------------------
bool XmlDataTable::existsRowIndex( size_t iRow ) const
{
	return iRow < mRows.size();
}
// ------------------------------------------------------------------------
size_t XmlDataTable::getColumnCount() const
{
	return mCols.size();
}
// ------------------------------------------------------------------------
size_t XmlDataTable::getRowCount() const
{
	return mRows.size();
}
// ------------------------------------------------------------------------
bool XmlDataTable::isEmpty() const
{
	return mRows.empty();
}
// ------------------------------------------------------------------------




// ------------------------------------------------------------------------
// XmlDataSet¿‡
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
XmlDataSet::~XmlDataSet()
{
}
// ------------------------------------------------------------------------
XmlDataTable & XmlDataSet::operator [] ( const std::string & strTable )
{
	for ( TDataTables::iterator it = mTables.begin();
		it != mTables.end();
		++it )
	{
		if ( it->getName() == strTable )
		{
			return *it;
		}
	}

	if ( true )
		throw ::EQException( strTable + " table not find", __FUNCTION__, __FILE__, __LINE__ ); 

	return mTables.front();
}
// ------------------------------------------------------------------------
XmlDataTable * XmlDataSet::getTable( const std::string & strTable )
{
	for ( TDataTables::iterator it = mTables.begin();
		it != mTables.end();
		++it )
	{
		if ( it->getName() == strTable )
		{
			return &*it;
		}
	}
	return NULL;
}
// ------------------------------------------------------------------------
XmlDataTable * XmlDataSet::addTable( const std::string & strTable )
{
	XmlDataTable * pTable = getTable( strTable );
	if ( pTable )
		return pTable;

	XmlDataTable tb;
	tb.setName( strTable );
	mTables.push_back( tb );
	return &mTables.back();
}
// ------------------------------------------------------------------------
void XmlDataSet::removeTable( const std::string & strTable )
{
	for ( TDataTables::iterator it = mTables.begin();
		it != mTables.end();
		++it )
	{
		if ( it->getName() == strTable )
		{
			mTables.erase( it );
			return;
		}
	}
}
// ------------------------------------------------------------------------
size_t XmlDataSet::getTableCount() const
{
	return mTables.size();
}
// ------------------------------------------------------------------------
XmlDataSet::TDataTables & XmlDataSet::tables() 
{
	return mTables;
}
// ------------------------------------------------------------------------
const XmlDataSet::TDataTables & XmlDataSet::tables() const
{
	return mTables;
}
// ------------------------------------------------------------------------



// ------------------------------------------------------------------------
// XmlDataCenter ¿‡
// ------------------------------------------------------------------------
XmlDataSet & XmlDataCenter::getDataSet()
{
	static XmlDataSet s_val;
	return s_val;
}
// ------------------------------------------------------------------------
