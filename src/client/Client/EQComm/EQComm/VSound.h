#pragma once

#ifndef __VSOUND_H
#define __VSOUND_H

#include "Voice.h"
#include "fmod.h"

//…˘“Ù¿‡:WAV,MP2,MP3,OGG,RAW
class VSound :
	public Voice
{
private:
	FSOUND_SAMPLE *mFModSample;
	FSOUND_STREAM *mFModStream;
	int mPlayChannel;
	int mLoadType;
	uint mVolume;

public:
	VSound(void);
	~VSound(void);

	virtual bool CreateFromMem(void *data, uint size, int ltype);
	virtual bool CreateFromFile(const char *file, int ltype);
	virtual void Release();
	virtual bool Play(bool loop);
	virtual void Stop();
	virtual bool Pause(bool pause);
	virtual int  GetPlayState();
	virtual uint GetVolume();
	virtual void SetVolume(uint volume);

protected:
	bool createSampleFromMem(void *data, uint size);
	bool createSampleFromFile(const char *file);
	bool createStreamFromMem(void *data, uint size);
	bool createStreamFromFile(const char *file);
};

#endif