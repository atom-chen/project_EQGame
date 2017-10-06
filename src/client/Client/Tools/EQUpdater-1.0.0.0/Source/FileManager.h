/******************************************************************************
* ����: xj
* ʱ��: 2010-11-24
* ����: �ļ�����ӿ�
******************************************************************************/

#pragma once

#ifndef __FileManager_H
#define __FileManager_H

class FileManager
{
public:
	/*
	˵������Դ�ļ�����Ŀ��λ��
	����
	  target��Ŀ�����λ�ã�������һ��Ŀ¼���߰�
	  file��  Դ�ļ����λ��
	*/
	virtual bool moveTo(const char *target, const char *file) = 0;

	/*
	˵������Դ�ļ���������Ŀ��λ�ã�����Ŀ������Ϊ���Ŀ������Ч��
	����
	  target��Ŀ�����λ�ã�������һ��Ŀ¼���߰�
	  files�� Դ�ļ����λ���б�
	  count�� Դ�ļ��б����
	*/
	virtual bool moveToBatch(const char *target, const char **files, int count) = 0;

public:
	FileManager(void) {}
	virtual ~FileManager(void) {}
};

#endif