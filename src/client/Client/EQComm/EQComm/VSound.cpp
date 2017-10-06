#include "EQCommPCH.h"
#include "VSound.h"
#include "fmod_errors.h"

#define MAX_SOUND_VOLUME 255

VSound::VSound(void)
{
	mFModSample = 0;
	mFModStream = 0;
	mPlayChannel = -1;
	mLoadType = -1;
	mVolume = 200;
}

VSound::~VSound(void)
{
}

bool VSound::CreateFromMem( void *data, uint size, int ltype )
{
	mLoadType = ltype;

	switch(mLoadType)
	{
	case VOICE_LOAD_BUFFER:
		return createSampleFromMem(data, size);
	case VOICE_LOAD_STREAM:
		return createStreamFromMem(data, size);
	}
	
	return false;
}

bool VSound::CreateFromFile( const char *file, int ltype )
{
	mLoadType = ltype;

	switch(mLoadType)
	{
	case VOICE_LOAD_BUFFER:
		return createSampleFromFile(file);
	case VOICE_LOAD_STREAM:
		return createStreamFromFile(file);
	}

	return false;
}

void VSound::Release()
{
	if(mFModSample)
	{
		FSOUND_Sample_Free(mFModSample);
		mFModSample= NULL;
	}

	if(mFModStream)
	{
		FSOUND_Stream_Close(mFModStream);
		mFModStream = NULL;
	}
}

bool VSound::Play( bool loop )
{
	switch(mLoadType)
	{
	case VOICE_LOAD_BUFFER:
		mPlayChannel= FSOUND_PlaySoundEx(FSOUND_FREE, mFModSample, NULL, true);
		break;
	case VOICE_LOAD_STREAM:
		mPlayChannel= FSOUND_Stream_PlayEx(FSOUND_FREE, mFModStream, NULL, true);
		break;
	}

	if(-1 == mPlayChannel)
	{
		std::string errMsg = "Play sound error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	switch(mLoadType)
	{
	case VOICE_LOAD_BUFFER:
		FSOUND_Sample_SetMode(mFModSample, loop ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
		break;
	case VOICE_LOAD_STREAM:
		FSOUND_Stream_SetMode(mFModStream, loop ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
		break;
	}

	FSOUND_SetPaused(mPlayChannel, false);
	FSOUND_SetVolume(mPlayChannel, mVolume);

	return true;
}

void VSound::Stop()
{
	if(-1 == mPlayChannel)
		return;

	switch(mLoadType)
	{
	case VOICE_LOAD_BUFFER:
		FSOUND_StopSound(mPlayChannel);
		break;
	case VOICE_LOAD_STREAM:
		FSOUND_Stream_Stop(mFModStream);
		break;
	}

}

bool VSound::Pause(bool pause)
{
	if(-1 == mPlayChannel)
		return false;

	if(!FSOUND_SetPaused(mPlayChannel, pause))
	{
		std::string errMsg = "Pause sound error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

int VSound::GetPlayState()
{
	if(-1 == mPlayChannel)
		return VOICE_PLAY_STOPED;

	if(FSOUND_IsPlaying(mPlayChannel))
		return VOICE_PLAY_PLAYING;

	if(FSOUND_GetPaused(mPlayChannel))
		return VOICE_PLAY_PAUSED;

	return VOICE_PLAY_STOPED;
}

uint VSound::GetVolume()
{
	return (mVolume * MAX_VOICE_VOLUME) / MAX_SOUND_VOLUME;
}

void VSound::SetVolume( uint volume )
{
	mVolume = (volume * MAX_SOUND_VOLUME) / MAX_VOICE_VOLUME;

	if(-1 != mPlayChannel)
		FSOUND_SetVolume(mPlayChannel, mVolume);
}

bool VSound::createSampleFromMem( void *data, uint size )
{
	mFModSample = FSOUND_Sample_Load(FSOUND_FREE, (const char *)data, FSOUND_LOADMEMORY, 0, size);

	if(0 == mFModSample)
	{
		std::string errMsg = "Load sample sound from memory error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

bool VSound::createSampleFromFile( const char *file )
{
	mFModSample = FSOUND_Sample_Load(FSOUND_FREE, file, FSOUND_NORMAL, 0, 0);

	if(0 == mFModSample)
	{
		std::string errMsg = "Load sample sound from file [";
		errMsg.append(file);
		errMsg.append("] error:");
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

bool VSound::createStreamFromMem( void *data, uint size )
{
	mFModStream = FSOUND_Stream_Open((const char *)data, FSOUND_LOADMEMORY, 0, size);

	if(0 == mFModStream)
	{
		std::string errMsg = "Load stream sound from memory error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

bool VSound::createStreamFromFile( const char *file )
{
	mFModStream = FSOUND_Stream_Open(file, FSOUND_NORMAL, 0, 0);

	if(0 == mFModStream)
	{
		std::string errMsg = "Load stream sound from file [";
		errMsg.append(file);
		errMsg.append("] error:");
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}
