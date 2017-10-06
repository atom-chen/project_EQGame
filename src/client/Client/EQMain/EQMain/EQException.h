/******************************************************************************
* 作者: Y3
* 时间: 2009-10-16
* 描述: 异常
******************************************************************************/
#ifndef EQException_H
#define EQException_H

#include "EQMainPrerequisites.h"

/** 异常. 使用他之前, 请务必了解设计的目的.
@remarks EQ项目的专用异常.
@par 
	第一, 为了使程序代码简洁, 函数的返回值不需要太多的bool和errCode之类值.
@par
	第二, 所有的异常目标就是使程序终止下来, 如果程序需要做错误处理且继续工作,
	请使用bool或errCode的返回值. 你可以理解为异常就是Release也能工作的断言.
	出于这个设计目标. 我们可以考虑在产生异常的时候把堆栈dump出来. 
@par
	第三, 异常类型只有一种, 无需扩展. 因为目的就是让程序终止.
@par
	之所以这样设计的目的是因为成熟的c++异常使用规范并不容易, 抛出异常和捕获异常的
	人经常不是同一个人, 所以使用起来就很困难, 且对程序员的能力要求比较高. 比如如果
	忘记捕获会出啥问题; 再由于c++的内存管理机制很可能在异常处理的时候导致内存泄露
	等其他问题.
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