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


int main(int argc, char* argv[])
{
	int producersCount = std::stoi(argv[1]);
	int blockSize = std::stoi(argv[2]);
	int blocksCount = std::stoi(argv[3]);

	int consumersCount = std::stoi(argv[4]);

	MutexPtr dequeLock(new std::mutex());
	MutexPtr crcsLock(new std::mutex());
	SemaphorePtr readSemapthore(new Semaphore(consumersCount));
	BlocksPoolPtr blocksPool(new std::deque<BlockPtr>);
	std::vector<BlocksProducer*> blocksProducers;

	//result of the computing
	std::vector<BlocksCrcsPtr> consumerCrcs;

	std::vector<BlocksConsumer*> blocksConsumers;

	std::fstream fout;
	fout.open("output.bin", std::ios::binary | std::ios::out);

	//create producer
	for (int i = 0; i < producersCount; i++)
	{
		auto producer = new BlocksProducer(blocksPool, dequeLock,producersCount, readSemapthore);
		producer->blockSize = blockSize;
		producer->blocksCount = blocksCount;
		blocksProducers.push_back(producer);
	}

	//create  consumers
	for (int i = 0; i < consumersCount; i++)
	{
		BlocksCrcsPtr consumerResults(new BlocksCrcs());

		auto consumer = new BlocksConsumer(blocksPool, dequeLock,readSemapthore, consumerResults, crcsLock);

		//associate result with consumer
		consumerCrcs.push_back(consumerResults);
		//consumerCrcs.insert(std::pair<BlocksCrcsPtr, BlocksConsumer*>(consumerResults, consumer));
		//save consumer
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
		//find different crcs
		bool firstCrc = true;
		uint32_t crc = 0;
		int checkedConsumersCount = 0;
		//for each consumer
		for (int i=0;i<consumersCount;i++)
		{
			//lock for chaging crcs
			std::lock_guard<std::mutex> lock(*crcsLock);
			auto currentBlockCrcIter = consumerCrcs[i]->begin();
			auto endBlocksCrcIter = consumerCrcs[i]->end();
			//dont check for first time
			if (firstCrc)
			{
				//there is some result
				if (currentBlockCrcIter != endBlocksCrcIter)
				{
					checkedConsumersCount++;
					crc = consumerCrcs[i]->begin()->second;
					firstCrc = true;
				}
				//block stil unready
				else
				{
					checkedConsumersCount = 0;
					break;
				}
			}
			else
			{
				//check for any result
				if (currentBlockCrcIter != endBlocksCrcIter)
				{
					checkedConsumersCount++;
					//compare prev and current crc
					if (crc != currentBlockCrcIter->second)
					{
						std::cout << crc << "!=" << currentBlockCrcIter->second << std::endl;
						auto block = currentBlockCrcIter->first;
						for (int i = 0; i < block->size(); i++)
						{
							fout << (*block)[i];
						}
						//it is not needed to check every consumer result from this moment
						checkedConsumersCount = consumersCount;
						break;
					}
					crc = currentBlockCrcIter->second;
				}
				//still no results
				else
				{
					checkedConsumersCount = 0;
					break;
				}
			}
		}
		//is every block checked?
		if (checkedConsumersCount == consumersCount)
		{
			checkedBlocksCount++;
			//remove first block for each consumer
			for (int i = 0; i < consumersCount; i++)
			{
				consumerCrcs[i]->erase(consumerCrcs[i]->begin());//erase(consumerCrcs[i].begin());
			}
			
			//lock blocks
			std::lock_guard<std::mutex> lock(*dequeLock);
			blocksPool->erase(blocksPool->begin());
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
	std::cout << blocksPool->size() << " blocks produced";
	for (int i = 0; i<consumersCount; i++)
	{
		std::cout << consumerCrcs[i]->size() << "\n";
	}

	system("pause");
    return 0;
}

