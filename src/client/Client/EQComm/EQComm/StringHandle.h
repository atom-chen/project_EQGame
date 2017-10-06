/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		10/3/2011	15:12
* �ļ�: 	StringHandle.h
* ����:		�ַ�������ģ��
******************
*********************************************************************/
#ifndef STRINGHANDLE_H
#define STRINGHANDLE_H

/**�ַ�������ģʽ
*/
enum FilterMode
{
	FILTERALG1,
	FILTERALG2,
	FILTERALG3,
};


/**�ַ���������
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
	/**�ִ������ӳ���
	*/
	// @param	mode --�ú��ֹ����㷨
	std::string stringFilter(char* source_s,int mode);

	/**�ִ������ӳ���
	*/

	/**����ִ��Ӵ��ӳ���
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
