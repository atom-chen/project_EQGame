/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		10/3/2011	16:38
* 文件: 	StringFilter.h
* 描述:		字符串过滤算法模块 集中所有过滤算法类
******************
*********************************************************************/
#ifndef STRINGFILTER_H
#define STRINGFILTER_H

class SpamString
{
public:
	SpamString(){}
	virtual ~SpamString(){}

public:
	static SpamString& getSingleton()
	{
		static SpamString spamString;
		return spamString;
	}

public:
	

protected:

private:

};

#define sSpamString SpamString::getSingleton()

//---------------------------------------------
/**算法2
*/

#endif
