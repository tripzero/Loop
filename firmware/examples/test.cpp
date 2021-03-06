/*
compile with:
g++ -I../ -std=c++11 test.cpp ../Loop.cpp -o test
*/

#include <Loop.h>

#include <iostream>

int main(int argc, char** argv)
{
	auto l = Loop::instance();

	auto repeating = l->addTimeout(1000,[]()
	{
		std::cout<<"repeating timeout"<<std::endl;
	});
	
	l->singleShot(1000, []()
	{
		std::cout<<"single shot timeout"<<std::endl;
	});

	/* This is noisy, disable to see other output
	l->addTask([]()
	{
		std::cout<<"processing task"<<std::endl;
	});*/

	l->singleShot(10000, [repeating, &l]()
	{
		l->removeTimeout(repeating);
		std::cout<<"stopping the repeating timeout"<<std::endl;
	});

	int a1 = 200;
	int a2 = 0;

	smooth(10, &a1, a2, [&a1, a2]()
	{
		std::cout<<"a1: "<<a1<<" a2: "<<a2<<std::endl;
	});

	while(1)
	{
		l->process();
	}

	return 1;
}
