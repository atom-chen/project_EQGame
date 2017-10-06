#pragma once

#ifndef __EQVOICEMGR_H
#define __EQVOICEMGR_H

#include <map>
#include <string>
#include "Voice.h"

//游戏声音类型，同种类型一次只能播放一个声音，不同类型声音可同时播放。
enum EQVOICE_CLASS
{
	EQVOICE_BACKGROUND,//背景音乐类型
	EQVOICE_EFF_NORMAL,//常用音效，若需要可继续细分类型。
};

//设置的音量类型
enum EQVOICE_VOLUME_TYPE
{
	EQVOICE_VOLUME_BG,//背景音乐音量
	EQVOICE_VOLUME_SYS,//系统音效音量

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
	//根据音效库表中的id播放
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