// RedundantCalculations.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "BlocksProducer.h"
#include <chrono>
#include <iostream>

int main()
{
	BlocksPool blocksPool(new std::deque<BlockPtr>);
	BlocksProducer blockProducer(blocksPool);


	std::chrono::time_point<std::chrono::system_clock> start, now;
	start = std::chrono::system_clock::now();
	int elapsed_seconds = 0;
	int prev_elapsed_seconds = 0;
	
	blockProducer.Start();
	do
	{
		now = std::chrono::system_clock::now();
		elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>
			(now - start).count();
		if (prev_elapsed_seconds != elapsed_seconds)
		{
			std::cout << elapsed_seconds << "\n";
			prev_elapsed_seconds = elapsed_seconds;
		}
	} while (elapsed_seconds < 30);
	blockProducer.Stop();
	
	std::cout << blocksPool->size() << " blocks produced";
	system("pause");
    return 0;
}

