#include "EQCommPCH.h"
#include "VoiceFactory.h"
#include "VSound.h"
#include "VMusic.h"

VoiceFactory::VoiceFactory(void)
{
}

VoiceFactory::~VoiceFactory(void)
{
}

Voice * VoiceFactory::Create( int type )
{
	Voice *voice = NULL;

	switch(type)
	{
	case VOICE_SOUND:
		voice = new VSound();
		break;
	case VOICE_MUSIC:
		voice = new VMusic();
		break;
	}

	return voice;

}

void VoiceFactory::Release( Voice *voice )
{
	voice->Release();
	delete voice;

}