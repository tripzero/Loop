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
	CallbackData* cb = new CallbackData(delay, cb);

	timeouts.push_back(cb);
}

void Loop::removeTimeout(int hnld)
{
	for(auto timeout : timeouts)
	{
		if(timeout.id() == hndl)
		{
			timeouts.remove(timeout);
		}
	}
}