#include "stdafx.h"
#include "BlocksConsumer.h"
#include <thread>

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

BlocksConsumer::BlocksConsumer(BlocksPoolPtr blocksDeque,
	MutexPtr dequeWriteFlag,
	SemaphorePtr readSemaphore, 
	BlocksCrcsPtr blocksCrcs,
	MutexPtr crcsLock)
{
	this->blocksDeque = blocksDeque;
	this->blocksCrcs = blocksCrcs;
	this->blocksDequeLock = dequeWriteFlag;
	this->dequeReadSemaphore = readSemaphore;
	this->crcsLock = crcsLock;
}


BlocksConsumer::~BlocksConsumer()
{
}

void BlocksConsumer::Start()
{
	doRun = true;
	std::thread runThread = std::thread(&BlocksConsumer::Run, this);
	runThread.detach();
}

void BlocksConsumer::Stop()
{
	doRun = false;
}

void BlocksConsumer::Run()
{
	//std::deque<BlockPtr>::iterator it = this->blocksDeque->begin();
	while (doRun)
	{
		if (this->blocksDeque->size() > 0)
		{
			BlockPtr block;
			std::lock_guard<std::mutex> lock(*blocksDequeLock);
			//try to find in crcs
			{
				//std::lock_guard<std::mutex> lock(*blocksDequeLock);
				this->dequeReadSemaphore->wait();
			}
			for (int i = 0; i < blocksDeque->size(); i++)
			{
				if (blocksCrcs->count((*blocksDeque)[i]) == 0)
				{
					block = (*blocksDeque)[i];
				}
			}
			//for (const auto& elem : *blocksDeque) 
			//{
			//	if (blocksCrcs->count(elem) == 0)
			//	{
			//		block = elem;
			//		break;
			//	}
			//}
			this->dequeReadSemaphore->notify();
			
			//lock crcs
			std::lock_guard<std::mutex> lock2(*crcsLock);
			//first not calculated block  was found
			if ( (bool)block != false )
			{
				auto crc = this->Crc32(block);
				this->blocksCrcs->insert(std::pair<BlockPtr, uint32_t>(block, crc));
			}
		}
	}
}

uint32_t BlocksConsumer::Crc32(BlockPtr block)
{
	uint32_t crc = 0;
	int k;
	size_t len = block->size();
	auto it = block->begin();

	crc = ~crc;
	while (len--) {
		crc ^= *it++;
		for (k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
	}
	return ~crc;
}
