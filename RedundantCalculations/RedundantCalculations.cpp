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
	BlocksPoolPtr blocksPool(new std::deque<BlockPtr>);
	std::vector<BlocksProducer*> blocksProducers;

	//result of the computing
	std::map<BlocksCrcsPtr, BlocksConsumer*> consumerCrcs;

	std::vector<BlocksConsumer*> blocksConsumers;

	std::fstream fout;
	fout.open("output.bin", std::ios::binary | std::ios::out);

	//create producer
	for (int i = 0; i < producersCount; i++)
	{
		auto producer = new BlocksProducer(blocksPool, dequeLock);
		producer->blockSize = blockSize;
		producer->blocksCount = blocksCount;
		blocksProducers.push_back(producer);
	}

	//create  consumers
	for (int i = 0; i < consumersCount; i++)
	{
		BlocksCrcsPtr consumerResults(new BlocksCrcs());

		auto consumer = new BlocksConsumer(blocksPool, dequeLock, consumerResults);

		//associate result with consumer
		consumerCrcs.insert(std::pair<BlocksCrcsPtr, BlocksConsumer*>(consumerResults, consumer));
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


	for (auto it = blocksProducers.begin(); it != blocksProducers.end(); it++)
	{
		(*it)->Start();
	}
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
		for (auto it = consumerCrcs.begin(); it != consumerCrcs.end(); it++)
		{
			auto currentBlockCrcIter = it->first->begin();
			auto endBlocksCrcIter = it->first->end();
			if (firstCrc)
			{
				//there is some result
				if (currentBlockCrcIter != endBlocksCrcIter)
				{
					checkedConsumersCount++;
					crc = (*it).first->begin()->second;
					firstCrc = true;
				}
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
			consumerCrcs.erase(consumerCrcs.begin());
			std::lock_guard<std::mutex> lock(*dequeLock);
			blocksPool->erase(blocksPool->begin());
		}


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

	for (auto it = blocksProducers.begin(); it != blocksProducers.end(); it++)
	{
		(*it)->Stop();
	}
	for (auto it = blocksConsumers.begin(); it != blocksConsumers.end(); it++)
	{
		(*it)->Stop();
	}
	std::cout << blocksPool->size() << " blocks produced";
	for (auto it = consumerCrcs.begin(); it != consumerCrcs.end(); it++)
	{
		std::cout << (*it).first->size() << "\n";
	}

	system("pause");
    return 0;
}

