#include <Loop/Loop.h>

Loop *loop = Loop::instance();

void setup()
{
	//Time Sync Every day
	loop->addTimeout(1000 * 60 * 60 * 24, [](){
		Spark.syncTime();
	});
}

void loop()
{
	loop->process();
}