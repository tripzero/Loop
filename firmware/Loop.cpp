#include "Loop.h"

#define PHOTON

#if defined(PHOTON)
#include "spark_wiring_ticks.h"
#else
#include <chrono>
#include <iostream>
#endif

Loop* Loop::loopInstance = nullptr;
unsigned int CallbackData::idCount = 0;

std::list<unsigned int> toRemove;

unsigned long msCount()
{
#if defined(PHOTON)
	return millis();
#else
	auto tm = std::chrono::steady_clock::now();
	double time = std::chrono::duration_cast<std::chrono::milliseconds>(tm.time_since_epoch()).count();
	return time;
#endif
}

void debug(const char* msg)
{
#if !defined(PHOTON)
	std::cout<<msg<<std::endl;
#endif
}

CallbackData::CallbackData(unsigned long d, Callback cb)
:mRef(CallbackData::idCount++), mDelay(d), mCb(cb), mSshot(false)
{
	mStarted = msCount();
}

void Loop::process()
{
	for(auto timeout : timeouts)
	{
		if( msCount() - timeout->started() > timeout->delay() )
		{
			// call the timeout
			timeout->callback()();
			
			if(timeout->isSingleShot())
			{
				toRemove.push_back(timeout->id());
			}
			else 
			{
				timeout->reset();
			}
		}
	}

	for(auto timeout : toRemove)
	{
		removeTimeout(timeout);
	}

	if(toRemove.size())
		toRemove.clear();

	for(auto hysterisis : hysterisises)
	{
		if(hysterisis->firstCondition() && hysterisis->secondCondition())
		{
			hysterisis->reset();
			hysterisis->callback()();
		}
		else if(hysterisis->firstCondition())
		{
			singleShot(hysterisis->delay(), [hysterisis]()
			{
				hysterisis->reset();
			});
		}
	}	
}

unsigned int Loop::addTimeout(unsigned long delay, CallbackData::Callback cb)
{
	CallbackData* cbd = new CallbackData(delay, cb);

	timeouts.push_back(cbd);

	return cbd->id();
}

void Loop::removeTimeout(unsigned int hndl)
{
	for(auto itr = timeouts.begin(); itr != timeouts.end();  itr++)
	{
		auto timeout = *itr;

		if(timeout->id() == hndl)
		{
			timeouts.erase(itr);
			delete timeout;
			return;
		}
	}
}

void Loop::singleShot(unsigned long delay, CallbackData::Callback cb)
{
	CallbackData* cbd = new CallbackData(delay, cb);
	cbd->setSingleShot(true);
	timeouts.push_back(cbd);
}

unsigned int Loop::addHysterisis(unsigned long delayMs, HysterisisData::Cb condition1, HysterisisData::Cb condition2, HysterisisData::Callback action)
{
	auto hyst = new HysterisisData(delayMs, condition1, condition2, action);

	hysterisises.push_back(hyst);

	return hyst->id();
}

void Loop::removeHysterisis(unsigned int hndl)
{
	for(auto itr = hysterisises.begin(); itr != hysterisises.end();  itr++)
	{
		auto hyst = *itr;

		if(hyst->id() == hndl)
		{
			hysterisises.erase(itr);
			delete hyst;
			return;
		}
	}
}