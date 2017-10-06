#pragma once

#ifndef __VOICE_H
#define __VOICE_H

//��Ч����
enum VOICE_TYPE
{
	VOICE_SOUND,//������:WAV,MP2,MP3,OGG,RAW
	VOICE_MUSIC //������:MOD,S3M,XM,IT,MID
};

//��Ч�������ͣ�����������Ч��
enum VOICE_LOAD_TYPE
{
	VOICE_LOAD_BUFFER,//һ���Լ��غͽ��뵽�ڴ棬�ɷ������ã��ʺ�С��Ч������
	VOICE_LOAD_STREAM //�߲��ű߼��ؽ��룬�ʺϴ󱳾����֡�
};

//��Ч����״̬
enum VOIC_PLAY_STATE
{
	VOICE_PLAY_STOPED,
	VOICE_PLAY_PLAYING,
	VOICE_PLAY_PAUSED
};

//��С����ֵ
#define MIN_VOICE_VOLUME 0
//�������ֵ
#define MAX_VOICE_VOLUME 100

//��Ч����
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
	//������С��Χ��MIN_VOICE_VOLUME - MAX_VOICE_VOLUME
	virtual uint GetVolume() = 0;
	virtual void SetVolume(uint volume) = 0;
};

#endif
