#pragma once

#ifndef __VMUSIC_H
#define __VMUSIC_H

#include "Voice.h"
#include "fmod.h"

//“Ù¿÷¿‡:MOD,S3M,XM,IT,MID
class VMusic :
	public Voice
{
private:
	FMUSIC_MODULE *mFModMusic;

public:
	VMusic(void);
	~VMusic(void);

	virtual bool CreateFromMem(void *data, uint size, int ltype);
	virtual bool CreateFromFile(const char *file, int ltype);
	virtual void Release();
	virtual bool Play(bool loop);
	virtual void Stop();
	virtual bool Pause(bool pause);
	virtual int  GetPlayState();
	//Volume:0-256
	virtual uint GetVolume();
	virtual void SetVolume(uint volume);
};

#endif