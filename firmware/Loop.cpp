#include "Loop.h"

Loop* Loop::loopInstance = nullptr;
unsigned int CallbackData::idCount = 0;

CallbackData::CallbackData(unsigned long d, Callback cb)
:mRef(CallbackData::idCount++), mDelay(d), mCb(cb)
{
	mStarted = millis();
}

void Loop::process()
{
	for(auto timeout : timeouts)
	{
		if( millis() - timeout->started() > timeout->delay() )
		{
			// call the timeout
			timeout->callback()();
			timeout->reset();
		}
	}

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
	int hndl = addTimeout(delay, [hndl, cb]()
	{
		Loop::instance()->removeTimeout(hndl);
		cb();
	});
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