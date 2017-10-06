#include "EQCommPCH.h"
#include "EQVoiceMgr.h"
#include "LogicTable.h"
#include "CommFunc.h"
#include "VoiceFactory.h"
#include "fmod.h"
#include "fmod_errors.h"
#include "ConfigOption.h"
#include "EQOgreSys.h"

//--------------------------------------------------------------------
template<> EQVoiceMgr* Ogre::Singleton<EQVoiceMgr>::ms_Singleton = 0;
//--------------------------------------------------------------------

EQVoiceMgr::EQVoiceMgr(void)
{
}

EQVoiceMgr::~EQVoiceMgr(void)
{
	release();
}

void EQVoiceMgr::init()
{
	if(EQOgreSys::getSingletonPtr() != NULL)
	{
		mVolumes[EQVOICE_VOLUME_BG] = EQOgreSys::getSingleton().getConfigOption()->getBgVolume();
		mVolumes[EQVOICE_VOLUME_SYS] = EQOgreSys::getSingleton().getConfigOption()->getSysVolume();
	}
	else
	{
		for(int i = 0; i < EQVOICE_VOLUME_CNT; i ++)
		{
			mVolumes[i] = 80;//初使化音量大小
		}
	}

	FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
	FSOUND_SetDriver(0);
	FSOUND_Stream_SetBufferSize(1024);
	if(!FSOUND_Init(44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH))
	{
		std::string errMsg = "Sound init error:";
		errMsg.append(FMOD_ErrorString(FSOUND_GetError()));
		LOGERR(errMsg);
	}

}

void EQVoiceMgr::release()
{
	for(EQVOICELIST::iterator itr = mVoices.begin(); itr != mVoices.end(); ++itr)
	{
		VoiceFactory::Release(itr->second.voice);
	}

	mVoices.clear();

	FSOUND_Close();

}

int EQVoiceMgr::getVoiceType( const char *file )
{
	static const char *soundtypes[] = {".wav",".mp3",".ogg",".mp2",".raw"};
	static const char *musictypes[] = {".mid",".mod",".s3m",".xm",".it"};

	std::string fname, fext;

	GetFileNameAndExt(file, fname, fext);

	size_t i;

	for(i = 0; i < sizeof(soundtypes) / sizeof(const char *); i ++)
	{
		if(0 == stricmp(soundtypes[i], fext.c_str()))
			return VOICE_SOUND;
	}

	for(i = 0; i < sizeof(musictypes) / sizeof(const char *); i ++)
	{
		if(0 == stricmp(musictypes[i], fext.c_str()))
			return VOICE_MUSIC;
	}

	return -1;

}

int EQVoiceMgr::getVolumeType( int cls )
{
	if(EQVOICE_BACKGROUND == cls)
		return EQVOICE_VOLUME_BG;

	return EQVOICE_VOLUME_SYS;

}

bool EQVoiceMgr::loadFromFile( const char *file, const char *name, int cls, int ltype )
{
	Voice *voice = VoiceFactory::Create(getVoiceType(file));

	if(NULL == voice)
		return false;

	if(false == voice->CreateFromFile(file, ltype))
	{
		VoiceFactory::Release(voice);
		return false;
	}

	EQVOICE_INFO vinfo;
	vinfo.cls = cls;
	vinfo.vtype = getVolumeType(cls);
	vinfo.voice = voice;
	//初使化声音大小
	voice->SetVolume(mVolumes[vinfo.vtype]);

	mVoices.insert(std::make_pair(std::string(file), vinfo));

	return true;
}

bool EQVoiceMgr::play( uint id, bool loop/* = false*/ )
{
	const VoiceRow *row = LogicTableManager::getSingleton().getVoiceRow(id);

	if(NULL == row)
		return false;

	if(mVoices.end() == mVoices.find(row->file))
		if(false == loadFromFile(row->file.c_str(), row->file.c_str(), row->cls, row->ltype))
			return false;

	return play(row->file.c_str(), loop);
}

void EQVoiceMgr::stop( uint id )
{
	const VoiceRow *row = LogicTableManager::getSingleton().getVoiceRow(id);

	if(NULL == row)
		return;

	stop(row->file.c_str());
}

bool EQVoiceMgr::play( const char *name, bool loop/* = false*/ )
{
	EQVOICELIST::iterator itr = mVoices.find(std::string(name));

	if(mVoices.end() == itr)
		return false;

	stopACls(itr->second.cls);

	return itr->second.voice->Play(loop);
}

void EQVoiceMgr::stop( const char *name )
{
	EQVOICELIST::iterator itr = mVoices.find(std::string(name));

	if(mVoices.end() == itr)
		return;

	return itr->second.voice->Stop();
}

void EQVoiceMgr::stopACls(int cls)
{
	for(EQVOICELIST::iterator itr = mVoices.begin(); itr != mVoices.end(); ++itr)
	{
		if(itr->second.cls != cls)
			continue;

		itr->second.voice->Stop();
	}
}

void EQVoiceMgr::stopAll()
{
	for(EQVOICELIST::iterator itr = mVoices.begin(); itr != mVoices.end(); ++itr)
	{
		itr->second.voice->Stop();
	}
}

void EQVoiceMgr::setVolume( int vtype, uint volume )
{
	for(EQVOICELIST::iterator itr = mVoices.begin(); itr != mVoices.end(); ++itr)
	{
		if(itr->second.vtype != vtype)
			continue;

		itr->second.voice->SetVolume(volume);
	}

	mVolumes[vtype] = volume;
}

void EQVoiceMgr::setAllVolume( uint volume )
{
	for(EQVOICELIST::iterator itr = mVoices.begin(); itr != mVoices.end(); ++itr)
	{
		itr->second.voice->SetVolume(volume);
	}

	for(int i = 0; i < EQVOICE_VOLUME_CNT; i ++)
		mVolumes[i] = volume;
}

uint EQVoiceMgr::getVolume( int vtype )
{
	return mVolumes[vtype];
}
