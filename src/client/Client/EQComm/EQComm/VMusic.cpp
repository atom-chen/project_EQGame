#include "EQCommPCH.h"
#include "VMusic.h"
#include "fmod_errors.h"

#define MAX_MUSIC_VOLUME 256

VMusic::VMusic(void)
{
	mFModMusic = NULL;

}

VMusic::~VMusic(void)
{
}

bool VMusic::CreateFromMem( void *data, uint size, int ltype )
{
	mFModMusic = FMUSIC_LoadSongEx((const char *)data, 0, size, FSOUND_LOADMEMORY, 0, 0);

	if(0 == mFModMusic)
	{
		std::string errMsg = "Load music from memory error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

bool VMusic::CreateFromFile( const char *file, int ltype )
{
	mFModMusic = FMUSIC_LoadSongEx(file, 0, 0, FSOUND_NORMAL, 0, 0);

	if(0 == mFModMusic)
	{
		std::string errMsg = "Load music from file [";
		errMsg.append(file);
		errMsg.append("] error:");
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

void VMusic::Release()
{
	if(mFModMusic)
	{
		FMUSIC_FreeSong(mFModMusic);
		mFModMusic = NULL;
	}

}

bool VMusic::Play( bool loop )
{
	if(0 == mFModMusic)
		return false;

	if(!FMUSIC_SetLooping(mFModMusic, loop))
	{
		std::string errMsg = "Play music error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	if(!FMUSIC_PlaySong(mFModMusic))
	{
		std::string errMsg = "Play music error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

void VMusic::Stop()
{
	if(0 == mFModMusic)
		return;

	FMUSIC_StopSong(mFModMusic);
}

bool VMusic::Pause(bool pause)
{
	if(0 == mFModMusic)
		return false;

	if(!FMUSIC_SetPaused(mFModMusic, pause))
	{
		std::string errMsg = "Pause music error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);

		return false;
	}

	return true;
}

int VMusic::GetPlayState()
{
	if(0 == mFModMusic)
		return VOICE_PLAY_STOPED;

	if(FMUSIC_IsPlaying(mFModMusic))
		return VOICE_PLAY_PLAYING;

	if(FMUSIC_GetPaused(mFModMusic))
		return VOICE_PLAY_PAUSED;

	return VOICE_PLAY_STOPED;
}

uint VMusic::GetVolume()
{
	if(0 == mFModMusic)
		return 0U;

	uint v = (uint)FMUSIC_GetMasterVolume(mFModMusic);

	return (v * MAX_VOICE_VOLUME) / MAX_MUSIC_VOLUME;
}

void VMusic::SetVolume( uint volume )
{
	if(0 == mFModMusic)
		return;

	volume = (volume * MAX_MUSIC_VOLUME) / MAX_VOICE_VOLUME;

	FMUSIC_SetMasterVolume(mFModMusic, volume);
}