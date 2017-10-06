#pragma once

#ifndef __MsgHandle_H
#define __MsgHandle_H

#include <map>
#include <vector>
#include <algorithm>

#include "ByteArray.h"

typedef ByteArray MsgPack;
typedef int MsgType;

//抽象Handle
class MsgHandleBase
{
public:
	virtual ~MsgHandleBase() {}
	virtual void operator()(MsgPack &) = 0;
};

//静态函数Handle
class MsgHandleFreeFun : public MsgHandleBase
{
public:
	typedef void (FreeFunHandle)(MsgPack &);

	MsgHandleFreeFun(FreeFunHandle* func) :
	m_fun(func)
	{}

	virtual void operator()(MsgPack &pack)
	{
		m_fun(pack);
	}

private:
	FreeFunHandle* m_fun;
};

//类成员函数Handle
template<typename T>
class MsgHandleMemberFun : public MsgHandleBase
{
public:
	typedef void(T::*MemberFunHandle)(MsgPack &);

	MsgHandleMemberFun(MemberFunHandle func, T* obj) :
	m_fun(func),
		m_obj(obj)
	{}

	virtual void operator()(MsgPack &pack)
	{
		(m_obj->*m_fun)(pack);
	}

private:
	MemberFunHandle m_fun;
	T* m_obj;
};

//各类Handle包装
class MsgHandleSlot
{
public:
	~MsgHandleSlot() {}

	MsgHandleSlot() : m_fun(0) {}

	MsgHandleSlot(MsgHandleFreeFun::FreeFunHandle *fun) : m_fun(new MsgHandleFreeFun(fun)) {}

	template<typename T>
	MsgHandleSlot(void(T::*fun)(MsgPack &), T *obj) : m_fun(new MsgHandleMemberFun<T>(fun, obj)) {}

	virtual void operator()(MsgPack &pack)
	{
		(*m_fun)(pack);
	}

	virtual void Release()
	{
		if(m_fun)
		{
			delete m_fun;
			m_fun = 0;
		}

		delete this;
	}

private:
	MsgHandleBase *m_fun;
};

typedef MsgHandleSlot MsgHandle;
typedef MsgHandleSlot* PMsgHandle;
typedef std::map<MsgType, PMsgHandle> MSGHANDLEMAP;

//创建Handle包装类实例
#define NewMsgHandle(cls, fun) new MsgHandleSlot(&cls::fun, this)

#endif
