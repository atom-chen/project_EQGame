#pragma once

#ifndef __EQVOICEMGR_H
#define __EQVOICEMGR_H

#include <map>
#include <string>
#include "Voice.h"

//��Ϸ�������ͣ�ͬ������һ��ֻ�ܲ���һ����������ͬ����������ͬʱ���š�
enum EQVOICE_CLASS
{
	EQVOICE_BACKGROUND,//������������
	EQVOICE_EFF_NORMAL,//������Ч������Ҫ�ɼ���ϸ�����͡�
};

//���õ���������
enum EQVOICE_VOLUME_TYPE
{
	EQVOICE_VOLUME_BG,//������������
	EQVOICE_VOLUME_SYS,//ϵͳ��Ч����

	EQVOICE_VOLUME_CNT
};

typedef struct tagEQVOICE_INFO
{
	int cls;//voice class
	int vtype;//volume type
	Voice *voice;//voice object
} EQVOICE_INFO, *LPEQVOICE_INFO;

typedef std::map<std::string, EQVOICE_INFO> EQVOICELIST;

class EQVoiceMgr :
	public Ogre::Singleton<EQVoiceMgr>
{
private:
	EQVOICELIST mVoices;
	uint mVolumes[EQVOICE_VOLUME_CNT];

public:
	EQVoiceMgr(void);
	~EQVoiceMgr(void);

	void init();
	void release();

	int  getVoiceType(const char *file);
	int  getVolumeType(int cls);
	bool loadFromFile(const char *file, const char *name, int cls, int ltype);
	//������Ч����е�id����
	bool play(uint id, bool loop = false);
	void stop(uint id);
	bool play(const char *name, bool loop = false);
	void stop(const char *name);
	void stopACls(int cls);
	void stopAll();
	void setVolume(int vtype, uint volume);
	void setAllVolume(uint volume);
	uint getVolume(int vtype);
};

#endif