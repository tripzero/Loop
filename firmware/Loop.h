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

	CallbackData(unsigned long d, Callback cb)
	:mRef(CallbackData::idCount++), mDelay(d), mCb(cb)
	{
		mStarted = millis();
	}

	unsigned int id() { return mRef; }
	unsigned long delay() { return mDelay; }
	Callback callback() { return mCb; }
	unsigned long started() { return mStarted; }
	
	virtual void reset()
	{
		mStarted = millis();
	}

	static unsigned int idCount;

private:
	int mRef;
	unsigned long mDelay;
	Callback mCb;
	unsigned long mStarted;
};

class HysterisisData: public CallbackData
{
public:
	typedef std::function<bool(void)> Cb;

	HysterisisData(unsigned long delay, Cb cond1, Cb cond2, Callback action)
	:CallbackData(delay, action), condition1(cond1),
	 condition2(cond2), mFirstConditionMet(false)
	{

	}

	bool firstCondition()
	{
		if (!mFirstConditionMet)
		{
			return mFirstConditionMet = condition1();
		}
	}

	bool secondCondition()
	{
		return condition2();
	}

	virtual void reset()
	{
		CallbackData::reset();
		mFirstConditionMet = false;
	}

private:
	Cb condition1;
	Cb condition2;
	bool mFirstConditionMet;
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

	unsigned int addTimeout(unsigned long delay, CallbackData::Callback);
	void removeTimeout(unsigned int hndl);

	void singleShot(unsigned long delay, CallbackData::Callback);

	/* check if two conditions are met within a given timespan.
	   If the conditions are met, do the action.
	   condition1 returning true will trigger the timer.
	 */
	unsigned int addHysterisis(unsigned long delayMs, HysterisisData::Cb condition1, HysterisisData::Cb condition2, HysterisisData::Callback action);
	void removeHysterisis(unsigned int hndl);

private: 
	Loop() { }
	static Loop* loopInstance;
	std::list<CallbackData*> timeouts;
	std::list<HysterisisData*> hysterisises;
	

	unsigned long lastTime;
};

#endif