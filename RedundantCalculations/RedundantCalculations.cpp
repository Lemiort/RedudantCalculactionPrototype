// RedundantCalculations.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "BlocksProducer.h"
#include "BlocksConsumer.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

//typedef std::map<BlocksCrcsPtr, BlocksConsumer> BlocksConsumerCrcs;
typedef std::shared_ptr<BlocksPool> BlocksDequePtr;

int main(int argc, char* argv[])
{
	int producersCount = std::stoi(argv[1]);
	int blockSize = std::stoi(argv[2]);
	int blocksCount = std::stoi(argv[3]);

	int consumersCount = std::stoi(argv[4]);

	BlocksDequePtr blocksDeque(new BlocksPool(blocksCount));

	std::vector<BlocksProducer*> blocksProducers;
	std::vector<BlocksConsumer*> blocksConsumers;

	std::fstream fout;
	fout.open("output.bin", std::ios::binary | std::ios::out);

	//create producer
	for (int i = 0; i < producersCount; i++)
	{
		blocksProducers.push_back(new BlocksProducer(blocksDeque));
		blocksProducers[i]->blocksCount = blocksCount;
		blocksProducers[i]->blockSize = blockSize;
	}

	//create  consumers
	for (int i = 0; i < consumersCount; i++)
	{
		BlocksCrcsPtr consumerResults(new BlocksCrcs());

		auto consumer = new BlocksConsumer(blocksDeque);
		blocksConsumers.push_back(consumer);
	}

	std::chrono::time_point<std::chrono::system_clock> start, now;
	start = std::chrono::system_clock::now();
	int elapsed_seconds = 0;
	int prev_elapsed_seconds = 0;
	
	//variables for time measuring
	start = std::chrono::system_clock::now();
	elapsed_seconds = 0;
	prev_elapsed_seconds = 0;

	//start producing
	for (auto it = blocksProducers.begin(); it != blocksProducers.end(); it++)
	{
		(*it)->Start();
	}
	//start consuming
	for (auto it = blocksConsumers.begin(); it != blocksConsumers.end(); it++)
	{
		(*it)->Start();
	}
	int checkedBlocksCount = 0;
	do
	{
		auto block = blocksDeque->GetBlock(0);
		auto checResult = blocksDeque->CheckCrcsForBlock(block, consumersCount);
		if (checResult == CheckCrcResult::DontMatch)
		{
			for (int i = 0; i < block->size(); i++)
			{
				fout << (*block)[i];
			}
			cout << "missmatch" << endl;
			blocksDeque->DeleteBlock(block);
			checkedBlocksCount++;
		}
		else if (checResult == CheckCrcResult::Match)
		{
			blocksDeque->DeleteBlock(block);
			checkedBlocksCount++;
		}

		//time measuring
		now = std::chrono::system_clock::now();
		elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>
			(now - start).count();
		if (prev_elapsed_seconds != elapsed_seconds)
		{
			std::cout << checkedBlocksCount <<" "<< elapsed_seconds << "s\n";
			prev_elapsed_seconds = elapsed_seconds;
		}
	} while (checkedBlocksCount < blocksCount);
	//blocksConsumer.Stop();

	//stop producing
	for (auto it = blocksProducers.begin(); it != blocksProducers.end(); it++)
	{
		(*it)->Stop();
	}
	//stop consuming
	for (auto it = blocksConsumers.begin(); it != blocksConsumers.end(); it++)
	{
		(*it)->Stop();
	}
	std::cout << blocksDeque->GetBlocksGlobalCounter() << " produced at all" << endl;
	std::cout << blocksDeque->GetBlocksCount() << " blocks stored in memory" << endl;
	std::cout << checkedBlocksCount << " blocks checked" << endl;

	system("pause");
    return 0;
}

