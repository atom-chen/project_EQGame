/******************************************************************
* ����: ��_Mike
* ʱ��: 2010-05-23
* ����: ��Ϸ��ɫ���Թ���
*******************************************************************/
#ifndef PLAYERPROPERTY
#define PLAYERPROPERTY

#include "NetPacket.h"
//struct player_property;

class PlayProperty
{
public:
	PlayProperty(void)
		//:mPro(0)
	{}
	~PlayProperty(void)
	{}

	player_property& getPro(){return mPro;}

	void reqProper();
	void notiBasic(ByteArray& recBuff);
	void notiProper(ByteArray& recBuff);
	void changeProper(notify_player_property_changed data);

private:
	player_property mPro;
};
#endif
