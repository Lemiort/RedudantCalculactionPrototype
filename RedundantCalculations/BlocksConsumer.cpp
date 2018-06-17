#include "stdafx.h"
#include "BlocksConsumer.h"
#include <thread>

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

BlocksConsumer::BlocksConsumer(std::shared_ptr<BlocksDeque> blocksDeque)
{
	this->blocksDeque = blocksDeque;
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
		if (this->blocksDeque->GetBlocksCount() > 0)
		{
			int i = 0;
			BlockPtr block;
			for (i = 0; i < this->blocksDeque->GetBlocksCount(); i++)
			{
				block = this->blocksDeque->GetBlock(i);
				if (processedBlocks.count(block) == 0)
					break;
			}

			//first not calculated block  was found
			if ( (bool)block != false )
			{
				auto crc = this->Crc32(block);
				this->blocksDeque->PushCrcForBlock(crc, block);
				this->processedBlocks.insert(block);
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
