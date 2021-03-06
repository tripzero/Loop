#ifndef _LOOP_H_
#define _LOOP_H_

#include <functional>
#include <list>
#include <memory>

unsigned long msCount();

namespace L
{
template<typename T> ::std::shared_ptr<T> make_shared(T* t)
{
	return ::std::shared_ptr<T>(t);
}
}

class CallbackData
{
public:
	typedef std::function<void (void)> Callback;
	typedef std::shared_ptr<CallbackData> Ptr;

	CallbackData(unsigned long d, Callback cb);

	unsigned int id() { return mRef; }
	unsigned long delay() { return mDelay; }
	Callback callback() { return mCb; }
	unsigned long started() { return mStarted; }
	bool isSingleShot() { return mSshot; }
	void setSingleShot(bool sshot) { mSshot = sshot; }
	
	virtual void reset()
	{
		mStarted = msCount();
	}

	static unsigned int idCount;

private:
	int mRef;
	unsigned long mDelay;
	Callback mCb;
	unsigned long mStarted;
	bool mSshot;
};

class HysterisisData: public CallbackData
{
public:
	typedef std::function<bool(void)> Cb;
	typedef std::shared_ptr<HysterisisData> Ptr;

	HysterisisData(unsigned long delay, Cb cond1, Cb cond2, Callback action)
	:CallbackData(delay, action), condition1(cond1),
	 condition2(cond2), mFirstConditionMet(false)
	{

	}

	bool firstCondition()
	{
		if (!mFirstConditionMet)
		{
			mFirstConditionMet = condition1();
		}

		return mFirstConditionMet;
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

	unsigned int addTask(CallbackData::Callback);
	void removeTask(unsigned int hndl);


private: 
	Loop() { }
	static Loop* loopInstance;
	std::list<CallbackData::Ptr> timeouts;
	std::list<HysterisisData::Ptr> hysterisises;
	std::list<CallbackData::Ptr> tasks;
	

	unsigned long lastTime;
};

template <class T>
void step(unsigned int delay, T * valueToChange, T targetValue, std::function<void(void)> stepCb)
{
	auto v = *valueToChange;
	if(v == targetValue)
		return;

	if(v < targetValue)
		v++;
	else if(v > targetValue)
		v--;

	*valueToChange = v;

	if(stepCb)
	{
		stepCb();
	}

	Loop::instance()->singleShot(delay, [=](){
		step(delay, valueToChange, targetValue, stepCb);
	});
}

template <class T>
void smooth(unsigned int delay, T * valueToChange, T targetValue, std::function<void(void)> stepCb)
{
	step(delay, valueToChange, targetValue, stepCb);
}

#endif