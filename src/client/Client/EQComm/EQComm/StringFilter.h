/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		10/3/2011	16:38
* �ļ�: 	StringFilter.h
* ����:		�ַ��������㷨ģ�� �������й����㷨��
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
/**�㷨2
*/

#endif
