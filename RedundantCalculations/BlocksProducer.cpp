#include "stdafx.h"
#include "BlocksProducer.h"
#include <thread>
#include <random>

BlocksProducer::BlocksProducer(std::shared_ptr<BlocksPool> blocksDeque)
{
	this->blocksDeque = blocksDeque;
}

void BlocksProducer::Start()
{
	doRun = true;
	std::thread runThread = std::thread(&BlocksProducer::Run, this);
	runThread.detach();
}

void BlocksProducer::Stop()
{
	doRun = false;
}


BlocksProducer::~BlocksProducer()
{
}

void BlocksProducer::Run()
{
	while (doRun)
	{
		if (this->blocksDeque->GetBlocksGlobalCounter() < this->blocksCount)
		{
			BlockPtr block = this->GenerateBlock();
			try
			{
				this->blocksDeque->PushBackBlock(block);
			}
			catch (std::out_of_range ex)
			{
				//its ok, just a race
			}
		}
	}
}

BlockPtr BlocksProducer::GenerateBlock()
{
	std::random_device rd;   // non-deterministic generator  
	std::mt19937 gen(rd());  // to seed mersenne twister.  
	std::uniform_int_distribution<> dist(0, 8); // distribute results between 0 and 8 inclusive.  

	BlockPtr block = BlockPtr(new std::vector<uint8_t>());
	for (int i = 0; i < blockSize; i++)
	{
		block->push_back(dist(gen));
	}

	return block;
}
