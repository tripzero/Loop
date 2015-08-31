#ifndef _LOOP_H_
#define _LOOP_H_

#include <functional>
#include <list>
#include <memory>

#include "spark_wiring_ticks.h"

class CallbackData
{
public:
	typedef std::function<void (void)> Callback;
	typedef std::unique_ptr

	CallbackData(unsigned long d, Callback cb)
	:mRef(CallbackData::idCount++), mDelay(d), mCb(cb)
	{
		mStarted = millis();
	}

	int id() { return mRef; }
	unsigned long delay() { return mDelay; }
	Callback callback() { return mCb; }
	unsigned long started() { return mStarted; }
	
	void reset()
	{
		mStarted = millis();
	}

	static int idCount;

private:
	int mRef;
	unsigned long mDelay;
	Callback mCb;
	unsigned long mStarted;
};

class Loop
{
public:
	static Loop* instance()
	{
		if(!loopInstance)
		{
			loopInstance = new Loop();
		}

		return loopInstance;
	}

	void process();

	int addTimeout(unsigned long delay, CallbackData::Callback);
	void removeTimeout(int hndl);

private: 
	Loop() { }
	static Loop* loopInstance;
	std::list<CallbackData*> timeouts;
	

	unsigned long lastTime;
};

#endif