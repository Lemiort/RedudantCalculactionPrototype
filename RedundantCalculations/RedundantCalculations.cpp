// RedundantCalculations.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "BlocksProducer.h"
#include "BlocksConsumer.h"
#include <chrono>
#include <iostream>

//typedef std::map<BlocksCrcsPtr, BlocksConsumer> BlocksConsumerCrcs;

int main()
{
	MutexPtr dequeLock(new std::mutex());
	BlocksPoolPtr blocksPool(new std::deque<BlockPtr>);
	//BlocksProducer blockProducer(blocksPool);
	std::vector<BlocksProducer*> blocksProducers;
	for (int i = 0; i < 8; i++)
	{
		blocksProducers.push_back(new BlocksProducer(blocksPool, dequeLock));
	}

	std::chrono::time_point<std::chrono::system_clock> start, now;
	start = std::chrono::system_clock::now();
	int elapsed_seconds = 0;
	int prev_elapsed_seconds = 0;
	
	for (auto it = blocksProducers.begin(); it != blocksProducers.end(); it++)
	{
		(*it)->Start();
	}
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
	} while (elapsed_seconds < 10);
	for (auto it = blocksProducers.begin(); it != blocksProducers.end(); it++)
	{
		(*it)->Stop();
	}
	
	std::cout << blocksPool->size() << " blocks produced";


	//BlocksCrcsPtr blocksCrcs(new BlocksCrcs());
	//BlocksConsumer blocksConsumer(blocksPool, blocksCrcs);
	std::map<BlocksCrcsPtr, BlocksConsumer*> consumerCrcs;

	std::vector<BlocksConsumer*> blocksConsumers;
	for (int i = 0; i < 8; i++)
	{
		BlocksCrcsPtr consumerResults(new BlocksCrcs());

		auto consumer = new BlocksConsumer(blocksPool, consumerResults);

		//associate result with consumer
		consumerCrcs.insert(std::pair<BlocksCrcsPtr, BlocksConsumer*>(consumerResults, consumer));
		//save consumer
		blocksConsumers.push_back(consumer);
	}

	start = std::chrono::system_clock::now();
	elapsed_seconds = 0;
	prev_elapsed_seconds = 0;

	//blocksConsumer.Start();
	for (auto it = blocksConsumers.begin(); it != blocksConsumers.end(); it++)
	{
		(*it)->Start();
	}
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
	//blocksConsumer.Stop();
	for (auto it = blocksConsumers.begin(); it != blocksConsumers.end(); it++)
	{
		(*it)->Stop();
	}

	//std::cout << blocksCrcs->size()<< " blocks consumed";
	for (auto it = consumerCrcs.begin(); it != consumerCrcs.end(); it++)
	{
		std::cout << (*it).first->size() << "\n";
	}

	system("pause");
    return 0;
}

