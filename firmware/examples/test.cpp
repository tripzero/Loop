#include <Loop.h>

#include <iostream>

int main(int argc, char** argv)
{
	auto l = Loop::instance();
	
	l->singleShot(1000, []()
	{
		std::cout<<"timeout"<<std::endl;
	});

	while(1)
	{
		l->process();
	}

	return 1;
}
