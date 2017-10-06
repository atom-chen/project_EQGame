#pragma once

#ifndef __VOICE_H
#define __VOICE_H

//音效类型
enum VOICE_TYPE
{
	VOICE_SOUND,//声音类:WAV,MP2,MP3,OGG,RAW
	VOICE_MUSIC //音乐类:MOD,S3M,XM,IT,MID
};

//音效加载类型，对于音乐无效。
enum VOICE_LOAD_TYPE
{
	VOICE_LOAD_BUFFER,//一次性加载和解码到内存，可反复利用，适合小音效声音。
	VOICE_LOAD_STREAM //边播放边加载解码，适合大背景音乐。
};

//音效播放状态
enum VOIC_PLAY_STATE
{
	VOICE_PLAY_STOPED,
	VOICE_PLAY_PLAYING,
	VOICE_PLAY_PAUSED
};

//最小音量值
#define MIN_VOICE_VOLUME 0
//最大音量值
#define MAX_VOICE_VOLUME 100

//音效基类
class Voice
{
public:
	virtual bool CreateFromMem(void *data, uint size, int ltype) = 0;
	virtual bool CreateFromFile(const char *file, int ltype) = 0;
	virtual void Release() = 0;
	virtual bool Play(bool loop) = 0;
	virtual void Stop() = 0;
	virtual bool Pause(bool pause) = 0;
	virtual int  GetPlayState() = 0;
	//声音大小范围：MIN_VOICE_VOLUME - MAX_VOICE_VOLUME
	virtual uint GetVolume() = 0;
	virtual void SetVolume(uint volume) = 0;
};

#endif
