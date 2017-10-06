/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-16
* ����: �쳣
******************************************************************************/
#ifndef EQException_H
#define EQException_H

#include "EQMainPrerequisites.h"

/** �쳣. ʹ����֮ǰ, ������˽���Ƶ�Ŀ��.
@remarks EQ��Ŀ��ר���쳣.
@par 
	��һ, Ϊ��ʹ���������, �����ķ���ֵ����Ҫ̫���bool��errCode֮��ֵ.
@par
	�ڶ�, ���е��쳣Ŀ�����ʹ������ֹ����, ���������Ҫ���������Ҽ�������,
	��ʹ��bool��errCode�ķ���ֵ. ��������Ϊ�쳣����ReleaseҲ�ܹ����Ķ���.
	����������Ŀ��. ���ǿ��Կ����ڲ����쳣��ʱ��Ѷ�ջdump����. 
@par
	����, �쳣����ֻ��һ��, ������չ. ��ΪĿ�ľ����ó�����ֹ.
@par
	֮����������Ƶ�Ŀ������Ϊ�����c++�쳣ʹ�ù淶��������, �׳��쳣�Ͳ����쳣��
	�˾�������ͬһ����, ����ʹ�������ͺ�����, �ҶԳ���Ա������Ҫ��Ƚϸ�. �������
	���ǲ�����ɶ����; ������c++���ڴ������ƺܿ������쳣�����ʱ�����ڴ�й¶
	����������.
*/
class EQException : public std::exception
{
public:
	EQException( const std::string& description, const std::string& source, const char* file, long line );

	/** Copy constructor.
	*/
	EQException(const EQException& rhs);

	/** Assignment operator.
	*/
	void operator = (const EQException& rhs);

	~EQException() throw() {}

public:
	/** Returns a string with the full description of this error.
	*/
	virtual const std::string& getFullDescription(void) const;

	/** Gets the source function.
	*/
	virtual const std::string &getSource() const { return source; }

	/** Gets source file name.
	*/
	virtual const std::string &getFile() const { return file; }

	/** Gets line number.
	*/
	virtual long getLine() const { return line; }

	/** Returns a string with only the 'description' field of this exception. Use 
	getFullDescriptionto get a full description of the error including line number,
	error number and what function threw the exception.
	*/
	virtual const std::string &getDescription(void) const { return description; }

	/// Override std::exception::what
	const char* what() const throw() { return getFullDescription().c_str(); }

protected:
	EQException();
	
protected:
	long line;
	std::string description;
	std::string source;
	std::string file;
	mutable std::string fullDesc;

};

#ifndef EQ_EXCEPT
#define EQ_EXCEPT( desc, src ) throw EQException(desc, src, __FILE__, __LINE__)
#endif


#ifndef THROW_IF
#define THROW_IF( reason ) if ( reason ) throw EQException( #reason, "", __FILE__, __LINE__);
#endif


#endif