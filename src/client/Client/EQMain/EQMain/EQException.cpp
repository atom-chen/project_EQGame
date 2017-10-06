#include "EQMainPCH.h"

#include "EQException.h"

//---------------------------------------------------------------------
EQException::EQException(const std::string& desc, const std::string& src, 
						 const char* fil, long lin) :
line( lin ),
description( desc ),
source( src ),
file( fil )
{
	LOGERR(getFullDescription());

}
//---------------------------------------------------------------------
EQException::EQException(const EQException& rhs)
: line( rhs.line ), 
description( rhs.description ), 
source( rhs.source ), 
file( rhs.file )
{
}
//---------------------------------------------------------------------
void EQException::operator = ( const EQException& rhs )
{
	description = rhs.description;
	source = rhs.source;
	file = rhs.file;
	line = rhs.line;
}
//---------------------------------------------------------------------
const std::string& EQException::getFullDescription(void) const
{
	if (fullDesc.empty())
	{

		Ogre::StringUtil::StrStreamType desc;

		desc <<  "EQ EXCEPTION(): "
			<< description 
			<< " in " << source;

		if( line > 0 )
		{
			desc << " at " << file << " (line " << line << ")";
		}

		fullDesc = desc.str();
	}

	return fullDesc;
}
