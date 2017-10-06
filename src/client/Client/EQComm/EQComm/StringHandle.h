/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		10/3/2011	15:12
* 文件: 	StringHandle.h
* 描述:		字符串处理模块
******************
*********************************************************************/
#ifndef STRINGHANDLE_H
#define STRINGHANDLE_H

/**字符串过滤模式
*/
enum FilterMode
{
	FILTERALG1,
	FILTERALG2,
	FILTERALG3,
};


/**字符串处理类
*/
class StringHandle
{
public:
	StringHandle();
	virtual ~StringHandle();

public:
	static StringHandle& getSingleton()
	{
		static StringHandle string_handle;
		return string_handle;
	}

public:
	/**字串过滤子程序
	*/
	// @param	mode --用何种过滤算法
	std::string stringFilter(char* source_s,int mode);

	/**字串搜索子程序
	*/

	/**获得字串子串子程序
	*/
	
	/**
	*/
	void clearup();
	

protected:

private:
	bool mHasload_dict;

};

#define sStringHandle StringHandle::getSingleton()

#endif
