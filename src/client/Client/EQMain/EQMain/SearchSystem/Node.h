/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	Point.h
	created:	D.W 2009-11-05 20:00
	purpose:	��ͼ�ڵ��ࣨѰ·�ã�
==============================================================================*/

#ifndef SSystem_Node_H
#define SSystem_Node_H

namespace SearchSystem
{
	class Node
	{
	public:
		int       note_id ;                                              //(����)�ڵ����� �ڵ�ID ����
		bool      note_open;                                             //(����)�ڵ����� �Ƿ��ڿ����б��� ����
		bool      note_closed;                                           //(����)�ڵ����� �Ƿ��ڹر��б��� ����
	}; 
}
#endif