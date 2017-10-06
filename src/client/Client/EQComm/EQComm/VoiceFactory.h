#pragma once

#ifndef __VOICEFACTORY_H
#define __VOICEFACTORY_H

#include "Voice.h"

class VoiceFactory
{
public:
	VoiceFactory(void);
	~VoiceFactory(void);

	static Voice *Create(int type);
	static void Release(Voice *voice);
};

#endif