/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	Point.h
	created:	D.W 2009-11-05 20:00
	purpose:	地图节点类（寻路用）
==============================================================================*/

#ifndef SSystem_Node_H
#define SSystem_Node_H

namespace SearchSystem
{
	class Node
	{
	public:
		int       note_id ;                                              //(单个)节点数组 节点ID 索引
		bool      note_open;                                             //(单个)节点数组 是否在开启列表中 索引
		bool      note_closed;                                           //(单个)节点数组 是否在关闭列表中 索引
	}; 
}
#endif