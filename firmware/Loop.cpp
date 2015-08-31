#include "Loop.h"

Loop* Loop::loopInstance = nullptr;
int CallbackData::idCount = 0;

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
}

int Loop::addTimeout(unsigned long delay, CallbackData::Callback cb)
{
	CallbackData* cbd = new CallbackData(delay, cb);

	timeouts.push_back(cbd);

	return cbd->id();
}

void Loop::removeTimeout(int hndl)
{
	for(auto timeout : timeouts)
	{
		if(timeout->id() == hndl)
		{
			timeouts.erase(timeout);
			delete timeout;
		}
	}
}