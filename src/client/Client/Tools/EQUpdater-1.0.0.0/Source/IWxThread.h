
#ifndef __IWxThread_H
#define __IWxThread_H

class IWxThread
{
public:
	virtual void ThreadPro(void *pv) = 0;
};

class WxThreadCls: public wxThread
{
protected:
	IWxThread *m_pThread;
	void *m_pContext;

public:
	virtual void *Entry()
	{
		if(m_pThread)
			m_pThread->ThreadPro(m_pContext);

		return 0;
	}

	WxThreadCls(IWxThread *pThread, void *pContext)
	{
		m_pThread = pThread;
		m_pContext = pContext;
	}

	virtual ~WxThreadCls(void)
	{

	}
};

#endif